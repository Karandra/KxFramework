#include "stdafx.h"
#include "Renderer.h"
#include "Column.h"
#include "Node.h"
#include "View.h"
#include "MainWindow.h"
#include "kxf/Drawing/GDIRenderer.h"

namespace kxf::UI::DataView
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

	void Renderer::CallDrawCellBackground(const Rect& cellRect, CellState cellState, bool noUserBackground)
	{
		auto gdi = m_GC->QueryInterface<GDIGraphicsContext>();

		const auto& cellOptions = m_Attributes.Options();
		const auto& cellBGOptions = m_Attributes.BGOptions();

		auto ClipRectIfNeeded = [this, &cellState](const Rect& rect)
		{
			if (GetView()->ContainsWindowStyle(CtrlStyle::VerticalRules))
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
		if (cellBGOptions.ContainsOption(CellBGOption::Header))
		{
			if (gdi)
			{
				Size offsetSize = GetView()->FromDIP(wxSize(0, 1));

				Rect buttonRect = cellRect;
				buttonRect.Width() += offsetSize.GetX();
				buttonRect.Height() += offsetSize.GetY();

				GDIBitmap canvas(cellRect.GetSize(), ColorDepthDB::BPP32);
				GDIMemoryContext memDC(canvas);
				wxRendererNative::Get().DrawHeaderButton(GetView(), gdi->GetWx(), Rect(-1, 0, buttonRect.GetWidth() + 1, buttonRect.GetHeight()), GetRenderEngine().GetControlFlags(cellState), wxHDR_SORT_ICON_NONE);
				if (!cellState.IsSelected())
				{
					Color lineColor = wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT);
					lineColor.SetAlpha8(48);

					memDC.SetPen(lineColor);
					memDC.DrawLine(Point(0, 0), Point(buttonRect.GetWidth() + 1, 0));
				}

				gdi->DrawTexture(canvas, cellRect);
			}
		}
		else if (cellBGOptions.ContainsOption(CellBGOption::Button))
		{
			if (gdi)
			{
				Rect buttonRect = cellRect.Clone().Inflate(1, 1);
				wxRendererNative::Get().DrawPushButton(GetView(), gdi->GetWx(), buttonRect, GetRenderEngine().GetControlFlags(cellState));
			}
		}
		else if (cellBGOptions.ContainsOption(CellBGOption::ComboBox))
		{
			if (gdi)
			{
				if (cellOptions.ContainsOption(CellOption::Editable))
				{
					wxRendererNative::Get().DrawComboBox(GetView(), gdi->GetWx(), cellRect, GetRenderEngine().GetControlFlags(cellState));
				}
				else
				{
					wxRendererNative::Get().DrawChoice(GetView(), gdi->GetWx(), cellRect, GetRenderEngine().GetControlFlags(cellState));
				}
			}
		}

		// Solid background color
		if (cellOptions.HasBackgroundColor())
		{
			Color color = m_Attributes.Options().GetBackgroundColor();
			GraphicsAction::ChangePen changePen(*m_GC, color);
			GraphicsAction::ChangeBrush changeBrush(*m_GC, color);
			m_GC->DrawRectangle(cellRect);
		}

		// Call derived class drawing
		if (!noUserBackground)
		{
			DrawCellBackground(cellRect, cellState);
		}
	}
	void Renderer::CallDrawCellContent(const Rect& cellRect, CellState cellState)
	{
		m_PaintRect = cellRect;

		auto gdi = m_GC->QueryInterface<GDIGraphicsContext>();
		RenderEngine renderEngine = GetRenderEngine();

		// Change text color
		GraphicsAction::ChangeFontBrush changeFontBrush(*m_GC);
		if (m_Attributes.Options().HasForegroundColor())
		{
			Color textColor = m_Attributes.Options().GetForegroundColor();
			if (!m_Attributes.Options().ContainsOption(CellOption::Enabled))
			{
				textColor.MakeDisabled();
			}
			changeFontBrush.Set(textColor);
		}
		else if (!m_Attributes.Options().ContainsOption(CellOption::Enabled))
		{
			changeFontBrush.Set(GetView()->GetForegroundColour().MakeDisabled());
		}

		// Change font
		GraphicsAction::ChangeFont changeFont(*m_GC);
		if (m_Attributes.FontOptions().NeedDCAlteration())
		{
			changeFont.Set(m_GC->GetRenderer().CreateFont(m_Attributes.GetEffectiveFont(GetView()->GetFont())));
		}

		// Adjust the rectangle ourselves to account for the alignment
		const Size cellSize = GetCellSize();
		Rect adjustedCellRect(cellRect);

		// Restrict height to row height.
		if (GetView()->ContainsWindowStyle(CtrlStyle::VariableRowHeight))
		{
			adjustedCellRect.SetHeight(GetMainWindow()->GetVariableRowHeight(*m_Node));
		}

		// Take alignment into account only if there is enough space, otherwise show as much contents as possible.
		const auto alignment = m_Attributes.Options().HasAlignment() ? m_Attributes.Options().GetAlignment() : GetEffectiveAlignment();

		if (cellSize.GetWidth() >= 0 && cellSize.GetWidth() < cellRect.GetWidth())
		{
			if (alignment & Alignment::CenterHorizontal)
			{
				adjustedCellRect.X() += renderEngine.CalcCenter(cellRect.GetWidth(), cellSize.GetWidth());
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
				adjustedCellRect.Y() += renderEngine.CalcCenter(cellRect.GetHeight(), cellSize.GetHeight());
			}
			else if (alignment & Alignment::Bottom)
			{
				adjustedCellRect.Y() += cellRect.GetHeight() - cellSize.GetHeight();
			}
			adjustedCellRect.SetHeight(cellSize.GetY());
		}

		// Draw highlighting selection
		if (m_Attributes.Options().ContainsOption(CellOption::HighlightItem) && !adjustedCellRect.IsEmpty())
		{
			if (gdi)
			{
				MainWindow* mainWindow = GetMainWindow();

				Rect highlightRect = Rect(adjustedCellRect).Inflate(2);
				RenderEngine::DrawSelectionRect(mainWindow, gdi->Get(), highlightRect, cellState.ToItemState(mainWindow));
			}
		}

		// Call derived class drawing
		DrawCellContent(adjustedCellRect, cellState);
		m_PaintRect = {};
	}
	void Renderer::CallOnActivateCell(Node& node, const Rect& cellRect, const wxMouseEvent* mouseEvent)
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
	Size Renderer::GetCellSize() const
	{
		return Size(0, 0);
	}

	MainWindow* Renderer::GetMainWindow() const
	{
		return m_Column ? m_Column->GetView()->GetMainWindow() : nullptr;
	}
	View* Renderer::GetView() const
	{
		return m_Column ? m_Column->GetView() : nullptr;
	}

	FlagSet<Alignment> Renderer::GetEffectiveAlignment() const
	{
		if (m_Alignment == Alignment::Invalid)
		{
			// If we don't have an explicit alignment ourselves, use that of the
			// column in horizontal direction and default vertical alignment

			FlagSet<Alignment> columnAlignment = m_Column->GetTitleAlignment();
			if (columnAlignment == Alignment::Invalid)
			{
				columnAlignment = Alignment::Left;
			}
			return columnAlignment|Alignment::CenterVertical;
		}
		return m_Alignment;
	}
}
