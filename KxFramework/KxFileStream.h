#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStreamWrappers.h"
#include "KxFramework/KxEnumClassOperations.h"

class KX_API KxFileStream:
	public KxStreamBase,
	public KxIOStreamHelper<KxFileStream>,
	public KxInputStreamWrapper<wxInputStream>,
	public KxOutputStreamWrapper<wxOutputStream>
{
	public:
		enum class Access
		{
			Invalid = -1,

			None = 0,
			Read = 1 << 0,
			Write = 1 << 1,
			ReadAttributes = 1 << 2,
			WriteAttributes = 1 << 2,

			RW = Read|Write,
			AllAccess = RW|ReadAttributes|WriteAttributes
		};
		enum class Share
		{
			Invalid = -1,

			Exclusive = 0,
			Read = 1 << 0,
			Write = 1 << 1,
			Delete = 1 << 2,

			Everything = Read|Write|Delete
		};
		enum class Disposition
		{
			Invalid = -1,

			OpenExisting,
			OpenAlways,
			CreateNew,
			CreateAlways,
		};
		enum class Flags
		{
			None = 0,

			Normal = 1 << 0,
			BackupSemantics = 1 << 1,
		};

	private:
		HANDLE m_Handle = INVALID_HANDLE_VALUE;
		Access m_AccessMode = DefaultAccess;
		Share m_ShareMode = DefaultShare;
		Disposition m_Disposition = DefaultDisposition;
		Flags m_Flags = DefaultFlags;
		mutable wxString m_FilePath;

		size_t m_LastRead = 0;
		size_t m_LastWrite = 0;
		Offset m_Position = 0;
		ErrorCode m_LastError = ErrorCode::Success;

	private:
		void SetLastStreamError(DWORD error, bool isWrite);

	protected:
		Offset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override;
		Offset OnSysTell() const override;
		size_t OnSysRead(void* buffer, size_t size) override;
		size_t OnSysWrite(const void* buffer, size_t size) override;
		
		bool IsOpened() const;
		bool DoClose();

	public:
		static constexpr Access DefaultAccess = Access::Read;
		static constexpr Share DefaultShare = Share::Read;
		static constexpr Disposition DefaultDisposition = Disposition::OpenExisting;
		static constexpr Flags DefaultFlags = Flags::Normal;

	public:
		KxFileStream();
		KxFileStream(HANDLE fileHandle, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags);
		KxFileStream(const wxString& filePath, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags);
		KxFileStream(const char* filePath, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags)
			:KxFileStream(wxString(filePath), accessMode, disposition, shareMode, flags)
		{
		}
		KxFileStream(const wchar_t* filePath, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags)
			:KxFileStream(wxString(filePath), accessMode, disposition, shareMode, flags)
		{
		}
		virtual ~KxFileStream();
		
	public:
		bool Open(HANDLE fileHandle, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags);
		bool Open(const wxString& filePath, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags);
		bool Open(const char* filePath, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags)
		{
			return Open(wxString(filePath), accessMode, disposition, shareMode, flags);
		}
		bool Open(const wchar_t* filePath, Access accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, Share shareMode = DefaultShare, Flags flags = DefaultFlags)
		{
			return Open(wxString(filePath), accessMode, disposition, shareMode, flags);
		}
		
	public:
		bool IsOk() const override;
		bool Close() override;
		bool Eof() const override;
		bool CanRead() const override;
		
		size_t GetSize() const override;
		Offset GetLength() const override;

		bool IsSeekable() const override;
		Offset SeekI(wxFileOffset offset, wxSeekMode mode = wxFromCurrent) override;
		Offset SeekO(wxFileOffset offset, wxSeekMode mode = wxFromCurrent) override;
		Offset TellI() const override;
		Offset TellO() const override;
		Offset Tell() const;
		Offset Seek(Offset offset, SeekMode mode = SeekMode::FromCurrent);

		char Peek() override;
		KxFileStream& Read(void* buffer, size_t size) override
		{
			ReadBuffer(buffer, size);
			return *this;
		}
		KxFileStream& Read(wxOutputStream& stream)
		{
			wxInputStream::Read(stream);
			return *this;
		}

		KxFileStream& Write(const void* buffer, size_t size) override
		{
			WriteBuffer(buffer, size);
			return *this;
		}
		KxFileStream& Write(wxInputStream& stream)
		{
			wxOutputStream::Write(stream);
			return *this;
		}

		size_t LastRead() const override;
		size_t LastWrite() const override;
		wxStreamError GetLastError() const;

		bool IsWriteable() const override;
		bool IsReadable() const override;

		bool Flush() override;
		bool SetAllocationSize(Offset offset = InvalidOffset) override;
		wxString GetFileName() const;
		HANDLE GetHandle() const;

	public:
		operator wxStreamBase*();
		operator const wxStreamBase*();

	public:
		wxDECLARE_ABSTRACT_CLASS(KxFileStream);
};

namespace KxEnumClassOperations
{
	KxImplementEnum(KxFileStream::Access);
	KxImplementEnum(KxFileStream::Share);
	KxAllowEnumCastOp(KxFileStream::Disposition);
	KxImplementEnum(KxFileStream::Flags);
}
