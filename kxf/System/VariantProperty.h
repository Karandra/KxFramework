#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/NativeUUID.h"
#include "kxf/General/DateTime.h"
#include "kxf/General/AlignedStorageObject.h"
#include "HResult.h"
struct tagPROPVARIANT;

namespace kxf
{
	enum class VariantPropertyType
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
	class VariantProperty final
	{
		private:
			AlignedStorageObject<tagPROPVARIANT, 24, alignof(uint64_t)> m_Value;

		private:
			HResult DoClear() noexcept;
			HResult DoCopy(const tagPROPVARIANT& other);
			void DoMove(tagPROPVARIANT&& other) noexcept;

			void AssignBool(bool value) noexcept;
			void AssignUUID(const NativeUUID& value);
			void AssignString(std::string_view value);
			void AssignString(std::wstring_view value);
			void AssignDateTime(const DateTime& value) noexcept;

			void AssignInt(int8_t value) noexcept;
			void AssignInt(int16_t value) noexcept;
			void AssignInt(int32_t value) noexcept;
			void AssignInt(int64_t value) noexcept;
			void AssignInt(uint8_t value) noexcept;
			void AssignInt(uint16_t value) noexcept;
			void AssignInt(uint32_t value) noexcept;
			void AssignInt(uint64_t value) noexcept;

			void AssignFloat(float value) noexcept;
			void AssignFloat(double value) noexcept;

			std::optional<int8_t> RetrieveInt8() const noexcept;
			std::optional<int16_t> RetrieveInt16() const noexcept;
			std::optional<int32_t> RetrieveInt32() const noexcept;
			std::optional<int64_t> RetrieveInt64() const noexcept;
			std::optional<uint8_t> RetrieveUInt8() const noexcept;
			std::optional<uint16_t> RetrieveUInt16() const noexcept;
			std::optional<uint32_t> RetrieveUInt32() const noexcept;
			std::optional<uint64_t> RetrieveUInt64() const noexcept;

			std::optional<float> RetrieveFloat32() const noexcept;
			std::optional<double> RetrieveFloat64() const noexcept;

			template<int nativeType, class TValue1, class TValue2>
			VariantProperty& AssignSimpleValue(TValue1& valueStore, TValue2 value) noexcept
			{
				if (GetNativeType() != nativeType)
				{
					DoClear();
				}

				valueStore = std::move(value);
				SetNativeType(nativeType);

				return *this;
			}

			template<int nativeType, class TValue>
			std::optional<TValue> RetrieveSimpleValue(const TValue& valueStore) const noexcept
			{
				if (GetNativeType() == nativeType)
				{
					return valueStore;
				}
				return {};
			}

		public:
			VariantProperty() noexcept;
			VariantProperty(const tagPROPVARIANT& value)
				:VariantProperty()
			{
				DoCopy(value);
			}
			VariantProperty(const VariantProperty& other)
				:VariantProperty()
			{
				DoCopy(*other.m_Value);
			}
			VariantProperty(tagPROPVARIANT&& other) noexcept
				:VariantProperty()
			{
				DoMove(std::move(other));
			}
			VariantProperty(VariantProperty&& other) noexcept
				:VariantProperty()
			{
				DoMove(std::move(*other.m_Value));
			}
			
			VariantProperty(bool value) noexcept
				:VariantProperty()
			{
				AssignBool(value);
			};
			VariantProperty(const char* value)
				:VariantProperty()
			{
				AssignString(value ? value : "");
			}
			VariantProperty(const wchar_t* value)
				:VariantProperty()
			{
				AssignString(value ? value : L"");
			}
			VariantProperty(const String& value)
				:VariantProperty()
			{
				AssignString(value.wx_str());
			}
			VariantProperty(const DateTime& value)
				:VariantProperty()
			{
				AssignDateTime(value);
			}
			VariantProperty(const NativeUUID& value)
				:VariantProperty()
			{
				AssignUUID(value);
			}

