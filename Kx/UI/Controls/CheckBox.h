#pragma once
#include "Kx/UI/Common.h"
#include <wx/checkbox.h>

namespace KxFramework::UI
{
	enum class CheckBoxStyle
	{
		None = 0,

		TwoState = wxCHK_2STATE,
		TriState = wxCHK_3STATE,
		AllowSelectThirdState = wxCHK_ALLOW_3RD_STATE_FOR_USER,

		AlignLeft = wxALIGN_LEFT,
		AlignRight = wxALIGN_RIGHT,
		AlignCenterHorizontal = wxALIGN_CENTRE_HORIZONTAL
	};
}
namespace KxFramework
{
	Kx_DeclareFlagSet(UI::CheckBoxStyle);
}

namespace KxFramework::UI
{
	class KX_API CheckBox: public wxCheckBox
	{
		public:
			static constexpr FlagSet<CheckBoxStyle> DefaultStyle = CheckBoxStyle::TwoState;

		public:
			CheckBox() = default;
			CheckBox(wxWindow* parent,
					 wxWindowID id,
					 const String& label,
					 FlagSet<CheckBoxStyle> style = DefaultStyle
			)
			{
				Create(parent, id, label, style);
			}
			bool Create(wxWindow* parent,
						wxWindowID id,
						const String& label,
						FlagSet<CheckBoxStyle> style = DefaultStyle
			);

		public:
			bool GetValue() const override
			{
				if (Is3State())
				{
					return wxCheckBox::Get3StateValue() == wxCHK_CHECKED;
				}
				return wxCheckBox::GetValue();
			}
			void SetValue(bool value) override
			{
				wxCheckBox::SetValue(value);
			}

			wxCheckBoxState GetCheckState() const
			{
				if (Is3State())
				{
					return wxCheckBox::Get3StateValue();
				}
				return wxCheckBox::GetValue() ? wxCHK_CHECKED : wxCHK_UNCHECKED;
			}
			void SetCheckState(wxCheckBoxState state)
			{
				wxCheckBox::Set3StateValue(state);
			}

			void Set3StateValue(wxCheckBoxState) = delete;
			wxCheckBoxState Get3StateValue() = delete;

		public:
			wxDECLARE_DYNAMIC_CLASS(CheckBox);
	};
}
