#pragma once
#include "Kx/UI/Common.h"
#include <wx/collpane.h>

namespace KxFramework::UI
{
	enum class CollapsiblePaneStyle
	{
		None = 0,

		NoTLWResize = wxCP_NO_TLW_RESIZE,
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::CollapsiblePaneStyle);
}

namespace KxFramework::UI
{
	class KX_API CollapsiblePane: public wxCollapsiblePane
	{
		public:
			static constexpr CollapsiblePaneStyle DefaultStyle = EnumClass::Combine<CollapsiblePaneStyle>(CollapsiblePaneStyle::NoTLWResize, WindowStyle::TabTraversal, WindowBorder::None);

		private:
			wxEvtHandler m_EvtHandler;

		private:
			void OnCollapsedExpanded(wxCollapsiblePaneEvent& event);

		public:
			CollapsiblePane()
			{
				PushEventHandler(&m_EvtHandler);
			}
			CollapsiblePane(wxWindow* parent,
							wxWindowID id,
							const wxString& label,
							CollapsiblePaneStyle style = DefaultStyle
			)
				:CollapsiblePane()
			{
				Create(parent, id, label, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const wxString& label,
						CollapsiblePaneStyle style = DefaultStyle
			);
			~CollapsiblePane()
			{
				PopEventHandler();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(CollapsiblePane);
	};
}
