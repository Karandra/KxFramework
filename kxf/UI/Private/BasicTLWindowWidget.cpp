#include "KxfPCH.h"
#include "BasicTLWindowWidget.h"
#include "kxf/Drawing/GDIRenderer/GDIIcon.h"
#include <wx/window.h>
#include <wx/toplevel.h>

namespace kxf::Private
{
	// BasicWindowWidgetBase
	wxTopLevelWindow* BasicTLWindowWidgetBase::GetWxWindow() const noexcept
	{
		return static_cast<wxTopLevelWindow*>(BasicWindowWidgetBase::GetWxWindow());
	}

	// BasicTopLevelWindowWidgetBase
	BitmapImage BasicTLWindowWidgetBase::GetIcon() const
	{
		return GDIIcon(GetWxWindow()->GetIcon());
	}
	void BasicTLWindowWidgetBase::SetIcon(const BitmapImage& icon)
	{
		GetWxWindow()->SetIcon(icon.ToGDIIcon().ToWxIcon());
	}

	ImageBundle BasicTLWindowWidgetBase::GetIconPack() const
	{
		return GetWxWindow()->GetIcons();
	}
	void BasicTLWindowWidgetBase::SetIconPack(const ImageBundle& icon)
	{
		GetWxWindow()->SetIcons(icon.ToWxIconBundle());
	}

	String BasicTLWindowWidgetBase::GetTitle() const
	{
		return GetWxWindow()->GetTitle();
	}
	void BasicTLWindowWidgetBase::SetTitle(const String& title)
	{
		GetWxWindow()->SetTitle(title);
	}

	// State
	bool BasicTLWindowWidgetBase::IsMinimized() const
	{
		return GetWxWindow()->IsIconized();
	}
	void BasicTLWindowWidgetBase::Minimize()
	{
		GetWxWindow()->Iconize();
	}

	bool BasicTLWindowWidgetBase::IsMaximized() const
	{
		return GetWxWindow()->IsMaximized();
	}
	void BasicTLWindowWidgetBase::Maximize()
	{
		return GetWxWindow()->Maximize();
	}
	void BasicTLWindowWidgetBase::Restore()
	{
		GetWxWindow()->Restore();
	}

	bool BasicTLWindowWidgetBase::EnableCloseButton(bool enable)
	{
		return GetWxWindow()->EnableCloseButton(enable);
	}
	bool BasicTLWindowWidgetBase::EnableMinimizeButton(bool enable)
	{
		return GetWxWindow()->EnableMinimizeButton(enable);
	}
	bool BasicTLWindowWidgetBase::EnableMaximizeButton(bool enable)
	{
		return GetWxWindow()->EnableMaximizeButton(enable);
	}

	bool BasicTLWindowWidgetBase::IsFullScreen() const
	{
		return GetWxWindow()->IsFullScreen();
	}
	bool BasicTLWindowWidgetBase::ShowFullScreen(bool show)
	{
		return GetWxWindow()->ShowFullScreen(show);
	}
	void BasicTLWindowWidgetBase::ShowWithoutActivation()
	{
		GetWxWindow()->ShowWithoutActivating();
	}

	// Misc
	std::shared_ptr<IWidget> BasicTLWindowWidgetBase::GetDefaultWidget() const
	{
		if (auto window = GetWxWindow()->GetDefaultItem())
		{
			return IWidget::FindByWindow(*window);
		}
		return nullptr;
	}
	void BasicTLWindowWidgetBase::SetDefaultWidget(const IWidget& widget)
	{
		GetWxWindow()->SetDefaultItem(widget.GetWxWindow());
	}

	std::shared_ptr<IPopupMenu> BasicTLWindowWidgetBase::GetSystemMenu() const
	{
		return nullptr;
	}
	void BasicTLWindowWidgetBase::RequestUserAttention(FlagSet<StdIcon> icon)
	{
		FlagSet<int> flags;
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Authentication));
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Information));
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Question));
		flags.Add(wxUSER_ATTENTION_INFO, icon.Contains(StdIcon::Warning));
		flags.Add(wxUSER_ATTENTION_ERROR, icon.Contains(StdIcon::Error));

		GetWxWindow()->RequestUserAttention(*flags);
	}

	bool BasicTLWindowWidgetBase::ShouldPreventApplicationExit() const
	{
		return m_PreventApplicationExit;
	}
	void BasicTLWindowWidgetBase::SetPreventApplicationExit(bool enable)
	{
		m_PreventApplicationExit = enable;
	}
}
