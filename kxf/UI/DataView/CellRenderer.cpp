#include "KxfPCH.h"
#include "CellRenderer.h"
#include "CellRendererHelper.h"
#include "Column.h"
#include "Node.h"
#include "../Widgets/WXUI/DataView/View.h"
#include "../Widgets/WXUI/DataView/MainWindow.h"
#include "kxf/Drawing/GraphicsRenderer.h"
#include "kxf/System/SystemInformation.h"

namespace kxf
{
	FlagSet<Alignment> IDataViewCellRenderer::GetEffectiveAlignment(const DataView::Node& node, const DataView::Column& column, FlagSet<Alignment> alignment) const
	{
		if (alignment == Alignment::Invalid)
		{
			// If we don't have an explicit alignment ourselves, use that of the
			// column in horizontal direction and default vertical alignment

			FlagSet<Alignment> titleAlignment = column.GetTitleAlignment();
			if (titleAlignment == Alignment::Invalid)
			{
				titleAlignment = Alignment::Left;
			}
			return titleAlignment|Alignment::CenterVertical;
		}
		return alignment;
	}
}

namespace kxf::DataView
{
	const CellAttributes& CellRenderer::SetupCellAttributes(CellState cellState)
	{
		auto view = m_Column->GetView();
		m_IsViewEnabled = view->IsEnabled();
		m_IsViewFocused = view->HasFocus();

		// Set up the attributes for this item if it's not empty
		m_DrawInfo.State = cellState;
		m_DrawInfo.Attributes = m_Node->GetCellAttributes(*m_Column, cellState);
		m_DrawInfo.Attributes.Options().ModOption(CellStyle::Enabled, m_IsViewEnabled);
		m_DrawInfo.Attributes.Options().ModOption(CellStyle::Editable, m_IsViewEnabled);

		return m_DrawInfo.Attributes;
	}
	void CellRenderer::SetupCellDisplayValue()
	{
		if (m_CellRenderer)
		{
			// Now check if we have a value and remember it for rendering it later.
			// Notice that we do it even if it's null, as the cell should be empty then and not show the last used value.
			m_CellRenderer->SetDisplayValue(m_Node->GetCellDisplayValue(*m_Column));
		}
	}

