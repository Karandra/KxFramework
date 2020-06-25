#pragma once
#include "Common.h"
#include "kxf/FileSystem/IFileSystem.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/General/String.h"
#include "kxf/General/BinarySize.h"
#include "kxf/General/StreamDelegate.h"
#include "kxf/System/UndefWindows.h"
#include "kxf/RTTI/QueryInterface.h"

namespace kxf::Compression
{
	class FileIndexView final
	{
		private:
			union
			{
				const size_t* Ptr = nullptr;
				size_t Index;
			} m_Data;
			size_t m_Size = 0;

		private:
			void AssignMultiple(const size_t* data, size_t count) noexcept
			{
				if (data && count != 0)
				{
					if (count == 1)
					{
						AssignSingle(*data);
					}
					else
					{
						m_Data.Ptr = data;
						m_Size = count;
					}
				}
			}
			void AssignSingle(size_t size_t) noexcept
			{
				m_Data.Index = size_t;
				m_Size = 1;
			}
			bool IsSingleIndex() const noexcept
			{
				return m_Size == 1;
			}

		public:
			FileIndexView() noexcept = default;
			FileIndexView(size_t size_t) noexcept
			{
				AssignSingle(size_t);
			}
			FileIndexView(const size_t* data, size_t count) noexcept
			{
				AssignMultiple(data, count);
			}
			explicit FileIndexView(const std::vector<size_t>& files) noexcept
			{
				AssignMultiple(files.data(), files.size());
			}
			
			template<class T, size_t N>
			explicit FileIndexView(const T(&container)[N]) noexcept
			{
				AssignMultiple(container, N);
			}

			template<class T, size_t N>
			explicit FileIndexView(const std::array<T, N>& container) noexcept
			{
				AssignMultiple(container.data(), container.size());
			}

		public:
			const size_t* data() const noexcept
			{
				if (IsSingleIndex())
				{
					return &m_Data.Index;
				}
				return m_Data.Ptr;
			}
			size_t size() const noexcept
			{
				return m_Size;
			}
			bool empty() const noexcept
			{
				return m_Size == 0;
			}

			size_t operator[](size_t index) const noexcept
			{
				return data()[index];
			}
			size_t front() const noexcept
			{
				return *data();
			}
			size_t back() const noexcept
			{
				return data()[size() - 1];
			}

			template<class T>
			std::vector<T> ToVector() const
			{
				const size_t* begin = this->data();
				const size_t* end = begin + this->size();

				if constexpr(std::is_same_v<T, size_t>)
				{
					return {begin, end};
				}
				else
				{
					std::vector<T> result;
					result.reserve(this->size());

					for (auto it = begin; it != end; ++it)
					{
						result.emplace_back(*it);
					}
					return result;
				}
			}

			explicit operator bool() const noexcept
			{
				return !empty();
			}
			bool operator!() const noexcept
			{
				return empty();
			}
	};
}

namespace kxf
{
	class KX_API IArchive: public RTTI::Interface<IArchive>
	{
		KxDeclareIID(IArchive, {0xb4327a42, 0x17a7, 0x44db, {0x84, 0xb, 0xc3, 0x24, 0x5b, 0x29, 0xca, 0xe8}});

		public:
			virtual ~IArchive() = default;

		public:
			virtual FSPath GetFilePath() const = 0;
			virtual bool IsOpened() const = 0;
			virtual bool Open(const FSPath& filePath) = 0;
			virtual void Close() = 0;

			virtual size_t GetItemCount() const = 0;
			virtual BinarySize GetOriginalSize() const = 0;
			virtual BinarySize GetCompressedSize() const = 0;
			double GetCompressionRatio() const
			{
				return GetSizeRatio(GetCompressedSize(), GetOriginalSize());
			}

		public:
			explicit operator bool() const
			{
				return IsOpened();
			}
			bool operator!() const
			{
				return !IsOpened();
			}
	};
}

namespace kxf
{
	class KX_API IArchiveExtraction;
	class KX_API IExtractionCallback: public RTTI::Interface<IExtractionCallback>
	{
		KxDeclareIID(IExtractionCallback, {0x8a6363c5, 0x35be, 0x4884, {0x8a, 0x35, 0x5e, 0x14, 0x5, 0x81, 0xbc, 0x25}});

		public:
			virtual ~IExtractionCallback() = default;

		public:
			virtual bool ShouldCancel()
			{
				return false;
			}

			virtual OutputStreamDelegate OnGetStream(size_t size_t) = 0;
			virtual bool OnOperationCompleted(size_t size_t, wxOutputStream& stream) = 0;
	};

	template<class TOutStream = wxOutputStream>
	class KX_API ExtractWithOptions: public IExtractionCallback
	{
		private:
			const IArchiveExtraction& m_Archive;

