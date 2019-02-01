#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"

namespace KxDataView2
{
	class KX_API View;
}

namespace KxDataView2
{
	class KX_API HeaderCtrl: public wxHeaderCtrl
	{
		private:
			View* m_View = nullptr;

		private:
			void FinishEditing();
			bool SendEvent(wxEventType type, int index);

			void OnClick(wxHeaderCtrlEvent& event);
			void OnRClick(wxHeaderCtrlEvent& event);
			void OnResize(wxHeaderCtrlEvent& event);
			void OnEndReorder(wxHeaderCtrlEvent& event);
			void OnWindowClick(wxMouseEvent& event);

		protected:
			virtual const wxHeaderColumn& GetColumn(unsigned int index) const override;
			virtual bool UpdateColumnWidthToFit(unsigned int index, int titleWidth) override;

		public:
			HeaderCtrl(View* parent);

		public:
			const View* GetView() const
			{
				return m_View;
			}
			View* GetView()
			{
				return m_View;
			}

			void ToggleSortByColumn(size_t column);

		public:
			wxDECLARE_NO_COPY_CLASS(HeaderCtrl);
	};
}
