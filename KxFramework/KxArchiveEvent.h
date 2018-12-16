#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileOperationEvent.h"

class KX_API KxArchiveEvent: public KxFileOperationEvent
{
	private:
		wxString m_Method;
		wxString m_Attributes;
		int64_t m_OriginalSize = 0;
		int64_t m_CompressedSize = 0;
		uint32_t m_CRC32 = 0;
		float m_Ratio = 0;

	public:
		KxArchiveEvent(wxEventType type = wxEVT_NULL, int id = 0);
		virtual ~KxArchiveEvent();
		virtual KxArchiveEvent* Clone() const override;

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
		
		float GetRatio() const
		{
			return m_Ratio;
		}
		void SetRatio(float value)
		{
			m_Ratio = value;
		}

		wxDECLARE_DYNAMIC_CLASS(KxArchiveEvent);
};

//////////////////////////////////////////////////////////////////////////
KX_DECLARE_EVENT(KxEVT_ARCHIVE, KxArchiveEvent);
KX_DECLARE_EVENT(KxEVT_ARCHIVE_PACK, KxArchiveEvent);
KX_DECLARE_EVENT(KxEVT_ARCHIVE_UNPACK, KxArchiveEvent);
KX_DECLARE_EVENT(KxEVT_ARCHIVE_SEARCH, KxArchiveEvent);
KX_DECLARE_EVENT(KxEVT_ARCHIVE_DONE, KxArchiveEvent);
