#include "stdafx.h"
#include "TextBoxWidget.h"

namespace
{
	constexpr std::wstring_view g_Stylesheet =
		#include "TextBoxWidget.css"
		;

	namespace Attribute
	{
		constexpr auto Label = "Label";
	}
}

namespace kxf::Sciter
{
	void TextBoxWidget::OnAttached()
	{
		Element thisNode = GetElement();

		m_EditArea = Element::Create("EditArea");
		if (thisNode.AppendChild(m_EditArea))
		{
			SetLabel(thisNode.GetAttribute(Attribute::Label));

			Widget::OnAttached();
		}
	}

	String TextBoxWidget::GetValue() const
	{
		return m_EditArea.GetValue();
	}
	void TextBoxWidget::SetValue(StringView value)
	{
		m_EditArea.SetValue(value);
		m_EditArea.Update(true);
	}

	String TextBoxWidget::GetLabel() const
	{
		return m_EditArea.GetAttribute(Attribute::Label);
	}
	void TextBoxWidget::SetLabel(const String& label)
	{
		m_EditArea.SetAttribute(Attribute::Label, label);
	}
}

namespace kxf::Sciter
{
	String TextBoxWidgetFactory::GetWidgetStylesheet() const
	{
		return String::FromView(g_Stylesheet);
	}
}
