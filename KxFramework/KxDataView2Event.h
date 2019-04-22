#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Common.h"
#include "KxFramework/DataView2/Row.h"

namespace KxDataView2
{
	class KX_API Node;
	class KX_API View;
	class KX_API Column;
	class KX_API MainWindow;
	class KX_API Model;
}

namespace KxDataView2
{
	class KX_API Event: public wxNotifyEvent
	{
		private:
			Node* m_Node = nullptr;
			Column* m_Column = nullptr;
			wxPoint m_Position = wxDefaultPosition;

			Row m_CacheHintFrom;
			Row m_CacheHintTo;

		protected:
			View* GetView() const;
			MainWindow* GetMainWindow() const;

		public:
			Event(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:wxNotifyEvent(type, winid)
			{
			}
			wxEvent* Clone() const override
			{
				return new Event(*this);
			}

		public:
			Node* GetNode() const
			{
				return m_Node;
			}
			void SetNode(Node* item)
			{
				m_Node = item;
			}

			Column* GetColumn() const
			{
				return m_Column;
			}
			void SetColumn(Column* column)
			{
				m_Column = column;
			}

			wxPoint GetPosition() const 
			{
				return m_Position;
			}
			void SetPosition(int x, int y)
			{
				m_Position.x = x;
				m_Position.y = y;
			}
			void SetPosition(const wxPoint& pos)
			{
				m_Position = pos;
			}

			// Return hints as physical rows
			std::tuple<Row, Row> GetCacheHint() const
			{
				return {m_CacheHintFrom, m_CacheHintTo};
			}
			Row GetCacheHintFrom() const
			{
				return m_CacheHintFrom;
			}
			Row GetCacheHintTo() const
			{
				return m_CacheHintTo;
			}
			
			// These are physical rows as user sees them, not logical
			void SetCacheHints(Row from, Row to)
			{
				m_CacheHintFrom = from;
				m_CacheHintTo = to;
			}
	};
}

namespace KxDataView2
{
	class KX_API EventEditor: public Event
	{
		private:
			wxAny m_Value;
			bool m_IsEditCancelled = false;

		public:
			EventEditor(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:Event(type, winid)
			{
			}
			wxEvent* Clone() const override
			{
				return new EventEditor(*this);
			}

		public:
			bool IsEditCancelled() const
			{
				return m_IsEditCancelled;
			}
			void SetEditCanceled(bool editCancelled = true)
			{
				m_IsEditCancelled = editCancelled;
			}

			const wxAny& GetValue() const
			{
				return m_Value;
			}
			void SetValue(wxAny&& value)
			{
				m_Value = std::move(value);
				value.MakeNull();
			}
			void SetValue(const wxAny& value)
			{
				m_Value = value;
			}
	};
}

namespace KxDataView2
{
	class KX_API EventDND: public Event
	{
		friend class KX_API MainWindow;

		private:
			wxDataObjectSimple* m_DataObject = nullptr;
			wxDragResult m_DropResult = wxDragNone;
			int m_DragFlags = wxDrag_CopyOnly;

		protected:
			wxDataObjectSimple* GetDataObject() const
			{
				return m_DataObject;
			}
			void SetDataObject(wxDataObjectSimple* object)
			{
				m_DataObject = object;
			}
			
			void SetDragFlags(int flags)
			{
				m_DragFlags = flags;
			}
			void SetDropEffect(wxDragResult effect)
			{
				m_DropResult = effect;
			}

		public:
			EventDND(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:Event(type, winid)
			{
			}
			wxEvent* Clone() const override
			{
				return new EventDND(*this);
			}

		public:
			// Drag
			wxDataObjectSimple* GetDragObject(const wxDataFormat& format) const;
			template<class T> T* GetDragObject(const wxDataFormat& format) const
			{
				static_assert(std::is_base_of_v<wxDataObjectSimple, T>);
				return dynamic_cast<T*>(GetDragObject(format));
			}

			int GetDragFlags() const
			{
				return m_DragFlags;
			}
			void DragDone(const wxDataObjectSimple& dataObject, int flags = wxDrag_CopyOnly);
			void DragCancel();

			// Drop
			wxDataObjectSimple* GetRecievedDataObject() const;
			template<class T> T* GetRecievedDataObject() const
			{
				static_assert(std::is_base_of_v<wxDataObjectSimple, T>);
				return dynamic_cast<T*>(GetRecievedDataObject());
			}
			
			wxDragResult GetDropResult() const
			{
				return m_DropResult;
			}
			void DropDone(wxDragResult result = wxDragNone);
			void DropCancel();
			void DropError();
	};
}

namespace KxDataView2
{
	KX_DECLARE_EVENT(EVENT_ITEM_SELECTED, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_HOVERED, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_ACTIVATED, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_COLLAPSED, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_EXPANDED, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_COLLAPSING, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_EXPANDING, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_CONTEXT_MENU, Event);
	KX_DECLARE_EVENT(EVENT_ITEM_VALUE_CHANGED, Event);

	KX_DECLARE_EVENT(EVENT_ITEM_EDIT_STARTING, EventEditor);
	KX_DECLARE_EVENT(EVENT_ITEM_EDIT_STARTED, EventEditor);
	KX_DECLARE_EVENT(EVENT_ITEM_EDIT_DONE, EventEditor);

	KX_DECLARE_EVENT(EVENT_ITEM_DRAG, EventDND);
	KX_DECLARE_EVENT(EVENT_ITEM_DROP, EventDND);
	KX_DECLARE_EVENT(EVENT_ITEM_DROP_POSSIBLE, EventDND);

	KX_DECLARE_EVENT(EVENT_COLUMN_HEADER_CLICK, Event);
	KX_DECLARE_EVENT(EVENT_COLUMN_HEADER_RCLICK, Event);
	KX_DECLARE_EVENT(EVENT_COLUMN_SORTED, Event);
	KX_DECLARE_EVENT(EVENT_COLUMN_MOVED, Event);

	KX_DECLARE_EVENT(EVENT_VIEW_CACHE_HINT, Event);
}
