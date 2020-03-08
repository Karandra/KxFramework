#include "KxStdAfx.h"
#include "KxFramework/KxDrive.h"
#include "KxFramework/KxFileStream.h"
#include <Kx/FileSystem/FSPath.h>

namespace
{
	constexpr char g_InvalidDrive = '\255';
}

void KxDrive::AssignFromChar(const wxUniChar& value)
{
	if (!value.GetAsChar(&m_Drive))
	{
		m_Drive = g_InvalidDrive;
	}
}
void KxDrive::AssignFromIndex(int index)
{
	m_Drive = g_InvalidDrive;
	if (index >= 0 && index <= 26)
	{
		AssignFromChar(index + 'A');
	}
}

KxDrive::Vector KxDrive::Enumerate()
{
	Vector drives;
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

bool KxDrive::IsValid() const
{
	return m_Drive >= 'A' && m_Drive <= 'Z';
}
bool KxDrive::DoesExist() const
{
	if (IsValid())
	{
		wxString path = GetPath();
		return ::GetDriveTypeW(path.wc_str()) != DRIVE_NO_ROOT_DIR;
	}
	return false;
}

wxString KxDrive::GetPath() const
{
	if (IsValid())
	{
		return wxString(m_Drive) + wxS(":\\");
	}
	return wxEmptyString;
}
int KxDrive::GetIndex() const
{
	if (IsValid())
	{
		return m_Drive - 'A';
	}
	return -1;
}
char KxDrive::GetChar() const
{
	if (IsValid())
	{
		return m_Drive;
	}
	return g_InvalidDrive;
}

wxString KxDrive::GetLabel() const
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
bool KxDrive::SetLabel(const wxString& label)
{
	if (IsValid())
	{
		wxString path = GetPath();
		return ::SetVolumeLabelW(path.wc_str(), label.IsEmpty() ? nullptr : label.wc_str());
	}
	return false;
}

KxDriveType KxDrive::GetType() const
{
	if (IsValid())
	{
		wxString path = GetPath();
		return static_cast<KxDriveType>(::GetDriveTypeW(path.wc_str()));
	}
	return KxDriveType::Unknown;
}
wxString KxDrive::GetFileSystemName() const
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
uint32_t KxDrive::GetSerialNumber() const
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

KxDrive::DriveInfo KxDrive::GetInfo() const
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

		DriveInfo driveInfo = {};
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
wxFileOffset KxDrive::GetTotalSpace() const
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
wxFileOffset KxDrive::GetUsedSpace() const
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
wxFileOffset KxDrive::GetFreeSpace() const
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

bool KxDrive::EjectMedia()
{
	if (IsValid())
	{
		KxFileStream stream(KxFSPath(GetPath()).GetFullPath(KxFSPathNamespace::Win32Device), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything);
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
