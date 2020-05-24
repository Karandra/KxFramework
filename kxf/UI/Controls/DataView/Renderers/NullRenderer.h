#pragma once
#include "../Renderer.h"

namespace kxf::UI::DataView
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
			NullRenderer(FlagSet<Alignment> = Alignment::Invalid)
				:Renderer(Alignment::Invalid)
			{
			}
			
		public:
			FlagSet<Alignment> GetEffectiveAlignment() const override
			{
				return Alignment::Invalid;
			}
			
		public:
			String GetTextValue(const wxAny& value) const override
			{
				return {};
			}
	};
}
