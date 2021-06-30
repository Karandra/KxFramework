#include "KxfPCH.h"
#include "IWidget.h"
#include "Private/Common.h"

namespace kxf
{
	std::shared_ptr<IWidget> IWidget::FindFocus() noexcept
	{
		if (auto window = wxWindow::FindFocus())
		{
			return Private::FindByWXObject(*window);
		}
		return nullptr;
	}
}
