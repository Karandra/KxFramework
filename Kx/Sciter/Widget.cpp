#include "stdafx.h"
#include "Widget.h"

namespace KxFramework::Sciter
{
	// Layout
	LayoutFlow Widget::GetLayoutFlow() const
	{
		String flow = m_Element.GetStyleAttribute("flow");
		if (flow == wxS("horizontal"))
		{
			return LayoutFlow::Horizontal;
		}
		else if (flow == wxS("vertical"))
		{
			return LayoutFlow::Vertical;
		}
		return LayoutFlow::None;
	}
	void Widget::SetLayoutFlow(LayoutFlow flow)
	{
		switch (flow)
		{
			case LayoutFlow::Horizontal:
			{
				m_Element.SetStyleAttribute("flow", "horizontal");
				break;
			}
			case LayoutFlow::Vertical:
			{
				m_Element.SetStyleAttribute("flow", "vertical");
				break;
			}
			default:
			{
				m_Element.RemoveStyleAttribute("flow");
				break;
			}
		};
	}

	// Size and position
	void Widget::SetSize(const Size& size)
	{
		if (size.GetWidth() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("width", size.GetWidth(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("width");
		}

		if (size.GetHeight() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("height", size.GetHeight(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("height");
		}
	}
	void Widget::SetPosition(const Point& pos)
	{
		m_Element.SetStyleAttribute("position", "relative");
		if (pos.GetX() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("left", pos.GetX(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("left");
		}

		if (pos.GetY() != wxDefaultCoord)
		{
			m_Element.SetStyleAttribute("top", pos.GetY(), SizeUnit::dip);
		}
		else
		{
			m_Element.RemoveStyleAttribute("top");
		}
	}
}
