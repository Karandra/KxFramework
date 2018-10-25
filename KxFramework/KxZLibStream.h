/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxStreamWrappers.h"
#include <wx/zstream.h>

//////////////////////////////////////////////////////////////////////////
class KxZLib
{
	public:
		static wxString GetLibraryName();
		static wxString GetLibraryVersion();
};

//////////////////////////////////////////////////////////////////////////
class KxZLibInputStream:
	public KxStreamBase,
	public KxInputStreamWrapper<wxZlibInputStream>
{
	public:
		KxZLibInputStream(wxInputStream& stream, int flags = wxZLIB_AUTO)
			:KxInputStreamWrapper(stream, flags)
		{
		}
		KxZLibInputStream(wxInputStream* stream, int flags = wxZLIB_AUTO)
			:KxInputStreamWrapper(stream, flags)
		{
		}

	public:
		virtual bool IsReadable() const override
		{
			return true;
		}
		virtual bool IsWriteable() const override
		{
			return false;
		}
};

//////////////////////////////////////////////////////////////////////////
class KxZLibOutputStream:
	public KxStreamBase,
	public KxOutputStreamWrapper<wxZlibOutputStream>
{
	public:
		KxZLibOutputStream(wxOutputStream& stream, int flags = wxZLIB_AUTO)
			:KxOutputStreamWrapper(stream, flags)
		{
		}
		KxZLibOutputStream(wxOutputStream* stream, int flags = wxZLIB_AUTO)
			:KxOutputStreamWrapper(stream, flags)
		{
		}

	public:
		virtual bool IsReadable() const override
		{
			return false;
		}
		virtual bool IsWriteable() const override
		{
			return true;
		}
};
