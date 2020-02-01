#include "KxStdAfx.h"
#include "Common.h"

namespace KxSciter
{
	wxStringView SizeUnitToString(SizeUnit unit)
	{
		#define Map(unit)	\
			case SizeUnit::##unit:	\
			{	\
				return wxS(#unit);	\
			}	\

		switch (unit)
		{
			// Absolute
			Map(cm);
			Map(mm);
			Map(in);
			Map(px);
			Map(pt);
			Map(pc);

			// Relative
			Map(em);
			Map(ex);
			Map(ch);
			Map(rem);
			Map(vw);
			Map(vh);
			Map(vmin);
			Map(vmax);

			case SizeUnit::Percent:
			{
				return wxS("%");
			}
		};
		return {};

		#undef Map
	}
}
