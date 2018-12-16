#pragma once
#include "KxFramework/KxFramework.h"

class KX_API KxDCClipper
{
	private:
		wxDC& m_DC;
		wxRegion m_Region;

	public:
		KxDCClipper(wxDC& dc, const wxRegion& region)
			:m_DC(dc), m_Region(region)
		{
		}
		KxDCClipper(wxDC& dc, const wxRect& rect)
			:m_DC(dc), m_Region(rect)
		{
		}
		~KxDCClipper()
		{
			if (m_Region.IsOk())
			{
				m_DC.DestroyClippingRegion();
			}
		}

	public:
		bool Add(const wxRegion& region)
		{
			return m_Region.Union(region);
		}
		bool Add(const wxRect& rect)
		{
			return m_Region.Union(rect);
		}

		bool Remove(const wxRegion& region)
		{
			return m_Region.Subtract(region);
		}
		bool Remove(const wxRect& rect)
		{
			return m_Region.Subtract(rect);
		}

		void Apply()
		{
			m_DC.SetDeviceClippingRegion(m_Region);
		}
};
