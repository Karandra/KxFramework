#pragma once
#include "Common.h"
#include "kxf/EventSystem/IEvtHandler.h"
#include "kxf/EventSystem/IEvent.h"
class wxWindow;

namespace kxf
{
	class KX_API IWidget: public RTTI::ExtendInterface<IWidget, IEvtHandler>
	{
		KxRTTI_DeclareIID(IWidget, {0xd5a7bb64, 0x7a68, 0x4906, {0x91, 0x3d, 0x7d, 0x3e, 0x3f, 0x84, 0xdf, 0xa0}});

		template<std::derived_from<IWidget> TWidget>
		friend std::shared_ptr<TWidget> NewWidget();

		public:
			static void AssociateWithWindow(wxWindow& window, IWidget& widget) noexcept;
			static void DissociateWithWindow(wxWindow& window) noexcept;

			static std::shared_ptr<IWidget> FindByWindow(const wxWindow& window) noexcept;
			static std::shared_ptr<IWidget> FindFocus() noexcept;

		protected:
			virtual void SaveReference(std::weak_ptr<IWidget> ref) = 0;

		public:
			virtual ~IWidget() = default;

		public:
			// Native interface
			virtual void* GetHandle() const = 0;
			virtual wxWindow* GetWxWindow() const = 0;

			// Lifetime management
			virtual std::shared_ptr<IWidget> LockReference() const = 0;
			virtual bool CreateWidget(IWidget* parent, const String& text, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize()) = 0;
			virtual bool CloseWidget(bool force = false) = 0;
			virtual bool DestroyWidget() = 0;
			virtual bool IsWidgetAlive() const = 0;

			// Event handling
			virtual IEvtHandler& GetEventHandler() = 0;

			virtual void PushEventHandler(IEvtHandler& evtHandler) = 0;
			virtual bool RemoveEventHandler(IEvtHandler& evtHandler) = 0;
			virtual IEvtHandler* PopEventHandler() = 0;

			// HiDPI support
			virtual float GetContentScaleFactor() const = 0;

