#pragma once
#include "Common.h"
#include "FSPath.h"
#include "Kx/General/StreamWrappers.h"
#include <wx/stream.h>
#include <wx/datetime.h>

namespace KxFramework
{
	class KX_API FileStream:
		public IStreamWrapper,
		public InputStreamWrapper<wxInputStream>,
		public OutputStreamWrapper<wxOutputStream>,
		public IOStreamWrapper<FileStream>
	{
		private:
			void* m_Handle = nullptr;
			FileStreamAccess m_AccessMode = FileStreamAccess::None;
			FileStreamShare m_ShareMode = FileStreamShare::Read;
			FileStreamDisposition m_Disposition = FileStreamDisposition::OpenExisting;
			FileStreamFlags m_Flags = FileStreamFlags::None;

			wxFileOffset m_Position = 0;
			StreamErrorCode m_LastError = StreamErrorCode::Success;
			size_t m_LastRead = 0;
			size_t m_LastWrite = 0;

		protected:
			wxFileOffset OnSysTell() const override;
			wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
			size_t OnSysRead(void* buffer, size_t size) override;
			size_t OnSysWrite(const void* buffer, size_t size) override;
			
			void DoSetLastError(uint32_t error, bool isWrite);
			bool DoIsOpened() const;
			bool DoClose();

		public:
			FileStream()
			{
				DoSetLastError(0, false);
			}
			FileStream(const FSPath& path,
					   FileStreamAccess access = FileStreamAccess::Read,
					   FileStreamDisposition disposition = FileStreamDisposition::OpenExisting,
					   FileStreamShare share = FileStreamShare::Read,
					   FileStreamFlags flags = FileStreamFlags::None
			)
				:FileStream()
			{
				Open(path, access, disposition, share, flags);
			}
			FileStream(const FileStream&) = delete;
			FileStream(FileStream&&) noexcept = delete;
			~FileStream()
			{
				DoClose();
			}
			
		public:
			void* GetHandle() const
			{
				return m_Handle;
			}
			FSPath GetFileSystemPath() const;

			bool AttachHandle(void* handle);
			bool AttachHandle(void* handle, FileStreamAccess access = FileStreamAccess::Read,
							  FileStreamDisposition disposition = FileStreamDisposition::OpenExisting,
							  FileStreamShare share = FileStreamShare::Read,
							  FileStreamFlags flags = FileStreamFlags::None
			);
			void* DetachHandle();

			bool Open(const FSPath& path, FileStreamAccess access = FileStreamAccess::Read,
					  FileStreamDisposition disposition = FileStreamDisposition::OpenExisting,
					  FileStreamShare share = FileStreamShare::Read,
					  FileStreamFlags flags = FileStreamFlags::None
			);
			bool Close() override
			{
				return DoClose();
			}

			bool IsOk() const override
			{
				return DoIsOpened();
			}
			bool Eof() const override;
			bool CanRead() const override;
			size_t GetSize() const override;
			wxFileOffset GetLength() const override;

			bool IsSeekable() const override;
			wxFileOffset SeekI(wxFileOffset offset, wxSeekMode mode = wxFromCurrent) override;
			wxFileOffset SeekO(wxFileOffset offset, wxSeekMode mode = wxFromCurrent) override;
			wxFileOffset TellI() const override;
			wxFileOffset TellO() const override;

			FileStream& Read(void* buffer, size_t size) override
			{
				wxInputStream::Read(buffer, size);
				return *this;
			}
			FileStream& Read(wxOutputStream& stream)
			{
				wxInputStream::Read(stream);
				return *this;
			}

			FileStream& Write(const void* buffer, size_t size) override
			{
				wxOutputStream::Write(buffer, size);
				return *this;
			}
			FileStream& Write(wxInputStream& stream)
			{
				wxOutputStream::Write(stream);
				return *this;
			}

			size_t LastRead() const override
			{
				return m_LastRead;
			}
			size_t LastWrite() const override
			{
				return m_LastWrite;
			}
			wxStreamError GetLastError() const
			{
				return static_cast<wxStreamError>(m_LastError);
			}

			bool IsWriteable() const override
			{
				return m_AccessMode & FileStreamAccess::Write;
			}
			bool IsReadable() const override
			{
				return m_AccessMode & FileStreamAccess::Read;
			}

			bool Flush() override;
			bool SetAllocationSize(BinarySize offset = {}) override;

			FileAttribute GetAttributes() const;
			bool SetAttributes(FileAttribute attributes);

			bool GetTimestamp(wxDateTime& creationTime, wxDateTime& modificationTime, wxDateTime& lastAccessTime) const;
			bool ChangeTimestamp(const wxDateTime& creationTime, const wxDateTime& modificationTime, const wxDateTime& lastAccessTime);

		public:
			explicit operator bool() const
			{
				return IsOk();
			}
			bool operator!() const
			{
				return !IsOk();
			}

			FileStream& operator=(const FileStream&) = delete;
			FileStream& operator=(FileStream&&) noexcept = delete;

		public:
			wxDECLARE_ABSTRACT_CLASS(FileStream);
	};
}
