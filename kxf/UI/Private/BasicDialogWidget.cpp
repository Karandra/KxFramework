#include "KxfPCH.h"
#include "BasicDialogWidget.h"
#include <wx/dialog.h>

namespace kxf::Private
{
	// BasicWindowWidgetBase
	wxDialog* BasicDialogWidgetBase::GetWxWindow() const noexcept
	{
		return static_cast<wxDialog*>(BasicTLWindowWidgetBase::GetWxWindow());
	}

	// BasicDialogWidgetBase
	bool BasicDialogWidgetBase::IsModal() const
	{
		return GetWxWindow()->IsModal();
	}
	void BasicDialogWidgetBase::EndModal(int retCode)
	{
		GetWxWindow()->EndModal(retCode);
	}
	int BasicDialogWidgetBase::ShowModal()
	{
		return GetWxWindow()->ShowModal();
	}
	int BasicDialogWidgetBase::GetReturnCode() const
	{
		return GetWxWindow()->GetReturnCode();
	}

	int BasicDialogWidgetBase::GetAffirmativeID() const
	{
		return GetWxWindow()->GetAffirmativeId();
	}
	void BasicDialogWidgetBase::SetAffirmativeID(int id)
	{
		GetWxWindow()->SetAffirmativeId(id);
	}

	int BasicDialogWidgetBase::GetEscapeID() const
	{
		return GetWxWindow()->GetEscapeId();
	}
	void BasicDialogWidgetBase::SetEscapeID(int id)
	{
		GetWxWindow()->SetEscapeId(id);
	}
}
