#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/UI/WindowRefreshScheduler.h"
#include <wx/html/htmlwin.h>
#include "Kx/General/Color.h"
using KxColor = KxFramework::Color;
class KX_API KxMenu;

class KX_API KxHTMLWindow: public KxFramework::WindowRefreshScheduler<wxHtmlWindow>, public wxTextEntry
{
	public:
		static wxString ProcessPlainText(const wxString& text);
		static bool SetupFontsUsing(const wxFont& normalFont, wxString& normalFace, wxString& fixedFace, int& pointSize);

	private:
		wxBitmap m_BackgroundBitmap;
		KxColor m_BackgroundColor;
		wxGraphicsRenderer* m_Renderer = nullptr;

		const bool m_IsEditable = false;

	private:
		void CopyTextToClipboard(const wxString& value) const;
		void CreateContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link = nullptr);
		void ExecuteContextMenu(KxMenu& menu, const wxHtmlLinkInfo* link = nullptr);
		
		void OnContextMenu(wxContextMenuEvent& event);
		void OnKey(wxKeyEvent& event);

	protected:
		virtual wxString OnProcessPlainText(const wxString& text) const;
		void OnHTMLLinkClicked(const wxHtmlLinkInfo& link) override;
		wxHtmlOpeningStatus OnHTMLOpeningURL(wxHtmlURLType type, const wxString& url, wxString* redirect) const override;
		
		bool DoSetFont(const wxFont& normalFont);
		bool DoSetValue(const wxString& value);
		bool DoAppendValue(const wxString& value);

		WXHWND GetEditHWND() const override
		{
			return nullptr;
		}
		wxWindow* GetEditableWindow() override
		{
			return this;
		}
		
		void OnEraseBackground(wxEraseEvent& event);
		void OnPaint(wxPaintEvent& event);

	public:
		static const long DefaultStyle = wxHW_DEFAULT_STYLE;
		
		KxHTMLWindow() = default;
		KxHTMLWindow(wxWindow* parent,
					  wxWindowID id,
					  const wxString& text = {},
					  long style = DefaultStyle
		)
		{
			Create(parent, id, text, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					const wxString& text = {},
					long style = DefaultStyle
		);
		~KxHTMLWindow() = default;

	public:
		// KxHTMLWindow
		wxBorder GetDefaultBorder() const override
		{
			return wxBORDER_THEME;
		}
		
		wxGraphicsRenderer* GetRenderer() const
		{
			return m_Renderer;
		}
		void SetRenderer(wxGraphicsRenderer* renderer)
		{
			m_Renderer = renderer;
			ScheduleRefresh();
		}

		wxBitmap GetHTMLBackgroundImage() const
		{
			return m_BackgroundBitmap;
		}
		void SetHTMLBackgroundImage(const wxBitmap& bitmap) override
		{
			m_BackgroundBitmap = bitmap;
			wxHtmlWindow::SetHTMLBackgroundImage(bitmap);
			ScheduleRefresh();
		}

		wxColour GetHTMLBackgroundColour() const override
		{
			return m_BackgroundColor;
		}
		void SetHTMLBackgroundColour(const wxColour& color) override
		{
			m_BackgroundColor = color;
			wxHtmlWindow::SetHTMLBackgroundColour(color);
			ScheduleRefresh();
		}

		wxString GetPage() const
		{
			const wxString* source = wxHtmlWindow::GetParser()->GetSource();
			return source ? *source : wxS("");
		}
		bool IsEmpty() const
		{
			const wxString* source = wxHtmlWindow::GetParser()->GetSource();
			return source == nullptr || source->IsEmpty();
		}

	public:
		// wxTextEntry
		wxString GetValue() const override
		{
			return const_cast<KxHTMLWindow*>(this)->ToText();
		}
		void SetValue(const wxString& value) override
		{
			DoSetValue(value);
		}
		void ChangeValue(const wxString& value) override
		{
			DoSetValue(value);
		}
		void AppendText(const wxString& text) override
		{
			DoAppendValue(text);
		}
		void WriteText(const wxString& text) override
		{
			DoAppendValue(text);
		}
		void Clear() override
		{
			wxHtmlWindow::SetPage({});
		}
		
		void Cut() override
		{
		}
		void Copy() override
		{
			CopyTextToClipboard(wxHtmlWindow::SelectionToText());
		}
		void Paste() override
		{
		}
		void Undo() override
		{
		}
		void Redo() override
		{
		}

		bool CanCopy() const override
		{
			return HasSelection();
		}
		bool CanCut() const override
		{
			return IsEditable() && HasSelection();
		}
		bool CanPaste() const override
		{
			return IsEditable();
		}
		bool CanUndo() const override
		{
			return IsEditable();
		}
		bool CanRedo() const override
		{
			return IsEditable();
		}

		wxTextPos GetInsertionPoint() const override
		{
			return 0;
		}
		wxTextPos GetLastPosition() const override
		{
			const wxString text = const_cast<KxHTMLWindow*>(this)->ToText();
			return !text.IsEmpty() ? text.length() - 1 : 0;
		}
		wxString GetRange(long from, long to) const override
		{
			const wxString text = const_cast<KxHTMLWindow*>(this)->ToText();
			return text.SubString(from, to);
		}
		void SetInsertionPoint(long pos) override
		{
		}
		void SetInsertionPointEnd() override
		{
		}
		void SetMaxLength(unsigned long len) override
		{
		}

		wxString GetStringSelection() const override
		{
			return const_cast<KxHTMLWindow*>(this)->SelectionToText();
		}
		bool HasSelection() const
		{
			return m_selection && !m_selection->IsEmpty();
		}
		void GetSelection(long* from, long* to) const override
		{
			if (from)
			{
				*from = m_selection ? m_selection->GetFromCharacterPos() : 0;
			}
			if (to)
			{
				*to = m_selection ? m_selection->GetToCharacterPos() : 0;
			}
		}
		void SetSelection(long from, long to) override
		{
			// Initialize selection if none
			wxHtmlWindow::SelectAll();

			m_selection->SetFromCharacterPos(from);
			m_selection->SetToCharacterPos(to);
		}
		void SelectAll() override
		{
			wxHtmlWindow::SelectAll();
		}
		void SelectNone() override
		{
			SetSelection(0, 0);
		}

		wxString GetHint() const override
		{
			return {};
		}
		bool SetHint(const wxString& hint) override
		{
			return false;
		}

		bool IsEditable() const override
		{
			return m_IsEditable;
		}
		void SetEditable(bool isEditable) override
		{
			// Not implemented
			//m_IsEditable = isEditable;
		}

	public:
		KxHTMLWindow& operator<<(const wxString& value)
		{
			AppendText(value);
			return *this;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxHTMLWindow);
};
