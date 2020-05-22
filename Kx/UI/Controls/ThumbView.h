#pragma once
#include "Kx/UI/Common.h"
#include "Kx/UI/WindowRefreshScheduler.h"
#include "Kx/EventSystem/Event.h"
#include <wx/systhemectrl.h>
#include <wx/vscroll.h>

namespace KxFramework::UI
{
	class KX_API ThumbViewItem final
	{
		private:
			wxBitmap m_Bitmap;

		public:
			ThumbViewItem(const wxBitmap& bitmap)
				:m_Bitmap(bitmap)
			{
			}

		public:
			const wxBitmap& GetBitmap() const
			{
				return m_Bitmap;
			}
			wxBitmap& GetBitmap()
			{
				return m_Bitmap;
			}
	};
}

namespace KxFramework::UI
{
	class KX_API ThumbView: public wxSystemThemedControl<WindowRefreshScheduler<wxVScrolledWindow>>
	{
		public:
			static constexpr WindowStyle DefaultStyle = WindowStyle::None;
			static inline const Size DefaultThumbSize = Size(256, 144);
			static constexpr double ThumbPaddingScale = 0.9;

			KxEVENT_MEMBER(wxCommandEvent, Selected);
			KxEVENT_MEMBER(wxCommandEvent, Activated);
			KxEVENT_MEMBER(wxContextMenuEvent, ContextMenu);

		private:
			static constexpr size_t InvalidItemIndex = std::numeric_limits<size_t>::max();

		private:
			Size m_ThumbSize = Size::UnspecifiedSize();
			Size m_Spacing = Size(1, 1);
			size_t m_Focused = InvalidItemIndex;
			size_t m_Selected = InvalidItemIndex;
			std::vector<ThumbViewItem> m_Items;

		private:
			void OnPaint(wxPaintEvent& event);
			void OnSize(wxSizeEvent& event);
			void OnMouse(wxMouseEvent& event);
			void OnKillFocus(wxFocusEvent& event);

			size_t GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const;
			Rect GetThumbRect(size_t row, size_t columnIndex, size_t beginRow);
			Rect GetFullThumbRect(size_t row, size_t columnIndex, size_t beginRow);
			Size GetFinalThumbSize() const;
			size_t CalcItemsPerRow() const;

			int OnGetRowHeight(size_t i) const override
			{
				return GetFinalThumbSize().GetHeight();
			}
			size_t CalcRowCount() const;
			void UpdateRowCount();
			ThumbViewItem& GetThumb(size_t i);
			wxBitmap CreateThumb(const wxBitmap& bitmap, const Size& size) const;

			void OnInternalIdle() override;

		public:
			ThumbView() = default;
			ThumbView(wxWindow* parent,
					  wxWindowID id,
					  WindowStyle style = DefaultStyle
			)
			{
				Create(parent, id, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						WindowStyle style = DefaultStyle
			);

		public:
			wxBorder GetDefaultBorder() const override
			{
				return wxBORDER_THEME;
			}
			bool ShouldInheritColours() const override
			{
				return true;
			}

			Size GetThumbSize() const;
			void SetThumbSize(const Size& size);
			Size GetSpacing() const;
			void SetSpacing(const Size& spacing);

			int GetSelectedThumb() const;
			void SetSelectedThumb(int index);

			size_t GetThumbsCount() const;
			size_t AddThumb(const wxBitmap& bitmap);
			size_t AddThumb(const String& filePath, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);
			size_t AddThumb(wxInputStream& stream, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);
			void RemoveThumb(size_t index);
			void ClearThumbs();

		public:
			wxDECLARE_DYNAMIC_CLASS(ThumbView);
	};
}
