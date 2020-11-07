#pragma once
#include "Common.h"
#include "Image.h"
class wxIconBundle;

namespace kxf
{
	enum class ImageBundleFlag: uint32_t
	{
		None = 0,

		SystemSize = 1 << 1,
		SystemSizeSmall = 1 << 2,
		NearestLarger = 1 << 3
	};
	KxFlagSet_Declare(ImageBundleFlag);
}

namespace kxf
{
	class KX_API ImageBundle: public RTTI::Interface<ImageBundle>
	{
		KxRTTI_DeclareIID(ImageBundle, {0x8e7461c, 0x1234, 0x49c0, {0xa0, 0x0, 0xe2, 0x16, 0x4a, 0x75, 0x7d, 0x85}});

		private:
			std::vector<Image> m_Items;

		public:
			ImageBundle() noexcept = default;
			ImageBundle(size_t initialCount)
			{
				m_Items.reserve(initialCount);
			}
			ImageBundle(const ImageBundle&) = default;
			ImageBundle(ImageBundle&&) noexcept = default;
			ImageBundle(const wxIconBundle& other);
			ImageBundle(const Image& other);
			virtual ~ImageBundle() = default;

		public:
			// ImageBundle
			bool IsNull() const;
			bool IsSameAs(const ImageBundle& other) const;

			bool Load(IInputStream& stream, ImageFormat format = ImageFormat::Any);
			bool Save(IOutputStream& stream, ImageFormat format) const;

			void AddImage(const Image& image);
			Image GetImage(Size desiredSize, FlagSet<ImageBundleFlag> sizeFallback = ImageBundleFlag::None) const;

			size_t GetImageCount() const
			{
				return m_Items.size();
			}
			Image GetImageAt(size_t index) const
			{
				if (index < m_Items.size())
				{
					return m_Items[index];
				}
				return {};
			}

			wxIconBundle ToWxIconBundle() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			ImageBundle& operator=(const ImageBundle&) = default;
			ImageBundle& operator=(ImageBundle&&) noexcept = default;
	};
}
