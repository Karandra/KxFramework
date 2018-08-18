#include "KxStdAfx.h"
#include "KxFramework/KxDrive.h"
#include "KxFramework/KxFile.h"

bool KxDrive::IsExist(char driveLetter)
{
	wxString drivePath = ToDrivePath(driveLetter);
	return driveLetter != ms_InvalidDrive && ::GetDriveTypeW(drivePath) != DRIVE_NO_ROOT_DIR;
}

KxDrive::DrivesArray KxDrive::Enumerate()
{
	DrivesArray list;
	DWORD length = ::GetLogicalDriveStringsW(0, NULL);
	if (length != 0)
	{
		LPWSTR string = new WCHAR[length];
		length = ::GetLogicalDriveStringsW(length, string);

		for (size_t i = 0; i <= length; i = i+sizeof(WCHAR)+2)
		{
			list.push_back(KxDrive((int)i));
			#if 0
			wxString drive(string+i);
			if (!drive.IsEmpty())
			{
				drive.Truncate(2);
				list.push_back(KxDrive(drive));
			}
			#endif
		}
		delete[] string;
	}
	return list;
}

KxDrive::KxDrive(const KxFile& filePath)
{
	FromString(filePath.GetDrive());
}

int KxDrive::ToIndex() const
{
	return m_Drive - 'A';
}
KxDriveType KxDrive::GetType() const
{
	wxString drivePath = ToDrivePath(m_Drive);
	return static_cast<KxDriveType>(::GetDriveTypeW(drivePath));
}
wxString KxDrive::GetLabel() const
{
	wxString drivePath = ToDrivePath(m_Drive);
	wxString value;
	const int maxLength = MAX_PATH + 1;
	::GetVolumeInformationW(drivePath, wxStringBuffer(value, maxLength), maxLength, NULL, NULL, NULL, NULL, 0);
	return value;
}
bool KxDrive::SetLabel(const wxString& label)
{
	wxString drivePath = ToDrivePath(m_Drive);
	return ::SetVolumeLabelW(drivePath, label.IsEmpty() ? NULL : label.wc_str());
}
wxString KxDrive::GetFileSystemName() const
{
	wxString drivePath = ToDrivePath(m_Drive);
	wxString value;
	const int maxLength = MAX_PATH + 1;
	::GetVolumeInformationW(drivePath, NULL, 0, NULL, NULL, NULL, wxStringBuffer(value, maxLength), maxLength);
	return value;
}
uint32_t KxDrive::GetSerialNumber() const
{
	wxString drivePath = ToDrivePath(m_Drive);
	DWORD value = 0;
	::GetVolumeInformationW(drivePath, NULL, 0, &value, NULL, NULL, NULL, 0);
	return value;
}
wxFileOffset KxDrive::GetTotalSpace() const
{
	wxString drivePath = ToDrivePath(m_Drive);
	ULARGE_INTEGER value;
	::GetDiskFreeSpaceExW(drivePath, NULL, &value, NULL);
	return value.QuadPart;
}
wxFileOffset KxDrive::GetUsedSpace() const
{
	wxString drivePath = ToDrivePath(m_Drive);

	ULARGE_INTEGER total;
	ULARGE_INTEGER free;
	::GetDiskFreeSpaceExW(drivePath, NULL, &total, NULL);
	::GetDiskFreeSpaceExW(drivePath, NULL, NULL, &free);
	return total.QuadPart - free.QuadPart;
}
wxFileOffset KxDrive::GetFreeSpace() const
{
	wxString drivePath = ToDrivePath(m_Drive);
	ULARGE_INTEGER value;
	::GetDiskFreeSpaceExW(drivePath, NULL, NULL, &value);
	return value.QuadPart;
}
KxDrive::DriveInfo KxDrive::GetInfo() const
{
	wxString drivePath = ToDrivePath(m_Drive);

	DriveInfo info;
	DWORD nMaximumComponentLength = 0;
	::GetVolumeInformationW(drivePath, NULL, 0, NULL, &nMaximumComponentLength, &info.FileSystemFlags, NULL, 0);
	info.LongFileNames = nMaximumComponentLength == 255;
	::GetDiskFreeSpaceW(drivePath, &info.SectorsPerCluster, &info.BytesPerSector, &info.NumberOfFreeClusters, &info.TotalNumberOfClusters);

	return info;
}
bool KxDrive::Eject()
{
	bool ret = false;
	wxString drivePath = "\\\\.\\" + ToDrivePath(m_Drive);
	HANDLE drive = ::CreateFileW(drivePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0);
	if (drive != INVALID_HANDLE_VALUE)
	{
		DWORD bytes = 0;
		::DeviceIoControl(drive, FSCTL_LOCK_VOLUME, 0, 0, 0, 0, &bytes, 0);
		::DeviceIoControl(drive, FSCTL_DISMOUNT_VOLUME, 0, 0, 0, 0, &bytes, 0);
		ret = ::DeviceIoControl(drive, IOCTL_STORAGE_EJECT_MEDIA, 0, 0, 0, 0, &bytes, 0);
		::CloseHandle(drive);
	}
	return ret;
}
