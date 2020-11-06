#include "stdafx.h"
#include "NativeWidget.h"
#include "../Host.h"
#include "kxf/Utility/Common.h"

namespace kxf::Sciter
{
	// NativeWidget
	wxWindow* NativeWidget::GetAttachmentParent() const
	{
		Element parentNode = GetElement().GetParent();
		if (wxWindow* window = parentNode.GetWindow())
		{
			return window;
		}
		return &GetHost().GetWindow();
	}

	void NativeWidget::OnAttached()
	{
		// Save window
		m_NativeWindow = &GetNativeWindow();

		// Assign basic properties
		Element node = GetElement();

		m_NativeWindow->SetName(node.GetAttribute("name"));
		if (auto id = node.GetAttribute("command-id").ToInt<wxWindowID>())
		{
			m_NativeWindow->SetId(*id);
		}

		if (Color backgroundColor = node.GetStyleAttributeColor("background-color"))
		{
			m_NativeWindow->SetBackgroundColour(backgroundColor);
		}
		if (Color foregroundColor = node.GetStyleAttributeColor("foreground-color"))
		{
			m_NativeWindow->SetForegroundColour(foregroundColor);
		}
		if (Font font = node.GetStyleFont())
		{
			m_NativeWindow->SetFont(font.ToWxFont());
		}

		// Attach the window
		node.AttachNativeWindow(m_NativeWindow->GetHandle());
		Widget::OnAttached();
	}
	void NativeWidget::OnDetached()
	{
		if (wxWindow* nativeWindow = Utility::ExchangeResetAndReturn(m_NativeWindow, nullptr))
		{
			nativeWindow->Destroy();
		}
		GetElement().DetachNativeWindow();

		Widget::OnDetached();
	}
}
