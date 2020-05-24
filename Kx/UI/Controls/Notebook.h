#pragma once
#include "Kx/UI/Common.h"
#include "Kx/Drawing/WithImageList.h"
#include <wx/notebook.h>

namespace kxf::UI
{
	enum class NotebookStyle
	{
		None = 0,

		Top = wxNB_TOP,
		Left = wxNB_LEFT,
		Right = wxNB_RIGHT,
		Bottom = wxNB_BOTTOM,

		Multiline = wxNB_MULTILINE,
		FixedWidth = wxNB_FIXEDWIDTH,
		NoPageTheme = wxNB_NOPAGETHEME,
	};
}
namespace kxf
{
	Kx_DeclareFlagSet(UI::NotebookStyle);
}

namespace kxf::UI
{
	class KX_API Notebook: public kxf::WithImageListWrapper<wxNotebook>
	{
		public:
			static constexpr FlagSet<NotebookStyle> DefaultStyle = NotebookStyle::Top;

		public:
			Notebook() = default;
			Notebook(wxWindow* parent,
					 wxWindowID id,
					 FlagSet<NotebookStyle> style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						FlagSet<NotebookStyle> style = DefaultStyle
			);

		public:
			bool ShouldInheritColours() const override
			{
				return true;
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(Notebook);
	};
}
