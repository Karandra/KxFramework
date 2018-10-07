#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxWinUndef.h"
class KxFileItem;

namespace KxIArchiveNS
{
	using IndexVector = std::vector<uint32_t>;
	using Buffer = std::vector<uint8_t>;
	using BufferMap = std::unordered_map<size_t, Buffer>;
}

//////////////////////////////////////////////////////////////////////////
class KxIArchive
{
	public:
		virtual ~KxIArchive() = default;

	public:
		virtual bool IsOK() const = 0;
		virtual bool Open(const wxString& filePath) = 0;
		virtual void Close() = 0;
		virtual wxString GetFilePath() const = 0;

		virtual size_t GetItemCount() const = 0;
		virtual wxString GetItemName(size_t index) const = 0;
		virtual int64_t GetOriginalSize() const = 0;
		virtual int64_t GetCompressedSize() const = 0;
		float GetCompressionRatio() const;
};

//////////////////////////////////////////////////////////////////////////
class KxIArchiveSearch
{
	public:
		virtual ~KxIArchiveSearch() = default;

	public:
		virtual void* FindFirstFile(const wxString& searchQuery, KxFileItem& fileItem) const = 0;
		virtual bool FindNextFile(void* handle, KxFileItem& item) const = 0;
		virtual void FindClose(void* handle) const = 0;

		bool FindFile(const wxString& searchQuery, KxFileItem& fileItem) const;
		bool FindFileInFolder(const wxString& folder, const wxString& filter, KxFileItem& fileItem) const;
};

//////////////////////////////////////////////////////////////////////////
class KxIArchiveExtraction
{
	public:
		virtual ~KxIArchiveExtraction() = default;

	protected:
		virtual bool DoExtractAll(const wxString& directory) const = 0;
		virtual bool DoExtractToDirectory(const KxIArchiveNS::IndexVector& indexes, const wxString& directory) const = 0;
		virtual bool DoExtractToFiles(const KxIArchiveNS::IndexVector& indexes, const KxStringVector& filePaths) const = 0;
		virtual KxIArchiveNS::Buffer DoExtractToMemory(size_t index) const = 0;
		virtual KxIArchiveNS::BufferMap DoExtractToMemory(const KxIArchiveNS::IndexVector& indexes) const = 0;

	public:
		// Extract entire archive into specified directory
		bool ExtractAll(const wxString& directory) const
		{
			return DoExtractAll(directory);
		}
		
		// Extract only specified files into directory
		bool ExtractToDirectory(const KxIArchiveNS::IndexVector& indexes, const wxString& directory) const
		{
			return DoExtractToDirectory(indexes, directory);
		}
		
		// Extract only specified files into corresponding files path
		bool ExtractToFiles(const KxIArchiveNS::IndexVector& indexes, const KxStringVector& filePaths) const
		{
			return DoExtractToFiles(indexes, filePaths);
		}

		// Extract specified file into memory buffer
		KxIArchiveNS::Buffer ExtractToMemory(size_t index) const
		{
			return DoExtractToMemory(index);
		}
		KxIArchiveNS::BufferMap ExtractToMemory(const KxIArchiveNS::IndexVector& indexes) const
		{
			return DoExtractToMemory(indexes);
		}
};

//////////////////////////////////////////////////////////////////////////
class KxIArchiveCompression
{
	public:
		virtual ~KxIArchiveCompression() = default;

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

//////////////////////////////////////////////////////////////////////////
template<class t_BoolProperties> class KxIArchivePropertiesBool
{
	public:
		virtual ~KxIArchivePropertiesBool() = default;

	public:
		using BoolProperties = t_BoolProperties;
		using BoolPropertiesValue = bool;

	public:
		virtual bool GetPropertyBool(t_BoolProperties property) const = 0;
		virtual void SetPropertyBool(t_BoolProperties property, bool value) = 0;
};

template<class t_IntProperties, class t_IntType = int> class KxIArchivePropertiesInt
{
	public:
		virtual ~KxIArchivePropertiesInt() = default;

	public:
		using IntProperties = t_IntProperties;
		using IntPropertiesValue = t_IntType;

	public:
		virtual t_IntType GetPropertyInt(t_IntProperties property) const = 0;
		virtual void SetPropertyInt(t_IntProperties property, t_IntType value) = 0;
};

template<class t_StringProperties> class KxIArchivePropertiesString
{
	public:
		virtual ~KxIArchivePropertiesString() = default;

	public:
		using StringProperties = t_StringProperties;
		using StringPropertiesValue = wxString;

	public:
		virtual wxString GetPropertyString(t_StringProperties property) const = 0;
		virtual void SetPropertyString(t_StringProperties property, const wxString& value) = 0;
};
