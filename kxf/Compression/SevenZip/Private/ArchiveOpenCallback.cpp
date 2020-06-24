#include "stdafx.h"
#include "ArchiveOpenCallback.h"

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP OpenArchive::QueryInterface(const ::IID& iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IArchiveOpenCallback)
		{
			*ppvObject = static_cast<IArchiveOpenCallback*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ICryptoGetTextPassword)
		{
			*ppvObject = static_cast<ICryptoGetTextPassword*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_ICryptoGetTextPassword2)
		{
			*ppvObject = static_cast<ICryptoGetTextPassword2*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP OpenArchive::SetTotal(const UInt64* files, const UInt64* bytes)
	{
		m_BytesTotal = bytes ? *bytes : 0;

		if (m_EvtHandler)
		{
			ArchiveEvent event = CreateEvent();
			return SendEvent(event) ? S_OK : E_ABORT;
		}
		return S_OK;
	}
	STDMETHODIMP OpenArchive::SetCompleted(const UInt64* files, const UInt64* bytes)
	{
		m_BytesCompleted = bytes ? *bytes : 0;

		if (m_EvtHandler)
		{
			ArchiveEvent event = CreateEvent();
			return SendEvent(event) ? S_OK : E_ABORT;
		}
		return S_OK;
	}
}
