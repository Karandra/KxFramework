#include "stdafx.h"
#include "ArchiveUpdateCallback.h"
#include "InStreamWrapper.h"
#include "Utility.h"
#include "kxf/System/VariantProperty.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP UpdateArchive::QueryInterface(const ::IID& iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IArchiveUpdateCallback)
		{
			*ppvObject = static_cast<IArchiveUpdateCallback*>(this);
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
		if (iid == IID_ICompressProgressInfo)
		{
			*ppvObject = static_cast<ICompressProgressInfo*>(this);
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP UpdateArchive::SetTotal(UInt64 bytes)
	{
		m_BytesTotal = bytes;

		if (m_EvtHandler)
		{
			ArchiveEvent event = CreateEvent();
			return SendEvent(event) ? S_OK : E_ABORT;
		}
		return S_OK;
	}
	STDMETHODIMP UpdateArchive::SetCompleted(const UInt64* bytes)
	{
		m_BytesCompleted = bytes ? *bytes : 0;

		if (m_EvtHandler)
		{
			ArchiveEvent event = CreateEvent();
			return SendEvent(event) ? S_OK : E_ABORT;
		}
		return S_OK;
	}

	STDMETHODIMP UpdateArchive::GetUpdateItemInfo(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive)
	{
		// Setting info for create (or append) mode
		// TODO: Support append mode

		if (newData)
		{
			*newData = 1;
		}
		if (newProperties)
		{
			*newProperties = 1;
		}
		if (indexInArchive)
		{
			*indexInArchive = std::numeric_limits<UInt32>::max();
		}

		if (m_EvtHandler && index < m_SourcePaths.size())
		{
			ArchiveEvent event = CreateEvent();
			event.SetItem(m_SourcePaths[index]);
			if (!SendEvent(event))
			{
				return E_ABORT;
			}
		}
		return S_OK;
	}
	STDMETHODIMP UpdateArchive::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value)
	{
		VariantProperty prop;

		if (propID == kpidIsAnti)
		{
			prop = false;
			if (value)
			{
				return *prop.CopyToNative(*value);
			}
			return E_INVALIDARG;
		}

		if (index >= m_SourcePaths.size())
		{
			return E_INVALIDARG;
		}

		const FileItem& fileInfo = m_SourcePaths[index];
		switch (propID)
		{
			case kpidPath:
			{
				prop = m_TargetPaths[index].GetFullPathWithNS(FSPathNamespace::Win32File).wc_str();
				break;
			}
			case kpidIsDir:
			{
				prop = fileInfo.IsDirectory();
				break;
			}
			case kpidSize:
			{
				// Apparently 7-Zip requires file size to be of 'uint64_t' type
				prop = fileInfo.GetSize().GetBytes<uint64_t>();
				break;
			}
			case kpidAttrib:
			{
				// 'uint32_t' seems to be correct type here
				prop = static_cast<uint32_t>(FileSystem::Private::MapFileAttributes(fileInfo.GetAttributes()));
				break;
			}
			case kpidCTime:
			{
				prop = fileInfo.GetCreationTime();
				break;
			}
			case kpidMTime:
			{
				prop = fileInfo.GetModificationTime();
				break;
			}
			case kpidATime:
			{
				prop = fileInfo.GetLastAccessTime();
				break;
			}
		};

		if (value)
		{
			return *prop.CopyToNative(*value);
		}
		return E_INVALIDARG;
	}
	STDMETHODIMP UpdateArchive::GetStream(UInt32 index, ISequentialInStream** inStream)
	{
		if (index >= m_SourcePaths.size())
		{
			return E_INVALIDARG;
		}

		const FileItem& fileInfo = m_SourcePaths[index];
		if (fileInfo.IsDirectory())
		{
			return S_OK;
		}

		auto fileStream = OpenFileToRead(fileInfo.GetFullPath());
		if (!fileStream)
		{
			return *Win32Error::GetLastError().ToHResult().value_or(E_FAIL);
		}

		auto wrapperStream = CreateObject<InStreamWrapper>(fileStream, m_EvtHandler);
		wrapperStream->SetSize(fileInfo.GetSize().GetBytes());
		*inStream = wrapperStream.Detach();

		if (m_EvtHandler)
		{
			ArchiveEvent event = CreateEvent();
			event.SetItem(fileInfo);
			if (!SendEvent(event))
			{
				return E_ABORT;
			}
		}
		return S_OK;
	}
	STDMETHODIMP UpdateArchive::SetOperationResult(Int32 operationResult)
	{
		return S_OK;
	}
}
