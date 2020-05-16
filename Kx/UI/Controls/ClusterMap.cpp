#include "stdafx.h"
#include "ClusterMap.h"
#include "Kx/Drawing/UxTheme.h"

namespace KxFramework::UI
{
	wxIMPLEMENT_DYNAMIC_CLASS(ClusterMap, wxControl);

	void ClusterMap::OnPaint(wxPaintEvent& event)
	{
		using namespace KxFramework;

		wxPaintDC dc(this);

		const DrawInfo drawInfo = GetDrawInfo();
		UxTheme::DrawParentBackground(*this, dc, wxRect({0, 0}, drawInfo.ClientSize));

		if (m_ItemCount != 0 && m_ItemSize > 0)
		{
			constexpr float contrast = 0.7f;
			const Color color = GetForegroundColour();
			dc.SetBrush(color);
			dc.SetPen(color.GetContrastColor(color.ChangeLightness(Angle::FromNormalized(contrast)), color.ChangeLightness(Angle::FromNormalized(-contrast))));

			wxPoint pos;
			int blocksDrawn = 0;
			for (size_t i = 0; i < m_ItemCount; i++)
			{
				// Draw the block
				wxRect blockRect(pos.x, pos.y, m_ItemSize, m_ItemSize);
				if (m_UnderMouseIndex != -1 && i == m_UnderMouseIndex)
				{
					blockRect.Deflate(FromDIP(wxSize(1, 1)));
				}
				dc.DrawRectangle(blockRect);
				blocksDrawn++;

				// Move right
				pos.x += drawInfo.Increment;

				// See if we need to move one row down

				if (blocksDrawn == drawInfo.ItemsY)
				{
					// Rewind to left side and move down 
					pos.x = 0;
					pos.y += drawInfo.Increment;

					// Reset counter
					blocksDrawn = 0;
				}
			}
		}
	}
	void ClusterMap::OnMouse(wxMouseEvent& event)
	{
		int index = HitTest(event.GetPosition());
		if (index != m_UnderMouseIndex)
		{
			m_UnderMouseIndex = index;
			Refresh();

			if (index != -1)
			{
				wxCommandEvent evt(wxEVT_TOOL_ENTER, GetId());
				evt.SetEventObject(this);
				evt.SetInt(m_UnderMouseIndex);

				ProcessWindowEvent(evt);
			}
		}
	}
	void ClusterMap::OnMouseLeave(wxMouseEvent& event)
	{
		m_UnderMouseIndex = -1;
		Refresh();
	}
	void ClusterMap::OnLeftUp(wxMouseEvent& event)
	{
		if (m_UnderMouseIndex != -1 && event.LeftUp())
		{
			wxCommandEvent evt(wxEVT_BUTTON, GetId());
			evt.SetEventObject(this);
			evt.SetInt(m_UnderMouseIndex);

			ProcessWindowEvent(evt);
		}
	}

	auto ClusterMap::GetDrawInfo() const -> DrawInfo
	{
		DrawInfo info;
		info.ClientSize = GetClientSize();
		info.Increment = m_ItemSize + m_Spacing;
		info.ItemsY = info.ClientSize.GetWidth() / info.Increment;
		info.ItemsX = std::ceil((float)m_ItemCount / info.ItemsY);

		return info;
	}

	wxPoint ClusterMap::CoordToXY(const DrawInfo& drawInfo, const wxPoint& pos) const
	{
		// See if we are inside actual space used by blocks
		const wxRect effectiveRect(0, 0, drawInfo.ItemsY * drawInfo.Increment, drawInfo.ItemsX * drawInfo.Increment);
		if (effectiveRect.Contains(pos))
		{
			return {pos.x / drawInfo.Increment, pos.y / drawInfo.Increment};
		}
		return wxDefaultPosition;
	}
	int ClusterMap::CoordToIndex(const DrawInfo& drawInfo, const wxPoint& pos) const
	{
		wxPoint xy = CoordToXY(drawInfo, pos);
		if (xy.IsFullySpecified())
		{
			int index = XYToIndex(drawInfo, xy);
			if (index != -1 && XYToCoordRect(drawInfo, xy).Contains(pos))
			{
				return index;
			}
		}
		return -1;
	}

	wxPoint ClusterMap::IndexToXY(const DrawInfo& drawInfo, int index) const
	{
		if (index >= 0 && (size_t)index < m_ItemCount)
		{
			const int y = index / drawInfo.ItemsY;
			const int x = index - (y * drawInfo.ItemsY) - 1;

			return {x, y};
		}
		return wxDefaultPosition;
	}
	int ClusterMap::XYToIndex(const DrawInfo& drawInfo, const wxPoint& xy) const
	{
		if (xy.IsFullySpecified())
		{
			int index = -1;
			if (xy.y == 0)
			{
				index = xy.x;
			}
			else
			{
				index = (xy.y * drawInfo.ItemsY) + xy.x;
			}

			if (index >= 0 && (size_t)index < m_ItemCount)
			{
				return index;
			}
		}
		return -1;
	}

	wxRect ClusterMap::XYToCoordRect(const DrawInfo& drawInfo, const wxPoint& xy) const
	{
		return {xy.x * drawInfo.Increment, xy.y * drawInfo.Increment, m_ItemSize, m_ItemSize};
	}
	wxRect ClusterMap::IndexToCoordRect(const DrawInfo& drawInfo, int index) const
	{
		wxPoint xy = IndexToXY(drawInfo, index);
		if (xy.IsFullySpecified())
		{
			return wxRect(xy.x * drawInfo.Increment, xy.y * drawInfo.Increment, m_ItemSize, m_ItemSize);
		}
		return {};
	}

	bool ClusterMap::Create(wxWindow* parent,
							wxWindowID id,
							const wxPoint& pos,
							const wxSize& size,
							long style
	)
	{
		PushEventHandler(&m_EvtHandler);
		if (wxSystemThemedControl::Create(parent, id, pos, size, style|wxFULL_REPAINT_ON_RESIZE))
		{
			EnableSystemTheme();
			SetDoubleBuffered(true);
			SetBackgroundStyle(wxBG_STYLE_PAINT);

			m_EvtHandler.Bind(wxEVT_PAINT, &ClusterMap::OnPaint, this);
			m_EvtHandler.Bind(wxEVT_MOTION, &ClusterMap::OnMouse, this);
			m_EvtHandler.Bind(wxEVT_ENTER_WINDOW, &ClusterMap::OnMouse, this);
			m_EvtHandler.Bind(wxEVT_LEAVE_WINDOW, &ClusterMap::OnMouseLeave, this);
			m_EvtHandler.Bind(wxEVT_LEFT_UP, &ClusterMap::OnLeftUp, this);
			return true;
		}
		return false;
	}
	ClusterMap::~ClusterMap()
	{
		PopEventHandler();
	}
}
