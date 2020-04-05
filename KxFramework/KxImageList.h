#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxEnumClassOperations.h"
#include "Kx/System/COM.h"
#include "Kx/General/Color.h"
using KxColor = KxFramework::Color;

enum class KxImageListDrawMode
{
	None = 0,
	Normal = wxIMAGELIST_DRAW_NORMAL,
	Transparent = wxIMAGELIST_DRAW_TRANSPARENT,
	Selected = wxIMAGELIST_DRAW_SELECTED,
	Focused = wxIMAGELIST_DRAW_FOCUSED,
};

struct IImageList2;
class KX_API KxImageList: public wxImageList
{
	public:
		using DrawMode = KxImageListDrawMode;
		enum
		{
			NO_IMAGE = wxWithImages::NO_IMAGE
		};

	protected:
		uint32_t m_Flags = 0;

	private:
		void OnCreate(int width, int height, bool mask, int initialCount);
		bool DoDraw(wxDC& dc, int index, const wxRect& rect, DrawMode drawMode = DrawMode::Normal, int overlayIndex = NO_IMAGE);

	public:
		KxImageList();
		KxImageList(int width, int height, int initialCount = 1);
		KxImageList(const wxSize& size, int initialCount = 1);
		KxImageList(const KxImageList&) = delete;
		KxImageList(KxImageList&& other);

	public:
		bool IsOk() const;
		bool HasMask() const;
		KxFramework::COMPtr<IImageList2> QueryInterface() const;
		
		bool Create(int width, int height, int initialCount = 1);
		bool Create(const wxSize& size, int initialCount = 1);

		bool Clear();
		bool RemoveAll();
		bool Remove(int index);

		int Add(const wxBitmap& bitmap);
		int Add(const wxIcon& icon);
		int Add(const wxImage& image);
		
		bool Replace(int index, const wxBitmap& bitmap);
		bool Replace(int index, const wxIcon& icon);
		bool Replace(int index, const wxImage& image);
		
		wxBitmap GetBitmap(int index) const;
		wxImage GetImage(int index) const;
		wxIcon GetIcon(int index) const;

		KxColor GetBackgroundColor() const;
		void SetBackgroundColor(const KxColor& color);
		bool SetOverlayImage(int index, int overlayIndex);

		bool Draw(wxDC& dc, int index, const wxPoint& point, DrawMode drawMode = DrawMode::Normal)
		{
			return DoDraw(dc, index, wxRect(point, wxDefaultSize), drawMode);
		}
		bool Draw(wxDC& dc, int index, const wxRect& rect, DrawMode drawMode = DrawMode::Normal)
		{
			return DoDraw(dc, index, rect, drawMode);
		}

		bool DrawOverlay(wxDC& dc, int index, int overlayIndex, const wxPoint& point, DrawMode drawMode = DrawMode::Normal)
		{
			return DoDraw(dc, index, wxRect(point, wxDefaultSize), drawMode, overlayIndex);
		}
		bool DrawOverlay(wxDC& dc, int index, int overlayIndex, const wxRect& rect, DrawMode drawMode = DrawMode::Normal)
		{
			return DoDraw(dc, index, rect, drawMode, overlayIndex);
		}

	public:
		explicit operator bool() const
		{
			return IsOk();
		}
		bool operator!() const
		{
			return !IsOk();
		}

		KxImageList& operator=(const KxImageList&) = delete;
		KxImageList& operator=(KxImageList&& other);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxImageList);
};

namespace KxEnumClassOperations
{
	KxImplementEnum(KxImageListDrawMode);
}
