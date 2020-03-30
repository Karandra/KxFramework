#pragma once
#include "Common.h"
#include "FSPath.h"
#include "KxFramework/KxWinUndef.h"

namespace KxFramework
{
	struct LegacyDriveInfo
	{
		uint32_t SectorsPerCluster = 0;
		uint32_t BytesPerSector = 0;
		uint32_t NumberOfFreeClusters = 0;
		uint32_t TotalNumberOfClusters = 0;
		FileSystemFeature FileSystemFeatures = FileSystemFeature::None;
		bool LongFileNames = false;
	};
}

namespace KxFramework
{
	class KX_API LegacyVolume final
	{
		public:
			static size_t Enumerate(std::function<bool(LegacyVolume)> func);
			static std::vector<LegacyVolume> Enumerate();

			static LegacyVolume FromChar(const wxUniChar& value)
			{
				LegacyVolume drive;
				drive.AssignFromChar(value);
				return drive;
			}
			static LegacyVolume FromChar(const wxString& value)
			{
				LegacyVolume drive;
				if (!value.IsEmpty())
				{
					drive.AssignFromChar(value[0]);
				}
				return drive;
			}
			static LegacyVolume FromIndex(int index)
			{
				LegacyVolume drive;
				drive.AssignFromIndex(index);
				return drive;
			}

		private:
			void AssignFromChar(const wxUniChar& value);
			void AssignFromIndex(int index);

			wxString DoGetPath() const;

		private:
			char m_Drive = 0;

		public:
			LegacyVolume() = default;
			LegacyVolume(const LegacyVolume&) = default;

		public:
			bool IsValid() const;
			bool DoesExist() const;

			FSPath GetPath() const;
			int GetIndex() const;
			char GetChar() const;

			wxString GetLabel() const;
			bool SetLabel(const wxString& label);

			DriveType GetType() const;
			wxString GetFileSystemName() const;
			FileSystemFeature GetFileSystemFeatures() const;
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
