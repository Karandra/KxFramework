#pragma once
#include "kxf/Common.hpp"
#include "kxf/General/String.h"
#include "kxf/General/StreamDelegate.h"
#include "kxf/General/LocallyUniqueID.h"
#include "kxf/General/UniversallyUniqueID.h"
#include "kxf/FileSystem/FSPath.h"
#include "kxf/FileSystem/FileItem.h"
#include "kxf/FileSystem/IFileSystem.h"

namespace kxf::Compression
{
	constexpr size_t InvalidIndex = std::numeric_limits<size_t>::max();
}

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
