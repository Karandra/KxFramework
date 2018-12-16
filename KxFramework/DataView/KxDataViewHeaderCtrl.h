#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
class KX_API KxDataViewCtrl;

class KX_API KxDataViewHeaderCtrl: public wxHeaderCtrl
{
	private:
		void FinishEditing();
		bool SendEvent(wxEventType type, int index);

		void OnClick(wxHeaderCtrlEvent& event);
		void OnRClick(wxHeaderCtrlEvent& event);
		void OnResize(wxHeaderCtrlEvent& event);
		void OnEndReorder(wxHeaderCtrlEvent& event);
		
		void OnWindowClick(wxMouseEvent& event);

	protected:
		// Implement/override wxHeaderCtrl functions by forwarding them to the main control
		virtual const wxHeaderColumn& GetColumn(unsigned int size_t) const override;
		virtual bool UpdateColumnWidthToFit(unsigned int index, int titleWidth) override;

	public:
		KxDataViewHeaderCtrl(KxDataViewCtrl* parent);

	public:
		const KxDataViewCtrl* GetOwner() const;
		KxDataViewCtrl* GetOwner();

		void ToggleSortByColumn(size_t column);

	public:
		wxDECLARE_NO_COPY_CLASS(KxDataViewHeaderCtrl);
};
