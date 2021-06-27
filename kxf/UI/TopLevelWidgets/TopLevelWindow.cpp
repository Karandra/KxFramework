#include "KxfPCH.h"
#include "TopLevelWindow.h"
#include "WXUI/Frame.h"

namespace kxf::Widgets
{
	TopLevelWindow::TopLevelWindow()
	{
		InitializeWithWindow();
	}
	TopLevelWindow::~TopLevelWindow() = default;

	bool TopLevelWindow::CreateWidget(IWidget* parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}
}
