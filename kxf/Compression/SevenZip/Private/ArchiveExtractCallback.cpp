#include "stdafx.h"
#include "ArchiveExtractCallback.h"
#include "OutStreamWrapper.h"
#include "Utility.h"
#include "kxf/System/VariantProperty.h"
#include "kxf/FileSystem/NativeFileSystem.h"
#include "kxf/FileSystem/FileStream.h"

namespace kxf::SevenZip::Private::Callback
{
	FileItem ExtractArchive::GetFileInfo(size_t fileIndex) const
	{
		return GetArchiveItem(m_Archive, fileIndex);
	}

	STDMETHODIMP ExtractArchive::QueryInterface(const ::IID& iid, void** ppvObject)
	{
		if (iid == __uuidof(IUnknown))
		{
			*ppvObject = reinterpret_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		if (iid == IID_IArchiveExtractCallback)
		{
			*ppvObject = static_cast<IArchiveExtractCallback*>(this);
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

	STDMETHODIMP ExtractArchive::SetTotal(UInt64 size)
	{
		// SetTotal is never called for ZIP and 7z formats
		return S_OK;
	}
	STDMETHODIMP ExtractArchive::SetCompleted(const UInt64* completeValue)
	{
		// For ZIP format SetCompleted only called once per 1000 files in central directory and once per 100 in local ones.
		// For 7Z format SetCompleted is never called.
		return S_OK;
	}
}

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP ExtractArchiveToDisk::GetStream(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode)
	{
		m_TargetPath = {};
		m_FileInfo = GetFileInfo(fileIndex);

		if (m_FileInfo)
		{
			HResult hr = GetTargetPath(fileIndex, m_FileInfo, m_TargetPath);
			if (hr.IsFalse())
			{
				m_TargetPath = m_Directory / m_FileInfo.GetFullPath();
			}

			if (hr)
			{
				if (!m_TargetPath)
				{
					return E_UNEXPECTED;
				}

				// Increment counters and call events
				m_BytesCompleted += m_FileInfo.GetSize().GetBytes();
				m_ItemCount++;

				if (m_FileInfo.IsDirectory())
				{
					// Creating the directory here supports having empty directories
					NativeFileSystem().CreateDirectory(m_TargetPath);
					*outStream = nullptr;

					return S_OK;
				}

				NativeFileSystem().CreateDirectory(m_TargetPath);
				auto fileStream = OpenFileToWrite(m_TargetPath);
				if (!fileStream)
				{
					return *Win32Error::GetLastError().ToHResult().value_or(E_FAIL);
				}

				auto wrapperStream = CreateObject<OutStreamWrapper_IStream>(fileStream, m_EvtHandler);
				wrapperStream->SetSize(m_FileInfo.GetSize().GetBytes());
				*outStream = wrapperStream.Detach();

				if (m_EvtHandler)
				{
					ArchiveEvent event = CreateEvent();
					event.SetItem(m_FileInfo);
					return SendEvent(event) ? S_OK : E_ABORT;
				}
				return S_OK;
			}
		}
		return E_FAIL;
	}
	STDMETHODIMP ExtractArchiveToDisk::PrepareOperation(Int32 askExtractMode)
	{
		m_TargetPath = {};
		return S_OK;
	}
	STDMETHODIMP ExtractArchiveToDisk::SetOperationResult(Int32 operationResult)
	{
		if (m_FileInfo)
		{
			FileStream stream(m_TargetPath, FileStreamAccess::WriteAttributes, FileStreamDisposition::OpenExisting, FileStreamShare::Everything, FileStreamFlags::BackupSemantics);
			if (stream)
			{
				stream.ChangeTimestamp(m_FileInfo.GetCreationTime(), m_FileInfo.GetModificationTime(), m_FileInfo.GetLastAccessTime());
				stream.SetAttributes(m_FileInfo.GetAttributes());
			}
		}
		return S_OK;
	}
}
