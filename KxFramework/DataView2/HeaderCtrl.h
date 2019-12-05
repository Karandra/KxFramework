#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/KxDataView2Event.h"
#include "KxFramework/KxImageList.h"
#include "KxFramework/KxWindowRefreshScheduler.h"
struct _HD_ITEMW;

namespace KxDataView2
{
	class KX_API View;
	class KX_API MainWindow;
	class KX_API Column;
	class KX_API Event;
}

namespace KxDataView2
{
	class KX_API HeaderCtrl: public KxWindowRefreshScheduler<wxHeaderCtrl>
	{
		friend class View;
		friend class MainWindow;
		friend class Column;

		public:
			struct EventResult
			{
				bool Processed = false;
				bool Allowed = false;
			};

		private:
			View* m_View = nullptr;
			HWND m_HeaderCtrlHandle = nullptr;

			std::unique_ptr<KxImageList> m_ImageList;
			Column* m_DraggedColumn = nullptr;
			Column* m_ResizedColumn = nullptr;
			bool m_UpdateColumns = false;

		private:
			HWND GetHeaderCtrlHandle() const;

			void FinishEditing();
			EventResult SendCtrlEvent(Event& event, wxEventType type, Column* column = nullptr, std::optional<wxRect> rect = {});
			EventResult SendCtrlEvent(wxEventType type, Column* column = nullptr, std::optional<wxRect> rect = {})
			{
				Event event;
				return SendCtrlEvent(event, type, column, std::move(rect));
			}

			void OnCreate(wxWindowCreateEvent& event);
			void OnDestroy(wxWindowDestroyEvent& event);

			void OnClick(wxHeaderCtrlEvent& event);
			void OnRClick(wxHeaderCtrlEvent& event);
			void OnWindowClick(wxMouseEvent& event);
			void OnSeparatorDClick(wxHeaderCtrlEvent& event);

			void OnResize(wxHeaderCtrlEvent& event);
			void OnResizeEnd(wxHeaderCtrlEvent& event);
			void OnReorderEnd(wxHeaderCtrlEvent& event);

		protected:
			const wxHeaderColumn& GetColumn(unsigned int index) const override;
			bool UpdateColumnWidthToFit(unsigned int index, int = 0) override;
			
			void DoUpdate(unsigned int = 0) override;
			void DoSetCount(unsigned int = 0) override;
			void DoMakeItem(_HD_ITEMW& item, const Column& column);
			bool MSWOnNotify(int ctrlID, WXLPARAM lParam, WXLPARAM* result) override;
			void OnInternalIdle() override;
			
		protected:
			void UpdateColumn(const Column& column);
			void UpdateColumnCount();

			wxRect GetDropdownRect(const Column& column) const;
			wxRect GetDropdownRect(size_t index) const;

		public:
			HeaderCtrl(View* parent);
			~HeaderCtrl();

		public:
			View* GetView() const
			{
				return m_View;
			}
			MainWindow* GetMainWindow() const;

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
