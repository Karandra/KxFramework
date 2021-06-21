#pragma once
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <array>
#include <vector>

namespace kxf
{
	class String;
	class IInputStream;
	class IOutputStream;
}

namespace kxf
{
	template<class TValue>
	struct BinarySerializer;

	class BinarySerializerException: public std::runtime_error
	{
		public:
			BinarySerializerException(const char* message);
			BinarySerializerException(const wchar_t* message);
			BinarySerializerException(const std::string& message);
			BinarySerializerException(const std::wstring& message);
			BinarySerializerException(const String& message);
	};
}
namespace kxf::Serialization
{
	template<class TValue>
	uint64_t WriteObject(IOutputStream& stream, const TValue& value)
	{
		if constexpr(std::is_enum_v<TValue>)
		{
			using T = std::underlying_type_t<TValue>;
			return BinarySerializer<T>().Serialize(stream, static_cast<T>(value));
		}
		else if constexpr(std::is_pointer_v<TValue>)
		{
			auto ptr = const_cast<const void*>(static_cast<void*>(value));
			return BinarySerializer<void*>().Serialize(stream, ptr);
		}
		else
		{
			return BinarySerializer<TValue>().Serialize(stream, value);
		}
	}

	template<size_t N>
	uint64_t WriteObject(IOutputStream& stream, const char(&value)[N])
	{
		return BinarySerializer<std::string_view>().Serialize(stream, std::string_view(value, N - 1));
	}

	template<size_t N>
	uint64_t WriteObject(IOutputStream& stream, const wchar_t(&value)[N])
	{
		return BinarySerializer<std::wstring_view>().Serialize(stream, std::wstring_view(value, N - 1));
	}

	template<class TValue>
	uint64_t ReadObject(IInputStream& stream, TValue& value)
	{
		if constexpr(std::is_enum_v<TValue>)
		{
			using T = std::underlying_type_t<TValue>;

			T valueInt = 0;
			auto read = BinarySerializer<T>().Deserialize(stream, valueInt);
			value = static_cast<TValue>(valueInt);

			return read;
		}
		else if constexpr(std::is_pointer_v<TValue>)
		{
			void* ptr = nullptr;
			auto read = BinarySerializer<void*>().Deserialize(stream, ptr);
			value = static_cast<TValue>(ptr);

			return read;
		}
		else
		{
			return BinarySerializer<TValue>().Deserialize(stream, value);
		}
	}
}

namespace kxf::Private
{
	class BufferBinarySerializer
	{
		protected:
			uint64_t DoWriteBuffer(IOutputStream& stream, const void* buffer, size_t length) const;
			uint64_t DoReadBuffer(IInputStream& stream, void* buffer, size_t length) const;
	};

	class IntBinarySerializer
	{
		private:
			uint64_t DoSerializeInteger(IOutputStream& stream, const void* buffer, size_t length, bool isSigned) const;
			uint64_t DoDeserializeInteger(IInputStream& stream, void* buffer, size_t length, bool isSigned) const;

		protected:
			template<class T>
			uint64_t SerializeInteger(IOutputStream& stream, const T& value) const
			{
				return DoSerializeInteger(stream, &value, sizeof(value), std::is_signed_v<T>);
			}

			template<class T>
			uint64_t DeserializeInteger(IInputStream& stream, T& value) const
			{
				return DoDeserializeInteger(stream, &value, sizeof(value), std::is_signed_v<T>);
			}
	};

	class FloatBinarySerializer
	{
		private:
			uint64_t DoSerializeFloat(IOutputStream& stream, const void* buffer, size_t length) const;
			uint64_t DoDeserializeFloat(IInputStream& stream, void* buffer, size_t length) const;

		protected:
			template<class T>
			uint64_t SerializeFloat(IOutputStream& stream, const T& value) const
			{
				return DoSerializeFloat(stream, &value, sizeof(value));
			}

			template<class T>
			uint64_t DeserializeFloat(IInputStream& stream, T& value) const
			{
				return DoDeserializeFloat(stream, &value, sizeof(value));
			}
	};

	class StringBinarySerializer
	{
		private:
			uint64_t DoSerializeString(IOutputStream& stream, const void* buffer, size_t length) const;
			uint64_t DoDeserializeString(IInputStream& stream, void* buffer, size_t& length) const;

		protected:
			template<class T>
			uint64_t SerializeString(IOutputStream& stream, const std::basic_string_view<T>& value) const
			{
				return DoSerializeString(stream, value.data(), value.size() * sizeof(T));
			}

			template<class T>
			uint64_t SerializeString(IOutputStream& stream, const std::basic_string<T>& value) const
			{
				return DoSerializeString(stream, value.data(), value.size() * sizeof(T));
			}

