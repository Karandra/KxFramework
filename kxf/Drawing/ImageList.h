#pragma once
#include "Common.h"
#include "IGDIObject.h"
#include "Bitmap.h"
#include "Image.h"
#include "Icon.h"
#include "Color.h"
#include "kxf/System/COM.h"
#include <wx/imaglist.h>
struct IImageList2;

namespace kxf
{
	enum class ImageListFlag: uint32_t
	{
		None = 0,

		Transparent = 1 << 0,
		Selected = 1 << 1,
		Focused = 1 << 2,
	};
	KxFlagSet_Declare(ImageListFlag);
}

namespace kxf
{
	class KX_API ImageList: public wxImageList, public RTTI::ExtendInterface<ImageList, IGDIObject>
	{
		KxRTTI_DeclareIID(ImageList, {0x50e3b888, 0xefda, 0x4d73, {0x88, 0x58, 0x1, 0xde, 0xf, 0xf8, 0xc8, 0xaf}});

		protected:
			uint32_t m_Flags = 0;

		private:
			void OnCreate(int width, int height, bool mask, int initialCount) noexcept;
			bool DoDraw(wxDC& dc, int index, const Rect& rect, FlagSet<ImageListFlag> flags = {}, int overlayIndex = Drawing::InvalidImageIndex) noexcept;

		public:
			ImageList() noexcept;
			ImageList(int width, int height, int initialCount = 1) noexcept;
			ImageList(const Size& size, int initialCount = 1) noexcept
				:ImageList(size.GetWidth(), size.GetWidth(), initialCount)
			{
			}
			ImageList(const ImageList&) = delete;
			ImageList(ImageList&& other) noexcept
			{
				*this = std::move(other);
			}

		public:
			// IGDIObject
			bool IsNull() const override;
			bool IsSameAs(const IGDIObject& other) const override;
			std::unique_ptr<IGDIObject> Clone() const override;

			void* GetHandle() const override;
			void* DetachHandle() override;
			void AttachHandle(void* handle) override;

			// ImageList
			bool HasMask() const noexcept;
			COMPtr<IImageList2> QueryNativeInterface() const noexcept;

			bool Create(int width, int height, int initialCount = 1) noexcept;
			bool Create(const Size& size, int initialCount = 1) noexcept;

			bool Clear() noexcept;
			bool RemoveAll() noexcept;
			bool Remove(int index) noexcept;

			int Add(const Bitmap& bitmap);
			int Add(const Icon& icon);
			int Add(const Image& image);

			bool Replace(int index, const Bitmap& bitmap);
			bool Replace(int index, const Icon& icon);
			bool Replace(int index, const Image& image);

			Bitmap GetBitmap(int index) const;
			Image GetImage(int index) const;
			Icon GetIcon(int index) const;

			Color GetBackgroundColor() const noexcept;
			void SetBackgroundColor(const Color& color) noexcept;
			bool SetOverlayImage(int index, int overlayIndex) noexcept;

			bool Draw(wxDC& dc, int index, const Point& point, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, Rect(point, Size::UnspecifiedSize()), flags);
			}
			bool Draw(wxDC& dc, int index, const Rect& rect, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, rect, flags);
			}

			bool DrawOverlay(wxDC& dc, int index, int overlayIndex, const Point& point, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, Rect(point, Size::UnspecifiedSize()), flags, overlayIndex);
			}
			bool DrawOverlay(wxDC& dc, int index, int overlayIndex, const Rect& rect, FlagSet<ImageListFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, rect, flags, overlayIndex);
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			ImageList& operator=(const ImageList&) = delete;
			ImageList& operator=(ImageList&& other) noexcept;

		public:
			wxDECLARE_DYNAMIC_CLASS(ImageList);
	};
}
