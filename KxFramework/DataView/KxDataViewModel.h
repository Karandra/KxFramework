#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/DataView/KxDataViewConstants.h"
#include "KxFramework/DataView/KxDataViewItem.h"
class KxDataViewCtrl;
class KxDataViewItemAttributes;
class KxDataViewMainWindow;
class KxDataViewColumn;

class KxDataViewModel;
class KxDataViewModelNotifier
{
	public:
		using Vector = std::vector<std::unique_ptr<KxDataViewModelNotifier>>;

	private:
		KxDataViewModel* m_Model = NULL;

	public:
		KxDataViewModelNotifier(KxDataViewModel* model = NULL)
			:m_Model(model)
		{
		}
		virtual ~KxDataViewModelNotifier()
		{
		}

	public:
		KxDataViewModel* GetOwner() const
		{
			return m_Model;
		}
		void SetOwner(KxDataViewModel* model)
		{
			m_Model = model;
		}

		virtual bool ItemsCleared() = 0;
		virtual bool ItemAdded(const KxDataViewItem& parent, const KxDataViewItem& item) = 0;
		virtual bool ItemChanged(const KxDataViewItem& item) = 0;
		virtual bool ItemDeleted(const KxDataViewItem& parent, const KxDataViewItem& item) = 0;
		virtual bool ValueChanged(const KxDataViewItem& item, const KxDataViewColumn* column) = 0;
		
		virtual bool BeforeReset()
		{
			return true;
		}
		virtual bool AfterReset()
		{
			return ItemsCleared();
		}
		virtual void Resort() = 0;
};

//////////////////////////////////////////////////////////////////////////
class KxDataViewModelGenerciNotifier: public KxDataViewModelNotifier
{
	private:
		KxDataViewMainWindow* m_MainWindow = NULL;

	public:
		KxDataViewModelGenerciNotifier(KxDataViewMainWindow* mainWindow, KxDataViewModel* model = NULL)
			:m_MainWindow(mainWindow), KxDataViewModelNotifier(model)
		{
		}

	public:
		virtual bool ItemsCleared() override;
		virtual bool ItemAdded(const KxDataViewItem& parent, const KxDataViewItem& item) override;
		virtual bool ItemChanged(const KxDataViewItem& item) override;
		virtual bool ItemDeleted(const KxDataViewItem& parent, const KxDataViewItem& item) override;
		virtual bool ValueChanged(const KxDataViewItem& item, const KxDataViewColumn* column) override;
		virtual void Resort() override;
};

//////////////////////////////////////////////////////////////////////////
class KxDataViewModel: public wxRefCounter, private KxDataViewModelNotifier
{
	private:
		KxDataViewModelNotifier::Vector m_Notifiers;

	public:
		KxDataViewModel()
			:KxDataViewModelNotifier(NULL)
		{
		}

	protected:
		// The user should not delete this class directly: he should use DecRef() instead!
		virtual ~KxDataViewModel()
		{
		}

	public:
		virtual bool IsListModel() const
		{
			return false;
		}
		virtual bool IsVirtualListModel() const
		{
			return false;
		}

		// Notifiers list
		void AddNotifier(KxDataViewModelNotifier* notifier);
		bool RemoveNotifier(KxDataViewModelNotifier* notifier);

		// Delegated notifiers
		virtual bool ItemsCleared() override;
		virtual bool ItemAdded(const KxDataViewItem& parent, const KxDataViewItem& item) override;
		virtual bool ItemChanged(const KxDataViewItem& item) override;
		virtual bool ItemDeleted(const KxDataViewItem& parent, const KxDataViewItem& item) override;
		virtual bool ValueChanged(const KxDataViewItem& item, const KxDataViewColumn* column) override;

		bool ItemAdded(const KxDataViewItem& item)
		{
			return ItemAdded(KxDataViewItem(), item);
		}
		bool ItemDeleted(const KxDataViewItem& item)
		{
			return ItemDeleted(KxDataViewItem(), item);
		}

