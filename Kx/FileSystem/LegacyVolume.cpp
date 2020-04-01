#include "KxStdAfx.h"
#include "LegacyVolume.h"
#include "FSPath.h"
#include "Kx/Utility/Common.h"
#include "KxFramework/KxFileStream.h"

namespace
{
	constexpr char g_InvalidDrive = '\255';
	constexpr size_t g_FirstLegacyVolume = 'A';
	constexpr size_t g_LastLegacyVolume = 'Z';
	constexpr size_t g_LegacyVolumesCount = g_LastLegacyVolume - g_FirstLegacyVolume + 1;

	constexpr bool IsDriveLetterValid(char c)
	{
		return c >= g_FirstLegacyVolume && c <= g_LastLegacyVolume;
	}
}

namespace KxFramework
{
	size_t LegacyVolume::EnumVolumes(std::function<bool(LegacyVolume)> func)
	{
		size_t count = 0;

		const DWORD driveMask = ::GetLogicalDrives();
		for (size_t i = 0; i < g_LegacyVolumesCount; i++)
		{
			if (driveMask & 1 << i)
			{
				count++;
				if (!std::invoke(func, FromIndex(i)))
				{
					break;
				}
			}
		}
		return count;
	}

	void LegacyVolume::AssignFromChar(const wxUniChar& value)
	{
		char c = g_InvalidDrive;
		if (value.GetAsChar(&c))
		{
			c = std::toupper(c);
			if (IsDriveLetterValid(c))
			{
				m_Drive = c;
			}
		}
	}
	void LegacyVolume::AssignFromIndex(int index)
	{
		m_Drive = g_InvalidDrive;
		if (index >= 0 && index <= g_LegacyVolumesCount)
		{
			AssignFromChar(index + g_FirstLegacyVolume);
		}
	}
	wxString LegacyVolume::DoGetPath() const
	{
		if (IsValid())
		{
			wxChar disk[] = wxS("\0:\\");
			disk[0] = wxUniChar(m_Drive);
			return disk;
		}
		return wxEmptyString;
	}

	bool LegacyVolume::IsValid() const
	{
		return IsDriveLetterValid(m_Drive);
	}
	bool LegacyVolume::DoesExist() const
	{
		if (IsValid())
		{
			wxString path = DoGetPath();
			return ::GetDriveTypeW(path.wc_str()) != DRIVE_NO_ROOT_DIR;
		}
		return false;
	}

	FSPath LegacyVolume::GetPath() const
	{
		return DoGetPath();
	}
	int LegacyVolume::GetIndex() const
	{
		if (IsValid())
		{
			return m_Drive - g_FirstLegacyVolume;
		}
		return -1;
	}
	wxUniChar LegacyVolume::GetChar() const
	{
		if (IsValid())
		{
			return m_Drive;
		}
		return g_InvalidDrive;
	}
}
