#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWinUndef.h"
class KX_API KxFileItem;

namespace KxArchive
{
	using FileIndex = uint32_t;
	using FileIndexVector = std::vector<FileIndex>;
	using FileIndexToPathMap = std::unordered_map<FileIndex, wxString>;
}

namespace KxArchive
{
	class KX_API IArchive
	{
		public:
			virtual ~IArchive() = default;

		public:
			virtual bool IsOK() const = 0;
			virtual bool Open(const wxString& filePath) = 0;
			virtual void Close() = 0;
			virtual wxString GetFilePath() const = 0;

			virtual int64_t GetOriginalSize() const = 0;
			virtual int64_t GetCompressedSize() const = 0;
			double GetCompressionRatio() const;

		public:
			explicit operator bool() const
			{
				return IsOK();
			}
			bool operator!() const
			{
				return !IsOK();
			}
	};

	class KX_API IArchiveItems
	{
		public:
			virtual ~IArchiveItems() = default;

		public:
			virtual size_t GetItemCount() const = 0;
			virtual KxFileItem GetItem(size_t fileIndex) const = 0;
	};
	
	class KX_API IArchiveSearch
	{
		public:
			virtual ~IArchiveSearch() = default;

		public:
			virtual void* FindFirstFile(const wxString& searchQuery, KxFileItem& fileItem) const = 0;
			virtual bool FindNextFile(void* handle, KxFileItem& item) const = 0;
			virtual void FindClose(void* handle) const = 0;

			bool FindFile(const wxString& searchQuery, KxFileItem& fileItem) const;
			bool FindFileInFolder(const wxString& folder, const wxString& filter, KxFileItem& fileItem) const;
	};
}

namespace KxArchive
{
	class KX_API IArchiveExtraction
	{
		public:
			virtual ~IArchiveExtraction() = default;

		public:
			// Extract entire archive into specified directory
			virtual bool ExtractAll(const wxString& directory) const = 0;
			
			// Extract only specified files into directory
			virtual bool ExtractToDirectory(const FileIndexVector& files, const wxString& directory) const = 0;
			virtual bool ExtractToDirectory(FileIndex fileIndex, const wxString& directory) const = 0;
			
			// Extract only specified files into corresponding files path
			virtual bool ExtractToFile(const FileIndexToPathMap& files) const = 0;
			virtual bool ExtractToFile(FileIndex fileIndex, const wxString& targetPath) const = 0;
			
			// Extract specified file to a stream
			virtual bool ExtractToStream(const FileIndexVector& files) const = 0;
			virtual bool ExtractToStream(FileIndex fileIndex, wxOutputStream& stream) const = 0;
	};
}

namespace KxArchive
{
	class KX_API IArchiveCompression
	{
		public:
			virtual ~IArchiveCompression() = default;

		public:
			// Includes the last directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
			// makes "MyFolder" the single root item in archive with the files within it included.
			virtual bool CompressDirectory(const wxString& directory, bool recursive) = 0;

			// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
			// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
			virtual bool CompressFiles(const wxString& directory, const wxString& searchFilter, bool recursive) = 0;
			virtual bool CompressSpecifiedFiles(const KxStringVector& sourcePaths, const KxStringVector& archivePaths) = 0;

			// Compress just this single file as the root item in the archive.
			// Second overload places compressed file into 'archivePath' folder inside the archive.
			virtual bool CompressFile(const wxString& sourcePath) = 0;
			virtual bool CompressFile(const wxString& sourcePath, const wxString& archivePath) = 0;
	};
}

namespace KxArchive
{
	template<class TPropertyIndex>
	class IBoolProperties
	{
		protected:
			~IBoolProperties() = default;

		public:
			using TBoolPropertyIndex = TPropertyIndex;
			using TBoolPropertyValue = bool;

		public:
			virtual bool GetPropertyBool(const TPropertyIndex& property) const = 0;
			virtual void SetPropertyBool(const TPropertyIndex& property, bool value) = 0;
	};

	template<class TPropertyIndex, class t_IntType = int>
	class IIntProperties
	{
		protected:
			~IIntProperties() = default;

		public:
			using TIntPropertyIndex = TPropertyIndex;
			using TIntPropertyValue = t_IntType;

		public:
			virtual t_IntType GetPropertyInt(const TPropertyIndex& property) const = 0;
			virtual void SetPropertyInt(const TPropertyIndex& property, t_IntType value) = 0;
	};

	template<class TPropertyIndex>
	class IStringProperties
	{
		protected:
			~IStringProperties() = default;

		public:
			using TStringPropertyIndex = TPropertyIndex;
			using TStringPropertyValue = wxString;

		public:
			virtual wxString GetPropertyString(const TPropertyIndex& property) const = 0;
			virtual void SetPropertyString(const TPropertyIndex& property, const wxString& value) = 0;
	};
}
