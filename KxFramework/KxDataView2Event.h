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

		public:
			Event(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:wxNotifyEvent(type, winid)
			{
			}
			virtual wxEvent* Clone() const override
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
	class KX_API EditorEvent: public Event
	{
		private:
			wxAny m_Value;
			bool m_IsEditCancelled = false;

		public:
			EditorEvent(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:Event(type, winid)
			{
			}
			virtual wxEvent* Clone() const override
			{
				return new EditorEvent(*this);
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
			const wxDataObject* m_DataObject = nullptr;
			bool m_IsDataObjectOwned = false;

			wxDragResult m_DropEffect = wxDragNone;
			wxDataFormat m_DataFormat;
			int m_DragFlags = wxDrag_CopyOnly;

		protected:
			void SetDataObject(const wxDataObject* object)
			{
				m_DataObject = object;
				m_IsDataObjectOwned = false;
			}
			std::unique_ptr<wxDataObject> TakeDataObject()
			{
				wxDataObject* temp = const_cast<wxDataObject*>(m_DataObject);
				m_DataObject = nullptr;
				return std::unique_ptr<wxDataObject>(temp);
			}

		public:
			EventDND(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:Event(type, winid)
			{
			}
			~EventDND()
			{
				if (m_IsDataObjectOwned)
				{
					delete m_DataObject;
				}
			}
			virtual wxEvent* Clone() const override
			{
				EventDND* clone = new EventDND(*this);
				clone->m_DataObject = nullptr;
				clone->m_IsDataObjectOwned = false;
				return clone;
			}

		public:
			void NewDataObject(std::unique_ptr<wxDataObject> object)
			{
				m_DataObject = object.release();
				m_IsDataObjectOwned = true;
			}
			const wxDataObject* GetDataObject() const
			{
				return m_DataObject;
			}
			template<class T> const T* GetDataObjectAs() const
			{
				return dynamic_cast<const T*>(m_DataObject);
			}
			bool HasDataObject() const
			{
				return m_DataObject != nullptr;
			}

			void SetDataFormat(const wxDataFormat& format)
			{
				m_DataFormat = format;
			}
			wxDataFormat GetDataFormat() const
			{
				return m_DataFormat;
			}

			void SetDropEffect(wxDragResult effect)
			{
				m_DropEffect = effect;
			}
			wxDragResult GetDropEffect() const
			{
				return m_DropEffect;
			}
	
			void SetDragFlags(int flags)
			{
				m_DragFlags = flags;
			}
			int GetDragFlags() const
			{
				return m_DragFlags;
			}
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

	KX_DECLARE_EVENT(EVENT_ITEM_EDIT_STARTING, EditorEvent);
	KX_DECLARE_EVENT(EVENT_ITEM_EDIT_STARTED, EditorEvent);
	KX_DECLARE_EVENT(EVENT_ITEM_EDIT_DONE, EditorEvent);

	KX_DECLARE_EVENT(EVENT_ITEM_DRAG, EventDND);
	KX_DECLARE_EVENT(EVENT_ITEM_DROP_POSSIBLE, EventDND);
	KX_DECLARE_EVENT(EVENT_ITEM_DROP, EventDND);

	KX_DECLARE_EVENT(EVENT_COLUMN_HEADER_CLICK, Event);
	KX_DECLARE_EVENT(EVENT_COLUMN_HEADER_RCLICK, Event);
	KX_DECLARE_EVENT(EVENT_COLUMN_SORTED, Event);
	KX_DECLARE_EVENT(EVENT_COLUMN_MOVED, Event);

	KX_DECLARE_EVENT(EVENT_VIEW_CACHE_HINT, Event);
}