			VariantProperty(int8_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			VariantProperty(int16_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			VariantProperty(int32_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			VariantProperty(int64_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			VariantProperty(uint8_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			VariantProperty(uint16_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			VariantProperty(uint32_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			VariantProperty(uint64_t value) noexcept
				:VariantProperty()
			{
				AssignInt(value);
			}
			
			VariantProperty(float value) noexcept
				:VariantProperty()
			{
				AssignFloat(value);
			}
			VariantProperty(double value) noexcept
				:VariantProperty()
			{
				AssignFloat(value);
			}

			~VariantProperty() noexcept;

		public:
			bool IsEmpty() const noexcept;
			VariantPropertyType GetType() const noexcept;
			int Compare(const VariantProperty& other) const noexcept;
			HResult CopyToNative(tagPROPVARIANT& nativeProperty) const;
			
			int GetNativeType() const noexcept;
			void SetNativeType(int nativeType) noexcept;

			std::optional<bool> ToBool() const noexcept;
			std::optional<NativeUUID> ToUUID() const noexcept;
			std::optional<String> ToString() const;
			std::optional<DateTime> ToDateTime() const noexcept;

			template<class T>
			std::optional<T> ToInt() const noexcept
			{
				static_assert(std::is_integral_v<T>, "integer type required");

				auto Repack = [](auto opt) -> std::optional<T>
				{
					if (opt)
					{
						return static_cast<T>(*opt);
					}
					return {};
				};
				switch (GetType())
				{
					case VariantPropertyType::Int8:
					{
						return Repack(RetrieveInt8());
					}
					case VariantPropertyType::Int16:
					{
						return Repack(RetrieveInt16());
					}
					case VariantPropertyType::Int32:
					{
						return Repack(RetrieveInt32());
					}
					case VariantPropertyType::Int64:
					{
						return Repack(RetrieveInt64());
					}

					case VariantPropertyType::UInt8:
					{
						return Repack(RetrieveUInt8());
					}
					case VariantPropertyType::UInt16:
					{
						return Repack(RetrieveUInt16());
					}
					case VariantPropertyType::UInt32:
					{
						return Repack(RetrieveUInt32());
					}
					case VariantPropertyType::UInt64:
					{
						return Repack(RetrieveUInt64());
					}
				};
				return {};
			}

			template<class T>
			std::optional<T> ToFloat() const noexcept
			{
				static_assert(std::is_floating_point_v<T>, "floating point number type required");

				auto Repack = [](auto opt) -> std::optional<T>
				{
					if (opt)
					{
						return static_cast<T>(*opt);
					}
					return {};
				};
				switch (GetType())
				{
					case VariantPropertyType::Float32:
					{
						return Repack(RetrieveFloat32());
					}
					case VariantPropertyType::Float64:
					{
						return Repack(RetrieveFloat64());
					}
				};
				return {};
			}

		public:
			VariantProperty& operator=(const tagPROPVARIANT& value)
			{
				DoCopy(value);
				return *this;
			}
			VariantProperty& operator=(const VariantProperty& other)
			{
				DoCopy(*other.m_Value);
				return *this;
			}
			VariantProperty& operator=(tagPROPVARIANT&& other) noexcept
			{
				DoMove(std::move(other));
				return *this;
			}
			VariantProperty& operator=(VariantProperty&& other) noexcept
			{
				DoMove(std::move(*other.m_Value));
				return *this;
			}

			bool operator==(const VariantProperty& other) const noexcept
			{
				return Compare(other) == 0;
			}
			bool operator!=(const VariantProperty& other) const noexcept
			{
				return Compare(other) != 0;
			}
			bool operator<(const VariantProperty& other) const noexcept
			{
				return Compare(other) < 0;
			}
			bool operator<=(const VariantProperty& other) const noexcept
			{
				return Compare(other) <= 0;
			}
			bool operator>(const VariantProperty& other) const noexcept
			{
				return Compare(other) > 0;
			}
			bool operator>=(const VariantProperty& other) const noexcept
			{
				return Compare(other) >= 0;
			}

			const tagPROPVARIANT* operator&() const noexcept
			{
				return &m_Value;
			}
			tagPROPVARIANT* operator&() noexcept
			{
				return &m_Value;
			}

			const tagPROPVARIANT& operator*() const noexcept
			{
				return *m_Value;
			}
			tagPROPVARIANT& operator*() noexcept
			{
				return *m_Value;
			}

			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}
	};
}
