#include "KxfPCH.h"
#include "LegacyVolume.h"
#include "FSPath.h"
#include "kxf/General/Enumerator.h"
#include "kxf/Utility/Common.h"

namespace
{
	constexpr char g_InvalidDrive = std::numeric_limits<unsigned char>::max();
	constexpr size_t g_FirstLegacyVolume = 'A';
	constexpr size_t g_LastLegacyVolume = 'Z';
	constexpr size_t g_LegacyVolumesCount = g_LastLegacyVolume - g_FirstLegacyVolume + 1;

	constexpr bool IsDriveLetterValid(char c)
	{
		return c >= g_FirstLegacyVolume && c <= g_LastLegacyVolume;
	}
}

namespace kxf
{
	Enumerator<LegacyVolume> LegacyVolume::EnumVolumes()
	{
		return [driveMask = ::GetLogicalDrives()](IEnumerator& en) mutable -> std::optional<LegacyVolume>
		{
			const size_t index = en.GetCurrentStep();
			if (index < g_LegacyVolumesCount)
			{
				if (driveMask & 1 << index)
				{
					return FromIndex(index);
				}
				en.SkipCurrent();
			}
			return {};
		};
	}

	void LegacyVolume::AssignFromChar(const UniChar& value)
	{
		m_Drive = g_InvalidDrive;
		if (auto c = value.ToASCII())
		{
			c = std::toupper(*c);
			if (IsDriveLetterValid(*c))
			{
				m_Drive = *c;
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
	String LegacyVolume::DoGetPath() const
	{
		if (IsValid())
		{
			XChar disk[] = kxS("\0:\\");
			disk[0] = UniChar(m_Drive).GetAs<XChar>();
			return disk;
		}
		return {};
	}

	bool LegacyVolume::IsValid() const
	{
		return IsDriveLetterValid(m_Drive);
	}
	bool LegacyVolume::DoesExist() const
	{
		if (IsValid())
		{
			String path = DoGetPath();
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
	UniChar LegacyVolume::GetChar() const
	{
		if (IsValid())
		{
			return m_Drive;
		}
		return g_InvalidDrive;
	}
}
