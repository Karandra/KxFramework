#pragma once
#include "KxFramework/KxFramework.h"

class KxHiDPIEnabledWindowBase
{
	protected:
		bool DoEnableNonClientDPIScaling(wxWindow& window, bool enable);

	public:
		virtual ~KxHiDPIEnabledWindowBase() = default;

	public:
		virtual bool EnableNonClientDPIScaling(bool enable = true) = 0;
};

template<class TWindow>
class KxHiDPIEnabledWindow: public TWindow, public KxHiDPIEnabledWindowBase
{
	public:
		bool EnableNonClientDPIScaling(bool enable = true) override
		{
			return DoEnableNonClientDPIScaling(static_cast<wxWindow&>(*this), enable);
		}
};
