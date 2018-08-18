#pragma once
#include "KxFramework/KxStatusBar.h"
#include "KxFramework/KxWithImageList.h"

enum
{
	KxSBE_INHERIT_COLORS = 1 << 0,
	KxSBE_SEPARATORS_ENABLED = 1 << 1,

	KxSBE_MASK = KxSBE_INHERIT_COLORS|KxSBE_SEPARATORS_ENABLED
};

class KxStatusBarEx: public KxStatusBar, public KxWithImageList
{

	private:
		static const char* m_SizeGripData[];
		static const wxBitmap m_SizeGripBitmap;

	private:
		wxColour m_ColorBorder = wxNullColour;
		std::unordered_map<int, int> m_Images;
		int m_State = wxCONTROL_NONE;
		bool m_IsSeparatorsVisible = false;
		long m_Style = DefaultStyle;
	
	private:
		void OnPaint(wxPaintEvent& event);
		void OnMouseDown(wxMouseEvent& event)
		{
			m_State = wxCONTROL_SELECTED|wxCONTROL_CURRENT|wxCONTROL_FOCUSED;
			Refresh();
		}
		void OnEnter(wxMouseEvent& event)
		{
			m_State = wxCONTROL_SELECTED|wxCONTROL_CURRENT;
			Refresh();
		}
		void OnLeave(wxMouseEvent& event)
		{
			m_State = wxCONTROL_NONE;
			Refresh();
		}
		
		wxEllipsizeMode GetEllipsizeMode() const;
		void MakeTopmost();

	public:
		static const int DefaultStyle = KxStatusBar::DefaultStyle|KxSBE_SEPARATORS_ENABLED;

		KxStatusBarEx() {}
		KxStatusBarEx(wxWindow* pParent,
					  wxWindowID id,
					  int fieldsCount = DefaultFiledsCount,
					  long style = DefaultStyle
		)
		{
			Create(pParent, id, fieldsCount, style);
		}
		bool Create(wxWindow* pParent,
					wxWindowID id,
					int fieldsCount = DefaultFiledsCount,
					long style = DefaultStyle
		);

	public:
		wxColour GetBorderColor() const
		{
			return m_ColorBorder;
		}
		void SetBorderColor(const wxColour& color = wxNullColour)
		{
			m_ColorBorder = color;
			Refresh();
		}
		
		virtual bool SetForegroundColour(const wxColour& colour) override
		{
			bool value = KxStatusBar::SetForegroundColour(colour);
			Refresh();
			return value;
		}
		virtual bool SetBackgroundColour(const wxColour& color = wxNullColour) override
		{
			bool ret = false;
			if (color.IsOk())
			{
				ret = KxStatusBar::SetBackgroundColour(color);
			}
			else
			{
				wxColour tNewColor = GetParent()->GetBackgroundColour().ChangeLightness(110);
				ret = KxStatusBar::SetBackgroundColour(tNewColor);
			}
			Refresh();
			return ret;
		}
		
		bool IsSeparatorsVisible() const
		{
			return m_IsSeparatorsVisible;
		}
		void SetSeparatorsVisible(bool bSeparators)
		{
			m_IsSeparatorsVisible = bSeparators;
			Refresh();
		}
		
		int GetStatusImage(int index)
		{
			if (index < GetFieldsCount())
			{
				return m_Images[index];
			}
			return -1;
		}
		void SetStatusImage(int imageIndex, int index)
		{
			if (index < GetFieldsCount())
			{
				m_Images[index] = imageIndex;
				Refresh();
			}
		}
		
		virtual void SetMinHeight(int height) override
		{
			KxStatusBar::SetMinHeight(height);
			wxFrame* pFrame = dynamic_cast<wxFrame*>(GetParent());
			if (pFrame)
			{
				SetMinSize(wxSize(wxDefaultCoord, height));
				pFrame->SetStatusBar(this);
			}
			Refresh();
		}
		virtual void SetStatusText(const wxString& text, int number = 0)
		{
			KxStatusBar::SetStatusText(text, number);
			Refresh();
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxStatusBarEx);
};
