#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDataViewModelExBase.h"

template<class BaseT>
class KxDataViewListModelExBase: public KxDataViewModelExBase<BaseT>
{
	public:
		KxDataViewListModelExBase()
		{
		}
		virtual ~KxDataViewListModelExBase()
		{
		}

	public:
		bool IsItemValid(const KxDataViewItem& item) const
		{
			return item.IsOK() && GetRow(item) < GetItemCount();
		}
		bool IsMoveValid(const KxDataViewItem& item, wxDirection value) const
		{
			if (IsItemValid(item))
			{
				size_t index = GetRow(item);
				if (value == wxUP)
				{
					return index > 0;
				}
				else
				{
					return index + 1 < GetItemCount();
				}
			}
			return false;
		}
		KxDataViewItem GetNextItem(const KxDataViewItem& item) const
		{
			return GetItem(GetRow(item) + 1);
		}
		KxDataViewItem GetPrevItem(const KxDataViewItem& item) const
		{
			return GetItem(GetRow(item) - 1);
		}
		
		void SelectItem(const KxDataViewItem& item = KxDataViewItem(), bool bUnselectAll = false)
		{
			KxDataViewModelExBase::SelectItem(item, bUnselectAll);
		}
		void SelectItem(size_t index, bool bUnselectAll = false)
		{
			KxDataViewModelExBase::SelectItem(GetItem(index), bUnselectAll);
		}
};
using KxDataViewListModelEx = KxDataViewListModelExBase<KxDataViewListModel>;
using KxDataViewVListModelEx = KxDataViewListModelExBase<KxDataViewVirtualListModel>;

//////////////////////////////////////////////////////////////////////////
template<class T, class BaseT> class KxDataViewVectorListModelEx: public BaseT
{
	public:
		using VectorType = T;
		using ValueType = typename VectorType::value_type;

	private:
		VectorType* m_Data = NULL;

	protected:
		virtual void OnSetDataVectorInternal()
		{
			if (GetView())
			{
				GetView()->Enable(HasDataVector());
			}
			OnSetDataVector();
		}

	protected:
		virtual void OnSwapItems(KxDataViewItem& tItem1, KxDataViewItem& tItem2)
		{
			if (IsItemValid(tItem1) && IsItemValid(tItem2))
			{
				std::iter_swap(m_Data->begin() + GetRow(tItem1), m_Data->begin() + GetRow(tItem2));
			}
		}
		virtual void OnRemoveItem(const KxDataViewItem& item)
		{
			if (IsItemValid(item))
			{
				m_Data->erase(m_Data->begin() + GetRow(item));
			}
		}
		virtual void OnClearItems()
		{
			m_Data->clear();
		}
		virtual void OnSetDataVector()
		{
		}

	public:
		virtual ~KxDataViewVectorListModelEx()
		{
			m_Data = NULL;
		}

	public:
		virtual size_t GetItemCount() const override
		{
			return m_Data ? m_Data->size() : 0;
		}
		KxDataViewItem GetItemByEntry(const ValueType& v) const
		{
			auto it = std::find(m_Data->begin(), m_Data->end(), v);
			if (it != m_Data->end())
			{
				return GetItem(std::distance(m_Data->begin(), it));
			}
			return KxDataViewItem();
		}

		bool HasDataVector() const
		{
			return m_Data != NULL;
		}
		VectorType* GetDataVector()
		{
			return m_Data;
		}
		const VectorType* GetDataVector() const
		{
			return m_Data;
		}
		void SetDataVector(VectorType* data = NULL)
		{
			// Clear control
			m_Data = NULL;
			OnSetDataVectorInternal();

			// Assign new and load data if needed
			if (data)
			{
				m_Data = data;
				OnSetDataVectorInternal();
			}

			// Refresh
			BaseT::RefreshItems();
		}
};
