#include "KxfPCH.h"
#include "ToolTip.h"
#include "../IDataViewWidget.h"
#include "../IDataViewColumn.h"
#include "../IDataViewNode.h"
#include "../IGraphicsRendererAwareWidget.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/Drawing/SizeRatio.h"
#include "kxf/UI/Windows/ToolTipEx.h"
#include <wx/textwrapper.h>

namespace kxf::DataView
{
	const IDataViewColumn& ToolTip::SelectAnchorColumn(const IDataViewColumn& currentColumn) const
	{
		if (m_AnchorColumn && m_AnchorColumn->IsVisible())
		{
			return *m_AnchorColumn;
		}
		return currentColumn;
	}
	const IDataViewColumn& ToolTip::SelectClipTestColumn(const IDataViewColumn& currentColumn) const
	{
		if (m_ClipTestColumn && m_ClipTestColumn->IsVisible())
		{
			return *m_ClipTestColumn;
		}
		return currentColumn;
	}

	Point ToolTip::GetPopupPosition(const IDataViewNode& node, const IDataViewColumn& column) const
	{
		const Rect rect = node.GetCellClientRect(SelectAnchorColumn(column));
		return rect.GetPosition() + Point(0, rect.GetHeight() + 1);
	}
	Point ToolTip::AdjustPopupPosition(const IDataViewNode& node, const IDataViewColumn& column, const Point& pos) const
	{
		auto widget = column.GetOwningWdget();
		if (std::shared_ptr<IGraphicsRendererAwareWidget> graphicsAware; widget && widget->QueryInterface(graphicsAware))
		{
			if (auto context = graphicsAware->GetActiveGraphicsRenderer()->CreateMeasuringContext(widget.get()))
			{
				const Size smallIcon = Geometry::SizeRatio::FromSystemSmallIcon();
				Size textExtent = context->GetTextExtent(m_Message);
				Size offset;

				if (auto icon = GetIconBitmap())
				{
					textExtent += icon.GetSize() * 1.5;
					offset = smallIcon / 2;
				}
				else if (auto icon = GetIconID(); icon != StdIcon::None)
				{
					textExtent += smallIcon * 1.5;
					offset = smallIcon / 2;
				}
				else
				{
					offset = smallIcon;
				}

				const Size screenSize = {wxSystemSettings::GetMetric(wxSYS_SCREEN_X), wxSystemSettings::GetMetric(wxSYS_SCREEN_Y)};
				Point adjustedPos = widget->ClientToScreen(pos);
				if (int right = adjustedPos.GetX() + textExtent.GetWidth(); right > screenSize.GetWidth())
				{
					adjustedPos.X() -= (right - screenSize.GetWidth()) + offset.GetWidth();
				}
				if (int bottom = adjustedPos.GetY() + textExtent.GetHeight(); bottom > screenSize.GetHeight())
				{
					adjustedPos.Y() -= (bottom - screenSize.GetHeight()) + offset.GetHeight();
				}
				return widget->ScreenToClient(adjustedPos);
			}
		}
		return Point::UnspecifiedPosition();
	}
	String ToolTip::StripMarkupIfNeeded(const IDataViewNode& node, const IDataViewColumn& column, const String& text) const
	{
		auto widget = column.GetOwningWdget();
		if (widget && !text.IsEmpty())
		{
			if (auto model = widget->GetDataModel())
			{
				auto renderer = node.GetCellRenderer(*model, column);
				if (renderer && renderer->IsMarkupEnabled())
				{
					return renderer->StripMarkup(text);
				}
			}
		}
		return text;
	}

	bool ToolTip::IsNull() const noexcept
	{
		if (HasAnyIcon())
		{
			return !m_Caption.IsEmpty() && !m_Message.IsEmpty();
		}
		return !m_Message.IsEmpty();
	}

	bool ToolTip::Show(const IDataViewNode& node, const IDataViewColumn& column)
	{
		auto widget = column.GetOwningWdget();
		if (widget && !m_Caption.IsEmpty())
		{
			UI::ToolTipEx tooltip(widget->GetWxWindow());
			tooltip.SetCaption(StripMarkupIfNeeded(node, column, m_Caption));
			tooltip.SetMessage(StripMarkupIfNeeded(node, column, m_Message));

			tooltip.RemoveOption(UI::ToolTipExStyle::LargeIcons);
			if (auto icon = GetIconBitmap())
			{
				tooltip.SetIcon(icon.ToGDIBitmap());
			}
			else
			{
				tooltip.SetIcon(GetIconID());
			}

			return tooltip.Popup(AdjustPopupPosition(node, column, GetPopupPosition(node, column)));
		}
		else
		{
			if (auto window = widget->GetWxWindow())
			{
				window->SetToolTip(StripMarkupIfNeeded(node, column, m_Message));
				return window->GetToolTip() != nullptr;
			}
		}
		return false;
	}
}
