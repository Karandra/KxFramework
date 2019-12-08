#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxColor.h"

namespace KxDCUtility
{
	KxColor GetAverageColor(const wxDC& dc, const wxRect& rect);
}

namespace KxDCUtility
{
	class LogicalFunctionChanger final
	{
		private:
			wxDC& m_DC;
			const wxRasterOperationMode m_OriginalMode = wxRasterOperationMode::wxCLEAR;

		public:
			LogicalFunctionChanger(wxDC& dc, wxRasterOperationMode newMode)
				:m_DC(dc), m_OriginalMode(dc.GetLogicalFunction())
			{
				m_DC.SetLogicalFunction(newMode);
			}
			~LogicalFunctionChanger()
			{
				m_DC.SetLogicalFunction(m_OriginalMode);
			}
	};
}
