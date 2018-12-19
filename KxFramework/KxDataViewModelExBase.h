#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDataView.h"
#include <typeinfo>
class KX_API KxDataViewModelExDragDropData;

template<class T = KxDataViewModel>
class KxDataViewModelExBase: public T
{
	private:
		KxDataViewCtrlStyles m_DataViewFlags = KxDataViewCtrl::DefaultStyle;
		KxDataViewCtrl* m_View = nullptr;

	private:
		void CreateDataView(wxWindow* window, wxSizer* sizer)
		{
			m_View = OnCreateDataView(window);
			m_View->AssignModel(this);

			if (sizer)
			{
				sizer->Add(OnGetDataViewWindow(), 1, wxEXPAND);
			}
		}

	protected:
		void SetView(KxDataViewCtrl* view)
		{
			m_View = view;
		}
		virtual KxDataViewCtrl* OnCreateDataView(wxWindow* window)
		{
			return new KxDataViewCtrl(window, wxID_NONE, m_DataViewFlags);
		}
		virtual wxWindow* OnGetDataViewWindow()
		{
			return GetView();
		}
		virtual void OnInitControl() = 0;

	public:
		void Create(wxWindow* window, wxSizer* sizer = nullptr)
		{
			CreateDataView(window, sizer);
			OnInitControl();
		}
		virtual ~KxDataViewModelExBase()
		{
		}

	public:
		KxDataViewCtrlStyles GetDataViewFlags() const
		{
			return m_DataViewFlags;
		}
		void SetDataViewFlags(KxDataViewCtrlStyles nDataViewFlags)
		{
			m_DataViewFlags = nDataViewFlags;
		}
		
		virtual KxDataViewCtrl* GetView() const
		{
			return m_View;
		}
		wxWindow* GetViewTLW() const
		{
			return wxGetTopLevelParent(GetView());
		}

		virtual void RefreshItems()
		{
			this->ItemsCleared();
		}
		void SelectItem(const KxDataViewItem& item = KxDataViewItem(), bool bUnselectAll = false)
		{
			if (bUnselectAll && GetView()->HasFlag(KxDV_MULTIPLE_SELECTION))
			{
				GetView()->UnselectAll();
			}
			GetView()->Select(item);
			GetView()->EnsureVisible(item);
			GetView()->GenerateSelectionEvent(item);
		}
};

template<class DragDropDataT = KxDataViewModelExDragDropData>
class KxDataViewModelExDragDropEnabled
{
	public:
		using DragDropDataObjectT = DragDropDataT;

	private:
		KxDataViewModelExDragDropData* m_DragDropDataObject = nullptr;

	private:
		void SetPosition(KxDataViewEventDND& event)
		{
			int x = -1;
			int y = -1;
			wxGetMousePosition(&x, &y);
			GetViewCtrl()->ScreenToClient(&x, &y);
			event.SetPosition(x, y);
		}

		void OnDrag(KxDataViewEventDND& event)
		{
			if (OnDragItems(event))
			{
				m_DragDropDataObject->SetFormat(GetDragDropFormat());
				event.SetDataFormat(m_DragDropDataObject->GetFormat());
				event.SetDataObject(m_DragDropDataObject);
			}
		}
		void OnDrop(KxDataViewEventDND& event)
		{
			SetPosition(event);
			if (!OnDropItems(event))
			{
				wxBell();
			}
			m_DragDropDataObject = nullptr;
		}
		void OnDropPossible(KxDataViewEventDND& event)
		{
			// This handler allows highlighting of possible drop target row
			if (!OnDropItemsPossible(event))
			{
				event.Veto();
			}
		}

	protected:
		virtual KxDataViewCtrl* GetViewCtrl() const = 0;

		virtual wxString GetDragDropFormat() const
		{
			return wxString::Format("KxDataViewModelExDragDropEnabled::!%p", (void*)(typeid(*this).hash_code()));
		}
		virtual bool OnDragItems(KxDataViewEventDND& event) = 0;
		virtual bool OnDropItems(KxDataViewEventDND& event) = 0;
		virtual bool OnDropItemsPossible(KxDataViewEventDND& event)
		{
			return true;
		}

		bool HasDragDropDataObject() const
		{
			return m_DragDropDataObject != nullptr;
		}
		template<class T = DragDropDataT> T* GetDragDropDataObject() const
		{
			static_assert(std::is_base_of_v<KxDataViewModelExDragDropData, T>, "T must be be derived from 'KxDataViewModelExDragDropData'");

			return static_cast<T*>(m_DragDropDataObject);
		}
		void SetDragDropDataObject(KxDataViewModelExDragDropData* pDataObject)
		{
			// We do not own this object
			m_DragDropDataObject = pDataObject;
		}

	public:
		void EnableDragAndDrop(bool enable = true)
		{
			KxDataViewCtrl* pView = GetViewCtrl();

			if (enable)
			{
				pView->EnableDragSource(GetDragDropFormat());
				pView->EnableDropTarget(GetDragDropFormat());

				pView->Bind(KxEVT_DATAVIEW_ITEM_DRAG, &KxDataViewModelExDragDropEnabled::OnDrag, this);
				pView->Bind(KxEVT_DATAVIEW_ITEM_DROP, &KxDataViewModelExDragDropEnabled::OnDrop, this);
				pView->Bind(KxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, &KxDataViewModelExDragDropEnabled::OnDropPossible, this);
			}
			else
			{
				pView->EnableDragSource(wxEmptyString);
				pView->EnableDropTarget(wxEmptyString);

				pView->Unbind(KxEVT_DATAVIEW_ITEM_DRAG, &KxDataViewModelExDragDropEnabled::OnDrag, this);
				pView->Unbind(KxEVT_DATAVIEW_ITEM_DROP, &KxDataViewModelExDragDropEnabled::OnDrop, this);
				pView->Unbind(KxEVT_DATAVIEW_ITEM_DROP_POSSIBLE, &KxDataViewModelExDragDropEnabled::OnDropPossible, this);
			}
		}
};

class KX_API KxDataViewModelExDragDropData: public wxDataObjectSimple
{
	private:
		virtual size_t GetDataSize() const override
		{
			return sizeof(size_t);
		}
		virtual bool GetDataHere(void* buffer) const override
		{
			*((size_t*)buffer) = 0;
			return true;
		}

	public:
		KxDataViewModelExDragDropData()
		{
		}
		KxDataViewModelExDragDropData(const wxString& sDataFormat)
			:wxDataObjectSimple(wxDataFormat(sDataFormat))
		{
		}
};
