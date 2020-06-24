#pragma once
#include "../Common.h"
#include "WithEvtHandler.h"
#include "PasswordHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include <7zip/CPP/7zip/Archive/IArchive.h>
#include <7zip/CPP/7zip/IPassword.h>
#include <7zip/CPP/7zip/ICoder.h>

namespace kxf::SevenZip::Private::Callback
{
	class UpdateArchive: public WithEvtHandler, public IArchiveUpdateCallback, public ICompressProgressInfo, public ICryptoGetTextPassword, public ICryptoGetTextPassword2
	{
		private:
			COM::RefCount<UpdateArchive> m_RefCount;
			PasswordHandler m_PasswordHandler;

		protected:
			FSPath m_DirectoryPrefix;
			FSPath m_OutputPath;
			const std::vector<FSPath>& m_TargetPaths;
			const std::vector<FileItem>& m_SourcePaths;
			
			size_t m_ExistingItemsCount = 0;
			int64_t m_BytesCompleted = 0;
			int64_t m_BytesTotal = 0;

		protected:
			ArchiveEvent CreateEvent(EventID id = ArchiveEvent::EvtProcess)
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent(id);
				event.SetTotal(m_BytesTotal);
				event.SetProcessed(m_BytesCompleted);
				event.SetDestination(m_OutputPath);

				return event;
			}

		public:
			UpdateArchive(FSPath directoryPrefix,
						  const std::vector<FileItem>& filePaths,
						  const std::vector<FSPath>& inArchiveFilePaths,
						  FSPath outputPath,
						  wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler),
				m_RefCount(*this),
				m_DirectoryPrefix(std::move(directoryPrefix)),
				m_SourcePaths(filePaths),
				m_TargetPaths(inArchiveFilePaths),
				m_OutputPath(std::move(outputPath))
			{
			}
			virtual ~UpdateArchive() = default;

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

			// IProgress
			STDMETHOD(SetTotal)(UInt64 bytes);
			STDMETHOD(SetCompleted)(const UInt64* bytes);

			// IArchiveUpdateCallback
			STDMETHOD(GetUpdateItemInfo)(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive);
			STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT* value);
			STDMETHOD(GetStream)(UInt32 index, ISequentialInStream** inStream);
			STDMETHOD(SetOperationResult)(Int32 operationResult);

			// ICompressProgressInfo
			STDMETHOD(SetRatioInfo)(const UInt64* inSize, const UInt64* outSize)
			{
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

		public:
			void SetExistingItemsCount(size_t existingItemsCount)
			{
				m_ExistingItemsCount = existingItemsCount;
			}
	};
}
