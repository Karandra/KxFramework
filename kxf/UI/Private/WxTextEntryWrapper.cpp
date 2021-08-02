#include "KxfPCH.h"
#include "WxTextEntryWrapper.h"
#include <wx/textentry.h>

namespace kxf::WXUI::Private
{
	void WxTextEntryWrapper::Initialize(wxTextEntry& textEntry) noexcept
	{
		m_TextEntry = &textEntry;
		if (auto evtHandler = dynamic_cast<wxEvtHandler*>(&textEntry))
		{
			evtHandler->Bind(wxEVT_TEXT, [&](wxCommandEvent& event)
			{
				m_IsModified = true;
				event.Skip();
			});
		}
	}

	bool WxTextEntryWrapper::CanCut() const
	{
		return m_TextEntry->CanCut();
	}
	bool WxTextEntryWrapper::CanCopy() const
	{
		return m_TextEntry->CanCopy();
	}
	bool WxTextEntryWrapper::CanPaste() const
	{
		return m_TextEntry->CanPaste();
	}
	bool WxTextEntryWrapper::CanUndo() const
	{
		return m_TextEntry->CanUndo();
	}
	bool WxTextEntryWrapper::CanRedo() const
	{
		return m_TextEntry->CanRedo();
	}

	void WxTextEntryWrapper::Cut()
	{
		m_TextEntry->Cut();
	}
	void WxTextEntryWrapper::Copy()
	{
		m_TextEntry->Copy();
	}
	void WxTextEntryWrapper::Paste()
	{
		m_TextEntry->Copy();
	}
	void WxTextEntryWrapper::Undo()
	{
		m_TextEntry->Undo();
	}
	void WxTextEntryWrapper::Redo()
	{
		m_TextEntry->Redo();
	}

	void WxTextEntryWrapper::ClearText()
	{
		m_TextEntry->ChangeValue({});
	}
	bool WxTextEntryWrapper::IsTextEmpty() const
	{
		return m_TextEntry->IsEmpty();
	}

	bool WxTextEntryWrapper::IsEditable() const
	{
		return m_TextEntry->IsEditable();
	}
	void WxTextEntryWrapper::SetEditable(bool isEditable)
	{
		m_TextEntry->SetEditable(isEditable);
	}

	bool WxTextEntryWrapper::IsModified() const
	{
		return m_IsModified;
	}
	void WxTextEntryWrapper::SetModified(bool isModified)
	{
		m_IsModified = isModified;
	}

	size_t WxTextEntryWrapper::GetLengthLimit() const
	{
		return m_LengthLimit;
	}
	void WxTextEntryWrapper::SetLengthLimit(size_t limit)
	{
		m_LengthLimit = limit;
		m_TextEntry->SetMaxLength(limit);
	}

	size_t WxTextEntryWrapper::GetTabWidth() const
	{
		return m_TabWidth;
	}
	void WxTextEntryWrapper::SetTabWidth(size_t width)
	{
		m_TabWidth = width;
	}

	size_t WxTextEntryWrapper::GetInsertionPoint() const
	{
		return m_TextEntry->GetInsertionPoint();
	}
	void WxTextEntryWrapper::SetInsertionPoint(size_t pos)
	{
		m_TextEntry->SetInsertionPoint(pos != npos ? pos : -1);
	}

	void WxTextEntryWrapper::SelectRange(size_t from, size_t to)
	{
		if (from == to || from == npos)
		{
			m_TextEntry->SelectNone();
		}
		else if (from == 0 && to == npos)
		{
			m_TextEntry->SelectAll();
		}
		else
		{
			m_TextEntry->SetSelection(from, to);
		}
	}
	std::pair<size_t, size_t> WxTextEntryWrapper::GetSelectionRange() const
	{
		long from = 0;
		long to = 0;
		m_TextEntry->GetSelection(&from, &to);

		return {static_cast<size_t>(from), static_cast<size_t>(to)};
	}
	String WxTextEntryWrapper::GetRange(size_t from, size_t to) const
	{
		if (from != npos)
		{
			return m_TextEntry->GetRange(from, to != npos ? to : -1);
		}
		return {};
	}
	void WxTextEntryWrapper::RemoveRange(size_t from, size_t to)
	{
		if (from != npos)
		{
			return m_TextEntry->Remove(from, to != npos ? to : -1);
		}
	}
	void WxTextEntryWrapper::ReplaceRange(size_t from, size_t to, const String& text)
	{
		if (from != npos)
		{
			return m_TextEntry->Replace(from, to != npos ? to : -1, text);
		}
	}

	String WxTextEntryWrapper::GetText() const
	{
		return m_TextEntry->GetValue();
	}
	void WxTextEntryWrapper::SetText(const String& text)
	{
		m_TextEntry->ChangeValue(text);
	}
	void WxTextEntryWrapper::AppendText(const String& text)
	{
		m_TextEntry->AppendText(text);
	}

	String WxTextEntryWrapper::GetHint() const
	{
		return m_TextEntry->GetHint();
	}
	void WxTextEntryWrapper::SetHint(const String& hint)
	{
		m_TextEntry->SetHint(hint);
	}
}
