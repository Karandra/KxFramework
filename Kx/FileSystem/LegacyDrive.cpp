#include "KxStdAfx.h"
#include "LegacyDrive.h"
#include "FSPath.h"
#include "Kx/Utility/Common.h"
#include "KxFramework/KxFileStream.h"

namespace KxFramework
{
	constexpr char g_InvalidDrive = '\255';
	constexpr size_t g_MaxLegacyDrives = 'Z' - 'A' + 1;

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
	FileSystemFeature MapDiskFeatures(DWORD nativeFeatures)
	{
		FileSystemFeature features = FileSystemFeature::None;
		Utility::ModFlagRef(features, FileSystemFeature::CasePreservedNames, nativeFeatures & FILE_CASE_PRESERVED_NAMES);
		Utility::ModFlagRef(features, FileSystemFeature::CaseSensitiveSearch, nativeFeatures & FILE_CASE_SENSITIVE_SEARCH);
		Utility::ModFlagRef(features, FileSystemFeature::FileCompression, nativeFeatures & FILE_FILE_COMPRESSION);
		Utility::ModFlagRef(features, FileSystemFeature::NamedStreams, nativeFeatures & FILE_NAMED_STREAMS);
		Utility::ModFlagRef(features, FileSystemFeature::PersistentACLS, nativeFeatures & FILE_PERSISTENT_ACLS);
		Utility::ModFlagRef(features, FileSystemFeature::ReadOnlyVolume, nativeFeatures & FILE_READ_ONLY_VOLUME);
		Utility::ModFlagRef(features, FileSystemFeature::SequentialWrite, nativeFeatures & FILE_SEQUENTIAL_WRITE_ONCE);
		Utility::ModFlagRef(features, FileSystemFeature::Encryption, nativeFeatures & FILE_SUPPORTS_ENCRYPTION);
		Utility::ModFlagRef(features, FileSystemFeature::ExtendedAttributes, nativeFeatures & FILE_SUPPORTS_EXTENDED_ATTRIBUTES);
		Utility::ModFlagRef(features, FileSystemFeature::HardLinks, nativeFeatures & FILE_SUPPORTS_HARD_LINKS);
		Utility::ModFlagRef(features, FileSystemFeature::ObjectIDs, nativeFeatures & FILE_SUPPORTS_OBJECT_IDS);
		Utility::ModFlagRef(features, FileSystemFeature::OpenFileByID, nativeFeatures & FILE_SUPPORTS_OPEN_BY_FILE_ID);
		Utility::ModFlagRef(features, FileSystemFeature::ReparsePoints, nativeFeatures & FILE_SUPPORTS_REPARSE_POINTS);
		Utility::ModFlagRef(features, FileSystemFeature::SparseFiles, nativeFeatures & FILE_SUPPORTS_SPARSE_FILES);
		Utility::ModFlagRef(features, FileSystemFeature::Transactions, nativeFeatures & FILE_SUPPORTS_TRANSACTIONS);
		Utility::ModFlagRef(features, FileSystemFeature::USNJournal, nativeFeatures & FILE_SUPPORTS_USN_JOURNAL);
		Utility::ModFlagRef(features, FileSystemFeature::Unicode, nativeFeatures & FILE_UNICODE_ON_DISK);
		Utility::ModFlagRef(features, FileSystemFeature::CompressedVolume, nativeFeatures & FILE_VOLUME_IS_COMPRESSED);
		Utility::ModFlagRef(features, FileSystemFeature::VolumeQuotas, nativeFeatures & FILE_VOLUME_QUOTAS);

		return features;
	}
}

