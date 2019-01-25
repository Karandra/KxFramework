/*
Copyright © 2018-2019 Kerber. All rights reserved.

You should have received a copy of the GNU LGPL v3
along with KxFramework. If not, see https://www.gnu.org/licenses/lgpl-3.0.html.
*/
#pragma once
#include "KxFramework/KxFramework.h"
#include "KxFramework/KxFormatTraits.h"
#include <string>
#include <string_view>

class KX_API KxFormatBase
{
	private:
		wxString m_String;
		size_t m_CurrentArgument = 1;
		bool m_IsUpperCase = false;

	protected:
		const wxString& GetString() const
		{
			return m_String;
		}
		wxString& GetString()
		{
			return m_String;
		}

		bool FindAndReplace(const wxString& string, const std::wstring_view& index, size_t startAt = 0);
		void FindAndReplace(const wxString& string, size_t index, size_t startAt = 0);
		void FindCurrentAndReplace(const wxString& string);

		void FormatString(const wxString& arg, int fieldWidth, wxUniChar fillChar);
		void FormatChar(wxUniChar arg, int fieldWidth, wxUniChar fillChar);
		void FormatBool(bool arg, int fieldWidth, wxUniChar fillChar);
		void FormatDouble(double arg, int precision, int fieldWidth, wxUniChar format, wxUniChar fillChar);
		void FormatPointer(const void* arg, int fieldWidth, wxUniChar fillChar);

		template<class T> static wxString FormatIntWithBase(T value, int base = 10, bool upper = false)
		{
			static const wxChar digitsL[] = wxS("0123456789abcdefghijklmnopqrstuvwxyz");
			static const wxChar digitsU[] = wxS("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
			const wxChar* digits = upper ? digitsU : digitsL;

			wxString result;
			if (base >= 2 && base <= 36)
			{
				do
				{
					result = digits[value % base] + result;
					value /= base;
				}
				while (value);

				if constexpr(std::is_signed<T>::value)
				{
					if (value < 0)
					{
						result = wxS('-') + result;
					}
				}
			}
			return result;
		};

	public:
		KxFormatBase(const wxString& format)
			:m_String(format)
		{
		}
		virtual ~KxFormatBase() = default;

	public:
		wxString ToString() const
		{
			return m_String;
		}
		operator wxString() const
		{
			return m_String;
		}

		size_t GetCurrentArgumentIndex() const
		{
			return m_CurrentArgument;
		}
		void SetCurrentArgumentIndex(size_t index)
		{
			m_CurrentArgument = index;
		}
		
		bool IsUpperCase() const
		{
			return m_IsUpperCase;
		}
		KxFormatBase& UpperCase(bool value = true)
		{
			m_IsUpperCase = value;
			return *this;
		}
		KxFormatBase& LowerCase(bool value = true)
		{
			m_IsUpperCase = !value;
			return *this;
		}
};

template<class FmtTraits = KxFormatTraits> class KxFormat: public KxFormatBase
{
	public:
		using FormatTraits = typename FmtTraits;
		template<class T> using TypeTraits = KxFormatTypeTraits<T>;

	protected:
		template<class T> void FormatInt(T&& arg, int fieldWidth, int base, wxUniChar fillChar)
		{
			FormatString(FormatIntWithBase(arg, base, IsUpperCase()), fieldWidth, fillChar);
		}

	public:
		KxFormat(const wxString& format)
			:KxFormatBase(format)
		{
		}

	public:
		KxFormat& UpperCase(bool value = true)
		{
			KxFormatBase::UpperCase(value);
			return *this;
		}
		KxFormat& LowerCase(bool value = true)
		{
			KxFormatBase::LowerCase(value);
			return *this;
		}

	public:
		template<class T> typename std::enable_if<TypeTraits<T>::FmtString(), KxFormat&>::type
		operator()(const T& arg,
				   int fieldWidth = FormatTraits::StringFiledWidth(),
				   wxUniChar fillChar = FormatTraits::StringFillChar())
		{
			TypeTraits<T> trait;
			if constexpr(trait.IsBool())
			{
				FormatBool(arg, fieldWidth, fillChar);
			}
			else if constexpr(trait.IsConstructibleToWxUniChar())
			{
				FormatChar(arg, fieldWidth, fillChar);
			}
			else if constexpr(trait.IsConstructibleToWxString())
			{
				FormatString(arg, fieldWidth, fillChar);
			}
			else if constexpr(trait.IsStringView() || trait.IsWStringView())
			{
				FormatString(wxString(arg.data(), arg.size()), fieldWidth, fillChar);
			}
			else
			{
				static_assert(false, "KxFormat: unsupported type for string formatting");
			}
			return *this;
		}
		
		template<class T> typename std::enable_if<TypeTraits<T>::FmtInteger(), KxFormat&>::type
		operator()(T arg,
				   int fieldWidth = FormatTraits::IntFiledWidth(),
				   int base = FormatTraits::IntBase(),
				   wxUniChar fillChar = FormatTraits::IntFillChar())
		{
			if constexpr(TypeTraits<T>().IsEnum())
			{
				FormatInt(static_cast<std::underlying_type_t<T>>(arg), fieldWidth, base, fillChar);
			}
			else
			{
				FormatInt(arg, fieldWidth, base, fillChar);
			}
			return *this;
		}

		template<class T> typename std::enable_if<TypeTraits<T>::FmtPointer(), KxFormat&>::type
		operator()(T arg,
				   int fieldWidth = FormatTraits::PtrFiledWidth(),
				   wxUniChar fillChar = FormatTraits::PtrFillChar())
		{
			FormatPointer(arg, fieldWidth, fillChar);
			return *this;
		}

		template<class T> typename std::enable_if<TypeTraits<T>::FmtFloat(), KxFormat&>::type
		operator()(T arg,
				   int precision = FormatTraits::FloatPrecision(),
				   int fieldWidth = FormatTraits::FloatFiledWidth(),
				   wxUniChar format = FormatTraits::FloatFormat(),
				   wxUniChar fillChar = FormatTraits::FloatFillChar())
		{
			FormatDouble(arg, precision, fieldWidth, format, fillChar);
			return *this;
		}
};
