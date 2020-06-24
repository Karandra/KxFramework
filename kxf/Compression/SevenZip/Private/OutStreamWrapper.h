#pragma once
#include "../Common.h"
#include "WithEvtHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include <7zip/CPP/7zip/IStream.h>

namespace kxf::SevenZip::Private
{
	class OutStream: public WithEvtHandler, public IOutStream
	{
		private:
			COM::RefCount<OutStream> m_RefCount;

		public:
			OutStream(wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this)
			{
			}
			virtual ~OutStream() = default;

		public:
			// IUnknown
			STDMETHOD(QueryInterface)(const ::IID& iid, void** ppvObject) override;
			STDMETHOD_(ULONG, AddRef)() override
			{
				return m_RefCount.AddRef();
			}
			STDMETHOD_(ULONG, Release)() override
			{
				return m_RefCount.Release();
			}
	};
}

namespace kxf::SevenZip::Private
{
	class OutStreamWrapper: public OutStream
	{
		protected:
			int64_t m_BytesWritten = 0;
			int64_t m_BytesTotal = -1;

		protected:
			virtual HResult DoWrite(const void* data, uint32_t size, uint32_t& written) = 0;
			virtual HResult DoSeek(int64_t offset, uint32_t seekMode, int64_t& newPosition) = 0;
			virtual HResult DoSetSize(int64_t size) = 0;

			ArchiveEvent CreateEvent(EventID id = ArchiveEvent::EvtProcess)
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent(id);
				event.SetTotal(m_BytesTotal);
				event.SetProcessed(m_BytesWritten);

				return event;
			}

		public:
			OutStreamWrapper(wxEvtHandler* evtHandler = nullptr)
				:OutStream(evtHandler)
			{
			}

		public:
			// ISequentialOutStream
			STDMETHOD(Write)(const void* data, UInt32 size, UInt32* written) override;

			// IOutStream
			STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition) override;
			STDMETHOD(SetSize)(UInt64 newSize) override;
	};

	class OutStreamWrapper_IStream: public OutStreamWrapper
	{
		protected:
			COMPtr<IStream> m_BaseStream;

		protected:
			HResult DoWrite(const void* data, uint32_t size, uint32_t& written) override
			{
				ULONG writtenBase = 0;
				HResult hr = m_BaseStream->Write(data, size, &writtenBase);
				written = writtenBase;

				return hr;
			}
			HResult DoSeek(int64_t offset, uint32_t seekMode, int64_t& newPosition) override
			{
				LARGE_INTEGER offsetLI = {};
				offsetLI.QuadPart = offset;

				ULARGE_INTEGER newPositionLI = {};
				HResult hr = m_BaseStream->Seek(offsetLI, seekMode, &newPositionLI);
				newPosition = newPositionLI.QuadPart;

				return hr;
			}
			HResult DoSetSize(int64_t size) override
			{
				ULARGE_INTEGER value = {};
				value.QuadPart = size;
				return m_BaseStream->SetSize(value);
			}

		public:
			OutStreamWrapper_IStream(COMPtr<IStream> baseStream, wxEvtHandler* evtHandler = nullptr)
				:OutStreamWrapper(evtHandler), m_BaseStream(std::move(baseStream))
			{
			}
	};
}
