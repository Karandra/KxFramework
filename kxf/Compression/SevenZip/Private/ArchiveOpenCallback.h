#pragma once
#include "../Common.h"
#include "WithEvtHandler.h"
#include "PasswordHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include <7zip/CPP/7zip/Archive/IArchive.h>
#include <7zip/CPP/7zip/IPassword.h>

namespace kxf::SevenZip::Private::Callback
{
	class OpenArchive: public WithEvtHandler, public IArchiveOpenCallback, public ICryptoGetTextPassword, public ICryptoGetTextPassword2
	{
		private:
			COM::RefCount<OpenArchive> m_RefCount;
			PasswordHandler m_PasswordHandler;

		protected:
			int64_t m_BytesCompleted = 0;
			int64_t m_BytesTotal = 0;

			int64_t m_ItemsCompleted = 0;
			int64_t m_ItemsTotal = 0;

		protected:
			ArchiveEvent CreateEvent(EventID id)
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent(id);
				if (id == IArchive::EvtOpenBytes)
				{
					event.SetProgress(m_BytesCompleted, m_BytesTotal);
				}
				else if (id == IArchive::EvtOpenItems)
				{
					event.SetProgress(m_ItemsCompleted, m_ItemsTotal);
				}
				else
				{
					return {};
				}
				return event;
			}

		private:
			bool SendProgressEvents()
			{
				ArchiveEvent bytesEvent = CreateEvent(IArchive::EvtOpenBytes);
				ArchiveEvent itemsEvent = CreateEvent(IArchive::EvtOpenItems);
				return SendEvent(bytesEvent) && SendEvent(bytesEvent);
			}

		public:
			OpenArchive(wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this), m_PasswordHandler(evtHandler)
			{
			}
			virtual ~OpenArchive() = default;

		public:
			// WithEvtHandler
			void SetEvtHandler(wxEvtHandler* evtHandler) noexcept override
			{
				WithEvtHandler::SetEvtHandler(evtHandler);
				m_PasswordHandler.SetEvtHandler(evtHandler);
			}

			// IUnknown
			STDMETHOD(QueryInterface)(const ::IID& iid, void** ppvObject);
			STDMETHOD_(ULONG, AddRef)() override
			{
				return m_RefCount.AddRef();
			}
			STDMETHOD_(ULONG, Release)() override
			{
				return m_RefCount.Release();
			}

			// IArchiveOpenCallback
			STDMETHOD(SetTotal)(const UInt64* files, const UInt64* bytes)
			{
				m_BytesTotal = bytes ? *bytes : -1;
				m_ItemsTotal = files ? *files : -1;

				if (m_EvtHandler && !SendProgressEvents())
				{
					return *HResult::Abort();
				}
				return *HResult::Success();
			}
			STDMETHOD(SetCompleted)(const UInt64* files, const UInt64* bytes)
			{
				m_BytesCompleted = bytes ? *bytes : -1;
				m_ItemsCompleted = files ? *files : -1;

				if (m_EvtHandler && !SendProgressEvents())
				{
					return *HResult::Abort();
				}
				return S_OK;
			}

			// ICryptoGetTextPassword
			STDMETHOD(CryptoGetTextPassword)(BSTR* password)
			{
				return *m_PasswordHandler.OnPasswordRequest(password);
			}

			// ICryptoGetTextPassword2
			STDMETHOD(CryptoGetTextPassword2)(Int32* passwordIsDefined, BSTR* password)
			{
				return *m_PasswordHandler.OnPasswordRequest(password, passwordIsDefined);
			}
	};
}
