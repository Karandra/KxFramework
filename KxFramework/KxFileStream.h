#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"

enum KxFileStreamSeek
{
	KxFS_SEEK_INVALID = -1,
	KxFS_SEEK_MIN = KxFS_SEEK_INVALID,

	KxFS_SEEK_BEGIN,
	KxFS_SEEK_CURRENT,
	KxFS_SEEK_END,

	KxFS_SEEK_MAX,
};
enum KxFileStreamAccess
{
	KxFS_ACCESS_INVALID = -1,

	KxFS_ACCESS_NONE = 0,
	KxFS_ACCESS_READ = 1 << 0,
	KxFS_ACCESS_WRITE = 1 << 1,
	KxFS_ACCESS_READ_ATTRIBUTES = 1 << 2,
	KxFS_ACCESS_WRITE_ATTRIBUTES = 1 << 2,

	KxFS_ACCESS_RW = KxFS_ACCESS_READ|KxFS_ACCESS_WRITE,
	KxFS_ACCESS_ALL = KxFS_ACCESS_RW|KxFS_ACCESS_READ_ATTRIBUTES|KxFS_ACCESS_WRITE_ATTRIBUTES
};
enum KxFileStreamShare
{
	KxFS_SHARE_INVALID = -1,

	KxFS_SHARE_EXCLUSIVE = 0,
	KxFS_SHARE_READ = 1 << 0,
	KxFS_SHARE_WRITE = 1 << 1,
	KxFS_SHARE_DELETE = 1 << 2,

	KxFS_SHARE_ALL = KxFS_SHARE_READ|KxFS_SHARE_WRITE|KxFS_SHARE_DELETE
};
enum KxFileStreamDisposition
{
	KxFS_DISP_INVALID = -1,

	KxFS_DISP_OPEN_EXISTING,
	KxFS_DISP_OPEN_ALWAYS,
	KxFS_DISP_CREATE_NEW,
	KxFS_DISP_CREATE_ALWAYS,
};
enum KxFileStreamFlags
{
	KxFS_FLAG_NONE = 0,

	KxFS_FLAG_NORMAL = 1 << 0,
	KxFS_FLAG_BACKUP_SEMANTICS = 1 << 1,
};

class KxFileStream: public virtual wxInputStream, public virtual wxOutputStream
{
	public:
		static wxString GetFileNameByHandle(HANDLE fileHandle);
		static wxFileOffset GetFileSizeByHandle(HANDLE fileHandle);

		static wxStreamError TranslateErrorCode(DWORD error, bool isWrite);
		static DWORD SeekModeToNative(KxFileStreamSeek mode);
		static DWORD AccessModeToNative(int mode);
		static DWORD ShareModeToNative(int mode);
		static DWORD DispositionToNative(KxFileStreamDisposition mode);
		static DWORD FlagsToNative(int flags);
		static KxFileStreamSeek WxSeekModeToKx(wxSeekMode mode);

	private:
		HANDLE m_Handle = INVALID_HANDLE_VALUE;
		KxFileStreamAccess m_AccessMode = DefaultAccessMode;
		KxFileStreamShare m_ShareMode = DefaultShareMode;
		KxFileStreamDisposition m_Disposition = DefaultDisposition;
		KxFileStreamFlags m_Flags = DefaultFlags;
		mutable wxString m_FilePath;

		DWORD m_LastRead = 0;
		DWORD m_LastWrite = 0;
		wxFileOffset m_Position = 0;
		wxStreamError m_LastError = wxSTREAM_NO_ERROR;

	private:
		template <class CharType> wxFileOffset ReadToNullChar() const
		{
			wxFileOffset readed = 0;
			bool isReadSuccess = false;
			CharType c = NULL;

			do
			{
				c = NULL;
				c = ReadObject<CharType>(&isReadSuccess);
				readed += LastRead();
			}
			while (isReadSuccess && c != NULL);
			return readed;
		}
		void SetError(DWORD error, bool isWrite);

	protected:
		virtual wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
		virtual wxFileOffset OnSysTell() const;
		virtual size_t OnSysRead(void* buffer, size_t size) override;
		virtual size_t OnSysWrite(const void* buffer, size_t size) override;
		bool IsOpened() const
		{
			return m_Handle != INVALID_HANDLE_VALUE;
		}

	public:
		static const KxFileStreamAccess DefaultAccessMode = KxFS_ACCESS_READ;
		static const KxFileStreamShare DefaultShareMode = KxFS_SHARE_READ;
		static const KxFileStreamDisposition DefaultDisposition = KxFS_DISP_OPEN_EXISTING;
		static const KxFileStreamFlags DefaultFlags = KxFS_FLAG_NORMAL;

		KxFileStream();
		KxFileStream(HANDLE fileHandle, int accessMode = DefaultAccessMode, KxFileStreamDisposition disposition = DefaultDisposition, int shareMode = DefaultShareMode, int flags = DefaultFlags);
		KxFileStream(const wxString& filePath, int accessMode = DefaultAccessMode, KxFileStreamDisposition disposition = DefaultDisposition, int shareMode = DefaultShareMode, int flags = DefaultFlags);
		
		bool Open(HANDLE fileHandle, int accessMode = DefaultAccessMode, KxFileStreamDisposition disposition = DefaultDisposition, int shareMode = DefaultShareMode, int flags = DefaultFlags);
		bool Open(const wxString& filePath, int accessMode = DefaultAccessMode, KxFileStreamDisposition disposition = DefaultDisposition, int shareMode = DefaultShareMode, int flags = DefaultFlags);
		
