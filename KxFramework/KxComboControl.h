#pragma once
#include "KxFramework/KxFramework.h"
class KxComboPopup;

class KxComboControl: public wxComboCtrl
{
	public:
		enum DrawFocus
		{
			Always = 1,
			Never = 0,
			Auto = -1,
		};

	private:
		KxComboPopup* m_PopupCtrl = NULL;
		DrawFocus m_FocusDrawMode = DrawFocus::Auto;

	protected:
		virtual void DoShowPopup(const wxRect& rect, int flags) override;

	public:
		static const int DefaultStyle = 0;

		KxComboControl() {}
		KxComboControl(wxWindow* window,
					   wxWindowID id,
					   const wxString& value = wxEmptyString,
					   long style = DefaultStyle,
					   const wxValidator& validator = wxDefaultValidator
		)
		{
			Create(window, id, value, style, validator);
		}
		bool Create(wxWindow* window,
					wxWindowID id,
					const wxString& value = wxEmptyString,
					long style = DefaultStyle,
					const wxValidator& validator = wxDefaultValidator
		);

	public:
		virtual bool ShouldDrawFocus() const;
		void SetFocusDrawMode(DrawFocus mode);
		
		KxComboPopup* GetPopupControl()
		{
			return m_PopupCtrl;
		}
		void SetPopupControl(KxComboPopup* popup);

	public:
		wxDECLARE_DYNAMIC_CLASS_NO_COPY(KxComboControl);
};
