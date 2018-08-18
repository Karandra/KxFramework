#include "KxStdAfx.h"
#include "KxFramework/KxThumbView.h"
#include "KxFramework/KxPanel.h"
#include "KxFramework/KxUtility.h"

KxThumbViewItem::KxThumbViewItem(const wxBitmap& bitmap)
	:m_Bitmap(bitmap)
{
}
KxThumbViewItem::~KxThumbViewItem()
{
}

//////////////////////////////////////////////////////////////////////////
wxDEFINE_EVENT(KxEVT_THUMBVIEW_SELECTED, wxCommandEvent);
wxDEFINE_EVENT(KxEVT_THUMBVIEW_ACTIVATED, wxCommandEvent);
wxDEFINE_EVENT(KxEVT_THUMBVIEW_CONTEXT_MENU, wxContextMenuEvent);

wxIMPLEMENT_DYNAMIC_CLASS(KxThumbView, wxVScrolledWindow);

const wxSize KxThumbView::DefaultThumbSize = wxSize(256, 144);
const float KxThumbView::ThumbPaddingScale = 0.9f;

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
	m_HasCahnges = true;
	UpdateView();
	event.Skip();
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

	bool isSelectionEvent = IsSelectionEvent(event);
	m_Focused = (size_t)-1;
	if (isSelectionEvent)
	{
		m_Selected = (size_t)-1;
	}

	int itemsInRow = CalcItemsPerRow();
	wxPoint pos = event.GetPosition();
	int row = VirtualHitTest(pos.y);
	if (row != wxNOT_FOUND)
	{
		if (pos.x != 0)
		{
			const size_t beginRow = GetVisibleRowsBegin();
			for (int columnIndex = 0; columnIndex < itemsInRow; columnIndex++)
			{
				wxRect thumbRect = GetFullThumbRect(row, columnIndex, beginRow);
				if (pos.x >= thumbRect.GetLeft() && pos.x <= thumbRect.GetRight())
				{
					size_t index = GetIndexByRowColumn(row, columnIndex, itemsInRow);
					m_Focused = index < GetThumbsCount() ? index : (size_t)-1;

					if (isSelectionEvent)
					{
						m_Selected = index < GetThumbsCount() ? index : (size_t)-1;
					}
					RefreshRect(GetClientRect());

					if (isSelectionEvent)
					{
						SendEvents(event);
					}
					return;
				}
			}
		}
	}

	SendEvents(event);
	RefreshRect(GetClientRect());
}
void KxThumbView::OnFocusLost(wxFocusEvent& event)
{
	m_Focused = (size_t)-1;
	RefreshRect(GetClientRect());
	event.Skip();
}
void KxThumbView::OnInternalIdle()
{
	UpdateView();
	wxVScrolledWindow::OnInternalIdle();
}
void KxThumbView::UpdateView()
{
	bool shouldDoRefresh = false;

	if (m_Focused != (size_t)-1 && !IsMouseInWindow())
	{
		m_Focused = (size_t)-1;
		shouldDoRefresh = true;
	}
	if (m_HasCahnges)
	{
		SetRowCount(CalcRowCount());
		shouldDoRefresh = true;
	}

	if (shouldDoRefresh)
	{
		Refresh();
	}
	m_HasCahnges = false;
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
int KxThumbView::CalcItemsPerRow() const
{
	int count = GetClientSize().GetWidth() / GetFinalThumbSize().GetWidth();
	return count == 0 ? 1 : count;
}
int KxThumbView::CalcRowCount() const
{
	return std::ceil(m_Thumbs.size() / (double)CalcItemsPerRow());
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

bool KxThumbView::Create(wxWindow* parent,
						 wxWindowID id,
						 long style
)
{
	if (wxVScrolledWindow::Create(parent, id, wxDefaultPosition, wxDefaultSize, style, "KxThumbView"))
	{
		EnableSystemTheme();
		SetDoubleBuffered(true);

		m_ThumbSize = DefaultThumbSize;
		SetRowCount(0);

		Bind(wxEVT_SIZE, &KxThumbView::OnSize, this);
		Bind(wxEVT_PAINT, &KxThumbView::OnPaint, this);
		Bind(wxEVT_MOTION, &KxThumbView::OnMouse, this);
		Bind(wxEVT_LEFT_DOWN, &KxThumbView::OnMouse, this);
		Bind(wxEVT_LEFT_DCLICK, &KxThumbView::OnMouse, this);
		Bind(wxEVT_RIGHT_UP, &KxThumbView::OnMouse, this);
		Bind(wxEVT_KILL_FOCUS, &KxThumbView::OnFocusLost, this);
		return true;
	}
	return false;
}
KxThumbView::~KxThumbView()
{
	ClearThumbs();
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
		m_ThumbSize = DefaultThumbSize;
	}
	Refresh();
}
wxSize KxThumbView::GetSpacing() const
{
	return m_Spacing;
}
void KxThumbView::SetSpacing(const wxSize& spacing)
{
	m_Spacing = spacing;
	m_Spacing.SetDefaults(wxSize(0, 0));
}

int KxThumbView::GetSelectedThumb() const
{
	return m_Selected != (size_t)-1 ? (int)m_Selected : wxNOT_FOUND;
}
void KxThumbView::SetSelectedThumb(int index)
{
	if (index >= 0 && (size_t)index < GetThumbsCount())
	{
		m_Selected = (size_t)index;
	}
	else
	{
		m_Selected = (size_t)-1;
	}
}

size_t KxThumbView::GetThumbsCount() const
{
	return m_Thumbs.size();
}
size_t KxThumbView::AddThumb(const wxBitmap& bitmap)
{
	m_HasCahnges = true;
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
			m_Focused = (size_t)-1;
		}
		if (index == m_Selected)
		{
			m_Selected = (size_t)-1;
		}

		m_HasCahnges = true;
		m_Thumbs.erase(m_Thumbs.begin() + index);
		Refresh();
	}
}
void KxThumbView::ClearThumbs()
{
	m_HasCahnges = true;

	m_Focused = (size_t)-1;
	m_Selected = (size_t)-1;
	m_Thumbs.clear();
	Refresh();
}
