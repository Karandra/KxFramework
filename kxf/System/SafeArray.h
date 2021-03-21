#pragma once
#include "Common.h"
#include "HResult.h"
struct tagSAFEARRAY;

namespace kxf
{
	template<class T>
	class SafeArrayItems;

	enum class SafeArrayType
	{
		Unknown = -1,
		None = 0,

		Bool,
		UUID,
		String,
		DateTime,

		Int8,
		Int16,
		Int32,
		Int64,
		UInt8,
		UInt16,
		UInt32,
		UInt64,

		Float32,
		Float64,
	};
}

namespace kxf
{
	class SafeArray final
	{
		template<class T>
		friend class SafeArrayItems;

		private:
			tagSAFEARRAY* m_SafeArray = nullptr;

		private:
			HResult DoClear() noexcept;
			HResult DoCopy(const tagSAFEARRAY& other) noexcept;

			HResult AccessData(void**& data) noexcept;
			void UnaccessData() noexcept;

		public:
			SafeArray() noexcept;
			SafeArray(const tagSAFEARRAY& other) noexcept;
			SafeArray(const SafeArray& other) noexcept;
			SafeArray(SafeArray&& other) noexcept;
			~SafeArray() noexcept;

		public:
			bool IsNull() const noexcept;
			void Attach(tagSAFEARRAY* ptr) noexcept;
			tagSAFEARRAY* Detach() noexcept;

			bool IsEmpty() const noexcept;
			size_t GetSize(size_t dimension = 1) const noexcept;
			size_t GetDimensions() const noexcept;
			SafeArrayType GetType() const noexcept;

			template<class TValue>
			SafeArrayItems<TValue> GetItems() noexcept
			{
				return *this;
			}

			template<class TValue>
			const SafeArrayItems<TValue> GetItems() const noexcept
			{
				return const_cast<SafeArray&>(*this);
			}

			tagSAFEARRAY** GetAddress() noexcept
			{
				return &m_SafeArray;
			}

		public:
			const tagSAFEARRAY* operator&() const noexcept
			{
				return m_SafeArray;
			}
			tagSAFEARRAY* operator&() noexcept
			{
				return m_SafeArray;
			}

			const tagSAFEARRAY& operator*() const noexcept
			{
				return *m_SafeArray;
			}
			tagSAFEARRAY& operator*() noexcept
			{
				return *m_SafeArray;
			}

			SafeArray& operator=(const SafeArray& other) noexcept;
			SafeArray& operator=(SafeArray&& other) noexcept;

			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}

namespace kxf
{
	template<class TValue_>
	class SafeArrayItems
	{
		public:
			using TValue = TValue_;

		private:
			SafeArray& m_SafeArray;
			TValue* m_Items = nullptr;
			HResult m_Result = HResult::Fail();

		public:
			SafeArrayItems(SafeArray& safeArray) noexcept
				:m_SafeArray(safeArray)
			{
				void** items = reinterpret_cast<void**>(&m_Items);
				m_Result = safeArray.AccessData(items);

				if (!m_Result)
				{
					m_Items = nullptr;
				}
			}
			~SafeArrayItems()
			{
				if (m_Items)
				{
					m_SafeArray.UnaccessData();
				}
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Items == nullptr || !m_Result.IsSuccess();
			}

		public:
			TValue& operator[](size_t index) noexcept
			{
				return m_Items[index];
			}
			const TValue& operator[](size_t index) const noexcept
			{
				return m_Items[index];
			}

			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}
	};
}
