#pragma once
#include "kxf/UI/Windows/Frame.h"

namespace kxf
{
	class DefaultRPCExchanger;
}

namespace kxf
{
	class DefaultRPCExchangerWindow final: public UI::Frame
	{
		private:
			DefaultRPCExchanger& m_Exchanger;

		protected:
			bool MSWHandleMessage(WXLRESULT* result, WXUINT msg, WXWPARAM wParam, WXLPARAM lParam) override;

		public:
			DefaultRPCExchangerWindow(DefaultRPCExchanger& exchanger, const UniversallyUniqueID& sessionID);
	};
}