	void CellRenderer::DrawCellBackground(const Rect& cellRect, CellState cellState, bool noUserBackground)
	{
		auto view = m_Column->GetView();
		auto& gc = *m_DrawInfo.GraphicsContext;
		auto& renderer = gc.GetRenderer();
		auto& rendererNative = IRendererNative::Get();
		const auto& cellOptions = m_DrawInfo.Attributes.Options();
		const auto& bgOptions = m_DrawInfo.Attributes.BGOptions();

		auto drawInfo = m_DrawInfo;
		drawInfo.State = cellState;
		drawInfo.CellRect = cellRect;
		CellRendererHelper renderHelper(GetOwningWdget(), gc, drawInfo);

		auto ClipRectIfNeeded = [&](const Rect& rect)
		{
			if (view->m_Style.Contains(WidgetStyle::VerticalRules))
			{
				if (m_Column->IsDisplayedLast() && (cellState.IsHotTracked() || cellState.IsSelected()))
				{
					return rect;
				}
				return Rect(rect.GetX(), rect.GetY(), rect.GetWidth() - 1, rect.GetHeight());
			}
			return rect;
		};

		// Special backgrounds
		if (bgOptions.ContainsOption(CellBGStyle::Header))
		{
			Size offsetSize = Size(view->FromDIP(wxSize(0, 1)));

			Rect buttonRect = cellRect;
			buttonRect.Width() += offsetSize.GetWidth();
			buttonRect.Height() += offsetSize.GetHeight();

			rendererNative.DrawHeaderButton(view, gc, buttonRect, renderHelper.GetControlFlags(cellState));
			if (!cellState.IsSelected())
			{
				Color lineColor = System::GetColor(SystemColor::Light3D);
				lineColor.SetAlpha8(48);

				auto pen = renderer.CreatePen(lineColor);
				m_DrawInfo.GraphicsContext->DrawLine(buttonRect.GetLeftBottom(), buttonRect.GetRightBottom(), *pen);
			}
		}
		else if (bgOptions.ContainsOption(CellBGStyle::Button))
		{
			Rect buttonRect = cellRect.Clone().Inflate(1, 1);
			rendererNative.DrawPushButton(view, gc, buttonRect, renderHelper.GetControlFlags(cellState));
		}
		else if (bgOptions.ContainsOption(CellBGStyle::ComboBox))
		{
			rendererNative.DrawComboBox(view, gc, cellRect, renderHelper.GetControlFlags(cellState));
		}

		// Solid background color
		if (cellOptions.HasBackgroundColor())
		{
			auto brush = renderer.CreateSolidBrush(cellOptions.GetBackgroundColor());
			gc.DrawRectangle(cellRect, *brush);
		}

		// Call derived class drawing
		if (m_CellRenderer && !noUserBackground)
		{
			m_CellRenderer->DrawBackground(*m_Node, *m_Column, drawInfo);
		}
	}
	std::pair<Size, Rect> CellRenderer::DrawCellContent(const Rect& cellRect, CellState cellState)
	{
		auto& gc = *m_DrawInfo.GraphicsContext;
		auto& renderer = gc.GetRenderer();
		auto& rendererNative = IRendererNative::Get();
		const auto& cellOptions = m_DrawInfo.Attributes.Options();
		const auto& fontOptions = m_DrawInfo.Attributes.FontOptions();
		const auto& bgOptions = m_DrawInfo.Attributes.BGOptions();

		auto view = m_Column->GetView();
		const bool isEnabled = cellOptions.ContainsOption(CellStyle::Enabled);

		auto drawInfo = m_DrawInfo;
		drawInfo.State = cellState;
		drawInfo.CellRect = cellRect;
		CellRendererHelper renderHelper(GetOwningWdget(), gc, drawInfo);

		// Change text color
		GraphicsAction::ChangeFontBrush changeFontBrush(gc);
		if (Color textColor = cellOptions.GetForegroundColor(); textColor || !isEnabled)
		{
			if (!textColor)
			{
				textColor = view->GetForegroundColour();
			}
			if (!isEnabled)
			{
				textColor = textColor.MakeDisabled();
			}
			changeFontBrush.Set(textColor);
		}

		// Change font
		GraphicsAction::ChangeFont changeFont(gc);
		if (!fontOptions.IsDefault())
		{
			changeFont.Set(renderer.CreateFont(m_DrawInfo.Attributes.GetEffectiveFont(view->GetFont())));
		}

		// Adjust the rectangle ourselves to account for the alignment
		const Size cellSize = GetEffectiveCellSize();
		Rect adjustedCellRect = cellRect;

		// Restrict height to row height
		if (view->m_Style.Contains(WidgetStyle::VariableRowHeight))
		{
			adjustedCellRect.SetHeight(view->m_ClientArea->GetVariableRowHeight(*m_Node));
		}

		// Take alignment into account only if there is enough space, otherwise show as much contents as possible.
		const auto alignment = cellOptions.HasAlignment() ? cellOptions.GetAlignment() : GetEffectiveAlignment();

		if (cellSize.GetWidth() >= 0 && cellSize.GetWidth() < cellRect.GetWidth())
		{
			if (alignment & Alignment::CenterHorizontal)
			{
				adjustedCellRect.X() += renderHelper.CalcCenter(cellRect.GetWidth(), cellSize.GetWidth());
			}
			else if (alignment & Alignment::Right)
			{
				adjustedCellRect.X() += cellRect.GetWidth() - cellSize.GetWidth();
			}
			adjustedCellRect.SetWidth(cellSize.GetWidth());
		}
		if (cellSize.GetHeight() >= 0 && cellSize.GetHeight() < cellRect.GetHeight())
		{
			if (alignment & Alignment::CenterVertical)
			{
				adjustedCellRect.Y() += renderHelper.CalcCenter(cellRect.GetHeight(), cellSize.GetHeight());
			}
			else if (alignment & Alignment::Bottom)
			{
				adjustedCellRect.Y() += cellRect.GetHeight() - cellSize.GetHeight();
			}
			adjustedCellRect.SetHeight(cellSize.GetHeight());
		}

		// Draw highlighting selection
		if (cellOptions.ContainsOption(CellStyle::HighlightItem) && !adjustedCellRect.IsEmpty())
		{
			Rect highlightRect = Rect(adjustedCellRect).Inflate(2);
			rendererNative.DrawItemSelectionRect(view, gc, highlightRect, cellState.ToNativeWidgetFlags(view->m_Widget));
		}

		// Call derived class drawing
		if (m_CellRenderer)
		{
			drawInfo.CellRect = adjustedCellRect;
			m_CellRenderer->DrawContent(*m_Node, *m_Column, drawInfo);
		}

		return {cellSize, adjustedCellRect};
	}
	void CellRenderer::OnActivateCell(Node& node, const Rect& cellRect, const WidgetMouseEvent* mouseEvent)
	{
		if (m_CellRenderer)
		{
			auto drawInfo = m_DrawInfo;
			drawInfo.CellRect = cellRect;
			drawInfo.MouseEvent = mouseEvent;

			Any value = m_CellRenderer->OnActivate(node, *m_Column, drawInfo);
			if (!value.IsNull() && node.SetCellValue(const_cast<Column&>(*m_Column), value))
			{
				node.RefreshCell(const_cast<Column&>(*m_Column));
			}
		}
	}

	IDataViewWidget& CellRenderer::GetOwningWdget() const
	{
		return m_Column->m_View->m_Widget;
	}
}
