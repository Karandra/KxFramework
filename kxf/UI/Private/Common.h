#pragma once
#include "../Common.h"
class wxObject;
class wxWindow;
class wxEvtHandler;

namespace kxf
{
	class IWidget;
}

namespace kxf::Private
{
	void AssociateWXObject(wxObject& object, IWidget& widget) noexcept;
	void AssociateWXObject(wxWindow& window, IWidget& widget) noexcept;
	void AssociateWXObject(wxEvtHandler& evtHandler, IWidget& widget) noexcept;

	void DissociateWXObject(wxObject& object) noexcept;
	void DissociateWXObject(wxWindow& window) noexcept;
	void DissociateWXObject(wxEvtHandler& evtHandler) noexcept;

	std::shared_ptr<IWidget> FindByWXObject(const wxObject& object) noexcept;
	std::shared_ptr<IWidget> FindByWXObject(const wxWindow& window) noexcept;
	std::shared_ptr<IWidget> FindByWXObject(const wxEvtHandler& evtHandler) noexcept;
}
