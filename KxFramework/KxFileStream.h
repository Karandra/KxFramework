/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStreamWrappers.h"

class KX_API KxFileStream:
	public KxStreamBase,
	public KxIOStreamHelper<KxFileStream>,
	public KxInputStreamWrapper<wxInputStream>,
	public KxOutputStreamWrapper<wxOutputStream>
{
	private:
		struct SAccess
		{
			enum Enum
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
		};
		struct SShare
		{
			enum Enum
			{
				Invalid = -1,

				Exclusive = 0,
				Read = 1 << 0,
				Write = 1 << 1,
				Delete = 1 << 2,

				Everything = Read|Write|Delete
			};
		};
		struct SDisposition
		{
			enum Enum
			{
				Invalid = -1,

				OpenExisting,
				OpenAlways,
				CreateNew,
				CreateAlways,
			};
		};
		struct SFlags
		{
			enum Enum
			{
				None = 0,

				Normal = 1 << 0,
				BackupSemantics = 1 << 1,
			};
		};

	public:
		using Access = SAccess::Enum;
		using Share = SShare::Enum;
		using Disposition = SDisposition::Enum;
		using Flags = SFlags::Enum;

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
		virtual Offset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
		virtual Offset OnSysTell() const;
		virtual size_t OnSysRead(void* buffer, size_t size) override;
		virtual size_t OnSysWrite(const void* buffer, size_t size) override;
		
		bool IsOpened() const;
		bool DoClose();

	public:
		static const Access DefaultAccess = Access::Read;
		static const Share DefaultShare = Share::Read;
		static const Disposition DefaultDisposition = Disposition::OpenExisting;
		static const Flags DefaultFlags = Flags::Normal;

	public:
		KxFileStream();
		KxFileStream(HANDLE fileHandle, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags);
		KxFileStream(const wxString& filePath, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags);
		KxFileStream(const char* filePath, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags)
			:KxFileStream(wxString(filePath), accessMode, disposition, shareMode, flags)
		{
		}
		KxFileStream(const wchar_t* filePath, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags)
			:KxFileStream(wxString(filePath), accessMode, disposition, shareMode, flags)
		{
		}
		virtual ~KxFileStream();
		
	public:
		bool Open(HANDLE fileHandle, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags);
		bool Open(const wxString& filePath, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags);
		bool Open(const char* filePath, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags)
		{
			return Open(wxString(filePath), accessMode, disposition, shareMode, flags);
		}
		bool Open(const wchar_t* filePath, int accessMode = DefaultAccess, Disposition disposition = DefaultDisposition, int shareMode = DefaultShare, int flags = DefaultFlags)
		{
			return Open(wxString(filePath), accessMode, disposition, shareMode, flags);
		}
		
	public:
		virtual bool IsOk() const override;
		virtual bool Close() override;
		virtual bool Eof() const override;
		virtual bool CanRead() const override;
		
		virtual size_t GetSize() const override;
		virtual Offset GetLength() const override;

		virtual bool IsSeekable() const override;
		virtual Offset SeekI(wxFileOffset offset, wxSeekMode mode = wxFromCurrent) override;
		virtual Offset SeekO(wxFileOffset offset, wxSeekMode mode = wxFromCurrent) override;
		virtual Offset TellI() const override;
		virtual Offset TellO() const override;
		Offset Tell() const;
		Offset Seek(Offset offset, SeekMode mode = SeekMode::FromCurrent);

		virtual char Peek();
		virtual KxFileStream& Read(void* buffer, size_t size) override
		{
			ReadBuffer(buffer, size);
			return *this;
		}
		virtual KxFileStream& Write(const void* buffer, size_t size) override
		{
			WriteBuffer(buffer, size);
			return *this;
		}

		virtual size_t LastRead() const override;
		virtual size_t LastWrite() const override;
		virtual wxStreamError GetLastError() const;

		virtual bool IsWriteable() const override;
		virtual bool IsReadable() const override;

		virtual bool Flush() override;
		virtual bool SetAllocationSize(Offset offset = InvalidOffset) override;
		wxString GetFileName() const;
		HANDLE GetHandle() const;

	public:
		operator wxStreamBase*();
		operator const wxStreamBase*();

	public:
		wxDECLARE_ABSTRACT_CLASS(KxFileStream);
};
