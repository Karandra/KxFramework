#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWinUndef.h"
class KX_API KxFileItem;

namespace KxArchive
{
	using FileIndex = uint32_t;
	using FileIndexVector = std::vector<FileIndex>;
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

			virtual size_t GetItemCount() const = 0;
			virtual wxString GetItemName(size_t index) const = 0;
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
		protected:
			// Extract entire archive into specified directory
			virtual bool DoExtractAll(const wxString& directory) const = 0;

			// Extract only specified files into directory
			virtual bool DoExtractToDirectory(const FileIndexVector& files, const wxString& directory) const = 0;

			// Extract only specified files into corresponding files path
			virtual bool DoExtractToFile(const FileIndexVector& files, const KxStringVector& filePaths) const = 0;

			// Extract specified file to a stream
			virtual bool DoExtractStream(FileIndex fileIndex, wxOutputStream& stream) = 0;

		public:
			virtual ~IArchiveExtraction() = default;

		public:
			// Extract entire archive into specified directory
			bool ExtractAll(const wxString& directory) const
			{
				return DoExtractAll(directory);
			}
			
			// Extract only specified files into directory
			bool ExtractToDirectory(const FileIndexVector& files, const wxString& directory) const
			{
				return DoExtractToDirectory(files, directory);
			}
			bool ExtractToDirectory(FileIndex fileIndex, const wxString& directory) const
			{
				return DoExtractToDirectory({fileIndex}, directory);
			}
			
			// Extract only specified files into corresponding files path
			bool ExtractToFile(const FileIndexVector& files, const KxStringVector& filePaths) const
			{
				return DoExtractToFile(files, filePaths);
			}
			bool ExtractToFile(FileIndex fileIndex, const wxString& targetPath) const
			{
				return DoExtractToFile({fileIndex}, {targetPath});
			}
			
			// Extract specified file to a stream
			bool ExtractToStream(FileIndex fileIndex, wxOutputStream& stream)
			{
				return DoExtractStream(fileIndex, stream);
			}
	};
}

namespace KxArchive
{
	class KX_API IArchiveCompression
	{
		public:
			virtual ~IArchiveCompression() = default;

		protected:
			virtual bool DoCompressFiles(const wxString& directory, const wxString& searchFilter, bool recursive) = 0;
			virtual bool DoCompressDirectory(const wxString& directory, bool recursive) = 0;
			virtual bool DoCompressSpecifiedFiles(const KxStringVector& sourcePaths, const KxStringVector& archivePaths) = 0;
			virtual bool DoCompressFile(const wxString& sourcePath) = 0;
			virtual bool DoCompressFile(const wxString& sourcePath, const wxString& archivePath) = 0;

		public:
			// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
			// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
			bool CompressFiles(const wxString& directory, const wxString& searchFilter, bool recursive)
			{
				return DoCompressFiles(directory, searchFilter, recursive);
			}
			bool CompressAllFiles(const wxString& directory, bool recursive)
			{
				return DoCompressFiles(directory, wxEmptyString, recursive);
			}
		
			// Includes the last directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
			// makes "MyFolder" the single root item in archive with the files within it included.
			bool CompressDirectory(const wxString& directory, bool recursive)
			{
				return DoCompressDirectory(directory, recursive);
			}
			bool CompressSpecifiedFiles(const KxStringVector& sourcePaths, const KxStringVector& archivePaths)
			{
				return DoCompressSpecifiedFiles(sourcePaths, archivePaths);
			}

			// Compress just this single file as the root item in the archive.
			bool CompressFile(const wxString& sourcePath)
			{
				return DoCompressFile(sourcePath);
			}

			// Same as above, but places compressed file into 'archivePath' folder inside the archive
			bool CompressFile(const wxString& sourcePath, const wxString& archivePath)
			{
				return DoCompressFile(sourcePath, archivePath);
			}
	};
}

namespace KxArchive
{
	template<class TPropertyIndex>
	class IArchiveBoolProperties
	{
		protected:
			~IArchiveBoolProperties() = default;

		public:
			using TBoolPropertyIndex = TPropertyIndex;
			using TBoolPropertyValue = bool;

		public:
			virtual bool GetPropertyBool(const TPropertyIndex& property) const = 0;
			virtual void SetPropertyBool(const TPropertyIndex& property, bool value) = 0;
	};

	template<class TPropertyIndex, class t_IntType = int>
	class IArchiveIntProperties
	{
		protected:
			~IArchiveIntProperties() = default;

		public:
			using TIntPropertyIndex = TPropertyIndex;
			using TIntPropertyValue = t_IntType;

		public:
			virtual t_IntType GetPropertyInt(const TPropertyIndex& property) const = 0;
			virtual void SetPropertyInt(const TPropertyIndex& property, t_IntType value) = 0;
	};

	template<class TPropertyIndex>
	class IArchiveStringProperties
	{
		protected:
			~IArchiveStringProperties() = default;

		public:
			using TStringPropertyIndex = TPropertyIndex;
			using TStringPropertyValue = wxString;

		public:
			virtual wxString GetPropertyString(const TPropertyIndex& property) const = 0;
			virtual void SetPropertyString(const TPropertyIndex& property, const wxString& value) = 0;
	};
}
