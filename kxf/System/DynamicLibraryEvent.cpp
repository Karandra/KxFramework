#include "stdafx.h"
#include "DynamicLibraryEvent.h"
#include "DynamicLibrary.h"

namespace kxf
{
	wxIMPLEMENT_DYNAMIC_CLASS(DynamicLibraryEvent, wxNotifyEvent);

	DynamicLibrary DynamicLibraryEvent::GetLibrary() const
	{
		DynamicLibrary library;
		library.AttachHandle(m_Handle);
		return library;
	}
}
