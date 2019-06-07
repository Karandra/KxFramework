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

			int ToNativeColumnIndex(size_t index) const;
			size_t FromNativeColumnIndex(int nativeIndex) const;

			void OnClick(wxHeaderCtrlEvent& event);
			void OnRClick(wxHeaderCtrlEvent& event);
			void OnResize(wxHeaderCtrlEvent& event);
			void OnReordered(wxHeaderCtrlEvent& event);
			void OnWindowClick(wxMouseEvent& event);

		protected:
			bool UpdateColumnWidthToFit(unsigned int index, int titleWidth) override;
			const wxHeaderColumn& GetColumn(unsigned int index) const override;
			bool MSWHandleNotify(WXLRESULT* result, int notification, WXWPARAM wParam, WXLPARAM lParam);

			void OnColumnInserted(Column& column);
			void UpdateColumn(Column& column);
			void UpdateColumn(size_t index);
			void UpdateDisplay();

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

		public:
			wxDECLARE_NO_COPY_CLASS(HeaderCtrl);
	};
}