		virtual ~KxFileStream();
		virtual bool Close() override;

	public:
		virtual bool IsOk() const override;
		virtual bool Eof() const override
		{
			return GetPosition() == GetFileSizeByHandle(m_Handle);
		}
		virtual bool CanRead() const override
		{
			return !Eof();
		}
		
		virtual wxFileOffset GetLength() const override
		{
			return GetFileSizeByHandle(m_Handle);
		}
		size_t GetSize() const override
		{
			return GetFileSizeByHandle(m_Handle);
		}
		virtual bool IsSeekable() const override
		{
			return ::GetFileType(m_Handle) == FILE_TYPE_DISK;
		}
		virtual wxFileOffset SeekI(wxFileOffset newPos, wxSeekMode mode = wxFromCurrent) override
		{
			return Seek(newPos, WxSeekModeToKx(mode));
		}
		virtual wxFileOffset SeekO(wxFileOffset newPos, wxSeekMode mode = wxFromCurrent) override
		{
			return Seek(newPos, WxSeekModeToKx(mode));
		}
		virtual wxFileOffset TellI() const override
		{
			return GetPosition();
		}
		virtual wxFileOffset TellO() const override
		{
			return GetPosition();
		}
		virtual size_t LastRead() const override
		{
			return m_LastRead;
		}
		virtual size_t LastWrite() const override
		{
			return m_LastWrite;
		}

		virtual wxStreamError GetLastError() const
		{
			return m_LastError;
		}
		bool IsWriteable() const
		{
			return m_AccessMode & KxFS_ACCESS_WRITE || m_AccessMode & KxFS_ACCESS_WRITE_ATTRIBUTES;
		}
		bool IsReadable() const
		{
			return m_AccessMode & KxFS_ACCESS_READ || m_AccessMode & KxFS_ACCESS_READ_ATTRIBUTES;
		}
		bool Flush()
		{
			return ::FlushFileBuffers(m_Handle);
		}
		bool SetEnd()
		{
			return ::SetEndOfFile(m_Handle);
		}
		wxFileOffset Seek(wxFileOffset offset, KxFileStreamSeek mode = KxFS_SEEK_CURRENT);
		wxFileOffset GetPosition() const
		{
			LARGE_INTEGER offset = {0};
			::SetFilePointerEx(m_Handle, offset, &offset, FILE_CURRENT);
			return offset.QuadPart;
		}
		wxString GetFileName() const
		{
			wxString out = GetFileNameByHandle(m_Handle);
			if (m_FilePath.IsEmpty())
			{
				m_FilePath = out;
			}
			if (out.IsEmpty())
			{
				return m_FilePath;
			}
			else
			{
				return out;
			}
		}
		HANDLE GetHandle() const
		{
			return m_Handle;
		}
		
		virtual char Peek()
		{
			if (!Eof())
			{
				char c = ReadObject<char>();
				Seek(-1, KxFS_SEEK_CURRENT);
				return c;
			}
			return 0;
		}
		virtual KxFileStream& Read(void* buffer, size_t size) override
		{
			ReadData(buffer, size);
			return *this;
		}
		bool ReadData(void* buffer, size_t size)
		{
			return OnSysRead(buffer, size) != 0;
		}
		template<class T> T ReadData(size_t size, bool* isSuccess = NULL)
		{
			T v;
			v.resize(size);
			KxUtility::SetIfNotNull(isSuccess, OnSysRead(v.data(), size) != 0);
			return v;
		}
		template<class T> T ReadObject(bool* isSuccess = NULL)
		{
			T v;
			KxUtility::SetIfNotNull(isSuccess, OnSysRead(&v, sizeof(v)) != 0);
			return v;
		}
		
		virtual KxFileStream& Write(const void* buffer, size_t size) override
		{
			WriteData(buffer, size);
			return *this;
		}
		bool WriteData(const void* buffer, size_t size)
		{
			return OnSysWrite(buffer, size) != 0;
		}
		template<class T> bool WriteData(const T& v)
		{
			return OnSysWrite(v.data(), v.size() * sizeof(T::value_type)) != 0;
		}
		template<class T> bool WriteObject(const T& v)
		{
			return OnSysWrite(&v, sizeof(T)) != 0;
		}

		wxString ReadStringCurrentLocale(size_t size, bool* isSuccess = NULL);
		bool WriteStringCurrentLocale(const wxString& v);
		wxString ReadStringASCII(size_t size, bool* isSuccess = NULL);
		bool WriteStringASCII(const wxString& v, char replacement = '_');
		wxString ReadStringUTF8(size_t size, bool* isSuccess = NULL);
		bool WriteStringUTF8(const wxString& v);
		wxString ReadStringUTF16(size_t size, bool* isSuccess = NULL);
		bool WriteStringUTF16(const wxString& v);
		wxString ReadStringUTF32(size_t size, bool* isSuccess = NULL);
		bool WriteStringUTF32(const wxString& v);

	public:
		operator wxStreamBase*()
		{
			if (IsWriteable())
			{
				return static_cast<wxOutputStream*>(this);
			}
			else
			{
				return static_cast<wxInputStream*>(this);
			}
		}
		operator const wxStreamBase*() const
		{
			if (IsWriteable())
			{
				return static_cast<const wxOutputStream*>(this);
			}
			else
			{
				return static_cast<const wxInputStream*>(this);
			}
		}

		wxDECLARE_ABSTRACT_CLASS(KxFileStream);
};
