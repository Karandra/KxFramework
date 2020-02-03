#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWindowRefreshScheduler.h"
#include "KxEvent.h"

KxEVENT_DECLARE_GLOBAL(wxCommandEvent, THUMBVIEW_SELECTED);
KxEVENT_DECLARE_GLOBAL(wxCommandEvent, THUMBVIEW_ACTIVATED);
KxEVENT_DECLARE_GLOBAL(wxContextMenuEvent, THUMBVIEW_CONTEXT_MENU);

class KX_API KxThumbViewItem final
{
	private:
		wxBitmap m_Bitmap;

	public:
		KxThumbViewItem(const wxBitmap& bitmap)
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

class KX_API KxThumbView: public KxWindowRefreshScheduler<wxSystemThemedControl<wxVScrolledWindow>>
{
	private:
		enum: size_t
		{
			InvalidItemIndex = std::numeric_limits<size_t>::max()
		};

	private:
		wxSize m_ThumbSize = wxDefaultSize;
		wxSize m_Spacing = wxSize(1, 1);
		size_t m_Focused = InvalidItemIndex;
		size_t m_Selected = InvalidItemIndex;
		std::vector<KxThumbViewItem> m_Thumbs;

	private:
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnMouse(wxMouseEvent& event);
		void OnKillFocus(wxFocusEvent& event);

		size_t GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const;
		wxRect GetThumbRect(size_t row, size_t columnIndex, size_t beginRow);
		wxRect GetFullThumbRect(size_t row, size_t columnIndex, size_t beginRow);
		wxSize GetFinalThumbSize() const;
		size_t CalcItemsPerRow() const;

		int OnGetRowHeight(size_t i) const override
		{
			return GetFinalThumbSize().GetHeight();
		}
		size_t CalcRowCount() const;
		void UpdateRowCount();
		KxThumbViewItem& GetThumb(size_t i);
		wxBitmap CreateThumb(const wxBitmap& bitmap, const wxSize& size) const;

		void OnInternalIdle() override;

	public:
		static const long DefaultStyle = 0;
		static const wxSize DefaultThumbSize;
		static const double ThumbPaddingScale;
		
		KxThumbView() {}
		KxThumbView(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle
		)
		{
			Create(parent, id, style);
		}
		bool Create(wxWindow* parent,
					wxWindowID id,
					long style = DefaultStyle
		);
		virtual ~KxThumbView();

	public:
		wxBorder GetDefaultBorder() const override
		{
			return wxBORDER_THEME;
		}
		bool ShouldInheritColours() const override
		{
			return true;
		}

		wxSize GetThumbSize() const;
		void SetThumbSize(const wxSize& size);
		wxSize GetSpacing() const;
		void SetSpacing(const wxSize& spacing);

		int GetSelectedThumb() const;
		void SetSelectedThumb(int index);

		size_t GetThumbsCount() const;
		size_t AddThumb(const wxBitmap& bitmap);
		size_t AddThumb(const wxString& filePath, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);
		size_t AddThumb(wxInputStream& stream, wxBitmapType type = wxBITMAP_TYPE_ANY, int index = -1);
		void RemoveThumb(size_t index);
		void ClearThumbs();

	public:
		wxDECLARE_DYNAMIC_CLASS(KxThumbView);
};
