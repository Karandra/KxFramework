#include "KxfPCH.h"
#include "RadioCheck.h"
#include "WXUI/RadioCheck.h"

namespace kxf::Widgets
{
	// RadioCheck
	RadioCheck::RadioCheck()
	{
		InitializeWxWidget();
	}
	RadioCheck::~RadioCheck() = default;

	// IWidget
	bool RadioCheck::CreateWidget(std::shared_ptr<IWidget> parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}

	// ICheckWidget
	bool RadioCheck::IsThirdStateSelectable() const
	{
		return false;
	}
	void RadioCheck::SetThirdStateSelectable(bool isSelectable)
	{
	}

	CheckWidgetValue RadioCheck::GetValue() const
	{
		return Get()->GetValue() ? CheckWidgetValue::Checked : CheckWidgetValue::Unchecked;
	}
	void RadioCheck::SetValue(CheckWidgetValue value)
	{
		Get()->SetValue(value == CheckWidgetValue::Checked);
	}
}
