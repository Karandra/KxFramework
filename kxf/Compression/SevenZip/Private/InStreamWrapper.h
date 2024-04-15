#pragma once
#include "../Common.h"
#include "Utility.h"
#include "WithEvtHandler.h"
#include "kxf/System/COM.h"
#include "kxf/Core/ErrorCode.h"
#include <7zip/CPP/7zip/IStream.h>

namespace kxf::SevenZip::Private
{
	class InStream: public WithEvtHandler, public IInStream, public IStreamGetSize
	{
		private:
			COM::RefCount<InStream> m_RefCount;

		public:
			InStream(IEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this)
			{
			}
			virtual ~InStream() = default;

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
	class InStreamWrapper: public InStream
	{
		protected:
			int64_t m_BytesRead = 0;
			int64_t m_BytesTotal = -1;

		protected:
			virtual HResult DoRead(void* data, uint32_t size, uint32_t& read) = 0;
			virtual HResult DoSeek(int64_t offset, uint32_t seekMode, int64_t& newPosition) = 0;
			virtual HResult DoGetSize(int64_t& size) const = 0;

			ArchiveEvent CreateEvent()
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent();
				event.SetTotal(m_BytesTotal);
				event.SetCompleted(m_BytesRead);

				return event;
			}
			bool SentWriteEvent()
			{
				ArchiveEvent event = CreateEvent();
				event.SetProgress(m_BytesRead, m_BytesTotal);
				return SendEvent(event, IArchiveUpdate::EvtReadStream);
			}

		public:
			InStreamWrapper(IEvtHandler* evtHandler = nullptr)
				:InStream(evtHandler)
			{
			}

		public:
			void SpecifyTotalSize(int64_t size) noexcept
			{
				m_BytesTotal = size;
				if (m_EvtHandler)
				{
					SentWriteEvent();
				}
			}

		public:
			// ISequentialInStream
			STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize) override;

			// IInStream
			STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition) override;

			// IStreamGetSize
			STDMETHOD(GetSize)(UInt64* size) override;
	};
}

namespace kxf::SevenZip::Private
{
	class InStreamWrapper_COM_IStream: public InStreamWrapper
	{
		protected:
			COMPtr<::IStream> m_Stream;

		protected:
			HResult DoRead(void* data, uint32_t size, uint32_t& read) override
			{
				ULONG readBase = 0;
				HResult hr = m_Stream->Read(data, size, &readBase);
				if (hr)
				{
					read = readBase;
				}
				return hr;
			}
			HResult DoSeek(int64_t offset, uint32_t seekMode, int64_t& newPosition) override
			{
				LARGE_INTEGER offsetBase = {};
				offsetBase.QuadPart = offset;

				ULARGE_INTEGER newPositionBase = {};
				HResult hr = m_Stream->Seek(offsetBase, seekMode, &newPositionBase);
				if (hr)
				{
					newPosition = newPositionBase.QuadPart;
				}
				return hr;
			}
			HResult DoGetSize(int64_t& size) const override
			{
				STATSTG stat = {};
				HResult hr = m_Stream->Stat(&stat, STATFLAG_NONAME);
				if (hr)
				{
					size = stat.cbSize.QuadPart;
				}
				return hr;
			}

		public:
			InStreamWrapper_COM_IStream(COMPtr<::IStream> stream, IEvtHandler* evtHandler = nullptr)
				:InStreamWrapper(evtHandler), m_Stream(std::move(stream))
			{
			}
	};
}

namespace kxf::SevenZip::Private
{
	class InStreamWrapper_IInputStream: public InStreamWrapper
	{
		protected:
			InputStreamDelegate m_Stream;

		private:
			HResult GetLastError() const
			{
				return m_Stream->GetLastError().IsSuccess() ? HResult::Success() : HResult::Fail();
			}

		protected:
			HResult DoRead(void* data, uint32_t size, uint32_t& read) override
			{
				m_Stream->Read(data, size);
				read = m_Stream->LastRead().ToBytes();

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
					newPosition = m_Stream->SeekI(offset, *streamSeek).ToBytes();
					return GetLastError();
				}
				return HResult::InvalidArgument();
			}
			HResult DoGetSize(int64_t& size) const override
			{
				size = m_Stream->GetSize().ToBytes();
				return GetLastError();
			}

		public:
			InStreamWrapper_IInputStream(InputStreamDelegate stream, IEvtHandler* evtHandler = nullptr)
				:InStreamWrapper(evtHandler), m_Stream(std::move(stream))
			{
			}
	};
}
