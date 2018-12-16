#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewRenderer.h"

enum KxDataViewProgressState
{
	KxDVR_PROGRESS_STATE_NORMAL,
	KxDVR_PROGRESS_STATE_PAUSED,
	KxDVR_PROGRESS_STATE_ERROR,
};

class KX_API KxDataViewProgressValue
{
	private:
		wxString m_Text;
		int m_Position = -1;
		KxDataViewProgressState m_State = KxDVR_PROGRESS_STATE_NORMAL;

	public:
		KxDataViewProgressValue(int position = -1, const wxString& text = wxEmptyString, KxDataViewProgressState state = KxDVR_PROGRESS_STATE_NORMAL)
			:m_Text(text), m_State(state)
		{
			SetPosition(position);
		}

	public:
		bool HasText() const
		{
			return !m_Text.IsEmpty();
		}
		const wxString& GetText() const
		{
			return m_Text;
		}
		void SetText(const wxString& text)
		{
			m_Text = text;
		}

		int GetRange() const
		{
			return 100;
		}

		bool HasPosition() const
		{
			return m_Position >= 0;
		}
		int GetPosition() const
		{
			return m_Position;
		}
		void SetPosition(int position)
		{
			m_Position = position <= GetRange() ? position : GetRange();
		}

		KxDataViewProgressState GetState() const
		{
			return m_State;
		}
		void SetState(KxDataViewProgressState state)
		{
			m_State = state;
		}
};

enum KxDataViewProgressSizeOption
{
	KxDVR_PROGRESS_HEIGHT_AUTO = 0,
	KxDVR_PROGRESS_HEIGHT_FIT,
};
class KX_API KxDataViewProgressRenderer: public KxDataViewRenderer
{
	private:
		KxDataViewProgressValue m_Value;
		KxDataViewProgressSizeOption m_SizeOption = KxDVR_PROGRESS_HEIGHT_AUTO;

	protected:
		virtual bool SetValue(const wxAny& value);

		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) override;
		virtual wxSize GetCellSize() const override;

	public:
		KxDataViewProgressRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_CENTER_VERTICAL|wxALIGN_CENTER_HORIZONTAL)
			:KxDataViewRenderer(cellMode, alignment)
		{
		}

	public:
		KxDataViewProgressSizeOption GetSizeOption() const
		{
			return m_SizeOption;
		}
		void SetSizeOption(KxDataViewProgressSizeOption option)
		{
			m_SizeOption = option;
		}
};
