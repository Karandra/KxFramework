#pragma once
#include "Common.h"
class wxWindow;

namespace kxf
{
	enum class WidgetStyle: uint64_t
	{
		None = 0
	};
	KxFlagSet_Declare(WidgetStyle);

	enum class WidgetExStyle: uint64_t
	{
		None = 0
	};
	KxFlagSet_Declare(WidgetExStyle);

	enum class WidgetBorder: uint64_t
	{
		None = 0
	};
	KxFlagSet_Declare(WidgetBorder);

	enum class WidgetSizeFlag: uint32_t
	{
		None = 0,

		Widget = FlagSetValue<WidgetSizeFlag>(0),
		WidgetMin = FlagSetValue<WidgetSizeFlag>(1),
		WidgetMax = FlagSetValue<WidgetSizeFlag>(2),
		WidgetBest = FlagSetValue<WidgetSizeFlag>(3),

		Client = FlagSetValue<WidgetSizeFlag>(4),
		ClientMin = FlagSetValue<WidgetSizeFlag>(5),
		ClientMax = FlagSetValue<WidgetSizeFlag>(6),
		ClientBest = FlagSetValue<WidgetSizeFlag>(7),

		Virtual = FlagSetValue<WidgetSizeFlag>(8),
		VirtualBest = FlagSetValue<WidgetSizeFlag>(9),

		Border = FlagSetValue<WidgetSizeFlag>(10)
	};
	KxFlagSet_Declare(WidgetSizeFlag);

	enum class WidgetColorFlag: uint32_t
	{
		None = 0,

		Background = FlagSetValue<WidgetColorFlag>(0),
		Foreground = FlagSetValue<WidgetColorFlag>(1),
		Border = FlagSetValue<WidgetColorFlag>(2),
		Text = FlagSetValue<WidgetColorFlag>(3)
	};
	KxFlagSet_Declare(WidgetColorFlag);

	enum class LayoutDirection
	{
		Default = -1,
		LeftToRight,
		RightToLeft
	};
}

namespace kxf
{
	class KX_API IWidget: public RTTI::ExtendInterface<IWidget, IEvtHandler>
	{
		KxRTTI_DeclareIID(IWidget, {0xd5a7bb64, 0x7a68, 0x4906, {0x91, 0x3d, 0x7d, 0x3e, 0x3f, 0x84, 0xdf, 0xa0}});

		public:
			virtual ~IWidget() = default;

		public:
			// Native interface
			virtual void* GetHandle() const = 0;

			virtual wxWindow* GetWxWindow() = 0;
			virtual const wxWindow* GetWxWindow() const = 0;

			// Lifetime management
			virtual std::shared_ptr<IWidget> LockReference() const = 0;

			virtual bool Close(bool force = false) = 0;
			virtual void Destroy() = 0;

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
			virtual void SetPosition(const Point& pos) = 0;;

			virtual void Center(FlagSet<Orientation> orientation = Orientation::Both) = 0;
			virtual void CenterOnParent(FlagSet<Orientation> orientation = Orientation::Both) = 0;

			// Size functions
			virtual Rect GetRect(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const = 0;
			virtual Size GetSize(WidgetSizeFlag sizeType = WidgetSizeFlag::Widget) const = 0;
			virtual void SetSize(const Size& size, FlagSet<WidgetSizeFlag> flags = WidgetSizeFlag::Widget) = 0;

			// Coordinate conversion functions
			virtual void SceenToClient(int& x, int& y) const = 0;
			Size SceenToClient(Size size) const
			{
				SceenToClient(size.Width(), size.Height());
				return size;
			}
			Point SceenToClient(Point point) const
			{
				SceenToClient(point.X(), point.Y());
				return point;
			}

			template<std::constructible_from<int, int> T>
			T SceenToClient(int x, int y) const
			{
				SceenToClient(x, y);
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
			virtual bool SetFocus() = 0;

			virtual bool IsFocusVisible() const = 0;
			virtual void SetFocusVisible(bool enable) = 0;

			// Layout
			virtual LayoutDirection GetLayoutDirection() const = 0;
			virtual void SetLayoutDirection(LayoutDirection direction) = 0;

			virtual void Fit() = 0;
			virtual void FitInterior() = 0;
			virtual bool Layout() = 0;

			// Child management functions
			virtual void AddChildWidget(std::shared_ptr<IWidget> widget) = 0;
			virtual void RemoveChildWidget(const IWidget& widget) = 0;
			virtual void DestroyChildWidgets() = 0;

			virtual std::shared_ptr<IWidget> FindChildWidget(const String& widgetName) const = 0;
			virtual Enumerator<std::shared_ptr<IWidget>> EnumChildWidgets() const = 0;

			// Sibling and parent management functions
			virtual std::shared_ptr<IWidget> GetParentWidget() const = 0;
			virtual void SetParentWidget(IWidget& widget) = 0;

			virtual std::shared_ptr<IWidget> GetNextSiblingWidget() const = 0;
			virtual std::shared_ptr<IWidget> GetPrevSiblingWidget() const = 0;

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
			virtual void SetTransparency(float value) = 0;

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
			virtual void SetVisible(bool enabled) = 0;
			void Show()
			{
				SetVisible(true);
			}
			void Hide()
			{
				SetVisible(false);
			}

			// Widget styles functions
			virtual FlagSet<WidgetStyle> GetWidgetStyle() const = 0;
			virtual void SetWidgetStyle(FlagSet<WidgetStyle> style) = 0;

			virtual FlagSet<WidgetExStyle> GetWidgetExStyle() const = 0;
			virtual void SetWidgetExStyle(FlagSet<WidgetExStyle> style) = 0;

			// Widget properties
			virtual int GetWidgetID() const = 0;
			virtual void SetWidgetID(int id) = 0;

			virtual String GetWidgetName() const = 0;
			virtual void SetWidgetName(const String& widgetName) = 0;

			virtual String GetWidgetText() const = 0;
			virtual void SetWidgetText(const String& widgetText) = 0;
	};
}
