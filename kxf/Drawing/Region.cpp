#include "stdafx.h"
#include "Region.h"
#include "Private/GDI.h"

namespace
{
	class wxRegionRefDataHack: public wxGDIRefData
	{
		public:
			HRGN m_region = nullptr;

		public:
			void Clear()
			{
				m_region = nullptr;
			}
	};
}

namespace kxf
{
	// IGDIObject
	void* Region::GetHandle() const
	{
		return m_Region.GetHRGN();
	}
	void* Region::DetachHandle()
	{
		if (wxRegionRefDataHack* refData = static_cast<wxRegionRefDataHack*>(m_Region.GetRefData()))
		{
			// 'GetResourceHandle' creates the actual pen object if it doesn't already exist
			void* handle = m_Region.GetHRGN();

			// Clear the internal structures
			refData->Clear();

			return handle;
		}
	}
	void Region::AttachHandle(void* handle)
	{
		m_Region = wxRegion(reinterpret_cast<WXHRGN>(handle));
	}
}
