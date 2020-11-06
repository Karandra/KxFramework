#include "stdafx.h"
#include "Font.h"
#include <wx/fontutil.h>

namespace
{
	class wxFontRefDataHack final: public wxObjectRefData
	{
		public:
			bool m_sizeUsingPixels = false;
			HFONT m_hFont = nullptr;
			wxNativeFontInfo m_nativeFontInfo;
	};
}

namespace kxf
{
	// IGDIObject
	void* Font::GetHandle() const
	{
		return m_Font.GetHFONT();
	}
	void* Font::DetachHandle()
	{
		if (wxFontRefDataHack* refData = static_cast<wxFontRefDataHack*>(m_Font.GetRefData()))
		{
			HFONT handle = nullptr;
			std::swap(refData->m_hFont, handle);
			return static_cast<void*>(handle);
		}
	}
	void Font::AttachHandle(void* handle)
	{
		m_Font = wxFont();

		if (handle)
		{
			LOGFONTW fontInfo = {};
			if (::GetObjectW(handle, sizeof(fontInfo), &fontInfo) != 0)
			{
				if (wxFontRefDataHack* refData = static_cast<wxFontRefDataHack*>(m_Font.GetRefData()))
				{
					refData->m_hFont = static_cast<HFONT>(handle);
					refData->m_nativeFontInfo.lf = fontInfo;
					refData->m_sizeUsingPixels = true;
					return;
				}
			}
			::DeleteObject(handle);
		}
	}

	// Font
}
