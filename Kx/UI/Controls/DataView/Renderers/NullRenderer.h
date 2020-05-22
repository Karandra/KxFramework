#pragma once
#include "../Renderer.h"

namespace KxFramework::UI::DataView
{
	class KX_API NullRenderer: public Renderer
	{
		protected:
			bool SetValue(const wxAny& value)
			{
				return false;
			}

			void DrawCellContent(const Rect& cellRect, CellState cellState) override
			{
			}
			Size GetCellSize() const override
			{
				return Size(0, 0);
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
