#pragma once
#include "Kx/UI/Common.h"
#include "Kx/UI/WindowRefreshScheduler.h"
#include <wx/systhemectrl.h>
#include <wx/combobox.h>

namespace KxFramework::UI
{
	enum class ComboBoxStyle
	{
		None = 0,

		Simple = wxCB_SIMPLE,
		Dropdown = wxCB_DROPDOWN,
		ReadOnly = wxCB_READONLY,
		ProcessEnter = wxTE_PROCESS_ENTER,
		SortAlphabetical = wxCB_SORT,
	};
}
namespace KxFramework::EnumClass
{
	Kx_EnumClass_AllowEverything(UI::ComboBoxStyle);
}

namespace KxFramework::UI
{
	class KX_API ComboBox: public WindowRefreshScheduler<wxSystemThemedControl<wxComboBox>>
	{
		public:
			static constexpr ComboBoxStyle DefaultStyle = ComboBoxStyle::ProcessEnter|ComboBoxStyle::Dropdown|ComboBoxStyle::ReadOnly;

		public:
			ComboBox() = default;
			ComboBox(wxWindow* parent,
					 wxWindowID id,
					 const String& value = {},
					 ComboBoxStyle style = DefaultStyle,
					 const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, style, validator);
			}
			ComboBox(wxWindow* parent,
					 wxWindowID id,
					 const String& value,
					 const wxPoint& pos,
					 const wxSize& size,
					 ComboBoxStyle style = DefaultStyle,
					 const wxValidator& validator = wxDefaultValidator
			)
			{
				Create(parent, id, value, pos, size, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& value = {},
						ComboBoxStyle style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			)
			{
				return Create(parent, id, value, wxDefaultPosition, wxDefaultSize, style, validator);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& value,
						const wxPoint& pos,
						const wxSize& size,
						ComboBoxStyle style = DefaultStyle,
						const wxValidator& validator = wxDefaultValidator
			);

		public:
			int InsertItem(const String& label, size_t index)
			{
				return Insert(label, index);
			}
			int AddItem(const String& label)
			{
				return Insert(label, GetCount());
			}
			void RemoveItem(size_t index)
			{
				Delete(index);
			}

			bool SetVisibleItemsCount(size_t count);

			String GetItemLabel(size_t index) const
			{
				return GetString(index);
			}
			void SetItemLabel(size_t index, const String& label)
			{
				SetString(index, label);
			}
			
			wxClientData* GetItemData(size_t index) const
			{
				return GetClientObject(index);
			}
			void SetItemData(size_t index, wxClientData* data)
			{
				SetClientObject(index, data);
			}

		public:
			wxDECLARE_DYNAMIC_CLASS(ComboBox);
	};
}
