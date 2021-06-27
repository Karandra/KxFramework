#include "KxfPCH.h"
#include "Dialog.h"
#include "WXUI/Dialog.h"

namespace kxf::Widgets
{
	Dialog::Dialog()
	{
		InitializeWithWindow();
	}
	Dialog::~Dialog() = default;

	bool Dialog::CreateWidget(IWidget* parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}
}
