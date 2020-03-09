#pragma once
#include "Common.h"
#include "KxFramework/KxWinUndef.h"

namespace KxFileSystem
{
	struct LegacyDriveInfo
	{
		uint32_t FileSystemFlags = 0;
		uint32_t SectorsPerCluster = 0;
		uint32_t BytesPerSector = 0;
		uint32_t NumberOfFreeClusters = 0;
		uint32_t TotalNumberOfClusters = 0;
		bool LongFileNames = false;
	};
}

namespace KxFileSystem
{
	class KX_API LegacyDrive final
	{
		public:
			static std::vector<LegacyDrive> Enumerate();

			static LegacyDrive FromChar(const wxUniChar& value)
			{
				LegacyDrive drive;
				drive.AssignFromChar(value);
				return drive;
			}
			static LegacyDrive FromChar(const wxString& value)
			{
				LegacyDrive drive;
				if (!value.IsEmpty())
				{
					drive.AssignFromChar(value[0]);
				}
				return drive;
			}
			static LegacyDrive FromIndex(int index)
			{
				LegacyDrive drive;
				drive.AssignFromIndex(index);
				return drive;
			}

		private:
			void AssignFromChar(const wxUniChar& value);
			void AssignFromIndex(int index);

		private:
			char m_Drive = 0;

		public:
			LegacyDrive() = default;
			LegacyDrive(const LegacyDrive&) = default;

		public:
			bool IsValid() const;
			bool DoesExist() const;

			wxString GetPath() const;
			int GetIndex() const;
			char GetChar() const;

			wxString GetLabel() const;
			bool SetLabel(const wxString& label);

			DriveType GetType() const;
			wxString GetFileSystemName() const;
			uint32_t GetSerialNumber() const;

			LegacyDriveInfo GetInfo() const;
			wxFileOffset GetTotalSpace() const;
			wxFileOffset GetUsedSpace() const;
			wxFileOffset GetFreeSpace() const;

			bool EjectMedia();

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
			}
	};
}
