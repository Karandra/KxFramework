#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxDataViewModelExBase.h"

template<class BaseT>
class KxDataViewTreeModelEx: public KxDataViewModelExBase<BaseT>
{
	private:
		virtual bool IsListModel() const override
		{
			return false;
		}
		virtual bool HasContainerColumns(const KxDataViewItem& item) const override
		{
			return true;
		}

	public:
		KxDataViewTreeModelEx()
		{
		}
		virtual ~KxDataViewTreeModelEx()
		{
		}
};

//////////////////////////////////////////////////////////////////////////
template<class T, class BaseT> class KxDataViewVectorTreeModelEx: public BaseT
{
	public:
		using DataType = T;

	private:
		DataType* m_Data = NULL;

	protected:
		virtual void OnSetDataVectorInternal() override
		{
			if (this->GetView())
			{
				this->GetView()->Enable(HasDataVector());
			}
			this->OnSetDataVector();
		}

	public:
		virtual ~KxDataViewVectorTreeModelEx()
		{
			m_Data = NULL;
		}

	public:
		bool HasDataVector() const
		{
			return m_Data != NULL;
		}
		DataType* GetDataVector()
		{
			return m_Data;
		}
		const DataType* GetDataVector() const
		{
			return m_Data;
		}
		void SetDataVector(DataType* data = NULL)
		{
			// Clear control
			m_Data = NULL;
			OnSetDataVectorInternal();
			BaseT::RefreshItems();

			// Assign new and load data if needed
			if (data)
			{
				m_Data = data;
				OnSetDataVectorInternal();
				BaseT::RefreshItems();
			}
		}
};
