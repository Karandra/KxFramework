#include "KxfPCH.h"
#include "IWidget.h"

namespace kxf
{
	std::shared_ptr<IWidget> IWidget::FindFocus() noexcept
	{
		if (auto window = wxWindow::FindFocus())
		{
			return FindByWXObject(*window);
		}
		return nullptr;
	}
}
