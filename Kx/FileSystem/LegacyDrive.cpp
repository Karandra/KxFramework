#include "KxStdAfx.h"
#include "LegacyDrive.h"
#include "FSPath.h"
#include "KxFramework/KxFileStream.h"

namespace KxFileSystem
{
	constexpr char g_InvalidDrive = '\255';

	DriveType MapDriveType(UINT type)
	{
		switch (type)
		{
			case DRIVE_UNKNOWN:
			{
				return DriveType::Unknown;
			}
			case DRIVE_NO_ROOT_DIR:
			{
				return DriveType::NotMounted;
			}
			case DRIVE_REMOVABLE:
			{
				return DriveType::Removable;
			}
			case DRIVE_REMOTE:
			{
				return DriveType::Remote;
			}
			case DRIVE_FIXED:
			{
				return DriveType::Fixed;
			}
			case DRIVE_CDROM:
			{
				return DriveType::Optical;
			}
			case DRIVE_RAMDISK:
			{
				return DriveType::Memory;
			}
		};
		return DriveType::Unknown;
	}
}

namespace KxFileSystem
{
	void LegacyDrive::AssignFromChar(const wxUniChar& value)
	{
		if (!value.GetAsChar(&m_Drive))
		{
			m_Drive = g_InvalidDrive;
		}
	}
	void LegacyDrive::AssignFromIndex(int index)
	{
		m_Drive = g_InvalidDrive;
		if (index >= 0 && index <= 26)
		{
			AssignFromChar(index + 'A');
		}
	}

	std::vector<LegacyDrive> LegacyDrive::Enumerate()
	{
		std::vector<LegacyDrive> drives;
		DWORD length = ::GetLogicalDriveStringsW(0, nullptr);
		if (length != 0)
		{
			wxString string;
			length = ::GetLogicalDriveStringsW(length, wxStringBuffer(string, length));

			constexpr int itemsPerArray = 4;
			drives.reserve(length / itemsPerArray);
			for (size_t i = 0; i <= length; i += itemsPerArray)
			{
				drives.emplace_back(FromIndex(i));
			}
		}
		return drives;
	}

	bool LegacyDrive::IsValid() const
	{
		return m_Drive >= 'A' && m_Drive <= 'Z';
	}
	bool LegacyDrive::DoesExist() const
	{
		if (IsValid())
		{
			wxString path = GetPath();
			return ::GetDriveTypeW(path.wc_str()) != DRIVE_NO_ROOT_DIR;
		}
		return false;
	}

	wxString LegacyDrive::GetPath() const
	{
		if (IsValid())
		{
			return wxString(m_Drive) + wxS(":\\");
		}
		return wxEmptyString;
	}
	int LegacyDrive::GetIndex() const
	{
		if (IsValid())
		{
			return m_Drive - 'A';
		}
		return -1;
	}
	char LegacyDrive::GetChar() const
	{
		if (IsValid())
		{
			return m_Drive;
		}
		return g_InvalidDrive;
	}

	wxString LegacyDrive::GetLabel() const
	{
		if (IsValid())
		{
			wxString path = GetPath();
			wxString label;
			const int maxLength = MAX_PATH + 1;
			::GetVolumeInformationW(path.wc_str(), wxStringBuffer(label, maxLength), maxLength, nullptr, nullptr, nullptr, nullptr, 0);
			return label;
		}
		return wxEmptyString;
	}
	bool LegacyDrive::SetLabel(const wxString& label)
	{
		if (IsValid())
		{
			wxString path = GetPath();
			return ::SetVolumeLabelW(path.wc_str(), label.IsEmpty() ? nullptr : label.wc_str());
		}
		return false;
	}

