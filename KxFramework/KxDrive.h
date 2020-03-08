#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWinUndef.h"

enum class KxDriveType
{
	Unknown = DRIVE_UNKNOWN,
	NoRootDirectory = DRIVE_NO_ROOT_DIR,
	Removable = DRIVE_REMOVABLE,
	Fixed = DRIVE_FIXED,
	CDROM = DRIVE_CDROM,
	RAMDisk = DRIVE_RAMDISK,
};

class KX_API KxDrive final
{
	public:
		using Vector = std::vector<KxDrive>;
		struct DriveInfo
		{
			uint32_t FileSystemFlags = 0;
			uint32_t SectorsPerCluster = 0;
			uint32_t BytesPerSector = 0;
			uint32_t NumberOfFreeClusters = 0;
			uint32_t TotalNumberOfClusters = 0;
			bool LongFileNames = false;
		};

	public:
		static Vector Enumerate();

		static KxDrive FromChar(const wxUniChar& value)
		{
			KxDrive drive;
			drive.AssignFromChar(value);
			return drive;
		}
		static KxDrive FromChar(const wxString& value)
		{
			KxDrive drive;
			if (!value.IsEmpty())
			{
				drive.AssignFromChar(value[0]);
			}
			return drive;
		}
		static KxDrive FromIndex(int index)
		{
			KxDrive drive;
			drive.AssignFromIndex(index);
			return drive;
		}

	private:
		void AssignFromChar(const wxUniChar& value);
		void AssignFromIndex(int index);

	private:
		char m_Drive = 0;

	public:
		KxDrive() = default;
		KxDrive(const KxDrive&) = default;

	public:
		bool IsValid() const;
		bool DoesExist() const;

		wxString GetPath() const;
		int GetIndex() const;
		char GetChar() const;

		wxString GetLabel() const;
		bool SetLabel(const wxString& label);

		KxDriveType GetType() const;
		wxString GetFileSystemName() const;
		uint32_t GetSerialNumber() const;

		DriveInfo GetInfo() const;
		wxFileOffset GetTotalSpace() const;
		wxFileOffset GetUsedSpace() const;
		wxFileOffset GetFreeSpace() const;

		bool EjectMedia();
};
