#pragma once
#include "Common.h"
#include "Kx/FileSystem/IFileSystem.h"
#include "Kx/FileSystem/FSPath.h"
#include "Kx/FileSystem/FileItem.h"
#include "Kx/General/String.h"
#include "Kx/General/BinarySize.h"
#include "Kx/General/StreamDelegate.h"
#include "Kx/System/UndefWindows.h"
#include "Kx/RTTI/QueryInterface.h"

namespace KxFramework::Compression
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

namespace KxFramework
{
	class KX_API IArchive: public RTTI::Interface<IArchive>
	{
		KxDecalreIID(IArchive, {0xb4327a42, 0x17a7, 0x44db, {0x84, 0xb, 0xc3, 0x24, 0x5b, 0x29, 0xca, 0xe8}});

		public:
			virtual ~IArchive() = default;

		public:
			virtual bool IsOK() const = 0;
			virtual bool Open(const String& filePath) = 0;
			virtual void Close() = 0;
			virtual FSPath GetFilePath() const = 0;

			virtual BinarySize GetOriginalSize() const = 0;
			virtual BinarySize GetCompressedSize() const = 0;
			double GetCompressionRatio() const
			{
				return GetSizeRatio(GetCompressedSize(), GetOriginalSize());
			}

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

	class KX_API IArchiveItems: public RTTI::Interface<IArchiveItems>
	{
		KxDecalreIID(IArchiveItems, {0x1455f21f, 0x1a17, 0x4ca2, {0xb5, 0x57, 0xaa, 0xa8, 0x68, 0xfb, 0x4b, 0x7e}});

		public:
			virtual ~IArchiveItems() = default;

		public:
			virtual size_t GetItemCount() const = 0;
			virtual FileItem GetItem(Compression::FileIndex fileIndex) const = 0;
			virtual size_t EnumItems(const FSPath& directory, IFileSystem::TEnumItemsFunc func, const FSPathQuery& query = {}, FSEnumItemsFlag flags = FSEnumItemsFlag::None) const = 0;
			
			FileItem FindItem(const FSPathQuery& query) const;
			FileItem FindItem(const FSPath& directory, const FSPathQuery& query) const;
	};
}

namespace KxFramework
{
	class KX_API IArchiveExtraction;
	class KX_API IExtractionCallback: public RTTI::Interface<IExtractionCallback>
	{
		KxDecalreIID(IExtractionCallback, {0x8a6363c5, 0x35be, 0x4884, {0x8a, 0x35, 0x5e, 0x14, 0x5, 0x81, 0xbc, 0x25}});

		public:
			virtual ~IExtractionCallback() = default;

		public:
			virtual bool ShouldCancel()
			{
				return false;
			}

			virtual OutputStreamDelegate OnGetStream(Compression::FileIndex fileIndex) = 0;
			virtual bool OnOperationCompleted(Compression::FileIndex fileIndex, wxOutputStream& stream) = 0;
	};

	template<class TOutStream = wxOutputStream>
	class KX_API ExtractWithOptions: public IExtractionCallback
	{
		private:
			const IArchiveExtraction& m_Archive;

			std::function<bool()> m_ShouldCancel;
			std::function<OutputStreamDelegate(Compression::FileIndex)> m_OnGetStream;
			std::function<bool(Compression::FileIndex, wxOutputStream&)> m_OnOperationCompleted;

		private:
			bool ShouldCancel() override
			{
				return m_ShouldCancel ? m_ShouldCancel() : false;
			}

			OutputStreamDelegate OnGetStream(Compression::FileIndex fileIndex) override
			{
				return m_OnGetStream ? m_OnGetStream(fileIndex) : nullptr;
			}
			bool OnOperationCompleted(Compression::FileIndex fileIndex, wxOutputStream& stream) override
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
			bool Execute(Compression::FileIndexView files)
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
					m_OnOperationCompleted = [func = std::forward<TFunc>(func)](Compression::FileIndex fileIndex, wxOutputStream& stream) -> bool
					{
						return std::invoke(func, fileIndex, static_cast<TOutStream&>(stream));
					};
				}
				return *this;
			}
	};

	class KX_API IArchiveExtraction: public RTTI::Interface<IArchiveExtraction>
	{
		KxDecalreIID(IArchiveExtraction, {0x105f744b, 0x904d, 0x4822, {0xb4, 0x7a, 0x57, 0x8b, 0x3e, 0xd, 0x95, 0xe6}});

		public:
			virtual ~IArchiveExtraction() = default;

		public:
			// Extracts files using provided callback interface
			virtual bool Extract(IExtractionCallback& callback) const = 0;
			virtual bool Extract(IExtractionCallback& callback, Compression::FileIndexView files) const = 0;

			// Extract entire archive or only specified files into a directory
			virtual bool ExtractToDirectory(const FSPath& directory) const;
			virtual bool ExtractToDirectory(const FSPath& directory, Compression::FileIndexView files) const;
			
			// Extract specified file into a stream
			virtual bool ExtractToStream(Compression::FileIndex fileIndex, wxOutputStream& stream) const;

			// Extract single file into specified path
			virtual bool ExtractToFile(Compression::FileIndex fileIndex, const FSPath& targetPath) const;

			template<class TOutStream = wxOutputStream>
			ExtractWithOptions<TOutStream> ExtractWith() const
			{
				return *this;
			}
	};
}

