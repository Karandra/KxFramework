#include "KxStdAfx.h"
#include "KxFramework/KxBitmapComboBox.h"
#include "KxFramework/KxIncludeWindows.h"

wxIMPLEMENT_DYNAMIC_CLASS(KxBitmapComboBox, wxBitmapComboBox)

bool KxBitmapComboBox::Create(wxWindow* parent,
							  wxWindowID id,
							  const wxString& value,
							  const wxPoint& pos,
							  const wxSize& size,
							  long 	style,
							  const wxValidator& validator
)
{
	if (wxBitmapComboBox::Create(parent, id, value, pos, size, 0, nullptr, style, validator))
	{
		EnableSystemTheme();
		SetDoubleBuffered(true);

		return true;
	}
	return false;
}

void KxBitmapComboBox::SetVisibleItemsCount(size_t count)
{
	::SendMessageW(GetHandle(), CB_SETMINVISIBLE, static_cast<WPARAM>(count), 0);
}
int KxBitmapComboBox::InsertItem(const wxString& s, size_t i, int imageID)
{
	int index = -1;
	if (HasImageList() && imageID != NO_IMAGE)
	{
		index = wxBitmapComboBox::Insert(s, GetImageList()->GetBitmap(imageID), i);
	}
	else
	{
		index = wxBitmapComboBox::Insert(s, wxNullBitmap, i);
	}

	if (index != -1)
	{
		m_ImageIDs[index] = imageID;
	}
	return index;
}
int KxBitmapComboBox::AddItem(const wxString& s, int imageID)
{
	return InsertItem(s, GetCount(), imageID);
}
void KxBitmapComboBox::RemoveItem(size_t i)
{
	wxBitmapComboBox::Delete(i);
	m_ImageIDs.erase(i);
}
void KxBitmapComboBox::Clear()
{
	wxBitmapComboBox::Clear();
	m_ImageIDs.clear();
}

int KxBitmapComboBox::GetItemImage(size_t i) const
{
	if (HasImageList() && m_ImageIDs.count(i))
	{
		return m_ImageIDs.at(i);
	}
	return NO_IMAGE;
}
void KxBitmapComboBox::SetItemImage(size_t i, int imageID)
{
	if (HasImageList())
	{
		if (imageID != NO_IMAGE)
		{
			m_ImageIDs[i] = imageID;
			wxBitmapComboBox::SetItemBitmap(i, GetImageList()->GetBitmap(imageID));
		}
		else
		{
			m_ImageIDs.erase(i);
			wxBitmapComboBox::SetItemBitmap(i, wxNullBitmap);
		}
	}
}
