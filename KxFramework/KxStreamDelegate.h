#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxDelegateInputStream: public wxFilterInputStream
{
	protected:
		// wxStreamBase 
		wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
		{
			return GetFilterInputStream()->SeekI(pos, mode);
		}
		wxFileOffset OnSysTell() const override
		{
			return GetFilterInputStream()->TellI();
		}
		
		// wxInputStream
		size_t OnSysRead(void* buffer, size_t size) override
		{
			GetFilterInputStream()->Read(buffer, size);
			return GetFilterInputStream()->LastRead();
		}

	public:
		KxDelegateInputStream(wxInputStream& stream)
			:wxFilterInputStream(stream)
		{
		}
		KxDelegateInputStream(std::unique_ptr<wxInputStream> stream)
			:wxFilterInputStream(stream.release())
		{
		}

	public:
		// wxStreamBase
		bool IsOk() const override
		{
			return GetFilterInputStream()->IsOk();
		}
		bool IsSeekable() const override
		{
			return GetFilterInputStream()->IsSeekable();
		}

		wxFileOffset GetLength() const override
		{
			return GetFilterInputStream()->GetLength();
		}
		size_t GetSize() const override
		{
			return GetFilterInputStream()->GetSize();
		}

		// wxInputStream
		bool CanRead() const override
		{
			return GetFilterInputStream()->CanRead();
		}
		bool Eof() const override
		{
			return GetFilterInputStream()->Eof();
		}

		char Peek() override
		{
			return GetFilterInputStream()->Peek();
		}
		KxDelegateInputStream& Read(void* buffer, size_t size) override
		{
			GetFilterInputStream()->Read(buffer, size);
			return *this;
		}
		size_t LastRead() const override
		{
			return GetFilterInputStream()->LastRead();
		}

		wxFileOffset SeekI(wxFileOffset pos, wxSeekMode mode = wxFromStart) override
		{
			return GetFilterInputStream()->SeekI(pos, mode);
		}
		wxFileOffset TellI() const override
		{
			return GetFilterInputStream()->TellI();
		}
};

class KX_API KxDelegateOutputStream: public wxFilterOutputStream
{
	protected:
		// wxStreamBase 
		wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
		{
			return GetFilterOutputStream()->SeekO(pos, mode);
		}
		wxFileOffset OnSysTell() const override
		{
			return GetFilterOutputStream()->TellO();
		}
		
		// wxOutputStream
		size_t OnSysWrite(const void* buffer, size_t size) override
		{
			GetFilterOutputStream()->Write(buffer, size);
			return GetFilterOutputStream()->LastWrite();
		}

	public:
		KxDelegateOutputStream(wxOutputStream& stream)
			:wxFilterOutputStream(stream)
		{
		}
		KxDelegateOutputStream(std::unique_ptr<wxOutputStream> stream)
			:wxFilterOutputStream(stream.release())
		{
		}

	public:
		// wxStreamBase 
		bool IsOk() const override
		{
			return GetFilterOutputStream()->IsOk();
		}
		bool IsSeekable() const override
		{
			return GetFilterOutputStream()->IsSeekable();
		}

		wxFileOffset GetLength() const override
		{
			return GetFilterOutputStream()->GetLength();
		}
		size_t GetSize() const override
		{
			return GetFilterOutputStream()->GetSize();
		}

		// wxOutputStream
		KxDelegateOutputStream& Write(const void* buffer, size_t size) override
		{
			GetFilterOutputStream()->Write(buffer, size);
			return *this;
		}
		size_t LastWrite() const override
		{
			return GetFilterOutputStream()->LastWrite();
		}
		
		wxFileOffset SeekO(wxFileOffset pos, wxSeekMode mode = wxFromStart) override
		{
			return GetFilterOutputStream()->SeekO(pos, mode);
		}
		wxFileOffset TellO() const override
		{
			return GetFilterOutputStream()->TellO();
		}

		bool Close() override
		{
			return GetFilterOutputStream()->Close();
		}
};
