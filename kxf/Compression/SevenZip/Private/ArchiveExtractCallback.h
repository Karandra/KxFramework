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

		protected:
			FileItem GetFileInfo(size_t fileIndex) const;

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
			STDMETHOD(SetTotal)(UInt64 size) override;
			STDMETHOD(SetCompleted)(const UInt64* completeValue) override;

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

namespace kxf::SevenZip::Private::Callback
{
	class ExtractArchiveWrapper: public ExtractArchive
	{
		protected:
			const IArchiveExtraction& m_Extraction;
			IExtractionCallback& m_Callback;

			OutputStreamDelegate m_Stream;
			size_t m_FileIndex = Compression::InvalidIndex;

		protected:
			bool ShouldCancel() const
			{
				return m_Callback.ShouldCancel();
			}

		public:
			ExtractArchiveWrapper(const IArchiveExtraction& extraction, IExtractionCallback& callback, wxEvtHandler* evtHandler = nullptr)
				:ExtractArchive(evtHandler), m_Callback(callback), m_Extraction(extraction)
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
	class ExtractArchiveToFS: public ExtractArchive
	{
		protected:
			const IArchiveExtraction& m_Extraction;
			FSPath m_Directory;
			IFileSystem& m_FileSystem;

			FSPath m_TargetPath;
			FileItem m_FileInfo;
			OutputStreamDelegate m_Stream;

		public:
			ExtractArchiveToFS() = default;
			ExtractArchiveToFS(const IArchiveExtraction& extraction, IFileSystem& fileSystem, FSPath directory, wxEvtHandler* evtHandler = nullptr)
				:ExtractArchive(evtHandler), m_Extraction(extraction), m_Directory(std::move(directory)), m_FileSystem(fileSystem)
			{
			}

		public:
			// IArchiveExtractCallback
			STDMETHOD(GetStream)(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode) override;
			STDMETHOD(PrepareOperation)(Int32 askExtractMode) override;
			STDMETHOD(SetOperationResult)(Int32 resultEOperationResult) override;

		public:
			virtual HResult GetTargetPath(uint32_t fileIndex, const FileItem& fileInfo, FSPath& targetPath) const
			{
				return HResult::False();
			}
	};
}

namespace kxf::SevenZip::Private::Callback
{
	class ExtractArchiveToStream: public ExtractArchive
	{
		protected:
			const IArchiveExtraction& m_Extraction;
			OutputStreamDelegate m_Stream;

		public:
			ExtractArchiveToStream() = default;
			ExtractArchiveToStream(const IArchiveExtraction& extraction, OutputStreamDelegate steram, wxEvtHandler* evtHandler = nullptr)
				:ExtractArchive(evtHandler), m_Extraction(extraction), m_Stream(std::move(steram))
			{
			}

		public:
			// IArchiveExtractCallback
			STDMETHOD(GetStream)(UInt32 fileIndex, ISequentialOutStream** outStream, Int32 askExtractMode) override;
			STDMETHOD(PrepareOperation)(Int32 askExtractMode) override;
			STDMETHOD(SetOperationResult)(Int32 resultEOperationResult) override;
	};
}