		bool ItemsAdded(const KxDataViewItem& parent, const KxDataViewItem::Vector& items)
		{
			bool ret = false;
			for (const KxDataViewItem& item: items)
			{
				ret = ItemAdded(parent, item);
				if (!ret)
				{
					return false;
				}
			}
			return ret;
		}
		bool ItemsAdded(const KxDataViewItem::Vector& items)
		{
			return ItemsAdded(KxDataViewItem(), items);
		}
		bool ItemsDeleted(const KxDataViewItem& parent, const KxDataViewItem::Vector& items)
		{
			bool ret = false;
			for (const KxDataViewItem& item: items)
			{
				ret = ItemDeleted(parent, item);
				if (!ret)
				{
					return false;
				}
			}
			return ret;
		}
		bool ItemsDeleted(const KxDataViewItem::Vector& items)
		{
			return ItemsDeleted(KxDataViewItem(), items);
		}

		// Default compare function
		virtual bool HasDefaultCompare() const
		{
			return false;
		}
		virtual bool Compare(const KxDataViewItem& item1, const KxDataViewItem& item2, const KxDataViewColumn* column) const;
		
		// Delegated action
		virtual bool BeforeReset() override;
		virtual bool AfterReset() override;
		virtual void Resort() override;

		/* Items */
		// Is the container just a header or an item with all columns
		virtual bool IsContainer(const KxDataViewItem& item) const = 0;
		virtual bool HasContainerColumns(const KxDataViewItem& item) const
		{
			return false;
		}
		
		// Return true if the given item has a value to display in the given
		// column: this is always true except for container items which by default
		// only show their label in the first column (but see HasContainerColumns())
		// Can be overridden directly if necessary.
		virtual bool HasValue(const KxDataViewItem& item, const KxDataViewColumn* column) const;
		
		// Override this if you want to disable specific items
		virtual bool IsEnabled(const KxDataViewItem& item, const KxDataViewColumn* column) const
		{
			return true;
		}
		virtual bool IsEditorEnabled(const KxDataViewItem& item, const KxDataViewColumn* column) const
		{
			return IsEnabled(item, column);
		}
		
		// Get text attribute, return false of default attributes should be used
		virtual bool GetItemAttributes(const KxDataViewItem& item, const KxDataViewColumn* column, KxDataViewItemAttributes& attributes, KxDataViewCellState cellState) const
		{
			return false;
		}
		
		// Get value into a 'wxAny'
		virtual void GetValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const = 0;
		
		// Get tooltip value into a 'wxAny'
		virtual void GetToolTip(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const
		{
		}

		// You can feed editor with different value if needed
		virtual void GetEditorValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const
		{
			return GetValue(value, item, column);
		}
		
		// Usually ValueChanged() should be called after changing the value in the model to update the control,
		// ChangeValue() does it on its own while SetValue() does not - so while you will override SetValue(),
		// you should be usually calling ChangeValue()
		virtual bool SetValue(const wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) = 0;

		bool ChangeValue(const wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column)
		{
			return SetValue(value, item, column) && ValueChanged(item, column);
		}

		// Define hierarchy
		virtual KxDataViewItem GetParent(const KxDataViewItem& item) const = 0;
		virtual void GetChildren(const KxDataViewItem& item, KxDataViewItem::Vector& children) const = 0;

	public:
		wxDECLARE_ABSTRACT_CLASS(KxDataViewModel);
};

//////////////////////////////////////////////////////////////////////////
class KxDataViewListModel: public KxDataViewModel
{
	public:
		// Derived classes should override these methods instead of {Get,Set}Value() and GetItemAttributes() inherited from the base class.
		virtual void GetValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const = 0;
		virtual void GetEditorValueByRow(wxAny& value, size_t row, const KxDataViewColumn* column) const
		{
			return GetValueByRow(value, row, column);
		}
		virtual bool SetValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column) = 0;
		virtual bool GetItemAttributesByRow(size_t row, const KxDataViewColumn* column, KxDataViewItemAttributes& attribute, KxDataViewCellState cellState) const
		{
			return false;
		}
		virtual bool IsEnabledByRow(size_t row, const KxDataViewColumn* column) const
		{
			return true;
		}
		virtual bool IsEditorEnabledByRow(size_t row, const KxDataViewColumn* column) const
		{
			return IsEnabledByRow(row, column);
		}
		virtual bool CompareByRow(size_t row1, size_t row2, const KxDataViewColumn* column) const
		{
			return row1 < row2;
		}

		bool ChangeValueByRow(const wxAny& value, size_t row, const KxDataViewColumn* column)
		{
			return SetValueByRow(value, row, column) && RowValueChanged(row, column);
		}

