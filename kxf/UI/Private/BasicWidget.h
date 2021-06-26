#pragma once
#include "Common.h"
#include "../IWidget.h"
#include "kxf/General/Enumerator.h"
#include "kxf/EventSystem/EvtHandler.h"
#include "kxf/EventSystem/EvtHandlerAccessor.h"
#include "kxf/EventSystem/EventHandlerStack.h"
#include "kxf/Utility/Enumerator.h"
#include <wx/window.h>

namespace kxf::Private
{
	template<class TDerived, class TWindow, class TInterface>
	class BasicWidget: public RTTI::Implementation<BasicWidget<TDerived, TWindow, TInterface>, TInterface>
	{
		private:
			wxWindow* m_Window = nullptr;
			std::weak_ptr<IWidget> m_WidgetReference;
			bool m_ShouldDelete = true;

			EvtHandlerStack m_EventHandlerStack;
			EvtHandler m_EvtHandler;
			bool m_VisibleFocusEnabled = true;
			float m_Transparency = 0.0f;

		private:
			TDerived& Self() noexcept
			{
				return static_cast<TDerived&>(*this);
			}
			const TDerived& Self() const noexcept
			{
				return static_cast<const TDerived&>(*this);
			}

		private:
			auto AccessThisEvtHandler()
			{
				return EventSystem::EvtHandlerAccessor(GetThisEvtHandler());
			}
			auto AccessTopEvtHandler()
			{
				return EventSystem::EvtHandlerAccessor(GetTopEvtHandler());
			}

			void OnWindowCreate(wxWindowCreateEvent& event)
			{
				Self().OnWindowCreate();
				event.Skip();
			}
			void OnWindowDestroy(wxWindowDestroyEvent& event)
			{
				Self().OnWindowDestroy();

				IWidget::DissociateWithWindow(*m_Window);
				m_Window = nullptr;
				m_ShouldDelete = false;

				event.Skip();
			}

			// IWidget
			void SaveReference(std::weak_ptr<IWidget> ref) override
			{
				m_WidgetReference = std::move(ref);
			}

		protected:
			IEvtHandler& GetThisEvtHandler() noexcept
			{
				return m_EvtHandler;
			}
			IEvtHandler& GetTopEvtHandler() noexcept
			{
				return *m_EventHandlerStack.GetTop();
			}

			TWindow* Get()
			{
				return static_cast<TWindow*>(m_Window);
			}
			const TWindow* Get() const
			{
				return static_cast<TWindow*>(m_Window);
			}

			// IEvtHandler
			LocallyUniqueID DoBind(const EventID& eventID, std::unique_ptr<IEventExecutor> executor, FlagSet<BindEventFlag> flags = {}) override
			{
				return AccessThisEvtHandler().DoBind(eventID, std::move(executor), flags);
			}
			bool DoUnbind(const EventID& eventID, IEventExecutor& executor) override
			{
				return AccessThisEvtHandler().DoUnbind(eventID, executor);
			}
			bool DoUnbind(const LocallyUniqueID& bindSlot) override
			{
				return AccessThisEvtHandler().DoUnbind(bindSlot);
			}

			bool OnDynamicBind(EventSystem::EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicBind(eventItem);
			}
			bool OnDynamicUnbind(EventSystem::EventItem& eventItem) override
			{
				return AccessThisEvtHandler().OnDynamicUnbind(eventItem);
			}

			std::unique_ptr<IEvent> DoQueueEvent(std::unique_ptr<IEvent> event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}) override
			{
				return AccessTopEvtHandler().DoQueueEvent(std::move(event), eventID, uuid, flags);
			}
			bool DoProcessEvent(IEvent& event, const EventID& eventID = {}, const UniversallyUniqueID& uuid = {}, FlagSet<ProcessEventFlag> flags = {}, IEvtHandler* onlyIn = nullptr) override
			{
				return AccessTopEvtHandler().DoProcessEvent(event, eventID, uuid, flags, onlyIn);
			}

