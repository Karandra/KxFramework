#pragma once
#include "Common.h"
#include "Bitmap.h"
#include "Image.h"
#include "Icon.h"
#include "kxf/General/Color.h"
#include "kxf/System/COM.h"
#include <wx/imaglist.h>
struct IImageList2;

namespace kxf
{
	enum class ImageListDrawItemFlag: uint32_t
	{
		None = 0,

		Transparent = 1 << 0,
		Selected = 1 << 1,
		Focused = 1 << 2,
	};
	KxFlagSet_Declare(ImageListDrawItemFlag);
}

namespace kxf
{
	class KX_API ImageList: public wxImageList
	{
		public:
			using DrawItemFlag = ImageListDrawItemFlag;

		protected:
			uint32_t m_Flags = 0;

		private:
			void OnCreate(int width, int height, bool mask, int initialCount) noexcept;
			bool DoDraw(wxDC& dc, int index, const Rect& rect, FlagSet<DrawItemFlag> flags = {}, int overlayIndex = Drawing::InvalidImageIndex) noexcept;

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
			bool IsNull() const noexcept;
			bool HasMask() const noexcept;
			COMPtr<IImageList2> QueryInterface() const noexcept;
			
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

			bool Draw(wxDC& dc, int index, const Point& point, FlagSet<DrawItemFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, Rect(point, Size::UnspecifiedSize()), flags);
			}
			bool Draw(wxDC& dc, int index, const Rect& rect, FlagSet<DrawItemFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, rect, flags);
			}

			bool DrawOverlay(wxDC& dc, int index, int overlayIndex, const Point& point, FlagSet<DrawItemFlag> flags = {}) noexcept
			{
				return DoDraw(dc, index, Rect(point, Size::UnspecifiedSize()), flags, overlayIndex);
			}
			bool DrawOverlay(wxDC& dc, int index, int overlayIndex, const Rect& rect, FlagSet<DrawItemFlag> flags = {}) noexcept
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
