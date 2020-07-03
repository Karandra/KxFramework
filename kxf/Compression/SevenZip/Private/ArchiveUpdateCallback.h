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

		private:
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
				m_BytesTotal = bytes;
				return *HResult::Success();
			}
			STDMETHOD(SetCompleted)(const UInt64* bytes) override
			{
				m_BytesCompleted = bytes ? *bytes : 0;
				return *HResult::Success();
			}

			// ICompressProgressInfo
			STDMETHOD(SetRatioInfo)(const UInt64* inSize, const UInt64* outSize) override
			{
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
	class UpdateArchiveWrapper: public UpdateArchive
	{
		private:
			IArchiveUpdate& m_Update;
			Compression::IUpdateCallback& m_Callback;

			FileItem m_Item;
			InputStreamDelegate m_Stream;

		private:
			bool ShouldCancel() const
			{
				return m_Callback.ShouldCancel();
			}

		public:
			UpdateArchiveWrapper(IArchiveUpdate& update, Compression::IUpdateCallback& callback, wxEvtHandler* evtHandler = nullptr)
				:UpdateArchive(evtHandler), m_Update(update), m_Callback(callback)
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
	class UpdateArchiveFromFS: public UpdateArchiveWrapper, private Compression::IUpdateCallback
	{
		protected:
			const IFileSystem& m_FileSystem;

			std::vector<FileItem> m_Files;
			FSPath m_Directory;

		public:
			UpdateArchiveFromFS(IArchiveUpdate& update, const IFileSystem& fileSystem, std::vector<FileItem> files, FSPath directory, wxEvtHandler* evtHandler = nullptr)
				:UpdateArchiveWrapper(update, *this, evtHandler), m_FileSystem(fileSystem), m_Files(std::vector(files)), m_Directory(std::move(directory))
			{
			}

		public:
			// IUpdateCallback
			bool ShouldCancel() const override
			{
				return false;
			}

			size_t OnGetUpdateMode(size_t index, bool& updateData, bool& updateProperties) override;
			FileItem OnGetProperties(size_t index) override;

			InputStreamDelegate OnGetStream(const FileItem& item) override;
			bool OnItemDone(const FileItem& item, wxInputStream& stream) override;
	};
}
