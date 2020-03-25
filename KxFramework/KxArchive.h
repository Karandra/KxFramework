#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFileItem.h"
#include <KxFramework/KxStreamDelegate.h>
#include "KxFramework/KxWinUndef.h"
#include "Kx/RTTI.hpp"

namespace KxArchive
{
	using FileIndex = uint32_t;
	using FileIndexVector = std::vector<FileIndex>;

	constexpr FileIndex InvalidFileIndex = std::numeric_limits<FileIndex>::max();
}

namespace KxArchive
{
	class FileIndexView final
	{
		private:
			union
			{
				const FileIndex* Ptr = nullptr;
				FileIndex Index;
			} m_Data;
			size_t m_Size = 0;

		private:
			void AssignMultiple(const FileIndex* data, size_t count)
			{
				if (data && count != 0)
				{
					m_Data.Ptr = data;
					m_Size = count;

					if (count == 1)
					{
						AssignSingle(*data);
					}
				}
			}
			void AssignSingle(FileIndex fileIndex)
			{
				m_Data.Index = fileIndex;
				m_Size = 1;
			}
			bool IsSingleIndex() const
			{
				return m_Size == 1;
			}

		public:
			FileIndexView() = default;
			FileIndexView(const FileIndex* data, size_t count)
			{
				AssignMultiple(data, count);
			}
			FileIndexView(const FileIndexVector& files)
			{
				AssignMultiple(files.data(), files.size());
			}
			FileIndexView(FileIndex fileIndex)
			{
				AssignSingle(fileIndex);
			}

			template<class T, size_t N>
			FileIndexView(const T(&container)[N])
			{
				AssignMultiple(container, N);
			}

			template<class T, size_t N>
			FileIndexView(const std::array<T, N>& container)
			{
				AssignMultiple(container.data(), container.size());
			}

		public:
			const FileIndex* data() const
			{
				if (IsSingleIndex())
				{
					return &m_Data.Index;
				}
				return m_Data.Ptr;
			}
			size_t size() const
			{
				return m_Size;
			}
			bool empty() const
			{
				return m_Size == 0;
			}

			FileIndex operator[](size_t index) const
			{
				return data()[index];
			}
			FileIndex front() const
			{
				return *data();
			}
			FileIndex back() const
			{
				return data()[size() - 1];
			}

			FileIndexVector CopyToVector() const
			{
				const FileIndex* data = this->data();
				const size_t size = this->size();

				return FileIndexVector(data, data + size);
			}

			explicit operator bool() const
			{
				return !empty();
			}
			bool operator!() const
			{
				return empty();
			}
	};
}

namespace KxArchive
{
	class KX_API IArchive: public KxFramework::RTTI::Interface<IArchive>
	{
		KxDecalreIID(IArchive, {0xb4327a42, 0x17a7, 0x44db, {0x84, 0xb, 0xc3, 0x24, 0x5b, 0x29, 0xca, 0xe8}});

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

	class KX_API IArchiveItems: public KxFramework::RTTI::Interface<IArchiveItems>
	{
		KxDecalreIID(IArchiveItems, {0x1455f21f, 0x1a17, 0x4ca2, {0xb5, 0x57, 0xaa, 0xa8, 0x68, 0xfb, 0x4b, 0x7e}});

		public:
			virtual ~IArchiveItems() = default;

		public:
			virtual size_t GetItemCount() const = 0;
			virtual KxFileItem GetItem(size_t fileIndex) const = 0;
	};
	
	class KX_API IArchiveSearch: public KxFramework::RTTI::Interface<IArchiveSearch>
	{
		KxDecalreIID(IArchiveSearch, {0x38c58054, 0x845d, 0x43c1, {0xa6, 0x6d, 0x46, 0xc4, 0xd2, 0x4d, 0x32, 0x3c}});

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
	class KX_API IArchiveExtraction;
	class KX_API IExtractionCallback: public KxFramework::RTTI::Interface<IExtractionCallback>
	{
		KxDecalreIID(IExtractionCallback, {0x8a6363c5, 0x35be, 0x4884, {0x8a, 0x35, 0x5e, 0x14, 0x5, 0x81, 0xbc, 0x25}});

