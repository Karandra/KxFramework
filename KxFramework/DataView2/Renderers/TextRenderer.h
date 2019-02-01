#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Renderer.h"

namespace KxDataView2
{
	class KX_API TextRenderer: public Renderer
	{
		public:
			static bool GetValueAsString(const wxAny& value, wxString& string)
			{
				return value.GetAs(&string);
			}
			static wxString GetValueAsString(const wxAny& value)
			{
				wxString string;
				GetValueAsString(value, string);
				return string;
			}

		private:
			wxString m_Text;

		protected:
			bool SetValue(const wxAny& value) override;

			void DrawCellContent(const wxRect& cellRect, CellState cellState) override;
			wxSize GetCellSize() const override;

		public:
			TextRenderer(int alignment = wxALIGN_INVALID)
				:Renderer(alignment)
			{
			}
	};
}
