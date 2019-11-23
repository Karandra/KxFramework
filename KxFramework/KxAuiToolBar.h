#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxAuiToolBarItem.h"
#include "KxFramework/KxAuiToolBarEvent.h"

class KX_API KxAuiToolBar: public wxAuiToolBar
{
	friend class KxAuiToolBarItem;

	private:
		std::unordered_map<wxWindowID, std::unique_ptr<KxAuiToolBarItem>> m_Items;
		wxColour m_ColorBorder;

	private:
		void EventHandler(wxAuiToolBarEvent& event);
		void OnLeftClick(wxCommandEvent& event);

		KxAuiToolBarItem* DoCreateTool(wxAuiToolBarItem* item);
		KxAuiToolBarItem* DoGetTool(const wxAuiToolBarItem& item);
		bool DoRemoveTool(wxAuiToolBarItem& item);
		bool DoRemoveTool(KxAuiToolBarItem& item)
		{
			return DoRemoveTool(*item.m_Item);
		}
		
		size_t DoGetToolIndex(const KxAuiToolBarItem& item) const;
		bool DoSetToolIndex(KxAuiToolBarItem& item, size_t newIndex);

	public:
		static const long DefaultStyle = wxAUI_TB_HORZ_LAYOUT;
		static const int DefaultSeparatorSize = 2;

		KxAuiToolBar() = default;
		KxAuiToolBar(wxWindow* parent,
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
		~KxAuiToolBar();

	public:
		wxColour GetBorderColor() const
		{
			return m_ColorBorder;
		}
		void SetBorderColor(const wxColour& color = wxNullColour)
		{
			m_ColorBorder = color;
			Refresh();
		}

		void ClearTools();
		bool IsOverflowVisible() const
		{
			return wxAuiToolBar::GetOverflowVisible();
		}
		bool IsGripperVisible() const
		{
			return wxAuiToolBar::GetGripperVisible();
		}
		bool IsItemsFits() const
		{
			return wxAuiToolBar::GetToolBarFits();
		}

		KxAuiToolBarItem* AddTool(const wxString& label, const wxBitmap& bitmap, wxItemKind kind = wxITEM_NORMAL, const wxString& shortHelp = wxEmptyString);
		KxAuiToolBarItem* AddTool(const wxString& label, const wxBitmap& bitmap, const wxBitmap& disabledBitmap, wxItemKind kind = wxITEM_NORMAL, const wxString& shortHelp = wxEmptyString);
		KxAuiToolBarItem* AddTool(const wxBitmap& bitmap, const wxBitmap& disabledBitmap, bool isToggle = false, const wxString& shortHelp = wxEmptyString, const wxString& longHelp = wxEmptyString);

		KxAuiToolBarItem* AddLabel(const wxString& label, const int width = -1);
		KxAuiToolBarItem* AddControl(wxControl* control, const wxString& label = wxEmptyString);
		KxAuiToolBarItem* AddSeparator();
		KxAuiToolBarItem* AddSpacer(int pixels);
		KxAuiToolBarItem* AddStretchSpacer(int proportion = 1);

		KxAuiToolBarItem* FindToolByPosition(const wxPoint& pos) const;
		KxAuiToolBarItem* FindToolByPosition(wxCoord x, wxCoord y) const
		{
			return FindToolByPosition(wxPoint(x, y));
		}
		KxAuiToolBarItem* FindToolByIndex(int index) const;
		KxAuiToolBarItem* FindToolByID(wxWindowID id) const;

		bool RemoveTool(KxAuiToolBarItem& tool);
		bool RemoveTool(int index);

		void UpdateUI();

	private:
		wxAuiToolBarItem* AddTool(int tool_id, const wxString &label, const wxBitmap &bitmap, const wxString &short_help_string = wxEmptyString, wxItemKind kind = wxITEM_NORMAL) = delete;
		wxAuiToolBarItem* AddTool(int tool_id, const wxString &label, const wxBitmap &bitmap, const wxBitmap &disabled_bitmap, wxItemKind kind, const wxString &short_help_string, const wxString &long_help_string, wxObject* client_data) = delete;
		wxAuiToolBarItem* AddTool(int tool_id, const wxBitmap &bitmap, const wxBitmap &disabled_bitmap, bool toggle = false, wxObject* client_data = nullptr, const wxString &short_help_string = wxEmptyString, const wxString &long_help_string = wxEmptyString) = delete;
		bool GetOverflowVisible() const = delete;
		bool GetToolBarFits() const = delete;
		bool GetGripperVisible() const = delete;
		wxAuiToolBarItem* FindTool(int id) const = delete;
		bool DeleteTool(int tool_id) = delete;
		bool DeleteByIndex(int tool_id) = delete;
		void ToggleTool(int tool_id, bool state) = delete;
		bool GetToolToggled(int tool_id) const = delete;
		void EnableTool(int tool_id, bool state) = delete;
		bool GetToolEnabled(int tool_id) const = delete;
		void SetToolDropDown(int tool_id, bool dropdown) = delete;
		bool GetToolDropDown(int tool_id) const = delete;
		void SetToolProportion(int tool_id, int proportion) = delete;
		int GetToolProportion(int tool_id) const = delete;
		wxRect GetToolRect(int tool_id) const = delete;
		void SetToolSticky(int tool_id, bool sticky) = delete;
		bool GetToolSticky(int tool_id) const = delete;
		wxString GetToolLabel(int tool_id) const = delete;
		void SetToolLabel(int tool_id, const wxString &label) = delete;
		wxBitmap GetToolBitmap(int tool_id) const = delete;
		void SetToolBitmap(int tool_id, const wxBitmap &bitmap) = delete;
		wxString GetToolShortHelp(int tool_id) const = delete;
		void SetToolShortHelp(int tool_id, const wxString &help_string) = delete;
		wxString GetToolLongHelp(int tool_id) const = delete;
		void SetToolLongHelp(int tool_id, const wxString &help_string) = delete;
		void SetCustomOverflowItems(const wxAuiToolBarItemArray &prepend, const wxAuiToolBarItemArray &append) = delete;
		bool GetToolFitsByIndex(int tool_id) const = delete;
		int GetToolPos(int tool_id) const = delete;
		int GetToolIndex(int tool_id) const = delete;
		bool GetToolFits(int tool_id) const = delete;

	public:
		wxDECLARE_DYNAMIC_CLASS(KxAuiToolBar);
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxAuiToolBarArt: public wxAuiDefaultToolBarArt
{
	private:
		KxAuiToolBar* m_Instance = nullptr;

	public:
		KxAuiToolBarArt(KxAuiToolBar* object)
			:m_Instance(object)
		{
		}
		virtual ~KxAuiToolBarArt()
		{
		}

	public:
		virtual void DrawPlainBackground(wxDC& dc, wxWindow* window, const wxRect& rect) override;
};
