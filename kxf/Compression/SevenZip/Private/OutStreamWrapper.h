#pragma once
#include "../Common.h"
#include "Utility.h"
#include "WithEvtHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include "kxf/IO/StreamDelegate.h"
#include "kxf/IO/FileStream.h"
#include <7zip/CPP/7zip/IStream.h>

namespace kxf::SevenZip::Private
{
	class OutStream: public WithEvtHandler, public IOutStream
	{
		private:
			COM::RefCount<OutStream> m_RefCount;

		public:
			OutStream(EvtHandler* evtHandler = nullptr)
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

			ArchiveEvent CreateEvent()
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent();
				event.SetProgress(m_BytesWritten, m_BytesTotal);

				return event;
			}
			bool SentWriteEvent()
			{
				ArchiveEvent event = CreateEvent();
				event.SetProgress(m_BytesWritten, m_BytesTotal);
				return SendEvent(event, IArchiveExtract::EvtWriteStream);
			}

		public:
			OutStreamWrapper(EvtHandler* evtHandler = nullptr)
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
			COMPtr<IStream> m_Stream;

		protected:
			HResult DoWrite(const void* data, uint32_t size, uint32_t& written) override
			{
				ULONG writtenBase = 0;
				HResult hr = m_Stream->Write(data, size, &writtenBase);
				written = writtenBase;

				return hr;
			}
			HResult DoSeek(int64_t offset, uint32_t seekMode, int64_t& newPosition) override
			{
				LARGE_INTEGER offsetLI = {};
				offsetLI.QuadPart = offset;

				ULARGE_INTEGER newPositionLI = {};
				HResult hr = m_Stream->Seek(offsetLI, seekMode, &newPositionLI);
				newPosition = newPositionLI.QuadPart;

				return hr;
			}
			HResult DoSetSize(int64_t size) override
			{
				ULARGE_INTEGER value = {};
				value.QuadPart = size;
				return m_Stream->SetSize(value);
			}

		public:
			OutStreamWrapper_IStream(COMPtr<IStream> baseStream, EvtHandler* evtHandler = nullptr)
				:OutStreamWrapper(evtHandler), m_Stream(std::move(baseStream))
			{
			}
	};

	class OutStreamWrapper_wxOutputStream: public OutStreamWrapper
	{
		protected:
			OutputStreamDelegate m_Stream;

		private:
			HResult GetLastError() const
			{
				return m_Stream->GetLastError() == wxSTREAM_NO_ERROR ? HResult::Success() : HResult::Fail();
			}

		protected:
			HResult DoWrite(const void* data, uint32_t size, uint32_t& written) override
			{
				m_Stream.Write(data, size);
				written = m_Stream.LastWrite();

				return GetLastError();
			}
			HResult DoSeek(int64_t offset, uint32_t seekMode, int64_t& newPosition) override
			{
				if (!m_Stream->IsSeekable())
				{
					return HResult::NotImplemented();
				}
				if (auto streamSeek = MapSeekMode(seekMode))
				{
					newPosition = m_Stream->SeekO(offset, *streamSeek);
					return GetLastError();
				}
				return HResult::InvalidArgument();
			}
			HResult DoSetSize(int64_t size) override
			{
				if (m_Stream && m_Stream->IsKindOf(wxCLASSINFO(FileStream)))
				{
					return static_cast<FileStream&>(*m_Stream).SetAllocationSize(size) ? HResult::Success() : HResult::Fail();
				}
				return HResult::NotImplemented();
			}

		public:
			OutStreamWrapper_wxOutputStream(OutputStreamDelegate stream, EvtHandler* evtHandler = nullptr)
				:OutStreamWrapper(evtHandler), m_Stream(std::move(stream))
			{
			}
	};
}
