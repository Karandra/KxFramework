#pragma once
#include "../Common.h"
#include "WithEvtHandler.h"
#include "PasswordHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include <7zip/CPP/7zip/Archive/IArchive.h>
#include <7zip/CPP/7zip/IPassword.h>

#undef True
#undef False

namespace kxf::SevenZip::Private::Callback
{
	class Extractor: public WithEvtHandler, public IArchiveExtractCallback, public ICryptoGetTextPassword, public ICryptoGetTextPassword2
	{
		private:
			COM::RefCount<Extractor> m_RefCount;
			PasswordHandler m_PasswordHandler;

		protected:
			COMPtr<IInArchive> m_Archive;

		protected:
			FileItem GetFileInfo(size_t fileIndex) const;

		public:
			Extractor(wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this), m_PasswordHandler(evtHandler)
			{
			}
			virtual ~Extractor() = default;

		public:

			void SetArchive(const COMPtr<IInArchive>& archive)
			{
				m_Archive = archive;
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
	class FileExtractor: public Extractor
	{
		protected:
			FSPath m_Directory;
			FSPath m_TargetPath;
			FileItem m_FileInfo;

			int64_t m_BytesCompleted = 0;
			size_t m_ItemCount = 0;

		public:
			FileExtractor() = default;
			FileExtractor(FSPath directory, wxEvtHandler* evtHandler = nullptr)
				:Extractor(evtHandler), m_Directory(std::move(directory))
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
