#pragma once
#include "KxFramework/KxFramework.h"
#include <wx/html/htmlwin.h>
class KX_API KxMenu;

class KX_API KxHTMLWindow: public wxHtmlWindow
{
	public:
		static wxString ProcessPlainText(const wxString& text);

	private:
		const bool m_IsEditable = false;
		wxString m_Value;

	private:
		void CreateContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link = nullptr);
		wxWindowID ExecuteContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link = nullptr);
		
		void OnContextMenu(wxContextMenuEvent& event);
		void OnKey(wxKeyEvent& event);

	protected:
		virtual wxString OnProcessPlainText(const wxString& text) const;
		virtual void OnHTMLLinkClicked(const wxHtmlLinkInfo& link) override;
		
		void DoSetFont(const wxFont& font);
		bool DoSetValue(const wxString& value);

	public:
		static const long DefaultStyle = wxHW_DEFAULT_STYLE;
		
		KxHTMLWindow() {}
		KxHTMLWindow(wxWindow* parent,
					  wxWindowID id,
					  const wxString& text = wxEmptyString,
					  long style = DefaultStyle
		)
		{
			Create(parent, id, text, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& text = wxEmptyString,
					long style = DefaultStyle
		);
		virtual ~KxHTMLWindow();

	public:
		virtual const wxString& GetValue() const;
		virtual bool SetValue(const wxString& value);
		virtual bool SetTextValue(const wxString& text);
		
		virtual void Clear();
		virtual bool IsEmpty() const;
		
		virtual bool IsEditable() const;
		virtual void SetEditable(bool isEditable);
		
		bool HasSelection() const;

	public:
		KxHTMLWindow& operator<<(const wxString& s);

		wxDECLARE_DYNAMIC_CLASS(KxHTMLWindow);
};
