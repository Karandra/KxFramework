#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewItem.h"
class KX_API KxDataViewCtrl;
class KX_API KxDataViewColumn;
class KX_API KxDataViewMainWindow;
class KX_API KxDataViewModel;
class KX_API KxDataViewListModel;

class KX_API KxDataViewEvent: public wxNotifyEvent
{
	private:
		KxDataViewModel* m_Model = NULL;
		KxDataViewColumn* m_Column = NULL;
		KxDataViewItem m_Item;
		wxPoint m_Position = wxDefaultPosition;
		wxAny m_Value;
		bool m_IsEditCancelled = false;
		size_t m_CacheHintFrom = 0;
		size_t m_CacheHintTo = 0;

	private:
		KxDataViewCtrl* GetOwner() const;
		KxDataViewMainWindow* GetMainWindow() const;
		KxDataViewListModel* GetListModel() const;

	public:
		KxDataViewEvent(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE);
		virtual wxEvent* Clone() const override
		{
			return new KxDataViewEvent(*this);
		}

	public:
		KxDataViewItem GetItem() const
		{
			return m_Item;
		}
		void SetItem(const KxDataViewItem& item)
		{
			m_Item = item;
		}

		KxDataViewColumn* GetColumn() const
		{
			return m_Column;
		}
		void SetColumn(KxDataViewColumn* column)
		{
			m_Column = column;
		}

		KxDataViewModel* GetModel() const
		{
			return m_Model;
		}
		void SetModel(KxDataViewModel *model)
		{
			m_Model = model;
		}

		const wxAny& GetValue() const
		{
			return m_Value;
		}
		wxAny& GetValue()
		{
			return m_Value;
		}

		bool IsEditCancelled() const
		{
			return m_IsEditCancelled;
		}
		void SetEditCanceled(bool editCancelled)
		{
			m_IsEditCancelled = editCancelled;
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
		size_t GetCacheHintFrom() const
		{
			return m_CacheHintFrom;
		}
		size_t GetCacheHintTo() const
		{
			return m_CacheHintTo;
		}
		
		// Return hints as logical rows. Will return invalid row if control uses non-list model
		size_t GetCacheHintRowFrom() const;
		size_t GetCacheHintRowTo() const;

		// Return hints as items
		KxDataViewItem GetCacheHintItemFrom() const;
		KxDataViewItem GetCacheHintItemTo() const;

		// These are physical rows as user sees them, not logical
		void SetCacheHints(size_t from, size_t to)
		{
			m_CacheHintFrom = from;
			m_CacheHintTo = to;
		}
};

//////////////////////////////////////////////////////////////////////////
class KX_API KxDataViewEventDND: public KxDataViewEvent
{
	private:
		wxDataObject* m_DataObject = NULL;
		wxDataFormat m_DataFormat;
		void* m_DataBuffer = NULL;
		size_t m_DataSize = 0;
		int m_DragFlags = 0;
		wxDragResult m_DropEffect = wxDragNone;

	public:
		KxDataViewEventDND(wxEventType type = wxEVT_NULL, wxWindowID winid = wxID_NONE);
		virtual wxEvent* Clone() const override
		{
			return new KxDataViewEventDND(*this);
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

//////////////////////////////////////////////////////////////////////////
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_SELECTED, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_HOVERED, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_ACTIVATED, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_COLLAPSED, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EXPANDED, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_COLLAPSING, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EXPANDING, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_CONTEXT_MENU, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_VALUE_CHANGED, KxDataViewEvent);

KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_ATTACH, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_DETACH, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_STARTING, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_STARTED, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_EDIT_DONE, KxDataViewEvent);

KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_HEADER_CLICK, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_SORTED, KxDataViewEvent);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_COLUMN_REORDERED, KxDataViewEvent);

KX_DECLARE_EVENT(KxEVT_DATAVIEW_CACHE_HINT, KxDataViewEvent);

KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_DRAG, KxDataViewEventDND);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, KxDataViewEventDND);
KX_DECLARE_EVENT(KxEVT_DATAVIEW_ITEM_DROP, KxDataViewEventDND);