		virtual bool RowPrepended()
		{
			return ItemAdded(GetItem(0));
		}
		virtual bool RowInserted(size_t position)
		{
			return ItemAdded(GetItem(position));
		}
		virtual bool RowAppended()
		{
			return ItemAdded(GetItem(GetItemCount()));
		}
		virtual bool RowDeleted(size_t row)
		{
			return ItemDeleted(GetItem(row));
		}
		virtual bool RowChanged(size_t row)
		{
			return ItemChanged(GetItem(row));
		}
		virtual bool RowValueChanged(size_t row, const KxDataViewColumn* column)
		{
			return ValueChanged(GetItem(row), column);
		}

		// Helper methods provided by list models only
		virtual size_t GetRow(const KxDataViewItem &item) const;
		virtual KxDataViewItem GetItem(size_t row) const;

		// Returns the number of rows
		virtual bool IsListModel() const override
		{
			return true;
		}
		virtual size_t GetItemCount() const = 0;
		bool IsEmpty() const
		{
			return GetItemCount() == 0;
		}

		// Implement some base class pure virtual directly
		virtual void GetChildren(const KxDataViewItem& item, KxDataViewItem::Vector& children) const override;
		virtual KxDataViewItem GetParent(const KxDataViewItem& item) const override
		{
			// Items never have valid parent in this model
			return KxDataViewItem();
		}
		virtual bool IsContainer(const KxDataViewItem& item) const override
		{
			// Only the invisible (and invalid) root item has children
			return item.IsTreeRootItem();
		}

		// And implement some others by forwarding them to our own ones
		virtual void GetValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const override
		{
			GetValueByRow(value, GetRow(item), column);
		}
		virtual void GetEditorValue(wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) const override
		{
			GetEditorValueByRow(value, GetRow(item), column);
		}
		virtual bool SetValue(const wxAny& value, const KxDataViewItem& item, const KxDataViewColumn* column) override
		{
			return SetValueByRow(value, GetRow(item), column);
		}
		virtual bool GetItemAttributes(const KxDataViewItem& item, const KxDataViewColumn* column, KxDataViewItemAttributes& attributes, KxDataViewCellState cellState) const override
		{
			return GetItemAttributesByRow(GetRow(item), column, attributes, cellState);
		}
		virtual bool IsEnabled(const KxDataViewItem& item, const KxDataViewColumn* column) const
		{
			return IsEnabledByRow(GetRow(item), column);
		}
		virtual bool IsEditorEnabled(const KxDataViewItem& item, const KxDataViewColumn* column) const
		{
			return IsEditorEnabledByRow(GetRow(item), column);
		}
		virtual bool Compare(const KxDataViewItem& item1, const KxDataViewItem& item2, const KxDataViewColumn* column) const override
		{
			return CompareByRow(GetRow(item1), GetRow(item2), column);
		}

	public:
		wxDECLARE_ABSTRACT_CLASS(KxDataViewListModel);
};

//////////////////////////////////////////////////////////////////////////
class KxDataViewVirtualListModel: public KxDataViewListModel
{
	public:
		enum: size_t
		{
			INVALID_ROW = (size_t)-1,
		};

	private:
		size_t m_Size = 0;

	public:
		KxDataViewVirtualListModel(size_t initialSize = 0);

	public:
		virtual bool IsVirtualListModel() const override
		{
			return true;
		}
		virtual size_t GetItemCount() const override
		{
			return m_Size;
		}

		virtual bool RowPrepended() override
		{
			m_Size++;
			return KxDataViewListModel::RowPrepended();
		}
		virtual bool RowInserted(size_t position) override
		{
			m_Size++;
			return KxDataViewListModel::RowInserted(position);
		}
		virtual bool RowAppended() override
		{
			m_Size++;
			return KxDataViewListModel::RowAppended();
		}
		virtual bool RowDeleted(size_t row) override
		{
			m_Size--;
			return KxDataViewListModel::RowDeleted(row);
		}
		bool Reset(size_t newSize)
		{
			bool before = BeforeReset();
			m_Size = newSize;
			bool after = AfterReset();

			return before && after;
		}

		// Compare
		virtual bool HasDefaultCompare() const
		{
			return true;
		}

	public:
		wxDECLARE_ABSTRACT_CLASS(KxDataViewVirtualListModel);
};
