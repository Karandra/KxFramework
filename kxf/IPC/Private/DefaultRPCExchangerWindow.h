#pragma once
#include <wx/dialog.h>

namespace kxf
{
	class UniversallyUniqueID;
	class DefaultRPCExchanger;
}

namespace kxf
{
	class DefaultRPCExchangerWindow final
	{
		private:
			DefaultRPCExchanger& m_Exchanger;
			uint32_t m_WindowClass = 0;
			void* m_Handle = nullptr;

		private:
			bool MSWHandleMessage(intptr_t& result, uint32_t msg, intptr_t wParam, intptr_t lParam);

		public:
			DefaultRPCExchangerWindow(DefaultRPCExchanger& exchanger) noexcept
				:m_Exchanger(exchanger)
			{
			}
			~DefaultRPCExchangerWindow() noexcept
			{
				Destroy();
			}

		public:
			void* GetHandle() const noexcept
			{
				return m_Handle;
			}

			bool Create(const UniversallyUniqueID& sessionID);
			bool Destroy() noexcept;
	};
}