			bool TryBefore(IEvent& event) override
			{
				return AccessThisEvtHandler().TryBefore(event);
			}
			bool TryAfter(IEvent& event) override
			{
				return AccessThisEvtHandler().TryAfter(event);
			}

		public:
			BasicWidget(const BasicWidget&) = delete;

		protected:
			BasicWidget(std::unique_ptr<wxWindow> window)
				:m_Window(window.release()), m_EventHandlerStack(m_EvtHandler)
			{
				IWidget::AssociateWithWindow(*m_Window, *this);

				m_Window->Bind(wxEVT_CREATE, &BasicWidget::OnWindowCreate, this);
				m_Window->Bind(wxEVT_DESTROY, &BasicWidget::OnWindowDestroy, this);
			}
			~BasicWidget()
			{
				if (m_Window)
				{
					IWidget::DissociateWithWindow(*m_Window);
				}
				if (m_ShouldDelete)
				{
					delete m_Window;
				}
			}

		public:
			// IEvtHandler
			bool ProcessPendingEvents() override
			{
				return m_EvtHandler.ProcessPendingEvents();
			}
			size_t DiscardPendingEvents() override
			{
				return m_EvtHandler.DiscardPendingEvents();
			}

			IEvtHandler* GetPrevHandler() const override
			{
				return m_EvtHandler.GetPrevHandler();
			}
			IEvtHandler* GetNextHandler() const override
			{
				return m_EvtHandler.GetNextHandler();
			}
			void SetPrevHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}
			void SetNextHandler(IEvtHandler* evtHandler) override
			{
				// Can't chain widgets
			}

			void Unlink() override
			{
				m_EvtHandler.Unlink();
			}
			bool IsUnlinked() const override
			{
				return m_EvtHandler.IsUnlinked();
			}

			bool IsEventProcessingEnabled() const override
			{
				return m_EvtHandler.IsEventProcessingEnabled();
			}
			void EnableEventProcessing(bool enable = true) override
			{
				m_EvtHandler.EnableEventProcessing(enable);
			}

			// --- IWidget ---
		public:
			// Native interface
			void* GetHandle() const override
			{
				return m_Window ? static_cast<void*>(m_Window->GetHandle()) : nullptr;
			}

			wxWindow* GetWxWindow() override
			{
				return m_Window;
			}
			const wxWindow* GetWxWindow() const override
			{
				return m_Window;
			}

			// Lifetime management
			std::shared_ptr<IWidget> LockReference() const override
			{
				return m_WidgetReference.lock();
			}

			bool CloseWidget(bool force = false) override
			{
				if (m_Window)
				{
					return m_Window->Close(force);
				}
				return false;
			}
			bool DestroyWidget() override
			{
				if (m_Window)
				{
					return m_Window->Destroy();
				}
				return false;
			}

			// Event handling
			IEvtHandler& GetEventHandler() override
			{
				return GetTopEvtHandler();
			}

			void PushEventHandler(IEvtHandler& evtHandler) override
			{
				m_EventHandlerStack.Push(evtHandler);
			}
			IEvtHandler* PopEventHandler() override
			{
				return m_EventHandlerStack.Pop();
			}
			bool RemoveEventHandler(IEvtHandler& evtHandler) override
			{
				if (&evtHandler != &m_EvtHandler)
				{
					return m_EventHandlerStack.Remove(evtHandler);
				}
				return false;
			}

			// HiDPI support
			float GetContentScaleFactor() const override
			{
				return static_cast<float>(m_Window->GetDPIScaleFactor());
			}
			void FromDIP(int& x, int& y) const override
			{
				auto temp = m_Window->FromDIP(wxPoint(x, y));
				x = temp.x;
				y = temp.y;
			}
			void ToDIP(int& x, int& y) const override
			{
				auto temp = m_Window->ToDIP(wxPoint(x, y));
				x = temp.x;
				y = temp.y;
			}

			using IWidget::FromDIP;
			using IWidget::ToDIP;

			// Positioning functions
			Point GetPosition() const override
			{
				return Point(m_Window->GetPosition());
			}
			void SetPosition(const Point& pos) override
			{
				m_Window->SetPosition(pos);
			}

			void Center(FlagSet<Orientation> orientation = Orientation::Both) override
			{
				m_Window->Center(*Private::MapOrientation(orientation));
			}
			void CenterOnParent(FlagSet<Orientation> orientation = Orientation::Both) override
			{
				m_Window->CenterOnParent(*Private::MapOrientation(orientation));
			}

			// Size functions
			Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override
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
			Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const override
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
			void SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags = WidgetSizeFlag::Widget)
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
			void ScreenToClient(int& x, int& y) const override
			{
				m_Window->ScreenToClient(&x, &y);
			}
			void ClientToScreen(int& x, int& y) const override
			{
				m_Window->ClientToScreen(&x, &y);
			}

			using IWidget::ScreenToClient;
			using IWidget::ClientToScreen;

			void DialogUnitsToPixels(int& x, int& y) const override
			{
				auto temp = m_Window->ConvertDialogToPixels(wxPoint(x, y));
				x = temp.x;
				y = temp.y;
			}
			void PixelsToDialogUnits(int& x, int& y) const override
			{
				auto temp = m_Window->ConvertPixelsToDialog(wxPoint(x, y));
				x = temp.x;
				y = temp.y;
			}

			using IWidget::DialogUnitsToPixels;
			using IWidget::PixelsToDialogUnits;

			// Focus
			bool IsFocusable() const override
			{
				return m_Window->IsFocusable();
			}
			bool HasFocus() const override
			{
				return m_Window->HasFocus();
			}
			void SetFocus() override
			{
				m_Window->SetFocus();
			}

			bool IsFocusVisible() const override
			{
				return m_VisibleFocusEnabled;
			}
			void SetFocusVisible(bool enable = true) override
			{
				m_Window->EnableVisibleFocus(enable);
			}

			// Layout
			LayoutDirection GetLayoutDirection() const override
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
			void SetLayoutDirection(LayoutDirection direction) override
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

			void Fit() override
			{
				m_Window->Fit();
			}
			void FitInterior() override
			{
				m_Window->FitInside();
			}
			bool Layout() override
			{
				return m_Window->Layout();
			}

			// Child management functions
			void AddChildWidget(IWidget& widget) override
			{
				m_Window->AddChild(widget.GetWxWindow());
			}
			void RemoveChildWidget(const IWidget& widget) override
			{
				m_Window->RemoveChild(const_cast<wxWindow*>(widget.GetWxWindow()));
			}
			void DestroyChildWidgets()
			{
				m_Window->DestroyChildren();
			}

			std::shared_ptr<IWidget> FindChildWidgetByID(int id) const override
			{
				if (auto window = m_Window->FindWindow(id))
				{
					return IWidget::FindByWindow(*window);
				}
				return {};
			}
			std::shared_ptr<IWidget> FindChildWidgetByName(const String& widgetName) const override
			{
				if (auto window = m_Window->FindWindow(widgetName))
				{
					return IWidget::FindByWindow(*window);
				}
				return {};
			}
			Enumerator<std::shared_ptr<IWidget>> EnumChildWidgets() const override
			{
				return Utility::EnumerateIterableContainer<std::shared_ptr<IWidget>>(m_Window->GetChildren(), [](wxWindow* window)
				{
					return IWidget::FindByWindow(*window);
				});
			}

			// Sibling and parent management functions
			std::shared_ptr<IWidget> GetParentWidget() const override
			{
				if (auto window = m_Window->GetParent())
				{
					return IWidget::FindByWindow(*window);
				}
				return {};
			}
			void SetParentWidget(IWidget& widget) override
			{
				m_Window->Reparent(widget.GetWxWindow());
			}

			std::shared_ptr<IWidget> GetNextSiblingWidget() const override
			{
				if (auto window = m_Window->GetNextSibling())
				{
					return IWidget::FindByWindow(*window);
				}
				return {};
			}
			std::shared_ptr<IWidget> GetPrevSiblingWidget() const override
			{
				if (auto window = m_Window->GetPrevSibling())
				{
					return IWidget::FindByWindow(*window);
				}
				return {};
			}

			// Drawing-related functions
			bool IsFrozen() const override
			{
				return m_Window->IsFrozen();
			}
			void Freeze() override
			{
				m_Window->Freeze();
			}
			void Thaw() override
			{
				m_Window->Thaw();
			}

			Rect GetRefreshRect() const override
			{
				return Rect(m_Window->GetUpdateClientRect());
			}
			void Refresh(const Rect& rect = Rect::UnspecifiedRect()) override
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

			Font GetFont() const override
			{
				return m_Window->GetFont();
			}
			void SetFont(const Font& font) override
			{
				m_Window->SetFont(font.ToGDIFont().ToWxFont());
			}

			Color GetColor(WidgetColorFlag colorType) const override
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
			void SetColor(const Color& color, FlagSet<WidgetColorFlag> flags) override
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

			float GetTransparency() const override
			{
				return m_Transparency;
			}
			bool SetTransparency(float value) override
			{
				if (m_Window->CanSetTransparent())
				{
					m_Transparency = std::clamp(value, 0.0f, 1.0f);
					return m_Window->SetTransparent(static_cast<wxByte>(255 - (m_Transparency * 255)));
				}
				return false;
			}

			// Widget state and visibility functions
			bool IsEnabled() const override
			{
				return m_Window->IsEnabled();
			}
			bool IsIntrinsicallyEnabled() const override
			{
				return m_Window->IsThisEnabled();
			}
			void SetEnabled(bool enabled) override
			{
				m_Window->Enable(enabled);
			}

			bool IsVisible() const override
			{
				return m_Window->IsShown();
			}
			bool IsDisplayed() const
			{
				return m_Window->IsShownOnScreen();
			}
			void SetVisible(bool visible) override
			{
				m_Window->Show(visible);
			}

			// Widget styles functions
			FlagSet<WidgetStyle> GetWidgetStyle() const override
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
			void SetWidgetStyle(FlagSet<WidgetStyle> style) override
			{
				FlagSet<int> wx = m_Window->GetWindowStyle();
				wx.Mod(wxTRANSPARENT_WINDOW, style.Contains(WidgetStyle::Invisible));
				wx.Mod(wxWANTS_CHARS, style.Contains(WidgetStyle::AllKeyEvents));
				wx.Mod(wxVSCROLL, style.Contains(WidgetStyle::ScrollVertical));
				wx.Mod(wxHSCROLL, style.Contains(WidgetStyle::ScrollHorizontal));
				wx.Mod(wxALWAYS_SHOW_SB, style.Contains(WidgetStyle::ScrollShowAlways));

				m_Window->SetWindowStyle(*wx);
			}

			FlagSet<WidgetExStyle> GetWidgetExStyle() const override
			{
				const FlagSet<int> wx = m_Window->GetWindowStyle();

				FlagSet<WidgetExStyle> style;
				return style;
			}
			void SetWidgetExStyle(FlagSet<WidgetExStyle> style) override
			{
				FlagSet<int> wx = m_Window->GetExtraStyle();
				m_Window->SetExtraStyle(*wx);
			}

			WidgetBorder GetWidgetBorder() const override
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
			void SetWidgetBorder(WidgetBorder border) override
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
			int GetWidgetID() const override
			{
				return m_Window->GetId();
			}
			void SetWidgetID(int id) override
			{
				m_Window->SetId(id);
			}

			String GetWidgetName() const override
			{
				return m_Window->GetName();
			}
			void SetWidgetName(const String& widgetName) override
			{
				m_Window->SetName(widgetName);
			}

			String GetWidgetText() const override
			{
				return m_Window->GetLabel();
			}
			void SetWidgetText(const String& widgetText) override
			{
				m_Window->SetLabel(widgetText);
			}

		public:
			BasicWidget& operator=(const BasicWidget&) = delete;
	};
}
