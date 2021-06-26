#include "KxfPCH.h"
#include "IWidget.h"

namespace kxf
{
	void IWidget::AssociateWithWindow(wxWindow& window, IWidget& widget) noexcept
	{
		window.SetClientData(static_cast<IWidget*>(&widget));
	}
	void IWidget::DissociateWithWindow(wxWindow& window) noexcept
	{
		window.SetClientData(nullptr);
	}
	std::shared_ptr<IWidget> IWidget::FindByWindow(const wxWindow& window) noexcept
	{
		return reinterpret_cast<IWidget*>(window.GetClientData())->LockReference();
	}
}
