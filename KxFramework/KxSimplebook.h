#pragma once
#include "KxFramework/KxFramework.h"

class KxSimplebook: public wxSimplebook
{
	public:
		static const long DefaultStyle = 0;

		KxSimplebook() {}
		KxSimplebook(wxWindow* parent,
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
		wxDECLARE_DYNAMIC_CLASS(KxSimplebook);
};
