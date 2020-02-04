#pragma once
#include "KxFramework/KxFramework.h"

template<class TBaseStream>
class KxDelegateStreamBase: public TBaseStream
{
	private:
		TBaseStream* m_Stream = nullptr;
		bool m_StreamOwned = false;

	private:
		void DeleteTargetStreamIfNeeded()
		{
			if (m_StreamOwned)
			{
				delete m_Stream;
				m_Stream = nullptr;
			}
			m_StreamOwned = false;
		}

	protected:
		KxDelegateStreamBase() = default;
		KxDelegateStreamBase(std::nullptr_t)
			:m_Stream(nullptr), m_StreamOwned(false)
		{
		}
		KxDelegateStreamBase(TBaseStream& stream)
			:m_Stream(&stream), m_StreamOwned(false)
		{
		}
		
		template<class T>
		KxDelegateStreamBase(std::unique_ptr<T> stream)
			:m_Stream(stream.release()), m_StreamOwned(true)
		{
			static_assert(std::is_base_of_v<TBaseStream, T>, "must inherit from 'TBaseStream' (wx[Input/Output]Stream)");
		}

		KxDelegateStreamBase(KxDelegateStreamBase&& other)
		{
			*this = std::move(other);
		}
		KxDelegateStreamBase(const KxDelegateStreamBase&) = delete;

		~KxDelegateStreamBase()
		{
			DeleteTargetStreamIfNeeded();
		}

	public:
		bool IsTargetStreamOwned() const
		{
			return m_StreamOwned;
		}
		bool HasTargetStream() const
		{
			return m_Stream != nullptr;
		}
		std::unique_ptr<TBaseStream> TakeTargetStream()
		{
			if (m_StreamOwned)
			{
				return std::unique_ptr<TBaseStream>(m_Stream);
			}
			return nullptr;
		}
		
		TBaseStream* GetTargetStream() const
		{
			return m_Stream;
		}
		TBaseStream& operator*() const
		{
			return *m_Stream;
		}
		TBaseStream* operator->() const
		{
			return m_Stream;
		}

		KxDelegateStreamBase& operator=(KxDelegateStreamBase&& other)
		{
			DeleteTargetStreamIfNeeded();
			m_Stream = other.m_Stream;
			other.m_Stream = nullptr;

			m_StreamOwned = other.m_StreamOwned;
			other.m_StreamOwned = false;

			TBaseStream::m_lastcount = other.m_lastcount;
			other.m_lastcount = 0;

			TBaseStream::m_lasterror = other.m_lasterror;
			other.m_lasterror = wxStreamError::wxSTREAM_NO_ERROR;

			return *this;
		}
		KxDelegateStreamBase& operator=(const KxDelegateStreamBase&) = delete;

		explicit operator bool() const
		{
			return HasTargetStream();
		}
		bool operator!() const
		{
			return !HasTargetStream();
		}

	public:
		// wxStreamBase
		bool IsOk() const override
		{
			return m_Stream && m_Stream->IsOk();
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
};

class KX_API KxDelegateInputStream: public KxDelegateStreamBase<wxInputStream>
{
	protected:
		// wxStreamBase
		wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
		{
			return GetTargetStream()->SeekI(pos, mode);
		}
		wxFileOffset OnSysTell() const override
		{
			return GetTargetStream()->TellI();
		}

		// wxInputStream
		size_t OnSysRead(void* buffer, size_t size) override
		{
			GetTargetStream()->Read(buffer, size);
			return GetTargetStream()->LastRead();
		}

	public:
		KxDelegateInputStream() = default;

		template<class... Args>
		KxDelegateInputStream(Args&&... arg)
			:KxDelegateStreamBase(std::forward<Args>(arg)...)
		{
		}

	public:
		// wxInputStream
		bool CanRead() const override
		{
			return GetTargetStream()->CanRead();
		}
		bool Eof() const override
		{
			return GetTargetStream()->Eof();
		}

		char Peek() override
		{
			return GetTargetStream()->Peek();
		}
		KxDelegateInputStream& Read(void* buffer, size_t size) override
		{
			GetTargetStream()->Read(buffer, size);
			return *this;
		}
		size_t LastRead() const override
		{
			return GetTargetStream()->LastRead();
		}

		wxFileOffset SeekI(wxFileOffset pos, wxSeekMode mode = wxFromStart) override
		{
			return GetTargetStream()->SeekI(pos, mode);
		}
		wxFileOffset TellI() const override
		{
			return GetTargetStream()->TellI();
		}
};

class KX_API KxDelegateOutputStream: public KxDelegateStreamBase<wxOutputStream>
{
	protected:
		// wxStreamBase
		wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) override
		{
			return GetTargetStream()->SeekO(pos, mode);
		}
		wxFileOffset OnSysTell() const override
		{
			return GetTargetStream()->TellO();
		}

		// wxOutputStream
		size_t OnSysWrite(const void* buffer, size_t size) override
		{
			GetTargetStream()->Write(buffer, size);
			return GetTargetStream()->LastWrite();
		}

	public:
		KxDelegateOutputStream() = default;

		template<class... Args>
		KxDelegateOutputStream(Args&&... arg)
			:KxDelegateStreamBase(std::forward<Args>(arg)...)
		{
		}

	public:
		// wxOutputStream
		KxDelegateOutputStream& Write(const void* buffer, size_t size) override
		{
			GetTargetStream()->Write(buffer, size);
			return *this;
		}
		size_t LastWrite() const override
		{
			return GetTargetStream()->LastWrite();
		}
		
		wxFileOffset SeekO(wxFileOffset pos, wxSeekMode mode = wxFromStart) override
		{
			return GetTargetStream()->SeekO(pos, mode);
		}
		wxFileOffset TellO() const override
		{
			return GetTargetStream()->TellO();
		}

		void Sync() override
		{
			GetTargetStream()->Sync();
		}
		bool Close() override
		{
			if (IsTargetStreamOwned())
			{
				return GetTargetStream()->Close();
			}
			return true;
		}
};