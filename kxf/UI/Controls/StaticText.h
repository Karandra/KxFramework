#pragma once
#include "kxf/UI/Common.h"
#include "kxf/UI/WindowRefreshScheduler.h"
#include <wx/stattext.h>

namespace kxf::UI
{
	enum class StaticTextStyle
	{
		None = 0,

		AlignLeft = Alignment::Left,
		AlignRight = Alignment::Right,
		AlignCenterHorizontal = wxALIGN_CENTRE_HORIZONTAL,
		NoAutoResize = wxST_NO_AUTORESIZE,

		EllipsizeStart = wxST_ELLIPSIZE_START,
		EllipsizeMiddle = wxST_ELLIPSIZE_MIDDLE,
		EllipsizeEnd = wxST_ELLIPSIZE_END,
	};
}
namespace kxf
{
	Kx_DeclareFlagSet(UI::StaticTextStyle);
}

namespace kxf::UI
{
	class KX_API StaticText: public WindowRefreshScheduler<wxStaticText>
	{
		public:
			static constexpr FlagSet<StaticTextStyle> DefaultStyle = StaticTextStyle::None;

		private:
			Color m_ColorNormal;
			Color m_ColorHighlight;
			Color m_ColorClick;

		private:
			void OnEnter(wxMouseEvent& event);
			void OnLeave(wxMouseEvent& event);
			void OnMouseDown(wxMouseEvent& event);

		public:
			StaticText() = default;
			StaticText(wxWindow* parent,
					   wxWindowID id,
					   const String& label,
					   FlagSet<StaticTextStyle> style = DefaultStyle
			)
			{
				Create(parent, id, label, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& label,
						FlagSet<StaticTextStyle> style = DefaultStyle
			);

		public:
			Color GetNormalColor()
			{
				return m_ColorNormal;
			}
			void SetNormalColor(const Color& color)
			{
				m_ColorNormal = color;
				SetForegroundColour(m_ColorNormal);
				ScheduleRefresh();
			}
			
			Color GetHighlightColor()
			{
				return m_ColorHighlight;
			}
			void SetHighlightColor(const Color& color)
			{
				m_ColorHighlight = color;
				ScheduleRefresh();
			}
			
			Color GetClickColor()
			{
				return m_ColorClick;
			}
			void SetClickColor(const Color& color)
			{
				m_ColorClick = color;
				ScheduleRefresh();
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(StaticText);
	};
}
