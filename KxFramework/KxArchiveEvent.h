#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileOperationEvent.h"

class KX_API KxArchiveEvent: public KxFileOperationEvent
{
	public:
		KxEVENT_MEMBER(KxArchiveEvent, Process);
		KxEVENT_MEMBER(KxArchiveEvent, Search);
		KxEVENT_MEMBER(KxArchiveEvent, Done);

		KxEVENT_MEMBER(KxArchiveEvent, GetInputStream);
		KxEVENT_MEMBER(KxArchiveEvent, GetOutputStream);

	private:
		wxString m_Method;
		wxString m_Attributes;
		int64_t m_OriginalSize = 0;
		int64_t m_CompressedSize = 0;
		uint32_t m_CRC32 = 0;
		uint32_t m_FileIndex = 0;
		double m_Ratio = 0;

		wxInputStream* m_InputStream = nullptr;
		wxOutputStream* m_OutputStream = nullptr;

	public:
		KxArchiveEvent(wxEventType type = wxEVT_NULL, int id = 0)
			:KxFileOperationEvent(type, id)
		{
		}

	public:
		KxArchiveEvent* Clone() const override
		{
			return new KxArchiveEvent(*this);
		}
		
		uint32_t GetFileIndex() const
		{
			return m_FileIndex;
		}
		void SetFileIndex(uint32_t value)
		{
			m_FileIndex = value;
		}

		uint32_t GetCRC() const
		{
			return m_CRC32;
		}
		void SetCRC(uint32_t value)
		{
			m_CRC32 = value;
		}
		
		wxString GetAttributes() const
		{
			return m_Attributes;
		}
		void SetAttributes(const wxString& string)
		{
			m_Attributes = string;
		}
		
		wxString GetMethod() const
		{
			return m_Method;
		}
		void SetMethod(const wxString& string)
		{
			m_Method = string;
		}

		int64_t GetOriginalSize() const
		{
			return m_OriginalSize;
		}
		void SetOriginalSize(int64_t value)
		{
			m_OriginalSize = value;
		}
		
		int64_t GetCompressedSize() const
		{
			return m_CompressedSize;
		}
		void SetCompressedSize(int64_t value)
		{
			m_CompressedSize = value;
		}
		
		double GetRatio() const
		{
			return m_Ratio;
		}
		void SetRatio(double value)
		{
			m_Ratio = value;
		}

		wxOutputStream* GetOutputStream() const
		{
			return m_OutputStream;
		}
		void SetOutputStream(wxOutputStream* stream)
		{
			m_OutputStream = stream;
		}

		wxInputStream* GetInputStream() const
		{
			return m_InputStream;
		}
		void SetInputStream(wxInputStream* stream)
		{
			m_InputStream = stream;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxArchiveEvent);
};
