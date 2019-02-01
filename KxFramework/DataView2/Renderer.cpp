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

	void Renderer::SetupCellAttributes(const Node& node, Column& column, CellState cellState)
	{
		m_Node = &node;
		m_Column = &column;
		m_Attributes.Reset();

		// Set up the attributes for this item if it's not empty. Reset attributes if they are not needed.
		if (!node.GetAttributes(m_Attributes, cellState, column))
		{
			m_Attributes.Reset();
		}
	}
	void Renderer::SetupCellValue(const Node& node, Column& column)
	{
		// Now check if we have a value and remember it for rendering it later.
		// Notice that we do it even if it's null, as the cell should be empty then
		// and not show the last used value.
		SetValue(node.GetValue(column));
	}

	void Renderer::CallDrawCellBackground(const wxRect& cellRect, CellState cellState)
	{
		wxDC& dc = GetGraphicsDC();

		if (m_Attributes.HasHeaderBackgound())
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
				lineColor.SetA(48);

				memDC.SetPen(lineColor);
				memDC.DrawLine(wxPoint(0, 0), wxPoint(buttonRect.GetWidth() + 1, 0));
			}

			dc.DrawBitmap(canvas, cellRect.GetPosition());
		}

		if (m_Attributes.HasBackgroundColor())
		{
			const KxColor& color = m_Attributes.GetBackgroundColor();
			wxDCPenChanger changePen(dc, color);
			wxDCBrushChanger changeBrush(dc, color);
			dc.DrawRectangle(cellRect);
		}
		DrawCellBackground(cellRect, cellState);
	}
	void Renderer::CallDrawCellContent(const wxRect& cellRect, CellState cellState)
	{
		RenderEngine renderEngine = GetRenderEngine();
		wxDC& dc = GetRegularDC();

		// Change text color
		wxDCTextColourChanger chnageTextColor(dc);
		if (m_Attributes.HasForegroundColor())
		{
			KxColor color = m_Attributes.GetForegroundColor();
			if (!m_Attributes.IsEnabled())
			{
				color.MakeDisabled();
			}
			chnageTextColor.Set(color);
		}
		else if (!m_Attributes.IsEnabled())
		{
			chnageTextColor.Set(GetView()->GetForegroundColour().MakeDisabled());
		}

		// Change font
		wxDCFontChanger changeFont(dc);
		if (m_Attributes.HasFontAttributes())
		{
			changeFont.Set(m_Attributes.GetEffectiveFont(dc.GetFont()));
		}

		// Adjust the rectangle ourselves to account for the alignment
		const wxSize cellSize = GetCellSize();
		wxRect adjustedCellRect(cellRect);

		// Restrict height to row height.
		if (GetView()->IsOptionEnabled(CtrlStyle::VariableRowHeight))
		{
			adjustedCellRect.SetHeight(GetMainWindow()->GetVariableRowHeight(*m_Node));
		}

		// Take alignment into account only if there is enough space, otherwise
		// show as much contents as possible.
		const wxAlignment alignment = m_Attributes.HasAlignment() ? m_Attributes.GetAlignment() : GetEffectiveAlignment();

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
		DrawCellContent(adjustedCellRect, cellState);
	}
	void Renderer::CallOnActivateCell(Node& node, const wxRect& cellRect, const wxMouseEvent* mouseEvent)
	{
		wxAny value = OnActivateCell(node, cellRect, mouseEvent);
		if (!value.IsNull() && node.SetValue(value, *m_Column))
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
		return m_Attributes.HasHeaderBackgound();
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
