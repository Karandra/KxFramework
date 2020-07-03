#include "stdafx.h"
#include "ArchiveUpdateCallback.h"
#include "InStreamWrapper.h"
#include "Utility.h"
#include "kxf/FileSystem/Private/NativeFSUtility.h"
#include "kxf/System/VariantProperty.h"

namespace kxf::SevenZip::Private::Callback
{
	FileItem UpdateArchive::GetExistingItem(size_t fileIndex) const
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
			auto DoGetProperty = [](PROPID propID, const FileItem& fileItem) -> VariantProperty
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

			m_Item = m_Callback.OnGetProperties(index);
			if (m_Item)
			{
				VariantProperty property = DoGetProperty(propID, m_Item);
				if (property)
				{
					return *property.CopyToNative(*value);
				}
			}
			return *HResult::InvalidArgument();
		}
		return *HResult::InvalidPointer();
	}
	STDMETHODIMP UpdateArchiveWrapper::GetStream(UInt32 index, ISequentialInStream** inStream)
	{
		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (!inStream)
		{
			return *HResult::InvalidPointer();
		}
		if (m_Callback.ShouldCancel())
		{
			return *HResult::Abort();
		}

		*inStream = nullptr;
		m_Stream = nullptr;
		if (m_Item)
		{
			if (m_EvtHandler && !SendItemEvent(IArchiveUpdate::EvtItem, m_Item))
			{
				return *HResult::Abort();
			}

			m_Stream = m_Callback.OnGetStream(m_Item);
			if (m_Stream)
			{
				if (ShouldCancel())
				{
					return *HResult::Abort();
				}

				auto wrapperStream = COM::CreateLocalInstance<InStreamWrapper_wxInputStream>(*m_Stream, m_EvtHandler);
				wrapperStream->SpecifyTotalSize(std::max(m_Stream->GetLength(), m_Item.GetSize().GetBytes()));
				*inStream = wrapperStream.Detach();

				return *HResult::Success();
			}
			return *HResult::False();
		}
		return *HResult::Fail();
	}
	STDMETHODIMP UpdateArchiveWrapper::SetOperationResult(Int32 operationResult)
	{
		InputStreamDelegate stream = std::move(m_Stream);

		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (operationResult == NArchive::NExtract::NOperationResult::kOK)
		{
			if (stream && !m_Callback.OnItemDone(m_Item, *stream))
			{
				return *HResult::Fail();
			}
			if (m_EvtHandler && !SendItemEvent(IArchiveUpdate::EvtItemDone, std::move(m_Item)))
			{
				return *HResult::Abort();
			}
		}
		return *HResult::Success();
	}
}

namespace kxf::SevenZip::Private::Callback
{
	size_t UpdateArchiveFromFS::OnGetUpdateMode(size_t index, bool& updateData, bool& updateProperties)
	{
		// We don't support updating existing files yet
		updateData = true;
		updateProperties = true;
		return Compression::InvalidIndex;
	}
	FileItem UpdateArchiveFromFS::OnGetProperties(size_t index)
	{
		if (index < m_Files.size())
		{
			FileItem item = m_Files[index];
			item.SetFullPath(item.GetFullPath().GetAfter(m_Directory));

			return item;
		}
		return {};
	}
	InputStreamDelegate UpdateArchiveFromFS::OnGetStream(const FileItem& item)
	{
		size_t index = item.GetUniqueID().ToLocallyUniqueID().ToInt();
		if (index < m_Files.size())
		{
			return m_FileSystem.OpenToRead(m_Files[index].GetFullPath());
		}
		return nullptr;
	}
	bool UpdateArchiveFromFS::OnItemDone(const FileItem& item, wxInputStream& stream)
	{
		return true;
	}
}
