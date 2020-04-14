#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/Drawing/WithImageList.h"

class KX_API KxNotebook: public KxFramework::WithImageListWrapper<wxNotebook>
{
	public:
		static const long DefaultStyle = 0;

		KxNotebook() = default;
		KxNotebook(wxWindow* parent,
				   wxWindowID id,
				   long style = DefaultStyle
		)
		{
			Create(parent, id, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle
		);

	public:
		virtual bool ShouldInheritColours() const override
		{
			return true;
		}

	public:
		wxDECLARE_DYNAMIC_CLASS(KxNotebook);
};