			virtual void FromDIP(int& x, int& y) const = 0;
			int FromDIP(int x) const
			{
				int y = Geometry::DefaultCoord;
				FromDIP(x, y);

				return x;
			}
			Size FromDIP(Size size) const
			{
				FromDIP(size.Width(), size.Height());
				return size;
			}
			Point FromDIP(Point point) const
			{
				FromDIP(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T FromDIP(int x, int y) const
			{
				FromDIP(x, y);
				return {x, y};
			}

			virtual void ToDIP(int& x, int& y) const = 0;
			int ToDIP(int x) const
			{
				int y = Geometry::DefaultCoord;
				ToDIP(x, y);

				return x;
			}
			Size ToDIP(Size size) const
			{
				ToDIP(size.Width(), size.Height());
				return size;
			}
			Point ToDIP(Point point) const
			{
				ToDIP(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T ToDIP(int x, int y) const
			{
				FromDIP(x, y);
				return {x, y};
			}

			// Positioning functions
			virtual Point GetPosition() const = 0;
			virtual void SetPosition(const Point& pos) = 0;

			virtual void Center(FlagSet<Orientation> orientation = Orientation::Both) = 0;
			virtual void CenterOnParent(FlagSet<Orientation> orientation = Orientation::Both) = 0;

			// Size functions
			virtual Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const = 0;
			virtual Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const = 0;
			virtual void SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags = WidgetSizeFlag::Widget) = 0;

			// Coordinate conversion functions
			virtual void ScreenToClient(int& x, int& y) const = 0;
			Size ScreenToClient(Size size) const
			{
				ScreenToClient(size.Width(), size.Height());
				return size;
			}
			Point ScreenToClient(Point point) const
			{
				ScreenToClient(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T ScreenToClient(int x, int y) const
			{
				ScreenToClient(x, y);
				return {x, y};
			}

			virtual void ClientToScreen(int& x, int& y) const = 0;
			Size ClientToScreen(Size size) const
			{
				ClientToScreen(size.Width(), size.Height());
				return size;
			}
			Point ClientToScreen(Point point) const
			{
				ClientToScreen(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T ClientToScreen(int x, int y) const
			{
				ClientToScreen(x, y);
				return {x, y};
			}

			virtual void DialogUnitsToPixels(int& x, int& y) const = 0;
			Size DialogUnitsToPixels(Size size) const
			{
				DialogUnitsToPixels(size.Width(), size.Height());
				return size;
			}
			Point DialogUnitsToPixels(Point point) const
			{
				DialogUnitsToPixels(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T DialogUnitsToPixels(int x, int y) const
			{
				DialogUnitsToPixels(x, y);
				return {x, y};
			}

			virtual void PixelsToDialogUnits(int& x, int& y) const = 0;
			Size PixelsToDialogUnits(Size size) const
			{
				PixelsToDialogUnits(size.Width(), size.Height());
				return size;
			}
			Point PixelsToDialogUnits(Point point) const
			{
				PixelsToDialogUnits(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T PixelsToDialogUnits(int x, int y) const
			{
				PixelsToDialogUnits(x, y);
				return {x, y};
			}

			// Focus
			virtual bool IsFocusable() const = 0;
			virtual bool HasFocus() const = 0;
			virtual void SetFocus() = 0;

			virtual bool IsFocusVisible() const = 0;
			virtual void SetFocusVisible(bool visible = true) = 0;

			// Layout
			virtual LayoutDirection GetLayoutDirection() const = 0;
			virtual void SetLayoutDirection(LayoutDirection direction) = 0;

			virtual void Fit() = 0;
			virtual void FitInterior() = 0;
			virtual bool Layout() = 0;

			// Child management functions
			virtual void AddChildWidget(IWidget& widget) = 0;
			virtual void RemoveChildWidget(const IWidget& widget) = 0;
			virtual void DestroyChildWidgets() = 0;

			virtual std::shared_ptr<IWidget> FindChildWidgetByID(WidgetID id) const = 0;
			virtual std::shared_ptr<IWidget> FindChildWidgetByName(const String& widgetName) const = 0;
			virtual Enumerator<std::shared_ptr<IWidget>> EnumChildWidgets() const = 0;

			// Sibling and parent management functions
			virtual std::shared_ptr<IWidget> GetParentWidget() const = 0;
			virtual void SetParentWidget(IWidget& widget) = 0;

			virtual std::shared_ptr<IWidget> GetPrevSiblingWidget() const = 0;
			virtual std::shared_ptr<IWidget> GetNextSiblingWidget() const = 0;

			// Drawing-related functions
			virtual bool IsFrozen() const = 0;
			virtual void Freeze() = 0;
			virtual void Thaw() = 0;

			virtual Rect GetRefreshRect() const = 0;
			virtual void Refresh(const Rect& rect = Rect::UnspecifiedRect()) = 0;

			virtual Font GetFont() const = 0;
			virtual void SetFont(const Font& font) = 0;

			virtual Color GetColor(WidgetColorFlag colorType) const = 0;
			virtual void SetColor(const Color& color, FlagSet<WidgetColorFlag> flags) = 0;

			virtual float GetTransparency() const = 0;
			virtual bool SetTransparency(float value) = 0;

			// Widget state and visibility functions
			virtual bool IsEnabled() const = 0;
			virtual bool IsIntrinsicallyEnabled() const = 0;
			virtual void SetEnabled(bool enabled) = 0;
			void Enable()
			{
				SetEnabled(true);
			}
			void Disable()
			{
				SetEnabled(false);
			}

			virtual bool IsVisible() const = 0;
			virtual bool IsDisplayed() const = 0;
			virtual void SetVisible(bool visible) = 0;
			void Show()
			{
				SetVisible(true);
			}
			void Hide()
			{
				SetVisible(false);
			}

			// Widget style functions
			virtual FlagSet<WidgetStyle> GetWidgetStyle() const = 0;
			virtual void SetWidgetStyle(FlagSet<WidgetStyle> style) = 0;

			virtual FlagSet<WidgetExStyle> GetWidgetExStyle() const = 0;
			virtual void SetWidgetExStyle(FlagSet<WidgetExStyle> style) = 0;

			virtual WidgetBorder GetWidgetBorder() const = 0;
			virtual void SetWidgetBorder(WidgetBorder border) = 0;

			// Widget properties
			virtual WidgetID GetWidgetID() const = 0;
			virtual void SetWidgetID(WidgetID id) = 0;

			virtual String GetWidgetName() const = 0;
			virtual void SetWidgetName(const String& widgetName) = 0;

			virtual String GetWidgetText(FlagSet<WidgetTextFlag> flags = {}) const = 0;
			virtual void SetWidgetText(const String& widgetText, FlagSet<WidgetTextFlag> flags = {}) = 0;
	};
}

namespace kxf
{
	template<std::derived_from<IWidget> TWidget>
	std::shared_ptr<TWidget> NewWidget()
	{
		auto widget = std::make_shared<TWidget>();
		static_cast<IWidget&>(*widget).SaveReference(widget);

		return widget;
	}

	template<std::derived_from<IWidget> TWidget, class... Args>
	std::shared_ptr<TWidget> NewWidget(IWidget* parent, const String& text, Point pos = Point::UnspecifiedPosition(), Size size = Size::UnspecifiedSize(), Args&&... arg)
	{
		auto widget = NewWidget<TWidget>();
		widget->TWidget::CreateWidget(parent, text, pos, size, std::forward<Args>(arg)...);

		return widget;
	}
}

namespace kxf::Widgets
{
	String RemoveMnemonics(const String& text);
	String EscapeMnemonics(const String& text);
}
