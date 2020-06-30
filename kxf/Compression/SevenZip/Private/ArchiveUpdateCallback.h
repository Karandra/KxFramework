#pragma once
#include "../Common.h"
#include "WithEvtHandler.h"
#include "PasswordHandler.h"
#include "kxf/System/COM.h"
#include "kxf/System/ErrorCode.h"
#include "kxf/Compression/IArchive.h"
#include <7zip/CPP/7zip/Archive/IArchive.h>
#include <7zip/CPP/7zip/IPassword.h>
#include <7zip/CPP/7zip/ICoder.h>

#undef True
#undef False

namespace kxf::SevenZip::Private::Callback
{
	class UpdateArchive: public WithEvtHandler, public IArchiveUpdateCallback, public ICompressProgressInfo, public ICryptoGetTextPassword, public ICryptoGetTextPassword2
	{
		private:
			COM::RefCount<UpdateArchive> m_RefCount;
			PasswordHandler m_PasswordHandler;

		protected:
			COMPtr<IInArchive> m_Archive;
			int64_t m_BytesCompleted = 0;
			int64_t m_BytesTotal = 0;

		protected:
			FileItem GetExistingFileInfo(size_t fileIndex) const;
			ArchiveEvent CreateEvent(EventID id = ArchiveEvent::EvtProcess)
			{
				ArchiveEvent event = WithEvtHandler::CreateEvent(id);
				event.SetTotal(m_BytesTotal);
				event.SetProcessed(m_BytesCompleted);

				return event;
			}

		public:
			UpdateArchive(wxEvtHandler* evtHandler = nullptr)
				:WithEvtHandler(evtHandler), m_RefCount(*this)
			{
			}
			virtual ~UpdateArchive() = default;

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
	};
}

namespace kxf::SevenZip::Private::Callback
{
	class UpdateArchiveWrapper: public UpdateArchive
	{
		private:
			IArchiveUpdating& m_Updating;
			Compression::IUpdateCallback& m_Callback;

			InputStreamDelegate m_Stream;

		public:
			UpdateArchiveWrapper(IArchiveUpdating& updating, Compression::IUpdateCallback& callback, wxEvtHandler* evtHandler = nullptr)
				:UpdateArchive(evtHandler), m_Updating(updating), m_Callback(callback)
			{
			}

		public:
			// IArchiveUpdateCallback
			STDMETHOD(GetUpdateItemInfo)(UInt32 index, Int32* newData, Int32* newProperties, UInt32* indexInArchive);
			STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT* value);
			STDMETHOD(GetStream)(UInt32 index, ISequentialInStream** inStream);
			STDMETHOD(SetOperationResult)(Int32 operationResult);
	};
}

namespace kxf::SevenZip::Private::Callback
{
	class UpdateArchiveFromFS: public UpdateArchiveWrapper, public Compression::IUpdateCallback
	{
		protected:
			const IFileSystem& m_FileSystem;

			std::vector<FileItem> m_Files;
			FSPath m_Directory;

		public:
			UpdateArchiveFromFS(IArchiveUpdating& updating, const IFileSystem& fileSystem, std::vector<FileItem> files, FSPath directory, wxEvtHandler* evtHandler = nullptr)
				:UpdateArchiveWrapper(updating, *this, evtHandler), m_FileSystem(fileSystem), m_Files(std::vector(files)), m_Directory(std::move(directory))
			{
			}

		public:
			// IUpdateCallback
			bool ShouldCancel() const override
			{
				return false;
			}

			size_t OnGetUpdateMode(size_t index, bool& updateData, bool& updateProperties) override
			{
				// We don't support updating existing files yet
				updateData = 1;
				updateProperties = 1;
				return Compression::InvalidIndex;
			}
			FileItem OnGetProperties(size_t index) override
			{
				if (index < m_Files.size())
				{
					FileItem item = m_Files[index];
					item.SetFullPath(item.GetFullPath().GetAfter(m_Directory));

					return item;
				}
				return {};
			}
			
			InputStreamDelegate OnGetStream(size_t index) override
			{
				if (index < m_Files.size())
				{
					return m_FileSystem.OpenToRead(m_Files[index].GetFullPath());
				}
				return nullptr;
			}
			bool OnOperationCompleted(size_t index, wxInputStream& stream) override
			{
				return true;
			}
	};
}
