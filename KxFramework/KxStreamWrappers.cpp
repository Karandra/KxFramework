/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#include "KxStdAfx.h"
#include "KxFramework/KxStreamWrappers.h"

#if 0
template <class CharType> KxStreamBase::Offset ReadToNullChar()
{
	KxStreamBase::Offset readed = 0;
	bool isReadSuccess = false;
	CharType c = nullptr;

	do
	{
		c = nullptr;
		isReadSuccess = ReadObject(c);
		readed += LastRead();
	}
	while (isReadSuccess && c != nullptr);
	return readed;
}
#endif
