/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"

struct IShellLinkW;
class KX_API KxShellLink
{
	public:
		enum ShowCommand
		{
			KxSHL_SHOW_INVALID_COMMAND = -1,

			KxSHL_SHOW_NORMAL = SW_SHOWNORMAL,
			KxSHL_SHOW_MAXIMIZED = SW_SHOWMAXIMIZED,
			KxSHL_SHOW_MINIMIZED = SW_SHOWMINNOACTIVE,
		};

	private:
		IShellLinkW* m_Instance = NULL;

	public:
		KxShellLink(const wxString& filePath = wxEmptyString);
		virtual ~KxShellLink();

	public:
		bool IsOK() const;
		bool Save(const wxString& path) const;

		wxString GetTarget() const;
		void SetTarget(const wxString& value);

		wxString GetArguments() const;
		void SetArguments(const wxString& value);

		wxString GetWorkingFolder() const;
		void SetWorkingFolder(const wxString& value);

		wxString GetDescription() const;
		void SetDescription(const wxString& value);

		wxString GetIconLocation(int* indexOut = NULL) const;
		void SetIconLocation(const wxString& value, int index = 0);
		int GetIconIndex() const;

		ShowCommand GetShowCommand() const;
		void SetShowCommand(ShowCommand command);

		wxKeyEvent GetHotKey() const;
		void SetHotKey(const wxKeyEvent& keyState);
};
