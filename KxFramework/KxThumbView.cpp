#include "KxStdAfx.h"
#include "KxFramework/KxThumbView.h"
#include "KxFramework/KxPanel.h"
#include "KxFramework/KxUtility.h"

KxEVENT_DEFINE_GLOBAL(wxCommandEvent, THUMBVIEW_SELECTED);
KxEVENT_DEFINE_GLOBAL(wxCommandEvent, THUMBVIEW_ACTIVATED);
KxEVENT_DEFINE_GLOBAL(wxContextMenuEvent, THUMBVIEW_CONTEXT_MENU);

wxIMPLEMENT_DYNAMIC_CLASS(KxThumbView, wxVScrolledWindow);

const wxSize KxThumbView::DefaultThumbSize = wxSize(256, 144);
const double KxThumbView::ThumbPaddingScale = 0.9;

void KxThumbView::OnPaint(wxPaintEvent& event)
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
				wxRect thumbRect = GetFullThumbRect(row, columnIndex, beginRow);
				if (thumbIndex == m_Focused)
				{
					wxRendererNative::Get().DrawItemSelectionRect(this, dc, thumbRect, wxCONTROL_CURRENT|wxCONTROL_CELL);
				}
				if (thumbIndex == m_Selected)
				{
					wxRendererNative::Get().DrawItemSelectionRect(this, dc, thumbRect, wxCONTROL_SELECTED|wxCONTROL_CURRENT|wxCONTROL_FOCUSED);
				}

				const wxBitmap& bitmap = m_Thumbs[thumbIndex].GetBitmap();
				wxPoint newPos = thumbRect.GetTopLeft() + (m_ThumbSize - bitmap.GetSize()) / 2;
				dc.DrawBitmap(bitmap, newPos);
			}
			else
			{
				return;
			}
		}
	}
}
void KxThumbView::OnSize(wxSizeEvent& event)
{
	event.Skip();

	UpdateRowCount();
	Refresh();
}
void KxThumbView::OnMouse(wxMouseEvent& event)
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
			return KxEVT_THUMBVIEW_SELECTED;
		}
		if (event.GetEventType() == wxEVT_LEFT_DCLICK)
		{
			return KxEVT_THUMBVIEW_ACTIVATED;
		}
		if (event.GetEventType() == wxEVT_RIGHT_UP)
		{
			return KxEVT_THUMBVIEW_CONTEXT_MENU;
		}
		return wxEVT_NULL;
	};
	auto SendEvents = [this, &TranslateEventType](wxMouseEvent& event)
	{
		wxEventType type = TranslateEventType(event);
		if (type == KxEVT_THUMBVIEW_CONTEXT_MENU)
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
	const wxPoint pos = event.GetPosition();
	const int row = VirtualHitTest(pos.y);

	const size_t oldSelection = m_Selected;
	const size_t oldFocus = m_Focused;
	if (row != wxNOT_FOUND && pos.x != 0)
	{
		const size_t beginRow = GetVisibleRowsBegin();
		for (int columnIndex = 0; columnIndex < itemsInRow; columnIndex++)
		{
			const wxRect thumbRect = GetFullThumbRect(row, columnIndex, beginRow);
			if (pos.x >= thumbRect.GetLeft() && pos.x <= thumbRect.GetRight())
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
void KxThumbView::OnKillFocus(wxFocusEvent& event)
{
	event.Skip();

	m_Focused = InvalidItemIndex;
	ScheduleRefresh();
}

size_t KxThumbView::GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const
{
	return row * itemsInRow + columnIndex;
}
wxRect KxThumbView::GetThumbRect(size_t row, size_t columnIndex, size_t beginRow)
{
	return wxRect(wxPoint(columnIndex * m_ThumbSize.GetWidth(), row * m_ThumbSize.GetHeight()), m_ThumbSize);
}
wxRect KxThumbView::GetFullThumbRect(size_t row, size_t columnIndex, size_t beginRow)
{
	wxPoint pos(columnIndex * m_ThumbSize.GetWidth(), row * m_ThumbSize.GetHeight());
	if (columnIndex != 0)
	{
		pos.x += m_Spacing.x * columnIndex;
	}
	if (row != 0 && row != beginRow)
	{
		pos.y += m_Spacing.y * (row - beginRow);
	}
	return wxRect(pos, m_ThumbSize);
}
wxSize KxThumbView::GetFinalThumbSize() const
{
	return m_ThumbSize + m_Spacing;
}
size_t KxThumbView::CalcItemsPerRow() const
{
	int count = GetClientSize().GetWidth() / GetFinalThumbSize().GetWidth();
	return count == 0 ? 1 : count;
}

size_t KxThumbView::CalcRowCount() const
{
	return std::ceil(m_Thumbs.size() / (double)CalcItemsPerRow());
}
void KxThumbView::UpdateRowCount()
{
	const size_t oldCount = GetRowCount();
	const size_t newCount = CalcRowCount();

	if (oldCount != newCount)
	{
		SetRowCount(newCount);
	}
}
KxThumbViewItem& KxThumbView::GetThumb(size_t i)
{
	return m_Thumbs[i];
}
wxBitmap KxThumbView::CreateThumb(const wxBitmap& bitmap, const wxSize& size) const
{
	wxBitmap result(size, 32);
	result.UseAlpha(true);
	wxMemoryDC dc(result);
	wxGCDC gcdc(dc);
	gcdc.SetBackground(*wxTRANSPARENT_BRUSH);
	gcdc.Clear();

	KxDrawablePanel::DrawScaledBitmap(gcdc.GetGraphicsContext(), bitmap, wxRect(wxPoint(0, 0), size), KxDrawablePanel::ScaleMode::Scale_AspectFit);
	return result;
}

void KxThumbView::OnInternalIdle()
{
	if (m_Focused != InvalidItemIndex && !IsMouseInWindow())
	{
		m_Focused = InvalidItemIndex;
		ScheduleRefresh();
	}

	wxVScrolledWindow::OnInternalIdle();
	WindowRefreshScheduler::OnInternalIdle();
}

bool KxThumbView::Create(wxWindow* parent,
						 wxWindowID id,
						 long style
)
{
	if (wxVScrolledWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize, style, "KxThumbView"))
	{
		EnableSystemTheme();
		SetDoubleBuffered(true);

		m_ThumbSize = FromDIP(DefaultThumbSize);
		SetRowCount(0);

		Bind(wxEVT_SIZE, &KxThumbView::OnSize, this);
		Bind(wxEVT_PAINT, &KxThumbView::OnPaint, this);
		Bind(wxEVT_MOTION, &KxThumbView::OnMouse, this);
		Bind(wxEVT_LEFT_DOWN, &KxThumbView::OnMouse, this);
		Bind(wxEVT_LEFT_DCLICK, &KxThumbView::OnMouse, this);
		Bind(wxEVT_RIGHT_UP, &KxThumbView::OnMouse, this);
		Bind(wxEVT_KILL_FOCUS, &KxThumbView::OnKillFocus, this);
		return true;
	}
	return false;
}
KxThumbView::~KxThumbView()
{
}

wxSize KxThumbView::GetThumbSize() const
{
	return m_ThumbSize;
}
void KxThumbView::SetThumbSize(const wxSize& size)
{
	m_ThumbSize = size;
	if (!m_ThumbSize.IsFullySpecified() || m_ThumbSize == wxSize(0, 0))
	{
		m_ThumbSize = FromDIP(DefaultThumbSize);
	}

	ScheduleRefresh();
}
wxSize KxThumbView::GetSpacing() const
{
	return m_Spacing;
}
void KxThumbView::SetSpacing(const wxSize& spacing)
{
	m_Spacing = spacing;
	m_Spacing.SetDefaults(wxSize(0, 0));

	ScheduleRefresh();
}

int KxThumbView::GetSelectedThumb() const
{
	return m_Selected != InvalidItemIndex ? (int)m_Selected : wxNOT_FOUND;
}
void KxThumbView::SetSelectedThumb(int index)
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

size_t KxThumbView::GetThumbsCount() const
{
	return m_Thumbs.size();
}
size_t KxThumbView::AddThumb(const wxBitmap& bitmap)
{
	ScheduleRefresh();
	UpdateRowCount();

	m_Thumbs.emplace_back(KxThumbViewItem(CreateThumb(bitmap, wxSize(m_ThumbSize).Scale(ThumbPaddingScale, ThumbPaddingScale))));
	return m_Thumbs.size() - 1;
}
size_t KxThumbView::AddThumb(const wxString& filePath, wxBitmapType type, int index)
{
	wxImage image;
	image.SetOption(wxIMAGE_OPTION_MAX_WIDTH, m_ThumbSize.GetWidth());
	image.SetOption(wxIMAGE_OPTION_MAX_HEIGHT, m_ThumbSize.GetHeight());
	image.LoadFile(filePath, type, index);

	return AddThumb(wxBitmap(image, 32));
}
size_t KxThumbView::AddThumb(wxInputStream& stream, wxBitmapType type, int index)
{
	wxImage image;
	image.SetOption(wxIMAGE_OPTION_MAX_WIDTH, m_ThumbSize.GetWidth());
	image.SetOption(wxIMAGE_OPTION_MAX_HEIGHT, m_ThumbSize.GetHeight());
	image.LoadFile(stream, type, index);

	return AddThumb(wxBitmap(image, 32));
}
void KxThumbView::RemoveThumb(size_t index)
{
	if (index < m_Thumbs.size())
	{
		if (index == m_Focused)
		{
			m_Focused = InvalidItemIndex;
		}
		if (index == m_Selected)
		{
			m_Selected = InvalidItemIndex;
		}
		m_Thumbs.erase(m_Thumbs.begin() + index);

		UpdateRowCount();
		ScheduleRefresh();
	}
}
void KxThumbView::ClearThumbs()
{
	ScheduleRefresh();
	UpdateRowCount();

	m_Focused = InvalidItemIndex;
	m_Selected = InvalidItemIndex;
	m_Thumbs.clear();
}
