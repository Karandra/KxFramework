#pragma once
#include "Common.h"
#include "FSPath.h"
#include "Kx/General/UUID.h"
#include "Kx/General/BinarySize.h"
#include "KxFramework/KxWinUndef.h"

namespace KxFramework
{
	class LegacyVolume;
}

namespace KxFramework
{
	class KX_API StorageVolume final
	{
		public:
			struct SpaceLayoutInfo final
			{
				BinarySize DataPerSector;
				uint32_t SectorsPerCluster = 0;
				uint32_t NumberOfFreeClusters = 0;
				uint32_t TotalNumberOfClusters = 0;
			};

		public:
			static size_t Enumerate(std::function<bool(StorageVolume)> func);
			static bool RemoveMountPoint(const FSPath& path);
			static bool RemoveMountPoint(const LegacyVolume& volume);

		private:
			wxChar m_Path[64] = {};

		private:
			void AssignPath(const wxChar* path, size_t length = wxString::npos)
			{
				if (length == wxString::npos)
				{
					length = std::char_traits<wxChar>::length(path);
				}
				std::char_traits<wxChar>::copy(m_Path, path, std::min(length, std::size(m_Path)));
			}
			void AssignPath(const wxString& path)
			{
				AssignPath(path.wc_str(), path.length());
			}

		public:
			StorageVolume() = default;
			StorageVolume(StorageVolume&&) = default;
			StorageVolume(const StorageVolume&) = default;
			StorageVolume(const UUID& id);
			StorageVolume(const FSPath& path);
			StorageVolume(const LegacyVolume& legacyVolume);

		public:
			bool IsValid() const;
			bool DoesExist() const;

			UUID GetUniqueID() const;
			FSPath GetPath() const;

			wxString GetLabel() const;
			bool SetLabel(const wxString& label);

			DriveType GetType() const;
			wxString GetFileSystem() const;
			FileSystemFeature GetFileSystemFeatures() const;
			uint32_t GetSerialNumber() const;

			std::optional<SpaceLayoutInfo> GetSpaceLayoutInfo() const;
			BinarySize GetTotalSpace() const;
			BinarySize GetUsedSpace() const;
			BinarySize GetFreeSpace() const;

			size_t EnumMountPoints(std::function<bool(FSPath)> func) const;
			bool SetMountPoint(const FSPath& path);
			bool SetMountPoint(const LegacyVolume& volume);

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
			
			StorageVolume& operator=(StorageVolume&&) = default;
			StorageVolume& operator=(const StorageVolume&) = default;
	};
}
