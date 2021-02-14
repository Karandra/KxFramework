#pragma once
#include "Common.h"
#include <wx/window.h>

namespace kxf::UI::Private
{
	class KX_API HiDPIEnabledWindowBase
	{
		protected:
			bool DoEnableNonClientDPIScaling(wxWindow& window, bool enable);

		public:
			virtual ~HiDPIEnabledWindowBase() = default;

		public:
			virtual bool EnableNonClientDPIScaling(bool enable = true) = 0;
	};
}

namespace kxf::UI
{
	template<class TWindow>
	class HiDPIEnabledWindow: public TWindow, public Private::HiDPIEnabledWindowBase
	{
		public:
			bool EnableNonClientDPIScaling(bool enable = true) override
			{
				static_assert(std::is_base_of_v<wxWindow, TWindow>, "wxWindow descendant required");

				return DoEnableNonClientDPIScaling(static_cast<wxWindow&>(*this), enable);
			}
	};
}
