#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"

namespace KxDataView2
{
	class KX_API View;
	class KX_API MainWindow;
	class KX_API Column;
}

namespace KxDataView2
{
	class KX_API HeaderCtrl: public wxHeaderCtrl
	{
		friend class View;
		friend class MainWindow;
		friend class Column;

		private:
			View* m_View = nullptr;

		private:
			void FinishEditing();
			bool SendEvent(wxEventType type, int index, std::optional<wxRect> rect = {});

			void OnClick(wxHeaderCtrlEvent& event);
			void OnRClick(wxHeaderCtrlEvent& event);
			void OnResize(wxHeaderCtrlEvent& event);
			void OnReordered(wxHeaderCtrlEvent& event);
			void OnWindowClick(wxMouseEvent& event);

		protected:
			const wxHeaderColumn& GetColumn(unsigned int index) const override;
			wxRect GetDropdownRect(size_t index) const;
			bool UpdateColumnWidthToFit(unsigned int index, int titleWidth) override;
			void UpdateColumn(size_t index);
			
			void UpdateColumnIndices();
			void DoUpdate(unsigned int index) override;
			void DoSetCount(unsigned int count) override;
			void DoInsertItem(const Column& column, size_t index);
			bool MSWHandleNotify(WXLRESULT* result, int notification, WXWPARAM wParam, WXLPARAM lParam);

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

			size_t GetColumnCount() const;
			Column* GetColumnAt(size_t index) const;
			Column* GetColumnDisplayedAt(size_t index) const;

			void ToggleSortByColumn(size_t index);
			void ToggleSortByColumn(Column& column);

			bool SetBackgroundColour(const wxColour& color) override;

		public:
			wxDECLARE_NO_COPY_CLASS(HeaderCtrl);
	};
}
