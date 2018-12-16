#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithImageList.h"
#include <wx/aui/auibook.h>
#include <wx/aui/dockart.h>

class KX_API KxAuiNotebook: public KxWithImageListWrapper<wxAuiNotebook>
{
	public:
		enum RendererType
		{
			Default,
			Generic,
			Simple
		};

	private:
		int m_TabIndent = 3;
		wxSize m_ContentMargin = wxSize(3, 3);
		RendererType m_CurrentRendererType = Default;

	public:
		static const long DefaultStyle = wxAUI_NB_TOP|wxAUI_NB_SCROLL_BUTTONS;

		KxAuiNotebook() {}
		KxAuiNotebook(wxWindow* parent,
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

	public:
		virtual bool ShouldInheritColours() const override
		{
			return true;
		}
		virtual bool InsertPage(size_t index, wxWindow* window, const wxString& label, bool select = false, int imageID = NO_IMAGE) override;
		
		wxSize GetContentMargin() const
		{
			return m_ContentMargin;
		}
		void SetContentMargin(const wxSize& margins)
		{
			m_ContentMargin = margins;
			m_ContentMargin.SetDefaults(wxSize(3, 3));
		}
		
		int GetTabIndent() const
		{
			return m_TabIndent;
		}
		void SetTabIndent(int value)
		{
			m_TabIndent = value;
		}

		RendererType GetRenderer() const
		{
			return m_CurrentRendererType;
		}
		void SetRenderer(RendererType value);

	public:
		wxDECLARE_DYNAMIC_CLASS(KxAuiNotebook);
};

//////////////////////////////////////////////////////////////////////////
using KxAuiDefaultTabArtBase = wxAuiDefaultTabArt;
class KX_API KxAuiDefaultTabArt: public KxAuiDefaultTabArtBase
{
	private:
		const KxAuiNotebook* m_Notebook = NULL;

	public:
		KxAuiDefaultTabArt(KxAuiNotebook* object);
		KxAuiDefaultTabArt* Clone() override
		{
			return new KxAuiDefaultTabArt(*this);
		}

	public:
		virtual void DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect) override;
		virtual void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent);
		virtual void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect);

		virtual int GetIndentSize() override
		{
			if (m_Notebook)
			{
				return m_Notebook->GetTabIndent();
			}
			return 3; // 3 is the default in wxAuiDefaultTabArt
		}
};

//////////////////////////////////////////////////////////////////////////
using KxAuiDefaultDockArtBase = wxAuiDefaultDockArt;
class KX_API KxAuiNotebookDockArtProvider: public KxAuiDefaultDockArtBase
{
	private:
		const KxAuiNotebook* m_Notebook = NULL;

	public:
		KxAuiNotebookDockArtProvider(KxAuiNotebook* object);

	public:
		virtual void DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect) override;
		virtual void DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane) override;
};
