#pragma once
#include "../Common.h"
#include "WithEvtHandler.h"
#include "PasswordHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include "kxf/Compression/IArchive.h"
#include <7zip/CPP/7zip/Archive/IArchive.h>
#include <7zip/CPP/7zip/IPassword.h>

#undef True
#undef False

namespace kxf::SevenZip::Private::Callback
{
	class ExtractArchive: public WithEvtHandler, public IArchiveExtractCallback, public ICryptoGetTextPassword, public ICryptoGetTextPassword2
	{
		private:
			COM::RefCount<ExtractArchive> m_RefCount;
			PasswordHandler m_PasswordHandler;

		protected:
			COMPtr<IInArchive> m_Archive;
			int64_t m_BytesCompleted = 0;
			int64_t m_BytesTotal = -1;

		protected:
			FileItem GetExistingItem(size_t fileIndex) const;
			
			ArchiveEvent CreateEvent(EventID id)
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent(id);
				event.SetProgress(m_BytesCompleted, m_BytesTotal);

				return event;
			}
			bool SendItemEvent(EventID id, FileItem item)
			{
				if (item)
				{
					ArchiveEvent event = CreateEvent(id);
					event.SetItem(std::move(item));
					return WithEvtHandler::SendEvent(event);
				}
				return true;
			}

		public:
			ExtractArchive(wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this), m_PasswordHandler(evtHandler)
			{
			}
			virtual ~ExtractArchive() = default;

		public:
			void SetArchive(COMPtr<IInArchive> archive)
			{
				m_Archive = std::move(archive);
			}

		public:
			// WithEvtHandler
			void SetEvtHandler(wxEvtHandler* evtHandler) noexcept override
			{
				WithEvtHandler::SetEvtHandler(evtHandler);
				m_PasswordHandler.SetEvtHandler(evtHandler);
			}

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

			// IProgress
			STDMETHOD(SetTotal)(UInt64 bytes) override
			{
				// SetTotal is never called for ZIP and 7z formats
				m_BytesTotal = bytes;

				return *HResult::Success();
			}
			STDMETHOD(SetCompleted)(const UInt64* bytes) override
			{
				// For ZIP format SetCompleted only called once per 1000 files in central directory and once per 100 in local ones.
				// For 7Z format SetCompleted is never called.
				m_BytesTotal = bytes ? *bytes : -1;

				return *HResult::Success();
			}

			// ICryptoGetTextPassword
			STDMETHOD(CryptoGetTextPassword)(BSTR* password) override
			{
				return *m_PasswordHandler.OnPasswordRequest(password);
			}

			// ICryptoGetTextPassword2
			STDMETHOD(CryptoGetTextPassword2)(Int32* passwordIsDefined, BSTR* password) override
			{
				return *m_PasswordHandler.OnPasswordRequest(password, passwordIsDefined);
			}
	};
}

namespace kxf::SevenZip::Private::Callback
{
	class ExtractArchiveWrapper: public ExtractArchive
	{
		private:
			const IArchiveExtract& m_Extract;
			Compression::IExtractCallback& m_Callback;

			FileItem m_Item;
			OutputStreamDelegate m_Stream;

		private:
			bool ShouldCancel() const
			{
				return m_Callback.ShouldCancel();
			}

		public:
			ExtractArchiveWrapper(const IArchiveExtract& extract, Compression::IExtractCallback& callback, wxEvtHandler* evtHandler = nullptr)
				:ExtractArchive(evtHandler), m_Callback(callback), m_Extract(extract)
			{
			}

		public:
			// IArchiveExtractCallback
			STDMETHOD(GetStream)(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode) override;
			STDMETHOD(PrepareOperation)(Int32 askExtractMode) override;
			STDMETHOD(SetOperationResult)(Int32 resultEOperationResult) override;
	};
}

namespace kxf::SevenZip::Private::Callback
{
	class ExtractArchiveToFS: public ExtractArchiveWrapper, private Compression::IExtractCallback
	{
		private:
			IFileSystem& m_FileSystem;
			FSPath m_Directory;
			FSPath m_TargetPath;

			bool m_ShouldCancel = false;

		public:
			ExtractArchiveToFS(const IArchiveExtract& extract, IFileSystem& fileSystem, FSPath directory, wxEvtHandler* evtHandler = nullptr)
				:ExtractArchiveWrapper(extract, *this, evtHandler), m_FileSystem(fileSystem), m_Directory(std::move(directory))
			{
			}

		public:
			// IArchiveExtract
			bool ShouldCancel() const override
			{
				return m_ShouldCancel;
			}
			OutputStreamDelegate OnGetStream(const FileItem& item) override;
			bool OnItemDone(const FileItem& item, wxOutputStream& stream) override;

		public:
			virtual bool GetTargetPath(const FileItem& item, FSPath& targetPath) const
			{
				return false;
			}
	};
}

namespace kxf::SevenZip::Private::Callback
{
	class ExtractArchiveToStream: public ExtractArchiveWrapper, private Compression::IExtractCallback
	{
		protected:
			OutputStreamDelegate m_Stream;

		public:
			ExtractArchiveToStream(const IArchiveExtract& extract, OutputStreamDelegate steram, wxEvtHandler* evtHandler = nullptr)
				:ExtractArchiveWrapper(extract, *this, evtHandler), m_Stream(std::move(steram))
			{
			}

		public:
			// IArchiveExtract
			bool ShouldCancel() const override
			{
				return false;;
			}
			OutputStreamDelegate OnGetStream(const FileItem& item) override;
			bool OnItemDone(const FileItem& item, wxOutputStream& stream) override;
	};
}