			template<class T>
			uint64_t DeserializeString(IInputStream& stream, std::basic_string<T>& value) const
			{
				size_t length = 0;
				uint64_t read = DoDeserializeString(stream, nullptr, length);

				value.resize(length);
				read += DoDeserializeString(stream, value.data(), length);
				return read;
			}
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<uint8_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const uint8_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, uint8_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<int8_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const int8_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, int8_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<uint16_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const uint16_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, uint16_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<int16_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const int16_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, int16_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<uint32_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const uint32_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, uint32_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<int32_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const int32_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, int32_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<uint64_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const uint64_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, uint64_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<int64_t> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const int64_t& value) const
		{
			return SerializeInteger(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, int64_t& value) const
		{
			return DeserializeInteger(stream, value);
		}
	};

	template<>
	struct BinarySerializer<float> final: private Private::FloatBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const float& value) const
		{
			return SerializeFloat(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, float& value) const
		{
			return DeserializeFloat(stream, value);
		}
	};

	template<>
	struct BinarySerializer<double> final: private Private::FloatBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const double& value) const
		{
			return SerializeFloat(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, double& value) const
		{
			return DeserializeFloat(stream, value);
		}
	};

	template<>
	struct BinarySerializer<void*> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const void*& value) const
		{
			return SerializeInteger(stream, reinterpret_cast<uint64_t>(value));
		}
		uint64_t Deserialize(IInputStream& stream, void*& value) const
		{
			uint64_t buffer = 0;
			auto read = DeserializeInteger(stream, buffer);
			value = reinterpret_cast<void*>(buffer);

			return read;
		}
	};

	template<>
	struct BinarySerializer<bool> final: private Private::IntBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const bool& value) const
		{
			return SerializeInteger(stream, static_cast<uint8_t>(value));
		}
		uint64_t Deserialize(IInputStream& stream, bool& value) const
		{
			uint8_t buffer = 0;
			auto read = DeserializeInteger(stream, buffer);
			value = static_cast<bool>(buffer);

			return read;
		}
	};
}

namespace kxf
{
	template<class T>
	struct BinarySerializer<std::basic_string<T>> final: private Private::StringBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const std::basic_string<T>& value) const
		{
			return SerializeString(stream, value);
		}
		uint64_t Deserialize(IInputStream& stream, std::basic_string<T>& value) const
		{
			return DeserializeString(stream, value);
		}
	};

	template<class T>
	struct BinarySerializer<std::basic_string_view<T>> final: private Private::StringBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const std::basic_string_view<T>& value) const
		{
			return SerializeString(stream, value);
		}
	};

	template<>
	struct BinarySerializer<const char*> final
	{
		uint64_t Serialize(IOutputStream& stream, const char* value) const
		{
			return Serialization::WriteObject(stream, value ? std::string_view(value) : std::string_view());
		}
	};

	template<>
	struct BinarySerializer<const wchar_t*> final
	{
		uint64_t Serialize(IOutputStream& stream, const wchar_t* value) const
		{
			return Serialization::WriteObject(stream, value ? std::wstring_view(value) : std::wstring_view());
		}
	};
}

namespace kxf
{
	template<class T, size_t N>
	struct BinarySerializer<std::array<T, N>> final: private Private::BufferBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const std::array<T, N>& value) const
		{
			uint64_t written = Serialization::WriteObject(stream, static_cast<uint64_t>(N));
			if constexpr(std::is_trivially_copyable_v<T>)
			{
				written += DoWriteBuffer(stream, value.data(), N * sizeof(T));
			}
			else
			{
				for (const auto& item: value)
				{
					written += Serialization::WriteObject(stream, item);
				}
			}
			return written;
		}
		uint64_t Deserialize(IInputStream& stream, std::array<T, N>& value) const
		{
			uint64_t length = 0;
			uint64_t read = Serialization::ReadObject(stream, length);
			if (length != N)
			{
				throw BinarySerializerException("Invalid item count");
			}

			if constexpr(std::is_trivially_copyable_v<T>)
			{
				read += DoReadBuffer(stream, value.data(), N * sizeof(T));
			}
			else
			{
				for (auto& item: value)
				{
					read += Serialization::ReadObject(stream, item);
				}
			}
			return read;
		}
	};

	template<class T>
	struct BinarySerializer<std::vector<T>> final: private Private::BufferBinarySerializer
	{
		uint64_t Serialize(IOutputStream& stream, const std::vector<T>& value) const
		{
			uint64_t written = Serialization::WriteObject(stream, static_cast<uint64_t>(value.size()));
			if constexpr(std::is_trivially_copyable_v<T>)
			{
				written += DoWriteBuffer(stream, value.data(), value.size() * sizeof(T));
			}
			else
			{
				for (const auto& item: value)
				{
					written += Serialization::WriteObject(stream, item);
				}
			}
			return written;
		}
		uint64_t Deserialize(IInputStream& stream, std::vector<T>& value) const
		{
			uint64_t length = 0;
			uint64_t read = Serialization::ReadObject(stream, length);

			value.resize(static_cast<size_t>(length));
			if constexpr(std::is_trivially_copyable_v<T>)
			{
				read += DoReadBuffer(stream, value.data(), length * sizeof(T));
			}
			else
			{
				for (auto& item: value)
				{
					read += Serialization::ReadObject(stream, item);
				}
			}
			return read;
		}
	};
}
