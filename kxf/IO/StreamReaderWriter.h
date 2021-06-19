#pragma once
#include "Common.h"
#include "IStream.h"
#include "kxf/Utility/Common.h"

namespace kxf::IO
{
	class InputStreamReader final
	{
		private:
			IInputStream& m_Stream;

		private:
			template<class C>
			static void DoRemoveTrailingNulls(C& data)
			{
				while (!data.empty() && data.back() == 0)
				{
					data.pop_back();
				}
			}
			
			template<class C>
			bool DoReadContainter(C& values, size_t count)
			{
				using T = typename C::value_type;
				static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				values.resize(count);
				return ReadBuffer(values.data(), values.size() * sizeof(T));
			}

		public:
			InputStreamReader(IInputStream& stream)
				:m_Stream(stream)
			{
			}

		public:
			template<class... T>
			bool Skip()
			{
				static_assert(sizeof...(T) != 0, "IOStreamSeeker::Skip<T...>: Skipping 0 bytes is not allowed");

				return m_Stream.SeekI(Utility::SizeOfParameterPackValues<T...>(), IOStreamSeek::FromCurrent).IsValid();
			}

			bool Skip(BinarySize count)
			{
				return m_Stream.SeekI(count, IOStreamSeek::FromCurrent).IsValid();
			}

			bool Rewind()
			{
				return m_Stream.SeekI(0, IOStreamSeek::FromStart).IsValid();
			}
			bool SeekToEnd()
			{
				return m_Stream.SeekI(0, IOStreamSeek::FromEnd).IsValid();
			}

			bool SeekFromStart(BinarySize offset)
			{
				return m_Stream.SeekI(offset, IOStreamSeek::FromStart).IsValid();
			}
			bool SeekFromEnd(BinarySize offset)
			{
				return m_Stream.SeekI(offset, IOStreamSeek::FromEnd).IsValid();
			}

		public:
			bool LastReadSuccess() const
			{
				return m_Stream.LastRead().IsValid();
			}

			bool ReadBuffer(void* buffer, size_t size)
			{
				return m_Stream.ReadAll(buffer, size);
			}
			
			template<class T>
			bool ReadObject(T& object)
			{
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				return ReadBuffer(std::addressof(object), sizeof(object));
			}
			
			template<class T>
			T ReadObject()
			{
				static_assert(std::is_default_constructible_v<T>, "T must be default constructible");

				T object;
				ReadObject(object);
				return object;
			}
			
			template<class T, size_t count>
			bool ReadArray(std::array<T, count>& values)
			{
				static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				return ReadBuffer(values.data(), values.size() * sizeof(T));
			}
			
			template<class T, size_t count>
			std::array<T, count> ReadArray()
			{
				std::array<T, count> values;
				ReadArray(values);
				return values;
			}

			template<class T>
			bool ReadVector(std::vector<T>& values, size_t count)
			{
				return DoReadContainter(values, count);
			}
			
			template<class T>
			std::vector<T> ReadVector(size_t count)
			{
				std::vector<T> values;
				DoReadContainter(values, count);
				return values;
			}

			template<class C>
			bool ReadContainter(C& values, size_t count)
			{
				return DoReadContainter(values, count);
			}
			
			template<class C>
			C ReadContainter(size_t count)
			{
				C values;
				DoReadContainter(values, count);
				return values;
			}

			bool ReadStringASCII(String& value, size_t size);
			String ReadStringASCII(size_t size)
			{
				String value;
				ReadStringASCII(value, size);
				return value;
			}

			bool ReadStringUTF8(String& value, size_t size);
			String ReadStringUTF8(size_t size)
			{
				String value;
				ReadStringUTF8(value, size);
				return value;
			}
			
			bool ReadStringUTF16(String& value, size_t size);
			String ReadStringUTF16(size_t size)
			{
				String value;
				ReadStringUTF16(value, size);
				return value;
			}
			
			bool ReadStringUTF32(String& value, size_t size);
			String ReadStringUTF32(size_t size)
			{
				String value;
				ReadStringUTF32(value, size);
				return value;
			}

			bool ReadStdString(std::string& value, size_t size)
			{
				if (DoReadContainter(value, size))
				{
					DoRemoveTrailingNulls(value);
					return true;
				}
				else
				{
					value.clear();
					return false;
				}
			}
			std::string ReadStdString(size_t size)
			{
				std::string value;
				ReadStdString(value, size);
				return value;
			}

			bool ReadStdWString(std::wstring& value, size_t size)
			{
				if (DoReadContainter(value, size))
				{
					DoRemoveTrailingNulls(value);
					return true;
				}
				else
				{
					value.clear();
					return false;
				}
			}
			std::wstring ReadStdWString(size_t size)
			{
				std::wstring value;
				ReadStdWString(value, size);
				return value;
			}
	};
}

namespace kxf::IO
{
	class OutputStreamWriter final
	{
		private:
			IOutputStream& m_Stream;

		private:
			template<class C>
			bool DoWriteContainter(const C& values)
			{
				using T = typename C::value_type;
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				return WriteBuffer(values.data(), values.size() * sizeof(T));
			}

		public:
			OutputStreamWriter(IOutputStream& stream)
				:m_Stream(stream)
			{
			}

		public:
			bool Rewind()
			{
				return m_Stream.SeekO(0, IOStreamSeek::FromStart).IsValid();
			}
			bool SeekToEnd()
			{
				return m_Stream.SeekO(0, IOStreamSeek::FromEnd).IsValid();
			}

			bool SeekFromStart(BinarySize offset)
			{
				return m_Stream.SeekO(offset, IOStreamSeek::FromStart).IsValid();
			}
			bool SeekFromEnd(BinarySize offset)
			{
				return m_Stream.SeekO(offset, IOStreamSeek::FromEnd).IsValid();
			}

		public:
			bool LastWriteSuccess() const
			{
				return m_Stream.LastWrite().IsValid();
			}

			bool WriteBuffer(const void* buffer, size_t size)
			{
				return m_Stream.WriteAll(buffer, size);
			}

			template<class T>
			bool WriteObject(const T& object)
			{
				static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

				return WriteBuffer(std::addressof(object), sizeof(T));
			}
			
			template<class T, size_t count>
			bool WriteArray(const std::array<T, count>& values)
			{
				return DoWriteContainter(values);
			}
			
			template<class T>
			bool WriteVector(const std::vector<T>& values)
			{
				return DoWriteContainter(values);
			}
			
			template<class C>
			bool WriteContainter(const C& values)
			{
				return DoWriteContainter(values);
			}

			bool WriteStringASCII(const String& value, char replaceWith = '_');
			bool WriteStringUTF8(const String& value);
			bool WriteStringUTF16(const String& value);
			bool WriteStringUTF32(const String& value);
	};
}
