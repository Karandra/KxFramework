#pragma once
#include "KxFramework/KxFramework.h"

KX_DECLARE_EVENT(KxEVT_THUMBVIEW_SELECTED, wxCommandEvent);
KX_DECLARE_EVENT(KxEVT_THUMBVIEW_ACTIVATED, wxCommandEvent);
KX_DECLARE_EVENT(KxEVT_THUMBVIEW_CONTEXT_MENU, wxContextMenuEvent);

class KX_API KxThumbView;
class KX_API KxThumbViewItem
{
	friend class KxThumbView;

	private:
		wxBitmap m_Bitmap;

	public:
		KxThumbViewItem(const wxBitmap& bitmap);
		~KxThumbViewItem();

	public:
		const wxBitmap& GetBitmap() const
		{
			return m_Bitmap;
		}
};

class KX_API KxThumbView: public wxSystemThemedControl<wxVScrolledWindow>
{
	private:
		wxSize m_ThumbSize = wxDefaultSize;
		wxSize m_Spacing = wxSize(1, 1);
		size_t m_Focused = (size_t)-1;
		size_t m_Selected = (size_t)-1;
		bool m_HasCahnges = true;
		std::vector<KxThumbViewItem> m_Thumbs;

	private:
		void OnPaint(wxPaintEvent& event);
		void OnSize(wxSizeEvent& event);
		void OnMouse(wxMouseEvent& event);
		void OnFocusLost(wxFocusEvent& event);
		virtual void OnInternalIdle() override;
		void UpdateView();
		void RefreshUpdatedRect()
		{
			RefreshRect(GetUpdateRegion().GetBox());
		}

		size_t GetIndexByRowColumn(size_t row, size_t columnIndex, size_t itemsInRow) const;
		wxRect GetThumbRect(size_t row, size_t columnIndex, size_t beginRow);
		wxRect GetFullThumbRect(size_t row, size_t columnIndex, size_t beginRow);
		wxSize GetFinalThumbSize() const;
		int CalcItemsPerRow() const;
		KxThumbViewItem& GetThumb(size_t i);

		virtual int OnGetRowHeight(size_t i) const override
		{
			return GetFinalThumbSize().GetHeight();
		}
		int CalcRowCount() const;
		wxBitmap CreateThumb(const wxBitmap& bitmap, const wxSize& size) const;

	public:
		static const long DefaultStyle = 0;
		static const wxSize DefaultThumbSize;
		static const float ThumbPaddingScale;
		
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
		virtual wxBorder GetDefaultBorder() const override
		{
			return wxBORDER_THEME;
		}
		virtual bool ShouldInheritColours() const override
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
