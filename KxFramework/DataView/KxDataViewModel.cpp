#include "KxStdAfx.h"
#include "KxFramework/DataView/KxDataViewModel.h"
#include "KxFramework/DataView/KxDataViewMainWindow.h"
#include "KxFramework/DataView/KxDataViewItem.h"
#include "KxFramework/DataView/KxDataViewColumn.h"

//////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewModel, wxObject);

bool KxDataViewModelGenerciNotifier::ItemsCleared()
{
	return m_MainWindow->ItemsCleared();
}
bool KxDataViewModelGenerciNotifier::ItemAdded(const KxDataViewItem& parent, const KxDataViewItem& item)
{
	return m_MainWindow->ItemAdded(parent, item);
}
bool KxDataViewModelGenerciNotifier::ItemChanged(const KxDataViewItem& item)
{
	return m_MainWindow->ItemChanged(item);
}
bool KxDataViewModelGenerciNotifier::ItemDeleted(const KxDataViewItem& parent, const KxDataViewItem& item)
{
	return m_MainWindow->ItemDeleted(parent, item);
}
bool KxDataViewModelGenerciNotifier::ValueChanged(const KxDataViewItem& item, const KxDataViewColumn* column)
{
	return m_MainWindow->ValueChanged(item, const_cast<KxDataViewColumn*>(column));
}
void KxDataViewModelGenerciNotifier::Resort()
{
	m_MainWindow->Resort();
}

//////////////////////////////////////////////////////////////////////////
void KxDataViewModel::AddNotifier(KxDataViewModelNotifier* notifier)
{
	notifier->SetOwner(this);
	m_Notifiers.emplace_back(notifier);
}
bool KxDataViewModel::RemoveNotifier(KxDataViewModelNotifier* notifier)
{
	auto it = std::find_if(m_Notifiers.begin(), m_Notifiers.end(), [notifier](const auto& value)
	{
		return value.get() == notifier;
	});
	if (it != m_Notifiers.end())
	{
		m_Notifiers.erase(it);
		return true;
	}
	return false;
}

bool KxDataViewModel::ItemsCleared()
{
	bool ret = true;
	for (auto& notifier: m_Notifiers)
	{
		if (!notifier->ItemsCleared())
		{
			ret = false;
		}
	}
	return ret;
}
bool KxDataViewModel::ItemAdded(const KxDataViewItem& parent, const KxDataViewItem& item)
{
	bool ret = true;
	for (auto& notifier: m_Notifiers)
	{
		if (!notifier->ItemAdded(parent, item))
		{
			ret = false;
		}
	}
	return ret;
}
bool KxDataViewModel::ItemChanged(const KxDataViewItem& item)
{
	bool ret = true;
	for (auto& notifier: m_Notifiers)
	{
		if (!notifier->ItemChanged(item))
		{
			ret = false;
		}
	}
	return ret;
}
bool KxDataViewModel::ItemDeleted(const KxDataViewItem& parent, const KxDataViewItem& item)
{
	bool ret = true;
	for (auto& notifier: m_Notifiers)
	{
		if (!notifier->ItemDeleted(parent, item))
		{
			ret = false;
		}
	}
	return ret;
}
bool KxDataViewModel::ValueChanged(const KxDataViewItem& item, const KxDataViewColumn* column)
{
	bool ret = true;
	for (auto& notifier: m_Notifiers)
	{
		if (!notifier->ValueChanged(item, column))
		{
			ret = false;
		}
	}
	return ret;
}

bool KxDataViewModel::Compare(const KxDataViewItem& item1, const KxDataViewItem& item2, const KxDataViewColumn* column) const
{
	wxAny value1;
	wxAny value2;
	GetValue(value1, item1, column);
	GetValue(value2, item2, column);

	if (!column->IsSortedAscending())
	{
		std::swap(value1, value2);
	}

	if (value1.CheckType<wxString>())
	{
		return value1.As<wxString>() < value2.As<wxString>();
	}
	else if (value1.CheckType<wxDateTime>())
	{
		return value1.As<wxDateTime>() < value2.As<wxDateTime>();
	}
	else if (value1.CheckType<int>())
	{
		return value1.As<int>() < value2.As<int>();
	}
	else
	{
		return item1.GetValue() < item2.GetValue();
	}
}

bool KxDataViewModel::BeforeReset()
{
	bool ret = true;
	for (auto& notifier: m_Notifiers)
	{
		if (!notifier->BeforeReset())
		{
			ret = false;
		}
	}
	return ret;
}
bool KxDataViewModel::AfterReset()
{
	bool ret = true;
	for (auto& notifier: m_Notifiers)
	{
		if (!notifier->AfterReset())
		{
			ret = false;
		}
	}
	return ret;
}
void KxDataViewModel::Resort()
{
	for (auto& notifier: m_Notifiers)
	{
		notifier->Resort();
	}
}

bool KxDataViewModel::HasValue(const KxDataViewItem& item, const KxDataViewColumn* column) const
{
	return !IsContainer(item) || HasContainerColumns(item);
}

//////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewListModel, KxDataViewModel);

size_t KxDataViewListModel::GetRow(const KxDataViewItem &item) const
{
	return item.GetValueUInt() - 1;
}
KxDataViewItem KxDataViewListModel::GetItem(size_t row) const
{
	return KxDataViewItem(row + 1);
}

void KxDataViewListModel::GetChildren(const KxDataViewItem& item, KxDataViewItem::Vector& children) const
{
	if (item.IsTreeRootItem())
	{
		size_t count = GetItemCount();
		children.reserve(count);

		for (size_t i = 0; i < count; i++)
		{
			children.push_back(GetItem(i));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
wxIMPLEMENT_ABSTRACT_CLASS(KxDataViewVirtualListModel, KxDataViewListModel);

KxDataViewVirtualListModel::KxDataViewVirtualListModel(size_t initialSize)
	:m_Size(initialSize)
{
}
