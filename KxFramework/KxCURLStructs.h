/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
class KX_API KxCURLSession;

class KX_API KxCURLReplyBase
{
	friend class KxCURLSession;

	private:
		int m_ErrorCode = -1;
		int m_ResponseCode = -1;

	protected:
		void SetErrorCode(int code)
		{
			m_ErrorCode = code;
		}
		void SetResponseCode(int code)
		{
			m_ResponseCode = code;
		}

		virtual void AddChunk(const void* data, size_t size) = 0;
		virtual size_t GetDownloaded() const = 0;

	public:
		virtual ~KxCURLReplyBase() = default;

	public:
		virtual bool IsOK() const
		{
			return m_ErrorCode != -1 && m_ResponseCode != -1;
		}

		int GetErrorCode() const
		{
			return m_ErrorCode;
		}
		int GetResponseCode() const
		{
			return m_ResponseCode;
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxCURLReply: public KxCURLReplyBase
{
	friend class KxCURLSession;

	private:
		wxString m_Buffer;
		size_t m_Downloaded = 0;

	private:
		void AddChunk(const wxString& data)
		{
			m_Buffer.Append(data);
			m_Downloaded += data.size() / sizeof(wxChar);
		}
		virtual void AddChunk(const void* data, size_t size) override
		{
			m_Buffer.Append(static_cast<const char*>(data), size);
			m_Downloaded += size;
		}
		virtual size_t GetDownloaded() const override
		{
			return m_Downloaded;
		}

	public:
		virtual bool IsOK() const override
		{
			return KxCURLReplyBase::IsOK() && !m_Buffer.IsEmpty();
		}

		operator const wxString&() const
		{
			return m_Buffer;
		}
		operator wxString&()
		{
			return m_Buffer;
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxCURLBinaryReply: public KxCURLReplyBase
{
	friend class KxCURLSession;

	private:
		wxMemoryBuffer m_Buffer;

	private:
		virtual void AddChunk(const void* data, size_t size) override
		{
			m_Buffer.AppendData(data, size);
		}
		virtual size_t GetDownloaded() const override
		{
			return m_Buffer.GetDataLen();
		}

	public:
		KxCURLBinaryReply()
			:m_Buffer(0)
		{
		}

	public:
		virtual bool IsOK() const override
		{
			return KxCURLReplyBase::IsOK() && !m_Buffer.IsEmpty();
		}

		const void* GetData() const
		{
			return m_Buffer.GetData();
		}
		size_t GetSize() const
		{
			return m_Buffer.GetDataLen();
		}

		wxString ToString() const
		{
			return wxString::FromUTF8(static_cast<const char*>(m_Buffer.GetData()), m_Buffer.GetDataLen());
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxCURLStreamReply: public KxCURLReplyBase
{
	friend class KxCURLSession;

	private:
		wxOutputStream& m_Stream;
		size_t m_Downloaded = 0;
		int64_t m_InitialPosition = 0;

	private:
		virtual void AddChunk(const void* data, size_t size) override
		{
			m_Stream.WriteAll(data, size);
			m_Downloaded += size;
		}
		virtual size_t GetDownloaded() const override
		{
			return m_InitialPosition + m_Downloaded;
		}

	public:
		KxCURLStreamReply(wxOutputStream& stream, int64_t initialPos = 0)
			:m_Stream(stream), m_InitialPosition(initialPos)
		{
			if (m_InitialPosition > 0)
			{
				m_Stream.SeekO(m_InitialPosition);
			}
		}

	public:
		virtual bool IsOK() const override
		{
			return KxCURLReplyBase::IsOK() && m_Stream.IsOk();
		}

		int64_t GetResumeFromPosition() const
		{
			return m_InitialPosition;
		}
		bool ShouldResumeFromPosition(int64_t& pos) const
		{
			pos = m_InitialPosition;
			return m_InitialPosition > 0;
		}
		bool ShouldResumeFromPosition() const
		{
			int64_t pos;
			return ShouldResumeFromPosition(pos);
		}

		const wxOutputStream& GetStream() const
		{
			return m_Stream;
		}
		wxOutputStream& GetStream()
		{
			return m_Stream;
		}
};
