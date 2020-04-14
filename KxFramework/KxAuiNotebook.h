#pragma once
#include "KxFramework/KxFramework.h"
#include "Kx/Drawing/WithImageList.h"
#include <wx/aui/auibook.h>
#include <wx/aui/dockart.h>

class KX_API KxAuiNotebook: public KxFramework::WithImageListWrapper<wxAuiNotebook>
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
		RendererType m_CurrentRendererType = Default;

	public:
		static const long DefaultStyle = wxAUI_NB_TOP|wxAUI_NB_SCROLL_BUTTONS;

		KxAuiNotebook() = default;
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
		KxAuiNotebook* m_Notebook = nullptr;

	public:
		KxAuiDefaultTabArt(KxAuiNotebook* object);
		KxAuiDefaultTabArt* Clone() override
		{
			return new KxAuiDefaultTabArt(*this);
		}

	public:
		void DrawBackground(wxDC& dc, wxWindow* window, const wxRect& rect) override;
		void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& pane, const wxRect& inRect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent) override;
		void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& inRect, int bitmapId, int buttonState, int orientation, wxRect* outRect) override;

		int GetIndentSize() override
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
		KxAuiNotebook* m_Notebook = nullptr;

	public:
		KxAuiNotebookDockArtProvider(KxAuiNotebook* object);

	public:
		void DrawBackground(wxDC& dc, wxWindow* window, int orientation, const wxRect& rect) override;
		void DrawBorder(wxDC& dc, wxWindow* window, const wxRect& rect, wxAuiPaneInfo& pane) override;
};