	DriveType LegacyDrive::GetType() const
	{
		if (IsValid())
		{
			wxString path = GetPath();
			return MapDriveType(::GetDriveTypeW(path.wc_str()));
		}
		return DriveType::Unknown;
	}
	wxString LegacyDrive::GetFileSystemName() const
	{
		if (IsValid())
		{
			wxString path = GetPath();
			wxString name;
			const int maxLength = MAX_PATH + 1;
			::GetVolumeInformationW(path.wc_str(), nullptr, 0, nullptr, nullptr, nullptr, wxStringBuffer(name, maxLength), maxLength);
			return name;
		}
		return wxEmptyString;
	}
	uint32_t LegacyDrive::GetSerialNumber() const
	{
		if (IsValid())
		{
			wxString path = GetPath();
			DWORD serialNumber = 0;
			::GetVolumeInformationW(path.wc_str(), nullptr, 0, &serialNumber, nullptr, nullptr, nullptr, 0);
			return serialNumber;
		}
		return std::numeric_limits<uint32_t>::max();
	}

	LegacyDriveInfo LegacyDrive::GetInfo() const
	{
		if (IsValid())
		{
			wxString path = GetPath();

			DWORD fileSystemFlags = 0;
			DWORD maximumComponentLength = 0;
			::GetVolumeInformationW(path.wc_str(), nullptr, 0, nullptr, &maximumComponentLength, &fileSystemFlags, nullptr, 0);

			DWORD bytesPerSector = 0;
			DWORD sectorsPerCluster = 0;
			DWORD numberOfFreeClusters = 0;
			DWORD totalNumberOfClusters = 0;
			::GetDiskFreeSpaceW(path.wc_str(), &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters);

			LegacyDriveInfo driveInfo = {};
			driveInfo.FileSystemFlags = fileSystemFlags;
			driveInfo.SectorsPerCluster = sectorsPerCluster;
			driveInfo.BytesPerSector = bytesPerSector;
			driveInfo.NumberOfFreeClusters = numberOfFreeClusters;
			driveInfo.TotalNumberOfClusters = totalNumberOfClusters;
			driveInfo.LongFileNames = maximumComponentLength == 255;

			return driveInfo;
		}
		return {};
	}
	wxFileOffset LegacyDrive::GetTotalSpace() const
	{
		if (IsValid())
		{
			wxString path = GetPath();
			ULARGE_INTEGER value = {};
			::GetDiskFreeSpaceExW(path.wc_str(), nullptr, &value, nullptr);
			return value.QuadPart;
		}
		return wxInvalidOffset;
	}
	wxFileOffset LegacyDrive::GetUsedSpace() const
	{
		if (IsValid())
		{
			wxString path = GetPath();

			ULARGE_INTEGER total = {};
			ULARGE_INTEGER free = {};
			::GetDiskFreeSpaceExW(path.wc_str(), nullptr, &total, nullptr);
			::GetDiskFreeSpaceExW(path.wc_str(), nullptr, nullptr, &free);
			return total.QuadPart - free.QuadPart;
		}
		return wxInvalidOffset;
	}
	wxFileOffset LegacyDrive::GetFreeSpace() const
	{
		if (IsValid())
		{
			wxString path = GetPath();
			ULARGE_INTEGER value = {};
			::GetDiskFreeSpaceExW(path.wc_str(), nullptr, nullptr, &value);
			return value.QuadPart;
		}
		return wxInvalidOffset;
	}

	bool LegacyDrive::EjectMedia()
	{
		if (IsValid())
		{
			KxFileStream stream(KxFSPath(GetPath()).GetFullPath(PathNamespace::Win32Device), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything);
			if (stream)
			{
				DWORD bytes = 0;
				::DeviceIoControl(stream.GetHandle(), FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &bytes, nullptr);
				::DeviceIoControl(stream.GetHandle(), FSCTL_DISMOUNT_VOLUME, nullptr, 0, nullptr, 0, &bytes, nullptr);
				return ::DeviceIoControl(stream.GetHandle(), IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0, &bytes, nullptr);
			}
		}
		return false;
	}
}