		public:
			virtual ~IExtractionCallback() = default;

		public:
			virtual bool ShouldCancel()
			{
				return false;
			}

			virtual KxDelegateOutputStream OnGetStream(FileIndex fileIndex) = 0;
			virtual bool OnOperationCompleted(FileIndex fileIndex, wxOutputStream& stream) = 0;
	};

	template<class TOutStream = wxOutputStream>
	class KX_API ExtractWithOptions: public IExtractionCallback
	{
		private:
			const IArchiveExtraction& m_Archive;

			std::function<bool()> m_ShouldCancel;
			std::function<KxDelegateOutputStream(FileIndex)> m_OnGetStream;
			std::function<bool(FileIndex, wxOutputStream&)> m_OnOperationCompleted;

		private:
			bool ShouldCancel() override
			{
				return m_ShouldCancel ? m_ShouldCancel() : false;
			}

			KxDelegateOutputStream OnGetStream(FileIndex fileIndex) override
			{
				return m_OnGetStream ? m_OnGetStream(fileIndex) : nullptr;
			}
			bool OnOperationCompleted(FileIndex fileIndex, wxOutputStream& stream) override
			{
				return m_OnOperationCompleted ? m_OnOperationCompleted(fileIndex, stream) : true;
			}
			
		public:
			ExtractWithOptions(const IArchiveExtraction& archive)
				:m_Archive(archive)
			{
				static_assert(std::is_base_of_v<wxOutputStream, TOutStream>, "invalid stream type");
			}

		public:
			bool Execute()
			{
				return m_Archive.Extract(*this);
			}
			bool Execute(FileIndexView files)
			{
				return m_Archive.Extract(*this, files);
			}
			
			template<class TFunc>
			ExtractWithOptions& ShouldCancel(TFunc&& func)
			{
				m_ShouldCancel = std::forward<TFunc>(func);
				return *this;
			}

			template<class TFunc>
			ExtractWithOptions& OnGetStream(TFunc&& func)
			{
				m_OnGetStream = std::forward<TFunc>(func);
				return *this;
			}

			template<class TFunc>
			ExtractWithOptions& OnOperationCompleted(TFunc&& func)
			{
				if constexpr(std::is_same_v<TOutStream, wxOutputStream>)
				{
					// Assign as is
					m_OnOperationCompleted = std::forward<TFunc>(func);
				}
				else
				{
					// Wrap inside lambda and cast stream type
					m_OnOperationCompleted = [func = std::forward<TFunc>(func)](FileIndex fileIndex, wxOutputStream& stream) -> bool
					{
						return std::invoke(func, fileIndex, static_cast<TOutStream&>(stream));
					};
				}
				return *this;
			}
	};

	class KX_API IArchiveExtraction: public KxFramework::RTTI::Interface<IArchiveExtraction>
	{
		KxDecalreIID(IArchiveExtraction, {0x105f744b, 0x904d, 0x4822, {0xb4, 0x7a, 0x57, 0x8b, 0x3e, 0xd, 0x95, 0xe6}});

		public:
			virtual ~IArchiveExtraction() = default;

		public:
			// Extracts files using provided callback interface
			virtual bool Extract(IExtractionCallback& callback) const = 0;
			virtual bool Extract(IExtractionCallback& callback, FileIndexView files) const = 0;

			// Extract entire archive or only specified files into a directory
			virtual bool ExtractToDirectory(const wxString& directory) const;
			virtual bool ExtractToDirectory(const wxString& directory, FileIndexView files) const;
			
			// Extract specified file into a stream
			virtual bool ExtractToStream(FileIndex fileIndex, wxOutputStream& stream) const;

			// Extract single file into specified path
			virtual bool ExtractToFile(FileIndex fileIndex, const wxString& targetPath) const;

