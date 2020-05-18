#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"

namespace KxFramework::UI::DataView
{
	class KX_API NullRenderer: public Renderer
	{
		protected:
			bool SetValue(const wxAny& value)
			{
				return false;
			}

			void DrawCellContent(const wxRect& cellRect, CellState cellState) override
			{
			}
			wxSize GetCellSize() const override
			{
				return wxSize(0, 0);
			}

		public:
			NullRenderer(int = wxALIGN_INVALID)
				:Renderer(wxALIGN_INVALID)
			{
			}
			
		public:
			wxAlignment GetEffectiveAlignment() const override
			{
				return wxALIGN_INVALID;
			}
			
		public:
			wxString GetTextValue(const wxAny& value) const override
			{
				return {};
			}
	};
}
