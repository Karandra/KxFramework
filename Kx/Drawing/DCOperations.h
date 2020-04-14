#pragma once
#include "Common.h"
#include "Kx/General/Color.h"
#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/region.h>

namespace KxFramework::Drawing
{
	Color GetAreaAverageColor(const wxDC& dc, const wxRect& rect);
}

namespace KxFramework
{
	class KX_API DCClip final
	{
		private:
			wxDC& m_DC;
			wxRegion m_Region;

		public:
			DCClip(wxDC& dc, const wxRect& rect)
				:m_DC(dc), m_Region(rect)
			{
			}
			DCClip(wxDC& dc, const wxRegion& region)
				:m_DC(dc), m_Region(region)
			{
			}
			~DCClip()
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

	class DCChangeLogicalFunction final
	{
		private:
			wxDC& m_DC;
			const wxRasterOperationMode m_OriginalMode = wxRasterOperationMode::wxCLEAR;

		public:
			DCChangeLogicalFunction(wxDC& dc, wxRasterOperationMode newMode)
				:m_DC(dc), m_OriginalMode(dc.GetLogicalFunction())
			{
				m_DC.SetLogicalFunction(newMode);
			}
			~DCChangeLogicalFunction()
			{
				m_DC.SetLogicalFunction(m_OriginalMode);
			}
	};
}
