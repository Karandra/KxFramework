#include "KxfPCH.h"
#include "Window.h"
#include "WXUI/Frame.h"

namespace kxf::Widgets
{
	Window::Window()
	{
		InitializeWithWindow();
	}
	Window::~Window() = default;

	bool Window::CreateWidget(IWidget* parent, const String& text, Point pos, Size size)
	{
		return Get()->Create(parent ? parent->GetWxWindow() : nullptr, text, pos, size);
	}
}