namespace KxFramework
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
	
	wxString LegacyDrive::DoGetPath() const
	{
		if (IsValid())
		{
			return wxString(m_Drive) + wxS(":\\");
		}
		return wxEmptyString;
	}

	size_t LegacyDrive::Enumerate(std::function<bool(LegacyDrive)> func)
	{
		DWORD length = ::GetLogicalDriveStringsW(0, nullptr);
		if (length != 0)
		{
			wxString string;
			length = ::GetLogicalDriveStringsW(length, wxStringBuffer(string, length));

			constexpr int itemsPerArray = 4;
			size_t count = 0;
			for (count = 0; count <= length; count += itemsPerArray)
			{
				if (!func(FromIndex(count)))
				{
					break;
				}
			}
			return count;
		}
		return 0;
	}
	std::vector<LegacyDrive> LegacyDrive::Enumerate()
	{
		std::vector<LegacyDrive> drives;
		drives.reserve(g_MaxLegacyDrives);
		Enumerate([&](LegacyDrive drive)
		{
			drives.emplace_back(std::move(drive));
			return true;
		});
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
			wxString path = DoGetPath();
			return ::GetDriveTypeW(path.wc_str()) != DRIVE_NO_ROOT_DIR;
		}
		return false;
	}

	FSPath LegacyDrive::GetPath() const
	{
		return DoGetPath();
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
			wxString path = DoGetPath();
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
			wxString path = DoGetPath();
			return ::SetVolumeLabelW(path.wc_str(), label.IsEmpty() ? nullptr : label.wc_str());
		}
		return false;
	}

	DriveType LegacyDrive::GetType() const
	{
		if (IsValid())
		{
			wxString path = DoGetPath();
			return MapDriveType(::GetDriveTypeW(path.wc_str()));
		}
		return DriveType::Unknown;
	}
	wxString LegacyDrive::GetFileSystemName() const
	{
		if (IsValid())
		{
			wxString path = DoGetPath();
			wxString name;
			const int maxLength = MAX_PATH + 1;
			::GetVolumeInformationW(path.wc_str(), nullptr, 0, nullptr, nullptr, nullptr, wxStringBuffer(name, maxLength), maxLength);
			return name;
		}
		return wxEmptyString;
	}
	FileSystemFeature LegacyDrive::GetFileSystemFeatures() const
	{
		wxString path = DoGetPath();
		DWORD nativeFeatures = 0;
		if (::GetVolumeInformationW(path.wc_str(), nullptr, 0, nullptr, nullptr, &nativeFeatures, nullptr, 0))
		{
			return MapDiskFeatures(nativeFeatures);
		}
		return FileSystemFeature::None;
	}
	uint32_t LegacyDrive::GetSerialNumber() const
	{
		if (IsValid())
		{
			wxString path = DoGetPath();
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
			wxString path = DoGetPath();

			DWORD fileSystemFlags = 0;
			DWORD maximumComponentLength = 0;
			::GetVolumeInformationW(path.wc_str(), nullptr, 0, nullptr, &maximumComponentLength, &fileSystemFlags, nullptr, 0);

			DWORD bytesPerSector = 0;
			DWORD sectorsPerCluster = 0;
			DWORD numberOfFreeClusters = 0;
			DWORD totalNumberOfClusters = 0;
			::GetDiskFreeSpaceW(path.wc_str(), &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters);

			LegacyDriveInfo driveInfo = {};
			driveInfo.SectorsPerCluster = sectorsPerCluster;
			driveInfo.BytesPerSector = bytesPerSector;
			driveInfo.NumberOfFreeClusters = numberOfFreeClusters;
			driveInfo.TotalNumberOfClusters = totalNumberOfClusters;
			driveInfo.FileSystemFeatures = MapDiskFeatures(fileSystemFlags);
			driveInfo.LongFileNames = maximumComponentLength == 255;

			return driveInfo;
		}
		return {};
	}
	wxFileOffset LegacyDrive::GetTotalSpace() const
	{
		if (IsValid())
		{
			wxString path = DoGetPath();
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
			wxString path = DoGetPath();

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
			wxString path = DoGetPath();
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
			KxFileStream stream(FSPath(GetPath()).GetFullPath(FSPathNamespace::Win32Device), KxFileStream::Access::Read, KxFileStream::Disposition::OpenExisting, KxFileStream::Share::Everything);
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
