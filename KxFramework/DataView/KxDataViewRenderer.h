#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWithOptions.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/DataView/KxDataViewCtrl.h"
#include "KxFramework/DataView/KxDataViewColumn.h"
#include "KxFramework/DataView/KxDataViewItemAttributes.h"
class KxDataViewCtrl;
class KxDataViewModel;
class KxDataViewMainWindow;

enum KxDataViewRendererOptions
{
	KxDVR_NONE = 0,
	KxDVR_IMAGELIST_BITMAP_DRAWING = 1 << 0,
	KxDVR_ALLOW_BITMAP_SCALEDOWN = 1 << 1,
};

class KxDataViewRenderer: public wxObject, public KxWithOptions<KxDataViewRendererOptions, KxDVR_NONE>
{
	friend class KxDataViewColumn;
	friend class KxDataViewMainWindow;
	friend class KxDataViewMainWindowMaxWidthCalculator;

	protected:
		enum class ProgressBarState
		{
			Normal,
			Paused,
			Error,
			Partial,
		};

	private:
		wxAlignment m_Alignment = wxALIGN_INVALID;
		wxEllipsizeMode m_EllipsizeMode = wxELLIPSIZE_END;
		KxDataViewCellMode m_CellMode = KxDATAVIEW_CELL_INERT;
		KxDataViewColumn* m_Column = NULL;
		
		wxDC* m_DC = NULL;
		wxGCDC* m_GCDC = NULL;
		bool m_Enabled = true;
		KxDataViewItemAttributes m_Attributes;

	private:
		/* Drawing */
		void PrepareItemToDraw(const KxDataViewItem& item, KxDataViewCellState cellState);
		bool CheckedGetValue(wxAny& value, const KxDataViewItem& item);

		void CallDrawCellBackground(const wxRect& cellRect, KxDataViewCellState cellState);
		void CallDrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState);

	protected:
		/* Activation */
		virtual bool HasActivator() const
		{
			return false;
		}
		virtual bool OnActivateCell(const KxDataViewItem& item, const wxRect& cellRect, const wxMouseEvent* mouseEvent = NULL)
		{
			return false;
		}

		/* Value */
		virtual bool ValidateValue(const wxAny& value) const
		{
			return true;
		}
		virtual bool SetValue(const wxAny& value) = 0;

		/* Drawing */
		const KxDataViewItemAttributes& GetItemAttributes() const
		{
			return m_Attributes;
		}
		
		virtual bool IsEnabled() const;
		void SetEnabled(bool enabled);

		virtual bool HasSolidBackground() const;
		virtual bool HasSpecialBackground() const;
		virtual void DrawCellBackground(const wxRect& cellRect, KxDataViewCellState cellState)
		{
		}
		virtual void DrawCellContent(const wxRect& cellRect, KxDataViewCellState cellState) = 0;
		virtual wxSize GetCellSize() const;
		
	public:
		// Drawing helpers
		int DoCalcCenter(int pos, int size) const;
		int DoFindFirstNewLinePos(const wxString& string) const;
		int DoGetControlFlags(KxDataViewCellState cellState) const;

		wxDC& GetDC() const
		{
			return *m_DC;
		}
		void SetDC(wxDC& dc)
		{
			m_DC = &dc;
		}
		
		bool HasGCDC() const
		{
			return m_GCDC != NULL;
		}
		wxGraphicsContext* GetGraphicsContext() const
		{
			return m_GCDC ? m_GCDC->GetGraphicsContext() : NULL;
		}
		wxDC& GetGCDC() const
		{
			return *m_GCDC;
		}
		void SetGCDC(wxGCDC& gc)
		{
			m_GCDC = &gc;
		}
		void UnsetGCDC()
		{
			m_GCDC = NULL;
		}

		wxSize DoGetTextExtent(const wxString& string) const;
		bool DoDrawText(const wxRect& cellRect, KxDataViewCellState cellState, const wxString& string, int offsetX = 0);
		bool DoDrawBitmap(const wxRect& cellRect, KxDataViewCellState cellState, const wxBitmap& bitmap);
		bool DoDrawProgressBar(const wxRect& cellRect, KxDataViewCellState cellState, int value, int range, ProgressBarState state = ProgressBarState::Normal);

	public:
		KxDataViewRenderer(KxDataViewCellMode cellMode = KxDATAVIEW_CELL_INERT, int alignment = wxALIGN_INVALID);
		virtual ~KxDataViewRenderer();

	public:
		KxDataViewMainWindow* GetMainWindow() const
		{
			return m_Column ? m_Column->GetOwner()->GetMainWindow() : NULL;
		}
		KxDataViewCtrl* GetView() const
		{
			return m_Column ? m_Column->GetOwner() : NULL;
		}
		KxDataViewModel* GetModel() const
		{
			return GetView()->GetModel();
		}
		
		KxDataViewColumn* GetColumn() const
		{
			return m_Column;
		}
		void SetColumn(KxDataViewColumn* column)
		{
			m_Column = column;
		}

		virtual wxAlignment GetEffectiveAlignment() const;
		wxAlignment GetAlignment() const
		{
			return m_Alignment;
		}
		void SetAlignment(wxAlignment alignment)
		{
			m_Alignment = alignment;
		}
		void SetAlignment(int alignment)
		{
			SetAlignment(static_cast<wxAlignment>(alignment));
		}

		wxEllipsizeMode GetEllipsizeMode() const
		{
			return m_EllipsizeMode;
		}
		void SetEllipsizeMode(wxEllipsizeMode mode)
		{
			m_EllipsizeMode = mode;
		}

		KxDataViewCellMode GetCellMode() const
		{
			return m_CellMode;
		}
		void SetCellMode(KxDataViewCellMode mode)
		{
			m_CellMode = mode;
		}

	public:
		wxDECLARE_ABSTRACT_CLASS(KxDataViewRenderer);
};
