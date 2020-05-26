#pragma once
#include "Common.h"
#include "FSPath.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/General/BinarySize.h"

namespace kxf
{
	class LegacyVolume;
}

namespace kxf
{
	class KX_API StorageVolume final
	{
		public:
			struct SpaceLayoutInfo final
			{
				BinarySize DataPerSector;
				uint32_t SectorsPerCluster = 0;
				uint32_t FreeClusters = 0;
				uint32_t TotalClusters = 0;
			};

		public:
			static size_t EnumVolumes(std::function<bool(StorageVolume)> func);
			static size_t EnumLegacyVolumes(std::function<bool(StorageVolume, LegacyVolume)> func);

			static bool RemoveMountPoint(const FSPath& path);
			static bool RemoveMountPoint(const LegacyVolume& volume);

		private:
			XChar m_Path[64 - sizeof(size_t)] = {};
			size_t m_Length = 0;

		private:
			void AssignPath(const XChar* path, size_t length = String::npos)
			{
				if (length == String::npos)
				{
					length = std::char_traits<XChar>::length(path);
				}

				m_Length = std::min(length, std::size(m_Path) - 1);
				std::char_traits<XChar>::copy(m_Path, path, m_Length);
			}
			void AssignPath(const String& path)
			{
				AssignPath(path.wc_str(), path.length());
			}

		public:
			StorageVolume() = default;
			StorageVolume(StorageVolume&&) = default;
			StorageVolume(const StorageVolume&) = default;
			StorageVolume(const UniversallyUniqueID& id);
			StorageVolume(const LegacyVolume& legacyVolume);
			StorageVolume(const FSPath& path);

		public:
			bool IsValid() const;
			bool DoesExist() const;

			UniversallyUniqueID GetUniqueID() const;
			FSPath GetPath() const;

			String GetLabel() const;
			bool SetLabel(const String& label);

			DriveType GetType() const;
			uint32_t GetSerialNumber() const;
			String GetFileSystem() const;
			FlagSet<FileSystemFeature> GetFileSystemFeatures() const;

			std::optional<SpaceLayoutInfo> GetSpaceLayoutInfo() const;
			BinarySize GetTotalSpace() const;
			BinarySize GetUsedSpace() const;
			BinarySize GetFreeSpace() const;

			LegacyVolume GetLegacyVolume() const;
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
			
			bool operator==(const StorageVolume& other) const
			{
				if (this != &other)
				{
					return m_Length == other.m_Length && std::char_traits<wxChar>::compare(m_Path, other.m_Path, m_Length) == 0;
				}
				return true;
			}
			bool operator!=(const StorageVolume& other) const
			{
				return !(*this == other);
			}

			StorageVolume& operator=(StorageVolume&&) = default;
			StorageVolume& operator=(const StorageVolume&) = default;
	};
}
