#pragma once
#include "../Common.h"
#include "WithEvtHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include <7zip/CPP/7zip/IStream.h>

namespace kxf::SevenZip::Private
{
	class InStreamWrapper: public WithEvtHandler, public IInStream, public IStreamGetSize
	{
		private:
			COM::RefCount<InStreamWrapper> m_RefCount;

		protected:
			COMPtr<IStream> m_BaseStream;

			int64_t m_BytesRead = 0;
			int64_t m_BytesTotal = -1;

		protected:
			ArchiveEvent CreateEvent(EventID id = ArchiveEvent::EvtProcess)
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent(id);
				event.SetTotal(m_BytesTotal);
				event.SetProcessed(m_BytesRead);

				return event;
			}

		public:
			InStreamWrapper(wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this)
			{
			}
			InStreamWrapper(const COMPtr<IStream>& baseStream, wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this), m_BaseStream(baseStream)
			{
			}
			virtual ~InStreamWrapper() = default;

		public:
			void SetSize(int64_t size) noexcept
			{
				m_BytesTotal = size;
			}

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

			// ISequentialInStream
			STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize) override;

			// IInStream
			STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition) override;

			// IStreamGetSize
			STDMETHOD(GetSize)(UInt64* size) override;
	};
}
