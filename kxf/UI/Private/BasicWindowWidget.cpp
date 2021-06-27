#include "KxfPCH.h"
#include "BasicWindowWidget.h"
#include "kxf/Drawing/GDIRenderer/GDIFont.h"
#include "kxf/Utility/Enumerator.h"
#include "kxf/Utility/String.h"
#include <wx/window.h>
#include <wx/colour.h>

#include <Windows.h>
#include <UxTheme.h>

namespace
{
	std::optional<int> MapWindowLongPtrIndex(kxf::NativeWidgetProperty index)
	{
		using namespace kxf;

		switch (index)
		{
			case NativeWidgetProperty::ID:
			{
				return GWLP_ID;
			}
			case NativeWidgetProperty::Style:
			{
				return GWL_STYLE;
			}
			case NativeWidgetProperty::ExStyle:
			{
				return GWL_EXSTYLE;
			}
			case NativeWidgetProperty::Instance:
			{
				return GWLP_HINSTANCE;
			}
			case NativeWidgetProperty::UserData:
			{
				return GWLP_USERDATA;
			}
			case NativeWidgetProperty::WindowProcedure:
			{
				return GWLP_WNDPROC;
			}

			case NativeWidgetProperty::DialogResult:
			{
				return DWLP_MSGRESULT;
			}
			case NativeWidgetProperty::DialogUserData:
			{
				return DWLP_USER;
			}
			case NativeWidgetProperty::DialogProcedure:
			{
				return DWLP_DLGPROC;
			}
		};
		return {};
	}
}

namespace kxf::Private
{
	void BasicWindowWidgetBase::OnWindowCreate(wxWindowCreateEvent& event)
	{
		event.Skip();
	}
	void BasicWindowWidgetBase::OnWindowDestroy(wxWindowDestroyEvent& event)
	{
		IWidget::DissociateWithWindow(*m_Window);
		m_Window = nullptr;
		m_ShouldDelete = false;

		event.Skip();
	}

	void BasicWindowWidgetBase::Initialize(std::unique_ptr<wxWindow> window)
	{
		if (!m_Window)
		{
			m_Window = window.release();
			m_ShouldDelete = true;

			IWidget::AssociateWithWindow(*m_Window, m_Widget);
			m_Window->Bind(wxEVT_CREATE, &BasicWindowWidgetBase::OnWindowCreate, this);
			m_Window->Bind(wxEVT_DESTROY, &BasicWindowWidgetBase::OnWindowDestroy, this);
		}
	}
	void BasicWindowWidgetBase::Uninitialize()
	{
		if (m_Window)
		{
			m_Window->Unbind(wxEVT_CREATE, &BasicWindowWidgetBase::OnWindowCreate, this);
			m_Window->Unbind(wxEVT_DESTROY, &BasicWindowWidgetBase::OnWindowDestroy, this);
			IWidget::DissociateWithWindow(*m_Window);

			if (m_ShouldDelete)
			{
				m_Window->Destroy();
				delete m_Window;
			}
		}

		m_Window = nullptr;
		m_ShouldDelete = false;
	}

	// Native interface
	void* BasicWindowWidgetBase::GetHandle() const noexcept
	{
		return m_Window ? static_cast<void*>(m_Window->GetHandle()) : nullptr;
	}

	// Lifetime management
	bool BasicWindowWidgetBase::IsWidgetAlive() const noexcept
	{
		return m_Window && !m_Window->IsBeingDeleted();
	}
	bool BasicWindowWidgetBase::CloseWidget(bool force)
	{
		if (m_Window)
		{
			return m_Window->Close(force);
		}
		return false;
	}
	bool BasicWindowWidgetBase::DestroyWidget()
	{
		if (m_Window)
		{
			return m_Window->Destroy();
		}
		return false;
	}

	// HiDPI support
	float BasicWindowWidgetBase::GetContentScaleFactor() const
	{
		return static_cast<float>(m_Window->GetDPIScaleFactor());
	}

