#pragma once
#include "Common.h"
#include "Event.h"
#include "EvtHandler.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf
{
	class KX_API IWithEvtHandler: public RTTI::Interface<IWithEvtHandler>
	{
		KxDeclareIID(IWithEvtHandler, {0x9672e207, 0xdb80, 0x4cc0, {0x91, 0x31, 0x72, 0x19, 0x26, 0xfa, 0xea, 0x22}});

		public:
			virtual ~IWithEvtHandler() = default;

		public:
			virtual EvtHandler* GetEvtHandler() const = 0;
			virtual void SetEvtHandler(EvtHandler& evtHandler) = 0;
			virtual void SetEvtHandler(std::unique_ptr<EvtHandler> evtHandler) = 0;
	};
}
