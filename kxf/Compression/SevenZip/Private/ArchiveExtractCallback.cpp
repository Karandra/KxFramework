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
	STDMETHODIMP ExtractArchiveWrapper::GetStream(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode)
	{
		m_Stream = nullptr;
		m_FileIndex = Compression::InvalidIndex;

		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
		{
			return *HResult::Success();
		}

		if (m_Stream = m_Callback.OnGetStream(fileIndex))
		{
			m_FileIndex = fileIndex;
			*outStream = CreateObject<OutStreamWrapper_wxOutputStream>(*m_Stream, m_EvtHandler).Detach();

			if (m_EvtHandler)
			{
				if (FileItem fileItem = GetFileInfo(fileIndex))
				{
					ArchiveEvent event = CreateEvent();
					event.SetItem(fileItem);
					if (!SendEvent(event))
					{
						return *HResult::Abort();
					}
				}
			}
		}
		return ShouldCancel() ? *HResult::Abort() : *HResult::Success();
	}
	STDMETHODIMP ExtractArchiveWrapper::PrepareOperation(Int32 askExtractMode)
	{
		return ShouldCancel() ? *HResult::Abort() : *HResult::Success();
	}
	STDMETHODIMP ExtractArchiveWrapper::SetOperationResult(Int32 operationResult)
	{
		OutputStreamDelegate stream = std::move(m_Stream);

		if (ShouldCancel())
		{
			return *HResult::Abort();
		}
		if (stream && !m_Callback.OnOperationCompleted(m_FileIndex, *stream))
		{
			return *HResult::Fail();
		}
		return *HResult::Success();
	}
}

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP ExtractArchiveToFS::GetStream(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode)
	{
		m_Stream = nullptr;
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
					return *HResult::Unexpected();
				}

				if (m_FileInfo.IsDirectory())
				{
					// Creating the directory here supports having empty directories
					m_FileSystem.CreateDirectory(m_TargetPath);
					*outStream = nullptr;

					return *HResult::Success();
				}

				m_FileSystem.CreateDirectory(m_TargetPath);
				m_Stream = m_FileSystem.OpenToWrite(m_TargetPath);
				if (!m_Stream)
				{
					return *Win32Error::GetLastError().ToHResult().value_or(HResult::Fail());
				}

				auto wrapperStream = CreateObject<OutStreamWrapper_wxOutputStream>(*m_Stream, m_EvtHandler);
				wrapperStream->SetSize(m_FileInfo.GetSize().GetBytes());
				*outStream = wrapperStream.Detach();

				if (m_EvtHandler)
				{
					ArchiveEvent event = CreateEvent();
					event.SetItem(m_FileInfo);
					return SendEvent(event) ? *HResult::Success() : *HResult::Abort();
				}
				return *HResult::Success();
			}
		}
		return *HResult::Fail();
	}
	STDMETHODIMP ExtractArchiveToFS::PrepareOperation(Int32 askExtractMode)
	{
		m_TargetPath = {};
		return *HResult::Success();
	}
	STDMETHODIMP ExtractArchiveToFS::SetOperationResult(Int32 operationResult)
	{
		OutputStreamDelegate stream = std::move(m_Stream);
		if (stream)
		{
			if (stream->IsKindOf(wxCLASSINFO(FileStream)))
			{
				FileStream& fileStream = static_cast<FileStream&>(*stream);
				fileStream.ChangeTimestamp(m_FileInfo.GetCreationTime(), m_FileInfo.GetModificationTime(), m_FileInfo.GetLastAccessTime());
				fileStream.SetAttributes(m_FileInfo.GetAttributes());

				return *HResult::Success();
			}

			m_FileSystem.ChangeAttributes(m_TargetPath, m_FileInfo.GetAttributes());
			m_FileSystem.ChangeTimestamp(m_TargetPath, m_FileInfo.GetCreationTime(), m_FileInfo.GetModificationTime(), m_FileInfo.GetLastAccessTime());
		}
		return *HResult::Success();
	}
}

namespace kxf::SevenZip::Private::Callback
{
	STDMETHODIMP ExtractArchiveToStream::GetStream(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode)
	{
		FileItem fileItem = GetFileInfo(fileIndex);
		if (fileItem && !fileItem.IsDirectory())
		{
			auto wrapperStream = CreateObject<OutStreamWrapper_wxOutputStream>(*m_Stream, m_EvtHandler);
			wrapperStream->SetSize(fileItem.GetSize().GetBytes());
			*outStream = wrapperStream.Detach();

			if (m_EvtHandler)
			{
				ArchiveEvent event = CreateEvent();
				event.SetItem(std::move(fileItem));
				return SendEvent(event) ? *HResult::Success() : *HResult::Abort();
			}
			return *HResult::Success();
		}
		return *HResult::Fail();
	}
	STDMETHODIMP ExtractArchiveToStream::PrepareOperation(Int32 askExtractMode)
	{
		return *HResult::Success();
	}
	STDMETHODIMP ExtractArchiveToStream::SetOperationResult(Int32 operationResult)
	{
		m_Stream = nullptr;
		return *HResult::Success();
	}
}
