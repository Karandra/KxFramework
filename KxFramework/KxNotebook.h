#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithImageList.h"

class KxNotebook: public KxWithImageListWrapper<wxNotebook>
{
	public:
		static const long DefaultStyle = 0;

		KxNotebook() {}
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
