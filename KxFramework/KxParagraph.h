#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxHTMLWindow.h"

class KX_API KxParagraph: public KxHTMLWindow
{
	private:
		wxString m_Text;

	private:
		bool DoSetValue(const wxString& text);

	public:
		static const long DefaultStyle = wxHW_DEFAULT_STYLE|wxBORDER_THEME;
		
		KxParagraph() {}
		KxParagraph(wxWindow* parent,
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
		virtual ~KxParagraph();

	public:
		virtual wxBorder GetDefaultBorder() const override
		{
			return wxBORDER_THEME;
		}

		virtual bool IsEmpty() const override
		{
			return m_Text.IsEmpty();
		}
		
		virtual wxString GetLabel() const override
		{
			return GetValue();
		}
		virtual void SetLabel(const wxString& text) override
		{
			SetValue(text);
		}
		
		virtual const wxString& GetValue() const override;
		virtual bool SetValue(const wxString& text) override;

	public:
		wxDECLARE_DYNAMIC_CLASS(KxParagraph);
};