			template<class TOutStream = wxOutputStream>
			ExtractWithOptions<TOutStream> ExtractWith() const
			{
				return *this;
			}
	};
}

namespace KxArchive
{
	class KX_API IArchiveCompression: public KxFramework::RTTI::Interface<IArchiveCompression>
	{
		KxDecalreIID(IArchiveCompression, {0xcf9bb9ac, 0x6519, 0x49d4, {0xa3, 0xb4, 0xcd, 0x63, 0x17, 0x52, 0xe1, 0x55}});

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
	class KX_API IArchiveProperties: public KxFramework::RTTI::Interface<IArchiveProperties>
	{
		KxDecalreIID(IArchiveProperties, {0x8ecede61, 0x7542, 0x4164, {0x99, 0x7c, 0xd6, 0x72, 0x57, 0x24, 0x94, 0x26}});

		public:
			virtual ~IArchiveProperties() = default;

		public:
			virtual std::optional<bool> GetPropertyBool(wxStringView property) const = 0;
			virtual bool SetPropertyBool(wxStringView property, bool value) = 0;

			virtual std::optional<int64_t> GetPropertyInt(wxStringView property) const = 0;
			virtual bool SetPropertyInt(wxStringView property, int64_t value) = 0;

			virtual std::optional<double> GetPropertyFloat(wxStringView property) const = 0;
			virtual bool SetPropertyFloat(wxStringView property, double value) = 0;

			virtual std::optional<wxString> GetPropertyString(wxStringView property) const = 0;
			virtual bool SetPropertyString(wxStringView property, wxStringView value) = 0;

		public:
			template<class T>
			auto GetProperty(wxStringView property) const
			{
				if constexpr(std::is_same_v<T, bool>)
				{
					return GetPropertyBool(property);
				}
				else if constexpr(std::is_integral_v<T> || std::is_enum_v<T>)
				{
					if (auto value = GetPropertyInt(property))
					{
						return std::optional<T>(static_cast<T>(*value));
					}
					return std::optional<T>();
				}
				else if constexpr(std::is_floating_point_v<T>)
				{
					if (auto value = GetPropertyFloat(property))
					{
						return std::optional<T>(static_cast<T>(*value));
					}
					return std::optional<T>();
				}
				else if constexpr(std::is_same_v<T, wxString>)
				{
					return GetPropertyString(property);
				}
				else
				{
					static_assert(false, "invalid property type");
				}
			}

			template<class T>
			bool SetProperty(const wxStringView& property, T&& value)
			{
				if constexpr (std::is_same_v<T, bool>)
				{
					return SetPropertyBool(property, value);
				}
				else if constexpr(std::is_integral_v<T> )
				{
					return SetPropertyInt(property, value);
				}
				else if constexpr(std::is_enum_v<T>)
				{
					return SetPropertyInt(property, static_cast<std::underlying_type_t<T>>(value));
				}
				else if constexpr(std::is_floating_point_v<T>)
				{
					return SetPropertyFloat(property, value);
				}
				else
				{
					static_assert(false, "invalid property type");
				}
			}
			bool SetProperty(const wxStringView& property, wxStringView value)
			{
				return SetPropertyString(property, value);
			}
			bool SetProperty(const wxStringView& property, const wxString& value)
			{
				return SetPropertyString(property, wxStringView(value.wc_str(), value.length()));
			}
	};
}

namespace KxArchive
{
	#define KxArchiveDeclareUserProperty(section, name)	constexpr wxChar section##_##name[] = wxS("User/") wxS(#section) wxS("/") wxS(#name);

	namespace Property
	{
		#define KxArchiveDeclareBaseProperty(section, name) constexpr wxChar section##_##name[] = wxS("KxArchive/") wxS(#section) wxS("/") wxS(#name);

		KxArchiveDeclareBaseProperty(Common, FilePath);
		KxArchiveDeclareBaseProperty(Common, ItemCount);
		KxArchiveDeclareBaseProperty(Common, OriginalSize);
		KxArchiveDeclareBaseProperty(Common, CompressedSize);

		KxArchiveDeclareBaseProperty(Compression, Level);
		KxArchiveDeclareBaseProperty(Compression, Solid);
		KxArchiveDeclareBaseProperty(Compression, Format);
		KxArchiveDeclareBaseProperty(Compression, Method);
		KxArchiveDeclareBaseProperty(Compression, MultiThreaded);
		KxArchiveDeclareBaseProperty(Compression, DictionarySize);

		#undef KxArchiveDeclareBaseProperty
	}
}
