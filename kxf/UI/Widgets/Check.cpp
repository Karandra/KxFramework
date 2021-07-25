#include "KxfPCH.h"
#include "Check.h"
#include "WXUI/Check.h"

namespace kxf::Widgets
{
	// Check
	Check::Check()
	{
		InitializeWxWidget();
	}
	Check::~Check() = default;

	// IWidget
	bool Check::CreateWidget(std::shared_ptr<IWidget> parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}

	// ICheckWidget
	bool Check::IsThirdStateSelectable() const
	{
		return Get()->Is3rdStateAllowedForUser();
	}
	void Check::SetThirdStateSelectable(bool isSelectable)
	{
		FlagSet<int> style = Get()->GetWindowStyle();
		style.Mod(wxCHK_ALLOW_3RD_STATE_FOR_USER, isSelectable);

		Get()->SetWindowStyle(*style);
	}

	CheckWidgetValue Check::GetValue() const
	{
		switch (Get()->GetCheckState())
		{
			case wxCheckBoxState::wxCHK_CHECKED:
			{
				return CheckWidgetValue::Checked;
			}
			case wxCheckBoxState::wxCHK_UNCHECKED:
			{
				return CheckWidgetValue::Unchecked;
			}
			case wxCheckBoxState::wxCHK_UNDETERMINED:
			{
				return CheckWidgetValue::Indeterminate;
			}
		};
		return CheckWidgetValue::Unchecked;
	}
	void Check::SetValue(CheckWidgetValue value)
	{
		switch (value)
		{
			case CheckWidgetValue::Checked:
			{
				Get()->SetCheckState(wxCheckBoxState::wxCHK_CHECKED);
				break;
			}
			case CheckWidgetValue::Unchecked:
			{
				Get()->SetCheckState(wxCheckBoxState::wxCHK_UNCHECKED);
				break;
			}
			case CheckWidgetValue::Indeterminate:
			{
				Get()->SetCheckState(wxCheckBoxState::wxCHK_UNDETERMINED);
				break;
			}
		};
	}
}
