#include "KxStdAfx.h"
#include "Renderer.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"

namespace KxDataView2
{
	bool Renderer::IsNullRenderer() const
	{
		MainWindow* mainWindow = GetMainWindow();
		return mainWindow ? this == &mainWindow->GetNullRenderer() : false;
	}

	void Renderer::SetupCellAttributes(CellState cellState)
	{
		// Set up the attributes for this item if it's not empty. Reset attributes if they are not needed.
		m_Attributes.Reset();
		if (!m_Node->GetAttributes(*m_Column, cellState, m_Attributes))
		{
			m_Attributes.Reset();
		}
	}
	void Renderer::SetupCellValue()
	{
		// Now check if we have a value and remember it for rendering it later.
		// Notice that we do it even if it's null, as the cell should be empty then and not show the last used value.
		SetValue(m_Node->GetValue(*m_Column));
	}

	void Renderer::CallDrawCellBackground(const wxRect& cellRect, CellState cellState, bool noUserBackground)
	{
		wxDC& dc = GetGraphicsDC();
		const auto& cellOptions = m_Attributes.Options();
		const auto& cellBGOptions = m_Attributes.BGOptions();
		
		auto ClipRectIfNeeded = [this, &cellState](const wxRect& rect)
		{
			if (GetView()->IsStyleEnabled(CtrlStyle::VerticalRules))
			{
				if (m_Column->IsDisplayedLast() && (cellState.IsHotTracked() || cellState.IsSelected()))
				{
					return rect;
				}
				return wxRect(rect.GetX(), rect.GetY(), rect.GetWidth() - 1, rect.GetHeight());
			}
			return rect;
		};

		// Special backgrounds
		if (cellBGOptions.IsEnabled(CellBGOption::Header))
		{
			wxSize offsetSize = GetView()->FromDIP(wxSize(0, 1));

			wxRect buttonRect = cellRect;
			buttonRect.width += offsetSize.x;
			buttonRect.height += offsetSize.y;

			wxBitmap canvas(cellRect.GetSize(), 32);
			wxMemoryDC memDC(canvas);
			wxRendererNative::Get().DrawHeaderButton(GetView(), memDC, wxRect(-1, 0, buttonRect.GetWidth() + 1, buttonRect.GetHeight()), GetRenderEngine().GetControlFlags(cellState), wxHDR_SORT_ICON_NONE);
			if (!cellState.IsSelected())
			{
				KxColor lineColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
				lineColor.SetAlpha8(48);

				memDC.SetPen(lineColor);
				memDC.DrawLine(wxPoint(0, 0), wxPoint(buttonRect.GetWidth() + 1, 0));
			}

			dc.DrawBitmap(canvas, cellRect.GetPosition());
		}
		else if (cellBGOptions.IsEnabled(CellBGOption::Button))
		{
			wxRect buttonRect = cellRect.Inflate(1, 1);
			wxRendererNative::Get().DrawPushButton(GetView(), dc, buttonRect, GetRenderEngine().GetControlFlags(cellState));
		}
		else if (cellBGOptions.IsEnabled(CellBGOption::ComboBox))
		{
			if (cellOptions.IsEnabled(CellOption::Editable))
			{
				wxRendererNative::Get().DrawComboBox(GetView(), dc, cellRect, GetRenderEngine().GetControlFlags(cellState));
			}
			else
			{
				wxRendererNative::Get().DrawChoice(GetView(), dc, cellRect, GetRenderEngine().GetControlFlags(cellState));
			}
		}

		// Solid background color
		if (cellOptions.HasBackgroundColor())
		{
			KxColor color = m_Attributes.Options().GetBackgroundColor();
			wxDCPenChanger changePen(dc, color);
			wxDCBrushChanger changeBrush(dc, color);
			dc.DrawRectangle(cellRect);
		}

		// Call derived class drawing
		if (!noUserBackground)
		{
			DrawCellBackground(cellRect, cellState);
		}
	}
	void Renderer::CallDrawCellContent(const wxRect& cellRect, CellState cellState, bool alwaysUseGC)
	{
		m_PaintRect = cellRect;
		m_AlwaysUseGC = alwaysUseGC;

		RenderEngine renderEngine = GetRenderEngine();
		wxDC& dc = HasRegularDC() && !m_AlwaysUseGC  ? GetRegularDC() : GetGraphicsDC();

		// Change text color
		wxDCTextColourChanger changeTextColor(dc);
		if (m_Attributes.Options().HasForegroundColor())
		{
			KxColor color = m_Attributes.Options().GetForegroundColor();
			if (!m_Attributes.Options().IsEnabled(CellOption::Enabled))
			{
				color.MakeDisabled();
			}
			changeTextColor.Set(color);
		}
		else if (!m_Attributes.Options().IsEnabled(CellOption::Enabled))
		{
			changeTextColor.Set(GetView()->GetForegroundColour().MakeDisabled());
		}

		// Change font
		wxDCFontChanger changeFont(dc);
		if (m_Attributes.FontOptions().NeedDCAlteration())
		{
			changeFont.Set(m_Attributes.GetEffectiveFont(dc.GetFont()));
		}

		// Adjust the rectangle ourselves to account for the alignment
		const wxSize cellSize = GetCellSize();
		wxRect adjustedCellRect(cellRect);

		// Restrict height to row height.
		if (GetView()->IsStyleEnabled(CtrlStyle::VariableRowHeight))
		{
			adjustedCellRect.SetHeight(GetMainWindow()->GetVariableRowHeight(*m_Node));
		}

		// Take alignment into account only if there is enough space, otherwise show as much contents as possible.
		const wxAlignment alignment = m_Attributes.Options().HasAlignment() ? m_Attributes.Options().GetAlignment() : GetEffectiveAlignment();

		if (cellSize.GetWidth() >= 0 && cellSize.GetWidth() < cellRect.GetWidth())
		{
			if (alignment & wxALIGN_CENTER_HORIZONTAL)
			{
				adjustedCellRect.x += renderEngine.CalcCenter(cellRect.GetWidth(), cellSize.GetWidth());
			}
			else if (alignment & wxALIGN_RIGHT)
			{
				adjustedCellRect.x += cellRect.GetWidth() - cellSize.GetWidth();
			}
			adjustedCellRect.SetWidth(cellSize.GetWidth());
		}
		if (cellSize.GetHeight() >= 0 && cellSize.GetHeight() < cellRect.GetHeight())
		{
			if (alignment & wxALIGN_CENTER_VERTICAL)
			{
				adjustedCellRect.y += renderEngine.CalcCenter(cellRect.GetHeight(), cellSize.GetHeight());
			}
			else if (alignment & wxALIGN_BOTTOM)
			{
				adjustedCellRect.y += cellRect.GetHeight() - cellSize.GetHeight();
			}
			adjustedCellRect.SetHeight(cellSize.y);
		}

		// Draw highlighting selection
		if (m_Attributes.Options().IsEnabled(CellOption::HighlightItem) && !adjustedCellRect.IsEmpty())
		{
			MainWindow* mainWindow = GetMainWindow();

			wxRect highlightRect = wxRect(adjustedCellRect).Inflate(2);
			RenderEngine::DrawSelectionRect(mainWindow, dc, highlightRect, cellState.ToItemState(mainWindow));
		}

		// Call derived class drawing
		DrawCellContent(adjustedCellRect, cellState);
		m_PaintRect = {};
	}
	void Renderer::CallOnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent)
	{
		wxAny value = OnActivateCell(node, cellRect, mouseEvent);
		if (!value.IsNull() && node.SetValue(*m_Column, value))
		{
			GetMainWindow()->OnCellChanged(node, m_Column);
		}
	}

	bool Renderer::HasSolidBackground() const
	{
		return !HasSpecialBackground();
	}
	bool Renderer::HasSpecialBackground() const
	{
		return !m_Attributes.BGOptions().IsDefault();
	}
	wxSize Renderer::GetCellSize() const
	{
		return wxSize(0, 0);
	}

	MainWindow* Renderer::GetMainWindow() const
	{
		return m_Column ? m_Column->GetView()->GetMainWindow() : nullptr;
	}
	View* Renderer::GetView() const
	{
		return m_Column ? m_Column->GetView() : nullptr;
	}

	wxAlignment Renderer::GetEffectiveAlignment() const
	{
		if (m_Alignment == wxALIGN_INVALID)
		{
			// If we don't have an explicit alignment ourselves, use that of the
			// column in horizontal direction and default vertical alignment

			wxAlignment columnAlignment = m_Column->GetTitleAlignment();
			if (columnAlignment == wxALIGN_INVALID)
			{
				columnAlignment = wxALIGN_LEFT;
			}
			return static_cast<wxAlignment>(columnAlignment|wxALIGN_CENTER_VERTICAL);
		}
		return m_Alignment;
	}
}
