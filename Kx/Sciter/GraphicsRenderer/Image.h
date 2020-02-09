#pragma once
#include "Kx/Sciter/Common.h"

namespace KxSciter
{
	struct ImageHandle;
}

namespace KxSciter
{
	class KX_API GraphicsBitmap final
	{
		public:
			enum class Format
			{
				None = 0,
				Raw,
				PNG,
				JPG,
				WEBP
			};

		private:
			ImageHandle* m_Handle = nullptr;

		private:
			void Acquire(ImageHandle* handle);
			void Release();

			void CopyFrom(const GraphicsBitmap& other)
			{
				Release();
				Acquire(other.m_Handle);
			}
			void CopyFrom(ImageHandle* handle)
			{
				Release();
				Acquire(handle);
			}
			void MoveFrom(GraphicsBitmap& other)
			{
				Release();
				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;
			}

		public:
			GraphicsBitmap() = default;
			GraphicsBitmap(ImageHandle* handle)
			{
				Acquire(handle);
			}
			GraphicsBitmap(const GraphicsBitmap& other)
			{
				*this = other;
			}
			GraphicsBitmap(GraphicsBitmap&& other)
			{
				*this = std::move(other);
			}
			GraphicsBitmap(const wxSize& size, bool withAlpha);
			GraphicsBitmap(const wxSize& size, const char* pixmapData, bool withAlpha)
			{
				CreateFromPixmap(size, pixmapData, withAlpha);
			}
			GraphicsBitmap(const wxImage& image);
			GraphicsBitmap(const wxBitmap& bitmap);
			GraphicsBitmap(wxInputStream& stream)
			{
				Load(stream);
			}
			~GraphicsBitmap()
			{
				Release();
			}
			
		public:
			bool IsOk() const
			{
				return m_Handle != nullptr;
			}
			ImageHandle* GetHandle() const
			{
				return m_Handle;
			}
			void MakeNull()
			{
				Release();
			}

			bool AttachHandle(ImageHandle* handle)
			{
				if (!IsOk())
				{
					m_Handle = handle;
					return true;
				}
				return false;
			}
			ImageHandle* DetachHandle()
			{
				ImageHandle* handle = m_Handle;
				m_Handle = nullptr;
				return handle;
			}

			// Construct image from B[n+0], G[n+1], R[n+2], A[n+3] data.
			// Size of pixmap data is pixmapWidth * pixmapHeight * 4.
			bool CreateFromPixmap(const wxSize& size, const char* pixmapData, bool withAlpha);

			bool Load(wxInputStream& stream);
			bool Save(wxOutputStream& stream, Format format, int quality = 100) const;

			bool Clear(const KxColor& color);
			wxSize GetSize() const;
			bool UsesAlpha() const;

			wxImage ConvertToImage() const;
			wxBitmap ConvertToBitmap() const;

		public:
			GraphicsBitmap& operator=(const GraphicsBitmap& other)
			{
				CopyFrom(other);
				return *this;
			}
			GraphicsBitmap& operator=(GraphicsBitmap&& other)
			{
				MoveFrom(other);
				return *this;
			}
			GraphicsBitmap& operator=(ImageHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}

			bool operator==(const GraphicsBitmap& other) const
			{
				return m_Handle == other.m_Handle;
			}
			bool operator!=(const GraphicsBitmap& other) const
			{
				return !(*this == other);
			}

			explicit operator bool() const
			{
				return IsOk();
			}
			bool operator!() const
			{
				return !IsOk();
			}
	};
}
