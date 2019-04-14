/*
Copyright © 2018 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxUtility.h"
#include <wx/ustring.h>
#include <vector>
#include <array>

class KX_API KxStreamBase
{
	public:
		using Offset = wxFileOffset;

		enum: Offset
		{
			InvalidOffset = wxInvalidOffset
		};
		enum class SeekMode
		{
			FromStart = wxSeekMode::wxFromStart,
			FromEnd = wxSeekMode::wxFromEnd,
			FromCurrent = wxSeekMode::wxFromCurrent,
		};
		enum class ErrorCode
		{
			Success = wxSTREAM_NO_ERROR,
			EndOfFile = wxSTREAM_EOF,
			ReadError = wxSTREAM_WRITE_ERROR,
			WriteError = wxSTREAM_READ_ERROR
		};

	public:
		virtual ~KxStreamBase() = default;

	public:
		virtual bool Flush() = 0;
		virtual bool SetAllocationSize(Offset offset = InvalidOffset) = 0;

		virtual bool IsWriteable() const = 0;
		virtual bool IsReadable() const = 0;
};

//////////////////////////////////////////////////////////////////////////
template<class BaseStreamT> class KxInputStreamWrapper: public BaseStreamT
{
	protected:
		template<class C> static void DoRemoveTrailingNulls(C& data)
		{
			while (!data.empty() && data.back() == 0)
			{
				data.pop_back();
			}
		}
		template<class C> bool DoReadContainter(C& values, size_t count)
		{
			using T = typename C::value_type;
			static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
			static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

			values.resize(count);
			return ReadBuffer(values.data(), values.size() * sizeof(T));
		}

	public:
		KxInputStreamWrapper(const KxInputStreamWrapper&) = delete;
		KxInputStreamWrapper(KxInputStreamWrapper&&) = delete;

		template<class... Args> KxInputStreamWrapper(Args&&... arg)
			:BaseStreamT(std::forward<Args>(arg)...)
		{
		}
		virtual ~KxInputStreamWrapper() = default;

	public:
		KxInputStreamWrapper& operator=(const KxInputStreamWrapper&) = delete;
		KxInputStreamWrapper& operator=(KxInputStreamWrapper&&) = delete;

	public:
		bool LastReadSuccess() const
		{
			return this->LastRead() != 0;
		}

		bool ReadBuffer(void* buffer, size_t size)
		{
			return this->OnSysRead(buffer, size) != 0;
		}
		wxMemoryBuffer ReadBuffer(size_t size)
		{
			wxMemoryBuffer buffer(size);
			ReadBuffer(buffer.GetData(), buffer.GetDataLen());
			return buffer;
		}
		
		template<class T> bool ReadObject(T& object)
		{
			static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

			return ReadBuffer(std::addressof(object), sizeof(object));
		}
		template<class T> T ReadObject()
		{
			static_assert(std::is_default_constructible_v<T>, "T must be default constructible");

			T object;
			ReadObject(object);
			return object;
		}
		
		template<class T, size_t count> bool ReadArray(std::array<T, count>& values)
		{
			static_assert(std::is_default_constructible_v<T>, "T must be default constructible");
			static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

			return ReadBuffer(values.data(), values.size() * sizeof(T));
		}
		template<class T, size_t count> std::array<T, count> ReadArray()
		{
			std::array<T, count> values;
			ReadArray(values);
			return values;
		}

		template<class T> bool ReadVector(std::vector<T>& values, size_t count)
		{
			return DoReadContainter(values, count);
		}
		template<class T> std::vector<T> ReadVector(size_t count)
		{
			std::vector<T> values;
			DoReadContainter(values, count);
			return values;
		}

		template<class C> bool ReadContainter(C& values, size_t count)
		{
			return DoReadContainter(values, count);
		}
		template<class C> C ReadContainter(size_t count)
		{
			C values;
			DoReadContainter(values, count);
			return values;
		}

		bool ReadStringACP(wxString& value, size_t size)
		{
			std::vector<char> buffer;
			if (ReadVector(buffer, size))
			{
				DoRemoveTrailingNulls(buffer);
				value.assign(buffer.data(), buffer.size());
				return true;
			}
			else
			{
				value.clear();
				return false;
			}
		}
		wxString ReadStringACP(size_t size)
		{
			wxString value;
			ReadStringACP(value, size);
			return value;
		}

		bool ReadStringASCII(wxString& value, size_t size)
		{
			std::vector<char> buffer;
			if (ReadVector(buffer, size))
			{
				DoRemoveTrailingNulls(buffer);
				value = wxString::FromAscii(buffer.data(), buffer.size());
				return true;
			}
			else
			{
				value.clear();
				return false;
			}
		}
		wxString ReadStringASCII(size_t size)
		{
			wxString value;
			ReadStringASCII(value, size);
			return value;
		}

		bool ReadStringUTF8(wxString& value, size_t size)
		{
			std::vector<char> buffer;
			if (ReadVector(buffer, size))
			{
				DoRemoveTrailingNulls(buffer);
				value = wxString::FromUTF8(buffer.data(), buffer.size());
				return true;
			}
			else
			{
				value.clear();
				return false;
			}
		}
		wxString ReadStringUTF8(size_t size)
		{
			wxString value;
			ReadStringUTF8(value, size);
			return value;
		}
		
		bool ReadStringUTF16(wxString& value, size_t size)
		{
			std::vector<wchar_t> buffer;
			if (ReadVector(buffer, size))
			{
				DoRemoveTrailingNulls(buffer);
				value.assign(buffer.data(), buffer.size());
				return true;
			}
			else
			{
				value.clear();
				return false;
			}
		}
		wxString ReadStringUTF16(size_t size)
		{
			wxString value;
			ReadStringUTF16(value, size);
			return value;
		}
		
		bool ReadStringUTF32(wxString& value, size_t size)
		{
			std::vector<wxChar32> buffer;
			if (ReadVector(buffer, size))
			{
				DoRemoveTrailingNulls(buffer);
				value = wxUString(wxScopedU32CharBuffer::CreateNonOwned(buffer.data(), buffer.size()));
				return true;
			}
			else
			{
				value.clear();
				return false;
			}
		}
		wxString ReadStringUTF32(size_t size)
		{
			wxString value;
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

//////////////////////////////////////////////////////////////////////////
template<class BaseStreamT> class KxOutputStreamWrapper: public BaseStreamT
{
	protected:
		template<class C> bool DoWriteContainter(const C& values)
		{
			using T = typename C::value_type;
			static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

			return WriteBuffer(values.data(), values.size() * sizeof(T));
		}

	public:
		KxOutputStreamWrapper(const KxOutputStreamWrapper&) = delete;
		KxOutputStreamWrapper(KxOutputStreamWrapper&&) = delete;

		template<class... Args> KxOutputStreamWrapper(Args&&... arg)
			:BaseStreamT(std::forward<Args>(arg)...)
		{
		}
		virtual ~KxOutputStreamWrapper() = default;

	public:
		KxOutputStreamWrapper& operator=(const KxOutputStreamWrapper&) = delete;
		KxOutputStreamWrapper& operator=(KxOutputStreamWrapper&&) = delete;

	public:
		bool LastWriteSuccess() const
		{
			return this->LastWrite() != 0;
		}

		bool WriteBuffer(const void* buffer, size_t size)
		{
			return this->OnSysWrite(buffer, size) != 0;
		}
		bool WriteBuffer(const wxMemoryBuffer& buffer)
		{
			return WriteBuffer(buffer.GetData(), buffer.GetDataLen());
		}

		template<class T> bool WriteObject(const T& object)
		{
			static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");

			return WriteBuffer(std::addressof(object), sizeof(T));
		}
		template<class T, size_t count> bool WriteArray(const std::array<T, count>& values)
		{
			return DoWriteContainter(values);
		}
		template<class T> bool WriteVector(const std::vector<T>& values)
		{
			return DoWriteContainter(values);
		}
		template<class C> bool WriteContainter(const C& values)
		{
			return DoWriteContainter(values);
		}

		bool WriteStringACP(const wxString& value)
		{
			const auto buffer = value.c_str().AsCharBuf();
			return WriteBuffer(buffer.data(), buffer.length());
		}
		bool WriteStringASCII(const wxString& value, char replaceWith = '_')
		{
			const auto buffer = value.ToAscii(replaceWith);
			return WriteBuffer(buffer.data(), buffer.length());
		}
		bool WriteStringUTF8(const wxString& value)
		{
			const auto buffer = value.ToUTF8();
			return WriteBuffer(buffer.data(), buffer.length());
		}
		bool WriteStringUTF16(const wxString& value)
		{
			const auto buffer = value.wchar_str();
			return WriteBuffer(buffer.data(), buffer.length());
		}
		bool WriteStringUTF32(const wxString& value)
		{
			wxUString buffer(value);
			return WriteBuffer(buffer.data(), buffer.length() * sizeof(wxChar32));
		}
};

//////////////////////////////////////////////////////////////////////////
template<class BaseStreamT> class KxIOStreamHelper
{
	private:
		constexpr static bool IsInputStream()
		{
			return std::is_base_of<wxInputStream, BaseStreamT>::value;
		}
		constexpr static bool IsOutputStream()
		{
			return std::is_base_of<wxOutputStream, BaseStreamT>::value;
		}

		BaseStreamT* GetThis()
		{
			return static_cast<BaseStreamT*>(this);
		}
		const BaseStreamT* GetThis() const
		{
			return static_cast<const BaseStreamT*>(this);
		}

		KxStreamBase::Offset SeekIO(KxStreamBase::Offset offset, wxSeekMode mode)
		{
			if constexpr(IsInputStream())
			{
				return GetThis()->SeekI(offset, mode) != wxInvalidOffset;
			}
			else if constexpr(IsOutputStream())
			{
				return GetThis()->SeekO(offset, mode) != wxInvalidOffset;
			}
			else
			{
				static_assert(false, "Unknown stream type");
			}
		}
		KxStreamBase::Offset TellIO() const
		{
			if constexpr(IsInputStream())
			{
				return GetThis()->TellI();
			}
			else if constexpr(IsOutputStream())
			{
				return GetThis()->TellO();
			}
			else
			{
				static_assert(false, "Unknown stream type");
			}
		}

	public:
		virtual ~KxIOStreamHelper() = default;

	public:
		bool Rewind()
		{
			return SeekIO(0, wxSeekMode::wxFromStart) != KxStreamBase::InvalidOffset;
		}
		bool SkipToEnd()
		{
			return SeekIO(0, wxSeekMode::wxFromEnd) != KxStreamBase::InvalidOffset;
		}
		
		bool Skip(KxStreamBase::Offset offset)
		{
			return SeekIO(offset, wxSeekMode::wxFromCurrent) != KxStreamBase::InvalidOffset;
		}
		template<class... Types> bool Skip()
		{
			static_assert(sizeof...(Types) != 0, "KxIOStreamHelper::Skip<Types...>: Skipping 0 bytes is not allowed");

			return SeekIO(KxUtility::SizeOfParameterPack<Types...>(), wxSeekMode::wxFromCurrent) != KxStreamBase::InvalidOffset;
		}

		bool SeekFromStart(KxStreamBase::Offset offset)
		{
			return SeekIO(offset, wxSeekMode::wxFromStart) != KxStreamBase::InvalidOffset;
		}
		bool SeekFromEnd(KxStreamBase::Offset offset)
		{
			return SeekIO(offset, wxSeekMode::wxFromEnd) != KxStreamBase::InvalidOffset;
		}

		KxStreamBase::Offset Tell() const
		{
			return TellIO();
		}
		KxStreamBase::Offset Seek(KxStreamBase::Offset offset, KxStreamBase::SeekMode mode = KxStreamBase::SeekMode::FromCurrent)
		{
			return SeekIO(offset, static_cast<wxSeekMode>(mode));
		}
};

//////////////////////////////////////////////////////////////////////////
template<class BaseStreamT> class KxIOStreamWrapper: public BaseStreamT, public KxIOStreamHelper<KxIOStreamWrapper<BaseStreamT>>
{
	public:
		template<class... Args> KxIOStreamWrapper(Args&&... args)
			:BaseStreamT(std::forward<Args>(args)...)
		{
		}
		virtual ~KxIOStreamWrapper() = default;
};
