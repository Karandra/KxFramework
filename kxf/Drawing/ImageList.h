#pragma once
#include "Common.h"
#include "kxf/General/Color.h"
#include "kxf/System/COM.h"
#include <wx/imaglist.h>
struct IImageList2;

namespace kxf
{
	enum class ImageListDrawItemFlag
	{
		None = 0,

		Transparent = 1 << 0,
		Selected = 1 << 1,
		Focused = 1 << 2,
	};
	Kx_DeclareFlagSet(ImageListDrawItemFlag);
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

			int Add(const wxBitmap& bitmap);
			int Add(const wxIcon& icon);
			int Add(const wxImage& image);
		
			bool Replace(int index, const wxBitmap& bitmap);
			bool Replace(int index, const wxIcon& icon);
			bool Replace(int index, const wxImage& image);
		
			wxBitmap GetBitmap(int index) const;
			wxImage GetImage(int index) const;
			wxIcon GetIcon(int index) const;

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