	void BasicWindowWidgetBase::FromDIP(int& x, int& y) const
	{
		auto temp = m_Window->FromDIP(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}
	void BasicWindowWidgetBase::ToDIP(int& x, int& y) const
	{
		auto temp = m_Window->ToDIP(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}

	// Positioning functions
	Point BasicWindowWidgetBase::GetPosition() const
	{
		return Point(m_Window->GetPosition());
	}
	void BasicWindowWidgetBase::SetPosition(const Point& pos)
	{
		m_Window->SetPosition(pos);
	}

	void BasicWindowWidgetBase::Center(FlagSet<Orientation> orientation)
	{
		m_Window->Center(*Private::MapOrientation(orientation));
	}
	void BasicWindowWidgetBase::CenterOnParent(FlagSet<Orientation> orientation)
	{
		m_Window->CenterOnParent(*Private::MapOrientation(orientation));
	}

	// Size functions
	Rect BasicWindowWidgetBase::GetRect(WidgetSizeFlag sizeType) const
	{
		return Rect([&]() -> wxRect
		{
			switch (sizeType)
			{
				case WidgetSizeFlag::Widget:
				{
					return m_Window->GetRect();
				}
				case WidgetSizeFlag::WidgetMin:
				{
					return {m_Window->GetPosition(), m_Window->GetMinSize()};
				}
				case WidgetSizeFlag::WidgetMax:
				{
					return {m_Window->GetPosition(), m_Window->GetMaxSize()};
				}
				case WidgetSizeFlag::WidgetBest:
				{
					return {m_Window->GetPosition(), m_Window->GetBestSize()};
				}

				case WidgetSizeFlag::Client:
				{
					return m_Window->GetClientRect();
				}
				case WidgetSizeFlag::ClientMin:
				{
					return {m_Window->GetClientRect().GetPosition(), m_Window->GetMinClientSize()};
				}
				case WidgetSizeFlag::ClientMax:
				{
					return {m_Window->GetClientRect().GetPosition(), m_Window->GetMaxClientSize()};
				}

				case WidgetSizeFlag::Virtual:
				{
					return {{0, 0}, m_Window->GetVirtualSize()};
				}
				case WidgetSizeFlag::VirtualBest:
				{
					return {{0, 0}, m_Window->GetBestVirtualSize()};
				}

				case WidgetSizeFlag::Border:
				{
					return {{0, 0}, m_Window->GetWindowBorderSize()};
				}
			}
			return {wxDefaultCoord, wxDefaultCoord, wxDefaultCoord, wxDefaultCoord};
		}());
	}
	Size BasicWindowWidgetBase::GetSize(WidgetSizeFlag sizeType) const
	{
		return Size([&]() -> wxSize
		{
			switch (sizeType)
			{
				case WidgetSizeFlag::Widget:
				{
					return m_Window->GetSize();
				}
				case WidgetSizeFlag::WidgetMin:
				{
					return m_Window->GetMinSize();
				}
				case WidgetSizeFlag::WidgetMax:
				{
					return m_Window->GetMaxSize();
				}
				case WidgetSizeFlag::WidgetBest:
				{
					return m_Window->GetBestSize();
				}

				case WidgetSizeFlag::Client:
				{
					return m_Window->GetClientSize();
				}
				case WidgetSizeFlag::ClientMin:
				{
					return m_Window->GetMinClientSize();
				}
				case WidgetSizeFlag::ClientMax:
				{
					return m_Window->GetMaxClientSize();
				}

				case WidgetSizeFlag::Virtual:
				{
					return m_Window->GetVirtualSize();
				}
				case WidgetSizeFlag::VirtualBest:
				{
					return m_Window->GetBestVirtualSize();
				}

				case WidgetSizeFlag::Border:
				{
					return m_Window->GetWindowBorderSize();
				}
			}
			return wxDefaultSize;
		}());
	}
	void BasicWindowWidgetBase::SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags)
	{
		if (flags.Contains(WidgetSizeFlag::Widget))
		{
			m_Window->SetSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::WidgetMin))
		{
			m_Window->SetMinSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::WidgetMax))
		{
			m_Window->SetMaxSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::WidgetBest))
		{
			if (size.IsFullySpecified())
			{
				m_Window->CacheBestSize(size);
			}
			else
			{
				m_Window->InvalidateBestSize();
			}
		}

		if (flags.Contains(WidgetSizeFlag::Client))
		{
			m_Window->SetClientSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::ClientMin))
		{
			m_Window->SetMinClientSize(size);
		}
		if (flags.Contains(WidgetSizeFlag::ClientMax))
		{
			m_Window->SetMaxClientSize(size);
		}

		if (flags.Contains(WidgetSizeFlag::Virtual))
		{
			m_Window->SetVirtualSize(size);
		}
	}

	// Coordinate conversion functions
	void BasicWindowWidgetBase::ScreenToClient(int& x, int& y) const
	{
		m_Window->ScreenToClient(&x, &y);
	}
	void BasicWindowWidgetBase::ClientToScreen(int& x, int& y) const
	{
		m_Window->ClientToScreen(&x, &y);
	}

	void BasicWindowWidgetBase::DialogUnitsToPixels(int& x, int& y) const
	{
		auto temp = m_Window->ConvertDialogToPixels(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}
	void BasicWindowWidgetBase::PixelsToDialogUnits(int& x, int& y) const
	{
		auto temp = m_Window->ConvertPixelsToDialog(wxPoint(x, y));
		x = temp.x;
		y = temp.y;
	}

	// Focus
	bool BasicWindowWidgetBase::IsFocusable() const
	{
		return m_Window->IsFocusable();
	}
	bool BasicWindowWidgetBase::HasFocus() const
	{
		return m_Window->HasFocus();
	}
	void BasicWindowWidgetBase::SetFocus()
	{
		m_Window->SetFocus();
	}

	bool BasicWindowWidgetBase::IsFocusVisible() const
	{
		return m_VisibleFocusEnabled;
	}
	void BasicWindowWidgetBase::SetFocusVisible(bool visible)
	{
		m_VisibleFocusEnabled = visible;
		m_Window->EnableVisibleFocus(visible);
	}

	// Layout
	LayoutDirection BasicWindowWidgetBase::GetLayoutDirection() const
	{
		switch (m_Window->GetLayoutDirection())
		{
			case wxLayoutDirection::wxLayout_LeftToRight:
			{
				return LayoutDirection::LeftToRight;
			}
			case wxLayoutDirection::wxLayout_RightToLeft:
			{
				return LayoutDirection::RightToLeft;
			}
		};
		return LayoutDirection::Default;
	}
	void BasicWindowWidgetBase::SetLayoutDirection(LayoutDirection direction)
	{
		switch (direction)
		{
			case LayoutDirection::LeftToRight:
			{
				m_Window->SetLayoutDirection(wxLayoutDirection::wxLayout_LeftToRight);
				break;
			}
			case LayoutDirection::RightToLeft:
			{
				m_Window->SetLayoutDirection(wxLayoutDirection::wxLayout_RightToLeft);
				break;
			}
			default:
			{
				m_Window->SetLayoutDirection(wxLayoutDirection::wxLayout_Default);
				break;
			}
		};
	}

	void BasicWindowWidgetBase::Fit()
	{
		m_Window->Fit();
	}
	void BasicWindowWidgetBase::FitInterior()
	{
		m_Window->FitInside();
	}
	bool BasicWindowWidgetBase::Layout()
	{
		return m_Window->Layout();
	}

	// Child management functions
	void BasicWindowWidgetBase::AddChildWidget(IWidget& widget)
	{
		m_Window->AddChild(widget.GetWxWindow());
	}
	void BasicWindowWidgetBase::RemoveChildWidget(const IWidget& widget)
	{
		m_Window->RemoveChild(const_cast<wxWindow*>(widget.GetWxWindow()));
	}
	void BasicWindowWidgetBase::DestroyChildWidgets()
	{
		m_Window->DestroyChildren();
	}

	std::shared_ptr<IWidget> BasicWindowWidgetBase::FindChildWidgetByID(int id) const
	{
		if (auto window = m_Window->FindWindow(id))
		{
			return IWidget::FindByWindow(*window);
		}
		return nullptr;
	}
	std::shared_ptr<IWidget> BasicWindowWidgetBase::FindChildWidgetByName(const String& widgetName) const
	{
		if (auto window = m_Window->FindWindow(widgetName))
		{
			return IWidget::FindByWindow(*window);
		}
		return nullptr;
	}
	Enumerator<std::shared_ptr<IWidget>> BasicWindowWidgetBase::EnumChildWidgets() const
	{
		return Utility::EnumerateIterableContainer<std::shared_ptr<IWidget>>(m_Window->GetChildren(), [](wxWindow* window)
		{
			return IWidget::FindByWindow(*window);
		});
	}

	// Sibling and parent management functions
	std::shared_ptr<kxf::IWidget> BasicWindowWidgetBase::GetParentWidget() const
	{
		if (auto window = m_Window->GetParent())
		{
			return IWidget::FindByWindow(*window);
		}
		return nullptr;
	}
	void BasicWindowWidgetBase::SetParentWidget(IWidget& widget)
	{
		m_Window->Reparent(widget.GetWxWindow());
	}

	std::shared_ptr<IWidget> BasicWindowWidgetBase::GetPrevSiblingWidget() const
	{
		if (auto window = m_Window->GetPrevSibling())
		{
			return IWidget::FindByWindow(*window);
		}
		return nullptr;
	}
	std::shared_ptr<IWidget> BasicWindowWidgetBase::GetNextSiblingWidget() const
	{
		if (auto window = m_Window->GetNextSibling())
		{
			return IWidget::FindByWindow(*window);
		}
		return nullptr;
	}

	// Drawing-related functions
	bool BasicWindowWidgetBase::IsFrozen() const
	{
		return m_Window->IsFrozen();
	}
	void BasicWindowWidgetBase::Freeze()
	{
		m_Window->Freeze();
	}
	void BasicWindowWidgetBase::Thaw()
	{
		m_Window->Thaw();
	}

	Rect BasicWindowWidgetBase::GetRefreshRect() const
	{
		return Rect(m_Window->GetUpdateClientRect());
	}
	void BasicWindowWidgetBase::Refresh(const Rect& rect)
	{
		if (rect.IsFullySpecified())
		{
			m_Window->RefreshRect(rect);
		}
		else
		{
			m_Window->Refresh();
		}
	}

	Font BasicWindowWidgetBase::GetFont() const
	{
		return m_Window->GetFont();
	}
	void BasicWindowWidgetBase::SetFont(const Font& font)
	{
		m_Window->SetFont(font.ToGDIFont().ToWxFont());
	}

	Color BasicWindowWidgetBase::GetColor(WidgetColorFlag colorType) const
	{
		switch (colorType)
		{
			case WidgetColorFlag::Background:
			{
				return m_Window->GetBackgroundColour();
			}
			case WidgetColorFlag::Text:
			case WidgetColorFlag::Foreground:
			{
				return m_Window->GetForegroundColour();
			}
		};
		return {};
	}
	void BasicWindowWidgetBase::SetColor(const Color& color, FlagSet<WidgetColorFlag> flags)
	{
		if (flags.Contains(WidgetColorFlag::Background))
		{
			m_Window->SetBackgroundColour(color);
		}
		if (flags.Contains(WidgetColorFlag::Foreground) || flags.Contains(WidgetColorFlag::Text))
		{
			m_Window->SetForegroundColour(color);
		}
	}

	float BasicWindowWidgetBase::GetTransparency() const
	{
		return m_Transparency;
	}
	bool BasicWindowWidgetBase::SetTransparency(float value)
	{
		if (m_Window->CanSetTransparent())
		{
			m_Transparency = std::clamp(value, 0.0f, 1.0f);
			return m_Window->SetTransparent(static_cast<wxByte>(255 - (m_Transparency * 255)));
		}
		return false;
	}

	// Widget state and visibility functions
	bool BasicWindowWidgetBase::IsEnabled() const
	{
		return m_Window->IsEnabled();
	}
	bool BasicWindowWidgetBase::IsIntrinsicallyEnabled() const
	{
		return m_Window->IsThisEnabled();
	}
	void BasicWindowWidgetBase::SetEnabled(bool enabled)
	{
		m_Window->Enable(enabled);
	}

	bool BasicWindowWidgetBase::IsVisible() const
	{
		return m_Window->IsShown();
	}
	bool BasicWindowWidgetBase::IsDisplayed() const
	{
		return m_Window->IsShownOnScreen();
	}
	void BasicWindowWidgetBase::SetVisible(bool visible)
	{
		m_Window->Show(visible);
	}

	// Widget style functions
	FlagSet<WidgetStyle> BasicWindowWidgetBase::GetWidgetStyle() const
	{
		const FlagSet<int> wx = m_Window->GetWindowStyle();

		FlagSet<WidgetStyle> style;
		style.Add(WidgetStyle::Invisible, wx.Contains(wxTRANSPARENT_WINDOW));
		style.Add(WidgetStyle::AllKeyEvents, wx.Contains(wxWANTS_CHARS));
		style.Add(WidgetStyle::ScrollVertical, wx.Contains(wxVSCROLL));
		style.Add(WidgetStyle::ScrollHorizontal, wx.Contains(wxHSCROLL));
		style.Add(WidgetStyle::ScrollShowAlways, wx.Contains(wxALWAYS_SHOW_SB));

		return style;
	}
	void BasicWindowWidgetBase::SetWidgetStyle(FlagSet<WidgetStyle> style)
	{
		FlagSet<int> wx = m_Window->GetWindowStyle();
		wx.Mod(wxTRANSPARENT_WINDOW, style.Contains(WidgetStyle::Invisible));
		wx.Mod(wxWANTS_CHARS, style.Contains(WidgetStyle::AllKeyEvents));
		wx.Mod(wxVSCROLL, style.Contains(WidgetStyle::ScrollVertical));
		wx.Mod(wxHSCROLL, style.Contains(WidgetStyle::ScrollHorizontal));
		wx.Mod(wxALWAYS_SHOW_SB, style.Contains(WidgetStyle::ScrollShowAlways));

		m_Window->SetWindowStyle(*wx);
	}

	FlagSet<WidgetExStyle> BasicWindowWidgetBase::GetWidgetExStyle() const
	{
		const FlagSet<int> wx = m_Window->GetWindowStyle();

		FlagSet<WidgetExStyle> style;
		return style;
	}
	void BasicWindowWidgetBase::SetWidgetExStyle(FlagSet<WidgetExStyle> style)
	{
		FlagSet<int> wx = m_Window->GetExtraStyle();
		m_Window->SetExtraStyle(*wx);
	}

	WidgetBorder BasicWindowWidgetBase::GetWidgetBorder() const
	{
		switch (m_Window->GetBorder())
		{
			case wxBorder::wxBORDER_NONE:
			{
				return WidgetBorder::None;
			}
			case wxBorder::wxBORDER_SIMPLE:
			{
				return WidgetBorder::Simple;
			}
			case wxBorder::wxBORDER_STATIC:
			{
				return WidgetBorder::Simple;
			}
			case wxBorder::wxBORDER_THEME:
			{
				return WidgetBorder::Theme;
			}
		}
		return WidgetBorder::Default;
	}
	void BasicWindowWidgetBase::SetWidgetBorder(WidgetBorder border)
	{
		FlagSet<int> style = m_Window->GetWindowStyle();

		style.Remove(wxBORDER_SUNKEN);
		style.Remove(wxBORDER_RAISED);
		style.Mod(wxBORDER_NONE, border == WidgetBorder::None);
		style.Mod(wxBORDER_DEFAULT, border == WidgetBorder::Default);
		style.Mod(wxBORDER_STATIC, border == WidgetBorder::Static);
		style.Mod(wxBORDER_THEME, border == WidgetBorder::Theme);

		m_Window->SetWindowStyle(*style);
	}

	// Widget properties
	int BasicWindowWidgetBase::GetWidgetID() const
	{
		return m_Window->GetId();
	}
	void BasicWindowWidgetBase::SetWidgetID(int id)
	{
		m_Window->SetId(id);
	}

	String BasicWindowWidgetBase::GetWidgetName() const
	{
		return m_Window->GetName();
	}
	void BasicWindowWidgetBase::SetWidgetName(const String& widgetName)
	{
		m_Window->SetName(widgetName);
	}
	String BasicWindowWidgetBase::GetWidgetText() const
	{
		return m_Window->GetLabel();
	}
	void BasicWindowWidgetBase::SetWidgetText(const String& widgetText)
	{
		m_Window->SetLabel(widgetText);
	}

	// INativeWidget
	void* BasicWindowWidgetBase::GetNativeHandle() const
	{
		return BasicWindowWidgetBase::GetHandle();
	}
	String BasicWindowWidgetBase::GetIntrinsicText() const
	{
		size_t length = ::InternalGetWindowText(m_Window->GetHandle(), nullptr, 0);
		if (length != 0)
		{
			String text;
			::InternalGetWindowText(m_Window->GetHandle(), Utility::StringBuffer(text, length - 1), length);

			return text;
		}
		return {};
	}

	intptr_t BasicWindowWidgetBase::GetWindowProperty(NativeWidgetProperty index) const
	{
		if (auto nativeIndex = MapWindowLongPtrIndex(index))
		{
			return static_cast<intptr_t>(::GetWindowLongPtrW(m_Window->GetHandle(), *nativeIndex));
		}
		return 0;
	}
	intptr_t BasicWindowWidgetBase::SetWindowProperty(NativeWidgetProperty index, intptr_t value)
	{
		if (auto nativeIndex = MapWindowLongPtrIndex(index))
		{
			return static_cast<intptr_t>(::SetWindowLongPtrW(m_Window->GetHandle(), *nativeIndex, value));
		}
		return 0;
	}

	bool BasicWindowWidgetBase::PostMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam)
	{
		return ::PostMessageW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
	}
	bool BasicWindowWidgetBase::NotifyMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam)
	{
		return ::SendNotifyMessageW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
	}
	bool BasicWindowWidgetBase::SendMessageSignal(uint32_t messageID, intptr_t wParam, intptr_t lParam)
	{
		return ::SendMessageCallbackW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), [](HWND handle, UINT messageID, ULONG_PTR context, LRESULT result)
		{
			IWidget& widget = reinterpret_cast<BasicWindowWidgetBase*>(context)->m_Widget;
			widget.ProcessSignal(&INativeWidget::OnMessageProcessed, handle, messageID, result);
		}, reinterpret_cast<ULONG_PTR>(this));
	}
	intptr_t BasicWindowWidgetBase::SendMessage(uint32_t messageID, intptr_t wParam, intptr_t lParam, TimeSpan timeout)
	{
		if (timeout.IsPositive())
		{
			DWORD_PTR result = 0;
			constexpr FlagSet<int> flags = SMTO_NORMAL|SMTO_ABORTIFHUNG|SMTO_ERRORONEXIT;
			::SendMessageTimeoutW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam), *flags, timeout.GetMilliseconds(), &result);

			return result;
		}
		else
		{
			return ::SendMessageW(m_Window->GetHandle(), messageID, static_cast<WPARAM>(wParam), static_cast<LPARAM>(lParam));
		}
	}

	HResult BasicWindowWidgetBase::SetWindowTheme(const String& applicationName, const std::vector<String>& subIDs)
	{
		if (!subIDs.empty())
		{
			String subIDString;
			for (const auto& id: subIDs)
			{
				if (!subIDString.IsEmpty())
				{
					subIDString += ';';
				}
				subIDString += id;
			}

			return ::SetWindowTheme(m_Window->GetHandle(), applicationName.wc_str(), subIDString.wc_str());
		}
		else
		{
			return ::SetWindowTheme(m_Window->GetHandle(), applicationName.wc_str(), nullptr);
		}
	}
}
