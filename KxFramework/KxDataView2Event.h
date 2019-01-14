#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView2/Common.h"
#include "KxFramework/DataView2/Row.h"

namespace Kx::DataView2
{
	class KX_API Node;
	class KX_API View;
	class KX_API Column;
	class KX_API MainWindow;
	class KX_API Model;
}

namespace Kx::DataView2
{
	class KX_API Event: public wxNotifyEvent
	{
		private:
			Node* m_Item = nullptr;
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
			Node* GetItem() const
			{
				return m_Item;
			}
			void SetItem(Node* item)
			{
				m_Item = item;
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

namespace Kx::DataView2
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

namespace Kx::DataView2
{
	class KX_API EventDND: public Event
	{
		private:
			wxDataObject* m_DataObject = nullptr;
			wxDataFormat m_DataFormat;
			void* m_DataBuffer = nullptr;
			size_t m_DataSize = 0;
			int m_DragFlags = 0;
			wxDragResult m_DropEffect = wxDragNone;

		public:
			EventDND(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE)
				:Event(type, winid)
			{
			}
			virtual wxEvent* Clone() const override
			{
				return new EventDND(*this);
			}

		public:
			void SetDataObject(wxDataObject* object)
			{
				m_DataObject = object;
			}
			wxDataObject* GetDataObject() const
			{
				return m_DataObject;
			}

			void SetDataFormat(const wxDataFormat& format)
			{
				m_DataFormat = format;
			}
			wxDataFormat GetDataFormat() const
			{
				return m_DataFormat;
			}
		
			void SetDataBuffer(void* buffer)
			{
				m_DataBuffer = buffer;
			}
			void* GetDataBuffer() const
			{
				return m_DataBuffer;
			}

			void SetDataSize(size_t size)
			{
				m_DataSize = size;
			}
			size_t GetDataSize() const
			{
				return m_DataSize;
			}
		
			void SetDragFlags(int flags)
			{
				m_DragFlags = flags;
			}
			int GetDragFlags() const
			{
				return m_DragFlags;
			}
		
			void SetDropEffect(wxDragResult effect)
			{
				m_DropEffect = effect;
			}
			wxDragResult GetDropEffect() const
			{
				return m_DropEffect;
			}
	};
}

namespace Kx::DataView2
{
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_SELECTED, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_HOVERED, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_ACTIVATED, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_COLLAPSED, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EXPANDED, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_COLLAPSING, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EXPANDING, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_CONTEXT_MENU, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_VALUE_CHANGED, Event);

	KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_HEADER_CLICK, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_SORTED, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_REORDERED, Event);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_CACHE_HINT, Event);

	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_ATTACH, EditorEvent);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_DETACH, EditorEvent);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_STARTING, EditorEvent);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_STARTED, EditorEvent);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_DONE, EditorEvent);

	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_DRAG, EventDND);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, EventDND);
	KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_DROP, EventDND);
}
