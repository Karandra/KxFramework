#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWinUndef.h"

enum KxDriveType
{
	KxDRIVE_UNKNOWN = DRIVE_UNKNOWN,
	KxDRIVE_NO_ROOT_DIR = DRIVE_NO_ROOT_DIR,
	KxDRIVE_REMOVABLE = DRIVE_REMOVABLE,
	KxDRIVE_FIXED = DRIVE_FIXED,
	KxDRIVE_CDROM = DRIVE_CDROM,
	KxDRIVE_RAMDISK = DRIVE_RAMDISK,
};

class KxFile;
class KxDrive
{
	public:
		static const char ms_InvalidDrive = '\255';
		using DrivesArray = std::vector<KxDrive>;
		struct DriveInfo
		{
			bool LongFileNames;
			DWORD FileSystemFlags;
			DWORD SectorsPerCluster;
			DWORD BytesPerSector;
			DWORD NumberOfFreeClusters;
			DWORD TotalNumberOfClusters;
		};

	private:
		static wxString ToDrivePath(char driveLetter)
		{
			return wxString(driveLetter) + ":\\";
		}
		static bool IsExist(char driveLetter);
		static char FromUniChar(const wxUniChar& driveLetter)
		{
			char drive = ms_InvalidDrive;
			if (driveLetter.GetAsChar(&drive))
			{
				return FromChar(drive);
			}
			return ms_InvalidDrive;
		}
		static char FromString(const wxString& driveLetter)
		{
			if (!driveLetter.IsEmpty())
			{
				return FromUniChar(driveLetter[0]);
			}
			return ms_InvalidDrive;
		}
		static char FromChar(char driveLetter)
		{
			if (IsExist(driveLetter))
			{
				return driveLetter;
			}
			return ms_InvalidDrive;
		}

	public:
		static DrivesArray Enumerate();

	private:
		char m_Drive = ms_InvalidDrive;

	public:
		KxDrive(char driveLetter)
		{
			m_Drive = FromChar(driveLetter);
		}
		KxDrive(const wxUniChar& driveLetter)
		{
			m_Drive = FromUniChar(driveLetter);
		}
		KxDrive(const wxString& driveLetter)
		{
			m_Drive = FromString(driveLetter);
		}
		KxDrive(int index)
		{
			m_Drive = FromChar((char)(index + 'A'));
		}
		KxDrive(const KxFile& filePath);

	public:
		bool IsOK() const
		{
			return IsExist(m_Drive);
		}
		
		int ToIndex() const;
		KxDriveType GetType() const;
		wxString GetLabel() const;
		bool SetLabel(const wxString& label = wxEmptyString);
		wxString GetFileSystemName() const;
		uint32_t GetSerialNumber() const;
		wxFileOffset GetTotalSpace() const;
		wxFileOffset GetUsedSpace() const;
		wxFileOffset GetFreeSpace() const;
		DriveInfo GetInfo() const;
		bool Eject();
};
