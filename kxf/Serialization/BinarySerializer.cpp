#include "KxfPCH.h"
#include "BinarySerializer.h"
#include "kxf/General/String.h"
#include "kxf/IO/IStream.h"

namespace
{
	template<class T>
	uint64_t SerializeInteger(kxf::IOutputStream& stream, const T& value)
	{
		uint64_t written = stream.Write(&value, sizeof(value)).LastWrite().ToBytes();
		if (written != sizeof(value))
		{
			throw kxf::BinarySerializerException("Could not write all bytes to the stream");
		}
		return written;
	}

	template<class T>
	uint64_t DesrializeInteger(kxf::IInputStream& stream, T& value)
	{
		uint64_t read = stream.Read(&value, sizeof(value)).LastRead().ToBytes();
		if (read != sizeof(value))
		{
			throw kxf::BinarySerializerException("Could not read the required amount of bytes");
		}
		return read;
	}

	template<class T>
	uint64_t SerializeView(kxf::IOutputStream& stream, const std::basic_string_view<T>& value)
	{
		const T* data = value.data();
		const uint64_t length = static_cast<uint64_t>(value.length());

		uint64_t written = stream.Write(&length, sizeof(length)).LastWrite().ToBytes();
		written += stream.Write(data, length).LastWrite().ToBytes();

		if (written != length + sizeof(length))
		{
			throw kxf::BinarySerializerException("Could not write all bytes to the stream");
		}
		return written;
	}

	template<class T>
	uint64_t DeserializeString(kxf::IInputStream& stream, std::basic_string<T>& value)
	{
		uint64_t length = 0;
		uint64_t read = stream.Read(&length, sizeof(length)).LastRead().ToBytes();
		if (read != sizeof(length))
		{
			throw kxf::BinarySerializerException("Could not read the string size data");
		}

		value.resize(length);
		read += stream.Read(value.data(), length).LastRead().ToBytes();
		if (stream.LastRead() != length)
		{
			throw kxf::BinarySerializerException("Could not read the required amount of bytes");
		}
		return read;
	}
}

namespace kxf
{
	BinarySerializerException::BinarySerializerException(const char* message)
		:runtime_error(message)
	{
	}
	BinarySerializerException::BinarySerializerException(const wchar_t* message)
		:BinarySerializerException(String(message))
	{
	}
	BinarySerializerException::BinarySerializerException(const std::string& message)
		:runtime_error(message)
	{
	}
	BinarySerializerException::BinarySerializerException(const std::wstring& message)
		:BinarySerializerException(String::FromView(message))
	{
	}
	BinarySerializerException::BinarySerializerException(const String& message)
		:runtime_error(message.ToUTF8())
	{
	}
}

namespace kxf
{
	namespace Private
	{
		uint64_t IntBinarySerializer::DoSerializeInteger(IOutputStream& stream, const void* buffer, size_t length, bool isSigned) const
		{
			uint64_t written = stream.Write(buffer, length).LastWrite().ToBytes();
			if (written != length)
			{
				throw BinarySerializerException("Could not write all bytes to the stream");
			}
			return written;
		}
		uint64_t IntBinarySerializer::DoDeserializeInteger(IInputStream& stream, void* buffer, size_t length, bool isSigned) const
		{
			uint64_t read = stream.Read(buffer, length).LastRead().ToBytes();
			if (read != length)
			{
				throw BinarySerializerException("Could not read the required amount of bytes");
			}
			return read;
		}

		uint64_t FloatBinarySerializer::DoSerializeFloat(IOutputStream& stream, const void* buffer, size_t length) const
		{
			uint64_t written = stream.Write(buffer, length).LastWrite().ToBytes();
			if (written != length)
			{
				throw BinarySerializerException("Could not write all bytes to the stream");
			}
			return written;
		}
		uint64_t FloatBinarySerializer::DoDeserializeFloat(IInputStream& stream, void* buffer, size_t length) const
		{
			uint64_t read = stream.Read(buffer, length).LastRead().ToBytes();
			if (read != length)
			{
				throw BinarySerializerException("Could not read the required amount of bytes");
			}
			return read;
		}
	}
}

namespace kxf
{
	uint64_t BinarySerializer<std::string>::Serialize(IOutputStream& stream, const std::string& value) const
	{
		return BinarySerializer<std::string_view>().Serialize(stream, value);
	}
	uint64_t BinarySerializer<std::string>::Deserialize(IInputStream& stream, std::string& value) const
	{
		return DeserializeString(stream, value);
	}

	uint64_t BinarySerializer<std::wstring>::Serialize(IOutputStream& stream, const std::wstring& value) const
	{
		return BinarySerializer<std::wstring_view>().Serialize(stream, value);
	}
	uint64_t BinarySerializer<std::wstring>::Deserialize(IInputStream& stream, std::wstring& value) const
	{
		return DeserializeString(stream, value);
	}

	uint64_t BinarySerializer<std::string_view>::Serialize(IOutputStream& stream, const std::string_view& value) const
	{
		return SerializeView(stream, value);
	}
	uint64_t BinarySerializer<std::wstring_view>::Serialize(IOutputStream& stream, const std::wstring_view& value) const
	{
		return SerializeView(stream, value);
	}
}
