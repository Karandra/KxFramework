#pragma once
#include "kxf/UI/Common.h"
#include <wx/collpane.h>

namespace kxf::UI
{
	enum class CollapsiblePaneStyle: uint32_t
	{
		None = 0,

		NoTLWResize = wxCP_NO_TLW_RESIZE,
	};
}
namespace kxf
{
	KxFlagSet_Declare(UI::CollapsiblePaneStyle);
	KxFlagSet_Extend(UI::CollapsiblePaneStyle, UI::WindowStyle);
}

namespace kxf::UI
{
	class KX_API CollapsiblePane: public wxCollapsiblePane
	{
		public:
			static constexpr FlagSet<CollapsiblePaneStyle> DefaultStyle = CollapsiblePaneStyle::NoTLWResize|(WindowStyle::TabTraversal|WindowBorder::None);

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
							FlagSet<CollapsiblePaneStyle> style = DefaultStyle
			)
				:CollapsiblePane()
			{
				Create(parent, id, label, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const wxString& label,
						FlagSet<CollapsiblePaneStyle> style = DefaultStyle
			);
			~CollapsiblePane()
			{
				PopEventHandler();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(CollapsiblePane);
	};
}
