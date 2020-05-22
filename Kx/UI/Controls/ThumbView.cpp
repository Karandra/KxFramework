#include "stdafx.h"
#include "ThumbView.h"
#include "Kx/UI/Windows/DrawablePanel.h"
#include <wx/renderer.h>
#include <wx/dcgraph.h>

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ThumbView, wxVScrolledWindow);

	void ThumbView::OnPaint(wxPaintEvent& event)
	{
		wxPaintDC dc(this);
		DoPrepareDC(dc);

		const int itemsInRow = CalcItemsPerRow();
		const size_t beginRow = GetVisibleRowsBegin();
		for (size_t row = beginRow; row < GetVisibleRowsEnd(); row++)
		{
			for (size_t columnIndex = 0; columnIndex < (size_t)itemsInRow; columnIndex++)
			{
				size_t thumbIndex = GetIndexByRowColumn(row, columnIndex, itemsInRow);
				if (thumbIndex < GetThumbsCount())
				{
					Rect thumbRect = GetFullThumbRect(row, columnIndex, beginRow);
					if (thumbIndex == m_Focused)
					{
						wxRendererNative::Get().DrawItemSelectionRect(this, dc, thumbRect, wxCONTROL_CURRENT|wxCONTROL_CELL);
					}
					if (thumbIndex == m_Selected)
					{
						wxRendererNative::Get().DrawItemSelectionRect(this, dc, thumbRect, wxCONTROL_SELECTED|wxCONTROL_CURRENT|wxCONTROL_FOCUSED);
					}

					const wxBitmap& bitmap = m_Items[thumbIndex].GetBitmap();
					wxPoint newPos = thumbRect.GetTopLeft() + ((wxSize)m_ThumbSize - bitmap.GetSize()) / 2;
					dc.DrawBitmap(bitmap, newPos);
				}
				else
				{
					return;
				}
			}
		}
	}
	void ThumbView::OnSize(wxSizeEvent& event)
	{
		event.Skip();

		UpdateRowCount();
		Refresh();
	}
	void ThumbView::OnMouse(wxMouseEvent& event)
	{
		event.Skip();

		auto IsSelectionEvent = [](wxMouseEvent& event) -> bool
		{
			return event.GetEventType() == wxEVT_LEFT_DOWN || event.GetEventType() == wxEVT_LEFT_DCLICK || event.GetEventType() == wxEVT_RIGHT_UP;
		};
		auto TranslateEventType = [](wxMouseEvent& event) -> wxEventType
		{
			if (event.GetEventType() == wxEVT_LEFT_DOWN)
			{
				return EvtSelected;
			}
			if (event.GetEventType() == wxEVT_LEFT_DCLICK)
			{
				return EvtActivated;
			}
			if (event.GetEventType() == wxEVT_RIGHT_UP)
			{
				return EvtContextMenu;
			}
			return wxEVT_NULL;
		};
		auto SendEvents = [this, &TranslateEventType](wxMouseEvent& event)
		{
			wxEventType type = TranslateEventType(event);
			if (type == EvtContextMenu)
			{
				wxContextMenuEvent evt(type, GetId());
				evt.SetInt(GetSelectedThumb());
				evt.SetPosition(event.GetPosition());
				HandleWindowEvent(evt);
			}
			else if (type != wxEVT_NULL)
			{
				wxCommandEvent evt(type, GetId());
				evt.SetInt(GetSelectedThumb());
				HandleWindowEvent(evt);
			}
		};

		const bool isSelectionEvent = IsSelectionEvent(event);
		const int itemsInRow = CalcItemsPerRow();
		const Point pos = event.GetPosition();
		const int row = VirtualHitTest(pos.GetY());

		const size_t oldSelection = m_Selected;
		const size_t oldFocus = m_Focused;
		if (row != wxNOT_FOUND && pos.GetX() != 0)
		{
			const size_t beginRow = GetVisibleRowsBegin();
			for (int columnIndex = 0; columnIndex < itemsInRow; columnIndex++)
			{
				const Rect thumbRect = GetFullThumbRect(row, columnIndex, beginRow);
				if (pos.GetX() >= thumbRect.GetLeft() && pos.GetX() <= thumbRect.GetRight())
				{
					const size_t index = GetIndexByRowColumn(row, columnIndex, itemsInRow);
					const size_t focus = index < GetThumbsCount() ? index : InvalidItemIndex;
					const size_t selection = index < GetThumbsCount() ? index : InvalidItemIndex;

					if (focus != oldFocus)
					{
						ScheduleRefresh();
						m_Focused = focus;
					}
					if (isSelectionEvent && selection != oldSelection)
					{
						ScheduleRefresh();
						m_Selected = selection;
					}

					if (isSelectionEvent)
					{
						SendEvents(event);
					}
					return;
				}
			}

			m_Focused = InvalidItemIndex;
			if (isSelectionEvent)
			{
				m_Selected = InvalidItemIndex;
			}
			ScheduleRefresh();
		}

		SendEvents(event);
	}
	void ThumbView::OnKillFocus(wxFocusEvent& event)
	{
		event.Skip();

		m_Focused = InvalidItemIndex;
		ScheduleRefresh();
	}

	size_t ThumbView::GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const
	{
		return row * itemsInRow + columnIndex;
	}
	Rect ThumbView::GetThumbRect(size_t row, size_t columnIndex, size_t beginRow)
	{
		return Rect(wxPoint(columnIndex * m_ThumbSize.GetWidth(), row * m_ThumbSize.GetHeight()), (wxSize)m_ThumbSize);
	}
	Rect ThumbView::GetFullThumbRect(size_t row, size_t columnIndex, size_t beginRow)
	{
		Point pos(columnIndex * m_ThumbSize.GetWidth(), row * m_ThumbSize.GetHeight());
		if (columnIndex != 0)
		{
			pos.X() += m_Spacing.GetWidth() * columnIndex;
		}
		if (row != 0 && row != beginRow)
		{
			pos.Y() += m_Spacing.GetHeight() * (row - beginRow);
		}
		return Rect((wxPoint)pos, (wxSize)m_ThumbSize);
	}
	Size ThumbView::GetFinalThumbSize() const
	{
		return m_ThumbSize + m_Spacing;
	}
	size_t ThumbView::CalcItemsPerRow() const
	{
		int count = GetClientSize().GetWidth() / GetFinalThumbSize().GetWidth();
		return count == 0 ? 1 : count;
	}

	size_t ThumbView::CalcRowCount() const
	{
		return std::ceil(m_Items.size() / (double)CalcItemsPerRow());
	}
	void ThumbView::UpdateRowCount()
	{
		const size_t oldCount = GetRowCount();
		const size_t newCount = CalcRowCount();

		if (oldCount != newCount)
		{
			SetRowCount(newCount);
		}
	}
	ThumbViewItem& ThumbView::GetThumb(size_t i)
	{
		return m_Items[i];
	}
	wxBitmap ThumbView::CreateThumb(const wxBitmap& bitmap, const Size& size) const
	{
		wxBitmap result(size, 32);
		result.UseAlpha(true);
		wxMemoryDC dc(result);
		wxGCDC gcdc(dc);
		gcdc.SetBackground(*wxTRANSPARENT_BRUSH);
		gcdc.Clear();

		DrawablePanel::DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, Rect(Point(0, 0), (wxSize)size), BitmapScaleMode::AspectFit);
		return result;
	}

	void ThumbView::OnInternalIdle()
	{
		if (m_Focused != InvalidItemIndex && !IsMouseInWindow())
		{
			m_Focused = InvalidItemIndex;
			ScheduleRefresh();
		}

		wxVScrolledWindow::OnInternalIdle();
		WindowRefreshScheduler::OnInternalIdle();
	}

	bool ThumbView::Create(wxWindow* parent,
						   wxWindowID id,
						   WindowStyle style
	)
	{
		if (wxVScrolledWindow::Create(parent, id, Point::UnspecifiedPosition(), Size::UnspecifiedSize(), ToInt(style), wxS("ThumbView")))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);

			m_ThumbSize = FromDIP((wxSize)DefaultThumbSize);
			SetRowCount(0);

			Bind(wxEVT_SIZE, &ThumbView::OnSize, this);
			Bind(wxEVT_PAINT, &ThumbView::OnPaint, this);
			Bind(wxEVT_MOTION, &ThumbView::OnMouse, this);
			Bind(wxEVT_LEFT_DOWN, &ThumbView::OnMouse, this);
			Bind(wxEVT_LEFT_DCLICK, &ThumbView::OnMouse, this);
			Bind(wxEVT_RIGHT_UP, &ThumbView::OnMouse, this);
			Bind(wxEVT_KILL_FOCUS, &ThumbView::OnKillFocus, this);
			return true;
		}
		return false;
	}

	Size ThumbView::GetThumbSize() const
	{
		return m_ThumbSize;
	}
	void ThumbView::SetThumbSize(const Size& size)
	{
		m_ThumbSize = size;
		if (!m_ThumbSize.IsFullySpecified() || m_ThumbSize == Size(0, 0))
		{
			m_ThumbSize = FromDIP((wxSize)DefaultThumbSize);
		}

		ScheduleRefresh();
	}
	Size ThumbView::GetSpacing() const
	{
		return m_Spacing;
	}
	void ThumbView::SetSpacing(const Size& spacing)
	{
		m_Spacing = spacing;
		m_Spacing.SetDefaults(Size(0, 0));

		ScheduleRefresh();
	}

	int ThumbView::GetSelectedThumb() const
	{
		return m_Selected != InvalidItemIndex ? (int)m_Selected : wxNOT_FOUND;
	}
	void ThumbView::SetSelectedThumb(int index)
	{
		if (index >= 0 && (size_t)index < GetThumbsCount())
		{
			m_Selected = (size_t)index;
		}
		else
		{
			m_Selected = InvalidItemIndex;
		}
	}

	size_t ThumbView::GetThumbsCount() const
	{
		return m_Items.size();
	}
	size_t ThumbView::AddThumb(const wxBitmap& bitmap)
	{
		ScheduleRefresh();
		UpdateRowCount();

		m_Items.emplace_back(ThumbViewItem(CreateThumb(bitmap, Size(m_ThumbSize).Scale(ThumbPaddingScale, ThumbPaddingScale))));
		return m_Items.size() - 1;
	}
	size_t ThumbView::AddThumb(const String& filePath, wxBitmapType type, int index)
	{
		wxImage image;
		image.SetOption(wxIMAGE_OPTION_MAX_WIDTH, m_ThumbSize.GetWidth());
		image.SetOption(wxIMAGE_OPTION_MAX_HEIGHT, m_ThumbSize.GetHeight());
		image.LoadFile(filePath, type, index);

		return AddThumb(wxBitmap(image, 32));
	}
	size_t ThumbView::AddThumb(wxInputStream& stream, wxBitmapType type, int index)
	{
		wxImage image;
		image.SetOption(wxIMAGE_OPTION_MAX_WIDTH, m_ThumbSize.GetWidth());
		image.SetOption(wxIMAGE_OPTION_MAX_HEIGHT, m_ThumbSize.GetHeight());
		image.LoadFile(stream, type, index);

		return AddThumb(wxBitmap(image, 32));
	}
	void ThumbView::RemoveThumb(size_t index)
	{
		if (index < m_Items.size())
		{
			if (index == m_Focused)
			{
				m_Focused = InvalidItemIndex;
			}
			if (index == m_Selected)
			{
				m_Selected = InvalidItemIndex;
			}
			m_Items.erase(m_Items.begin() + index);

			UpdateRowCount();
			ScheduleRefresh();
		}
	}
	void ThumbView::ClearThumbs()
	{
		ScheduleRefresh();
		UpdateRowCount();

		m_Focused = InvalidItemIndex;
		m_Selected = InvalidItemIndex;
		m_Items.clear();
	}
}
