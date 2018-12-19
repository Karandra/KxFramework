#pragma once
#include "Defs.h"

namespace KxUxTheme
{
	enum HandleFlags: uint32_t
	{
		None = 0,
		NonClient = 1 << 0,
		ForceRectSizing = 1 << 1,
	};

	class Handle
	{
		private:
			HTHEME m_Handle = nullptr;

		public:
			Handle(const wxWindow* window, const wchar_t* classes);
			Handle(const wxWindow* window, const wchar_t* classes, uint32_t flags);
			Handle(const wxWindow* window, const wxString& classes)
				:Handle(window, classes.wc_str())
			{
			}
			Handle(const wxWindow* window, const wxString& classes, uint32_t flags)
				:Handle(window, classes.wc_str(), flags)
			{
			}
			~Handle();

		public:
			operator HTHEME() const
			{
				return m_Handle;
			}
			
			bool IsOK() const
			{
				return m_Handle != nullptr;
			}
			operator bool() const
			{
				return IsOK();
			}
			bool operator!() const
			{
				return !IsOK();
			}

		public:
			wxDECLARE_NO_COPY_CLASS(Handle);
	};
}
