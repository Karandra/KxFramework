#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxMenu.h"
#include <wx/html/htmlwin.h>

class KxHTMLWindow: public wxHtmlWindow
{
	public:
		static wxString ProcessPlainText(const wxString& text);

	private:
		const bool m_IsEditable = false;
		KxMenu m_ContextMenu;
		wxString m_Value;

	private:
		void DoSetFont(const wxFont& font);
		void CreateContextMenu();
		void CopyTextToClipboard(const wxString& value) const;

		void OnShowContextMenu(KxMenuEvent& event);
		void OnContextMenu(wxContextMenuEvent& event);
		void OnKey(wxKeyEvent& event);

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
		virtual void Clear();
		virtual bool IsEmpty() const
		{
			return m_Value.IsEmpty();
		}
		virtual bool IsEditable() const
		{
			return m_IsEditable;
		}
		virtual void SetEditable(bool bIsEditable)
		{
			// Not implemented
			//m_IsEditable = bIsEditable;
		}
		bool SetTextValue(const wxString& text);

		const KxMenu* GetContextMenu() const
		{
			return &m_ContextMenu;
		}
		KxMenu* GetContextMenu()
		{
			return &m_ContextMenu;
		}

	public:
		KxHTMLWindow& operator<<(const wxString& s);

		wxDECLARE_DYNAMIC_CLASS(KxHTMLWindow);
};
