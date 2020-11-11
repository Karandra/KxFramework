#include "stdafx.h"
#include "GDICanvas.h"
#include <wx/msw/dc.h>

namespace kxf
{
	// IGDIObject
	void* GDICanvas::GetHandle() const
	{
		return m_DC ? m_DC->GetHDC() : nullptr;
	}
	void* GDICanvas::DetachHandle()
	{
		if (m_DC)
		{
			if (auto impl = dynamic_cast<wxMSWDCImpl*>(m_DC->GetImpl()))
			{
				void* handle = impl->GetHDC();
				impl->SetHDC(nullptr, false);

				return handle;
			}
		}
		return nullptr;
	}
	void GDICanvas::AttachHandle(void* handle)
	{
		if (m_DC)
		{
			if (auto impl = dynamic_cast<wxMSWDCImpl*>(m_DC->GetImpl()))
			{
				impl->SetHDC(static_cast<HDC>(handle), true);
				return;
			}
		}

		// Delete the handle if we can't attach it
		::DeleteDC(static_cast<HDC>(handle));
	}
}
