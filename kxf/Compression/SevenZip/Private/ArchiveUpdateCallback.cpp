#include "stdafx.h"
#include "ArchiveUpdateCallback.h"
#include "InStreamWrapper.h"
#include "Utility.h"
#include "kxf/System/VariantProperty.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"

namespace kxf::SevenZip::Private::Callback
{
	FileItem UpdateArchive::GetExistingFileInfo(size_t fileIndex) const
	{
		return GetArchiveItem(*m_Archive, fileIndex);
	}

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
}

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP UpdateArchiveWrapper::GetUpdateItemInfo(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive)
	{
		if (m_Callback.ShouldCancel())
		{
			return *HResult::Abort();
		}

		if (indexInArchive && newData && newProperties)
		{
			bool updateData = false;
			bool updateProperties = false;
			size_t newIndex = m_Callback.OnGetUpdateMode(index, updateData, updateProperties);

			if (newIndex != Compression::InvalidIndex)
			{
				*indexInArchive = newIndex;
				*newProperties = updateProperties;
				*newData = updateData;

				return *HResult::Success();
			}
			
			// Assume new file
			*newData = 1;
			*newProperties = 1;
			*indexInArchive = std::numeric_limits<UInt32>::max();
		}
		return *HResult::InvalidPointer();
	}
	STDMETHODIMP UpdateArchiveWrapper::GetProperty(UInt32 index, PROPID propID, PROPVARIANT* value)
	{
		if (m_Callback.ShouldCancel())
		{
			return *HResult::Abort();
		}

		if (value)
		{
			auto DoGetProperty = [](PROPID propID, FileItem fileItem) -> VariantProperty
			{
				switch (propID)
				{
					case kpidIsAnti:
					{
						// WTF is this property?
						return false;
					}
					case kpidPath:
					{
						return fileItem.GetFullPath().GetFullPath();
					}
					case kpidIsDir:
					{
						return fileItem.IsDirectory();
					}
					case kpidSize:
					{
						// Apparently 7-Zip requires file size to be of 'uint64_t' type
						return fileItem.GetSize().GetBytes<uint64_t>();
					}
					case kpidAttrib:
					{
						// Attributes should be of 'uint32_t' type
						return static_cast<uint32_t>(FileSystem::Private::MapFileAttributes(fileItem.GetAttributes()));
					}
					case kpidCTime:
					{
						return fileItem.GetCreationTime();
					}
					case kpidMTime:
					{
						return fileItem.GetModificationTime();
					}
					case kpidATime:
					{
						return fileItem.GetLastAccessTime();
					}
				};
				return {};
			};

			VariantProperty property = DoGetProperty(propID, m_Callback.OnGetProperties(index));
			if (property)
			{
				return *property.CopyToNative(*value);
			}
			return *HResult::InvalidArgument();
		}
		return *HResult::InvalidPointer();
	}
	
	STDMETHODIMP UpdateArchiveWrapper::GetStream(UInt32 index, ISequentialInStream** inStream)
	{
		*inStream = nullptr;
		if (m_Callback.ShouldCancel())
		{
			return *HResult::Abort();
		}

		m_Stream = m_Callback.OnGetStream(index);
		if (m_Stream)
		{
			auto wrapperStream = COM::CreateObject<InStreamWrapper_wxInputStream>(*m_Stream, m_EvtHandler);
			wrapperStream->SpecifyTotalSize(m_Stream->GetLength());
			*inStream = wrapperStream.Detach();

			return *HResult::Success();
		}
		return *HResult::False();
	}
	STDMETHODIMP UpdateArchiveWrapper::SetOperationResult(Int32 operationResult)
	{
		m_Stream = nullptr;

		if (m_Callback.ShouldCancel())
		{
			return *HResult::Abort();
		}
		return *HResult::Success();
	}
}