			std::function<bool()> m_ShouldCancel;
			std::function<OutputStreamDelegate(size_t)> m_OnGetStream;
			std::function<bool(size_t, wxOutputStream&)> m_OnOperationCompleted;

		private:
			bool ShouldCancel() override
			{
				return m_ShouldCancel ? m_ShouldCancel() : false;
			}

			OutputStreamDelegate OnGetStream(size_t size_t) override
			{
				return m_OnGetStream ? m_OnGetStream(size_t) : nullptr;
			}
			bool OnOperationCompleted(size_t size_t, wxOutputStream& stream) override
			{
				return m_OnOperationCompleted ? m_OnOperationCompleted(size_t, stream) : true;
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
					m_OnOperationCompleted = [func = std::forward<TFunc>(func)](size_t index, wxOutputStream& stream) -> bool
					{
						return std::invoke(func, index, static_cast<TOutStream&>(stream));
					};
				}
				return *this;
			}
	};

	class KX_API IArchiveExtraction: public RTTI::Interface<IArchiveExtraction>
	{
		KxDeclareIID(IArchiveExtraction, {0x105f744b, 0x904d, 0x4822, {0xb4, 0x7a, 0x57, 0x8b, 0x3e, 0xd, 0x95, 0xe6}});

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
			virtual bool ExtractToStream(size_t size_t, wxOutputStream& stream) const;

			// Extract single file into specified path
			virtual bool ExtractToFile(size_t size_t, const FSPath& targetPath) const;

			template<class TOutStream = wxOutputStream>
			ExtractWithOptions<TOutStream> ExtractWith() const
			{
				return *this;
			}
	};
}

namespace kxf
{
	class KX_API IArchiveCompression: public RTTI::Interface<IArchiveCompression>
	{
		KxDeclareIID(IArchiveCompression, {0xcf9bb9ac, 0x6519, 0x49d4, {0xa3, 0xb4, 0xcd, 0x63, 0x17, 0x52, 0xe1, 0x55}});

		public:
			virtual ~IArchiveCompression() = default;

		public:
			// Includes the last directory as the root in the archive, e.g. specifying "C:\Temp\MyFolder"
			// makes "MyFolder" the single root item in archive with the files within it included.
			virtual bool CompressDirectory(const FSPath& directory, bool recursive) = 0;

			// Excludes the last directory as the root in the archive, its contents are at root instead. E.g.
			// specifying "C:\Temp\MyFolder" make the files in "MyFolder" the root items in the archive.
			virtual bool CompressFiles(const FSPath& directory, const FSPath& searchFilter, bool recursive) = 0;
			virtual bool CompressSpecifiedFiles(const std::vector<wxString>& sourcePaths, const std::vector<wxString>& archivePaths) = 0;

			// Compress just this single file as the root item in the archive.
			// Second overload places compressed file into 'archivePath' folder inside the archive.
			virtual bool CompressFile(const String& sourcePath) = 0;
			virtual bool CompressFile(const String& sourcePath, const String& archivePath) = 0;
	};
}

namespace kxf
{
	class KX_API IArchiveProperties: public RTTI::Interface<IArchiveProperties>
	{
		KxDeclareIID(IArchiveProperties, {0x8ecede61, 0x7542, 0x4164, {0x99, 0x7c, 0xd6, 0x72, 0x57, 0x24, 0x94, 0x26}});

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
	};
}

namespace kxf::Compression
{
	#define Kx_Compression_DeclareUserProperty(section, name)	constexpr XChar section##_##name[] = wxS("User/") wxS(#section) wxS("/") wxS(#name);

	namespace Property
	{
		#define Kx_Compression_DeclareBaseProperty(section, name) constexpr XChar section##_##name[] = wxS("Archive/") wxS(#section) wxS("/") wxS(#name);

		Kx_Compression_DeclareBaseProperty(Common, FilePath);
		Kx_Compression_DeclareBaseProperty(Common, ItemCount);
		Kx_Compression_DeclareBaseProperty(Common, OriginalSize);
		Kx_Compression_DeclareBaseProperty(Common, CompressedSize);

		Kx_Compression_DeclareBaseProperty(Compression, Format);
		Kx_Compression_DeclareBaseProperty(Compression, Method);
		Kx_Compression_DeclareBaseProperty(Compression, Level);
		Kx_Compression_DeclareBaseProperty(Compression, Solid);
		Kx_Compression_DeclareBaseProperty(Compression, MultiThreaded);
		Kx_Compression_DeclareBaseProperty(Compression, DictionarySize);

		#undef KxArchiveDeclareBaseProperty
	}
}