namespace KxFramework
{
	class KX_API IArchiveCompression: public RTTI::Interface<IArchiveCompression>
	{
		KxDecalreIID(IArchiveCompression, {0xcf9bb9ac, 0x6519, 0x49d4, {0xa3, 0xb4, 0xcd, 0x63, 0x17, 0x52, 0xe1, 0x55}});

		public:
			virtual ~IArchiveCompression() = default;

		public:
			// Includes the last directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
			// makes "MyFolder" the single root item in archive with the files within it included.
			virtual bool CompressDirectory(const FSPath& directory, bool recursive) = 0;

			// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
			// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
			virtual bool CompressFiles(const FSPath& directory, const FSPath& searchFilter, bool recursive) = 0;
			virtual bool CompressSpecifiedFiles(const KxStringVector& sourcePaths, const KxStringVector& archivePaths) = 0;

			// Compress just this single file as the root item in the archive.
			// Second overload places compressed file into 'archivePath' folder inside the archive.
			virtual bool CompressFile(const String& sourcePath) = 0;
			virtual bool CompressFile(const String& sourcePath, const String& archivePath) = 0;
	};
}

namespace KxFramework
{
	class KX_API IArchiveProperties: public RTTI::Interface<IArchiveProperties>
	{
		KxDecalreIID(IArchiveProperties, {0x8ecede61, 0x7542, 0x4164, {0x99, 0x7c, 0xd6, 0x72, 0x57, 0x24, 0x94, 0x26}});

		public:
			virtual ~IArchiveProperties() = default;

		public:
			virtual std::optional<bool> GetPropertyBool(StringView property) const = 0;
			virtual bool SetPropertyBool(StringView property, bool value) = 0;

			virtual std::optional<int64_t> GetPropertyInt(StringView property) const = 0;
			virtual bool SetPropertyInt(StringView property, int64_t value) = 0;

			virtual std::optional<double> GetPropertyFloat(StringView property) const = 0;
			virtual bool SetPropertyFloat(StringView property, double value) = 0;

			virtual std::optional<String> GetPropertyString(StringView property) const = 0;
			virtual bool SetPropertyString(StringView property, StringView value) = 0;

		public:
			template<class T>
			auto GetProperty(StringView property) const
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
				else if constexpr(std::is_same_v<T, String>)
				{
					return GetPropertyString(property);
				}
				else
				{
					static_assert(false, "invalid property type");
				}
			}

			template<class T>
			bool SetProperty(const StringView& property, T&& value)
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
			
			bool SetProperty(const StringView& property, StringView value)
			{
				return SetPropertyString(property, value);
			}
			bool SetProperty(const StringView& property, const String& value)
			{
				return SetPropertyString(property, StringViewOf(value));
			}
	};
}

namespace KxFramework::Compression
{
	#define Kx_Compression_DeclareUserProperty(section, name)	constexpr wxChar section##_##name[] = wxS("User/") wxS(#section) wxS("/") wxS(#name);

	namespace Property
	{
		#define Kx_Compression_DeclareBaseProperty(section, name) constexpr wxChar section##_##name[] = wxS("Archive/") wxS(#section) wxS("/") wxS(#name);

		Kx_Compression_DeclareBaseProperty(Common, FilePath);
		Kx_Compression_DeclareBaseProperty(Common, ItemCount);
		Kx_Compression_DeclareBaseProperty(Common, OriginalSize);
		Kx_Compression_DeclareBaseProperty(Common, CompressedSize);

		Kx_Compression_DeclareBaseProperty(Compression, Level);
		Kx_Compression_DeclareBaseProperty(Compression, Solid);
		Kx_Compression_DeclareBaseProperty(Compression, Format);
		Kx_Compression_DeclareBaseProperty(Compression, Method);
		Kx_Compression_DeclareBaseProperty(Compression, MultiThreaded);
		Kx_Compression_DeclareBaseProperty(Compression, DictionarySize);

		#undef KxArchiveDeclareBaseProperty
	}
}
