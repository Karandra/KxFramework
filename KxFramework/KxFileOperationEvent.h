#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxFileOperationEvent: public wxNotifyEvent
{
	private:
		wxString m_Destination;
		wxString m_Source;
		int64_t m_MajorProcessed = 0;
		int64_t m_MajorTotal = 0;
		int64_t m_MinorProcessed = 0;
		int64_t m_MinorTotal = 0;
		int64_t m_Speed = 0;

	private:
		bool IsPartKnown(int64_t current, int64_t max) const
		{
			return !(current <= 0 && max <= 0);
		}

	public:
		KxFileOperationEvent(wxEventType type = wxEVT_NULL, int id = 0);
		virtual ~KxFileOperationEvent();
		virtual KxFileOperationEvent* Clone() const;

		bool IsStopped() const
		{
			return !wxNotifyEvent::IsAllowed();
		}
		void Stop()
		{
			wxNotifyEvent::Veto();
		}
		bool IsMajorComplete() const
		{
			return GetMajorProcessed() >= GetMajorTotal();
		}
		bool IsMinorComplete() const
		{
			return GetMinorProcessed() >= GetMinorTotal();
		}
		bool IsMinorKnown() const
		{
			return IsPartKnown(m_MinorProcessed, m_MinorTotal);
		}
		bool IsMajorKnown() const
		{
			return IsPartKnown(m_MajorProcessed, m_MajorTotal);
		}
		void SetMajorComplete()
		{
			SetMajorProcessed(GetMajorTotal());
		}
		void SetMinorComplete()
		{
			SetMinorProcessed(GetMinorTotal());
		}

		wxString GetCurrent() const
		{
			return GetString();
		}
		void SetCurrent(const wxString& string)
		{
			SetString(string);
		}
		wxString GetSource() const
		{
			return m_Source;
		}
		void SetSource(const wxString& string)
		{
			m_Source = string;
		}
		wxString GetDestination() const
		{
			return m_Destination;
		}
		void SetDestination(const wxString& string)
		{
			m_Destination = string;
		}

		int64_t GetMajorProcessed() const
		{
			return m_MajorProcessed;
		}
		void SetMajorProcessed(int64_t value)
		{
			m_MajorProcessed = value;
		}
		int64_t GetMajorTotal() const
		{
			return m_MajorTotal;
		}
		void SetMajorTotal(int64_t value)
		{
			m_MajorTotal = value;
		}

		int64_t GetMinorProcessed() const
		{
			return m_MinorProcessed;
		}
		void SetMinorProcessed(int64_t value)
		{
			m_MinorProcessed = value;
		}
		int64_t GetMinorTotal() const
		{
			return m_MinorTotal;
		}
		void SetMinorTotal(int64_t value)
		{
			m_MinorTotal = value;
		}

		int64_t GetSpeed() const
		{
			return m_Speed;
		}
		void SetSpeed(int64_t value)
		{
			m_Speed = value;
		}

		wxDECLARE_DYNAMIC_CLASS(KxFileOperationEvent);
};

//////////////////////////////////////////////////////////////////////////
KX_DECLARE_EVENT(KxEVT_FILEOP_COPY, KxFileOperationEvent);
KX_DECLARE_EVENT(KxEVT_FILEOP_MOVE, KxFileOperationEvent);
KX_DECLARE_EVENT(KxEVT_FILEOP_REMOVE, KxFileOperationEvent);
KX_DECLARE_EVENT(KxEVT_FILEOP_COPY_FOLDER, KxFileOperationEvent);
KX_DECLARE_EVENT(KxEVT_FILEOP_MOVE_FOLDER, KxFileOperationEvent);
KX_DECLARE_EVENT(KxEVT_FILEOP_REMOVE_FOLDER, KxFileOperationEvent);
KX_DECLARE_EVENT(KxEVT_FILEOP_SEARCH, KxFileOperationEvent);
KX_DECLARE_EVENT(KxEVT_FILEOP_RENAME, KxFileOperationEvent);
