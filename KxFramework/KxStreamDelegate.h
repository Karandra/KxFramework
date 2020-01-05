#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxDelegateInputStream: public wxInputStream
{
	private:
		wxInputStream* m_Stream;
		const bool m_StreamOwned = false;

	protected:
		// wxStreamBase 
		wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
		{
			return m_Stream->SeekI(pos, mode);
		}
		wxFileOffset OnSysTell() const override
		{
			return m_Stream->TellI();
		}
		
		// wxInputStream
		size_t OnSysRead(void* buffer, size_t size) override
		{
			m_Stream->Read(buffer, size);
			return m_Stream->LastRead();
		}

	public:
		KxDelegateInputStream(wxInputStream& stream)
			:m_Stream(&stream), m_StreamOwned(false)
		{
		}
		KxDelegateInputStream(std::unique_ptr<wxInputStream> stream)
			:m_Stream(stream.release()), m_StreamOwned(true)
		{
		}
		~KxDelegateInputStream()
		{
			if (m_StreamOwned)
			{
				delete m_Stream;
			}
		}

	public:
		wxInputStream* GetTargetStream() const
		{
			return m_Stream;
		}

	public:
		// wxStreamBase
		bool IsOk() const override
		{
			return m_Stream->IsOk();
		}
		bool IsSeekable() const override
		{
			return m_Stream->IsSeekable();
		}

		wxFileOffset GetLength() const override
		{
			return m_Stream->GetLength();
		}
		size_t GetSize() const override
		{
			return m_Stream->GetSize();
		}

		// wxInputStream
		bool CanRead() const override
		{
			return m_Stream->CanRead();
		}
		bool Eof() const override
		{
			return m_Stream->Eof();
		}

		char Peek() override
		{
			return m_Stream->Peek();
		}
		KxDelegateInputStream& Read(void* buffer, size_t size) override
		{
			m_Stream->Read(buffer, size);
			return *this;
		}
		size_t LastRead() const override
		{
			return m_Stream->LastRead();
		}

		wxFileOffset SeekI(wxFileOffset pos, wxSeekMode mode = wxFromStart) override
		{
			return m_Stream->SeekI(pos, mode);
		}
		wxFileOffset TellI() const override
		{
			return m_Stream->TellI();
		}
};

class KX_API KxDelegateOutputStream: public wxOutputStream
{
	private:
		wxOutputStream* m_Stream;
		const bool m_StreamOwned = false;

	protected:
		// wxStreamBase 
		wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
		{
			return m_Stream->SeekO(pos, mode);
		}
		wxFileOffset OnSysTell() const override
		{
			return m_Stream->TellO();
		}
		
		// wxOutputStream
		size_t OnSysWrite(const void* buffer, size_t size) override
		{
			m_Stream->Write(buffer, size);
			return m_Stream->LastWrite();
		}

	public:
		KxDelegateOutputStream(wxOutputStream& stream)
			:m_Stream(&stream), m_StreamOwned(false)
		{
		}
		KxDelegateOutputStream(std::unique_ptr<wxOutputStream> stream)
			:m_Stream(stream.release()), m_StreamOwned(true)
		{
		}
		~KxDelegateOutputStream()
		{
			if (m_StreamOwned)
			{
				delete m_Stream;
			}
		}

	public:
		wxOutputStream* GetTargetStream() const
		{
			return m_Stream;
		}

	public:
		// wxStreamBase 
		bool IsOk() const override
		{
			return m_Stream->IsOk();
		}
		bool IsSeekable() const override
		{
			return m_Stream->IsSeekable();
		}

		wxFileOffset GetLength() const override
		{
			return m_Stream->GetLength();
		}
		size_t GetSize() const override
		{
			return m_Stream->GetSize();
		}

		// wxOutputStream
		KxDelegateOutputStream& Write(const void* buffer, size_t size) override
		{
			m_Stream->Write(buffer, size);
			return *this;
		}
		size_t LastWrite() const override
		{
			return m_Stream->LastWrite();
		}
		
		wxFileOffset SeekO(wxFileOffset pos, wxSeekMode mode = wxFromStart) override
		{
			return m_Stream->SeekO(pos, mode);
		}
		wxFileOffset TellO() const override
		{
			return m_Stream->TellO();
		}

		void Sync() override
		{
			m_Stream->Sync();
		}
		bool Close() override
		{
			if (m_StreamOwned)
			{
				return m_Stream->Close();
			}
			return true;
		}
};
