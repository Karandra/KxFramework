#pragma once
#include "KxFramework/KxFramework.h"

namespace KxVersionInternal
{
	struct VersionPart
	{
		public:
			int m_Numeric = -1;
			wxChar m_String[9] = {wxS('\0')};

		public:
			VersionPart() {}
			VersionPart(int n, wxChar* s = NULL, size_t count = 0)
				:m_Numeric(n)
			{
				if (s)
				{
					SetString(s, count);
				}
			}

		public:
			bool IsOk() const
			{
				return m_Numeric >= 0;
			}
			bool IsStringOK() const
			{
				return ::wcsnlen_s(m_String, ARRAYSIZE(m_String)) != 0;
			}
			void SetString(wxChar* s, size_t count = 0)
			{
				const size_t max = ARRAYSIZE(m_String) - 1;
				wcsncpy_s(m_String, s, (count == 0 || count > max) ? max : count);
				m_String[max] = wxS('\0');
			}
	};

	class StringPartAdapter
	{
		private:
			std::basic_string_view<wxChar, std::char_traits<wxChar>> m_Str;

		public:
			StringPartAdapter(const wxChar* s)
				:m_Str(s)
			{
			}

		public:
			bool empty() const
			{
				return m_Str.empty();
			}

			bool operator==(const StringPartAdapter& other) const;
			bool operator!=(const StringPartAdapter& other) const;
			bool operator<(const StringPartAdapter& other) const;
			bool operator<=(const StringPartAdapter& other) const;
			bool operator>(const StringPartAdapter& other) const;
			bool operator>=(const StringPartAdapter& other) const;
	};

	constexpr const size_t DefaultVersionSize = 8;
	using DefaultVersionT = std::array<KxVersionInternal::VersionPart, DefaultVersionSize>;
}

enum KxVersionType
{
	KxVERSION_INVALID = -1,

	KxVERSION_DEFAULT,
	KxVERSION_DATETIME,
	KxVERSION_INTEGER,
};

class KX_API KxVersion
{
	private:
		enum class Cmp
		{
			INV = -2,

			LT = -1,
			EQ = 0,
			GT = 1
		};
		template<class T> static Cmp CompareValues(const T& v1, const T& v2)
		{
			if (v1 > v2)
			{
				return Cmp::GT;
			}
			else if (v1 < v2)
			{
				return Cmp::LT;
			}
			return Cmp::EQ;
		}
		
		static Cmp Compare(const KxVersion& left, const KxVersion& right);

	private:
		using DefaultT = KxVersionInternal::DefaultVersionT;
		using DateTimeT = wxDateTime;
		using IntegerT = int;

		std::variant<DefaultT, DateTimeT, IntegerT> m_Value;
		KxVersionType m_Type = KxVERSION_INVALID;
		size_t m_Count = 0;

	private:
		KxVersionType Create(const wxString& source, KxVersionType type);

		/* Default */
		const DefaultT& GetDefault() const
		{
			return std::get<KxVERSION_DEFAULT>(m_Value);
		}
		DefaultT& SetDefault(const DefaultT& v = DefaultT())
		{
			m_Value = v;
			return std::get<KxVERSION_DEFAULT>(m_Value);
		}
		bool ParseDefault(const wxString& source);
		wxString FormatDefault() const;

		int GetNumericPart(size_t i) const
		{
			const auto& v = GetDefault();
			return v[i].IsOk() ? v[i].m_Numeric : 0;
		}
		const wchar_t* GetStringPart(size_t i) const
		{
			const auto& v = GetDefault();
			return v[i].IsOk() && v[i].IsStringOK() ? v[i].m_String : NULL;
		}
		const KxVersionInternal::StringPartAdapter GetStringPartView(size_t i) const
		{
			return GetStringPart(i);
		}

		/* DateTime */
		const DateTimeT& GetDateTime() const
		{
			return std::get<KxVERSION_DATETIME>(m_Value);
		}
		DateTimeT& SetDateTime(const DateTimeT& v = wxDefaultDateTime)
		{
			m_Value = v;
			return std::get<KxVERSION_DATETIME>(m_Value);
		}
		bool HasTimePart() const;
		bool ParseDateTime(const wxString& source);
		wxString FormatDateTime() const;

		/* Integer */
		const IntegerT& GetInteger() const
		{
			return std::get<KxVERSION_INTEGER>(m_Value);
		}
		IntegerT& SetInteger(const IntegerT& v = -1)
		{
			m_Value = v;
			return std::get<KxVERSION_INTEGER>(m_Value);
		}
		bool ParseInteger(const wxString& source);
		bool CheckInteger(const IntegerT& v) const
		{
			return v >= 0;
		}
		wxString FormatInteger() const;

	public:
		KxVersion() {}
		KxVersion(const wxString& s, KxVersionType type = KxVERSION_INVALID);
		KxVersion(const char* s, KxVersionType type = KxVERSION_INVALID)
			:KxVersion(wxString(s), type)
		{
		}
		KxVersion(const wchar_t* s, KxVersionType type = KxVERSION_INVALID)
			:KxVersion(wxString(s), type)
		{
		}
		KxVersion(const DateTimeT& t);
		KxVersion(const IntegerT& i);
		KxVersion(const wxVersionInfo& versionInfo);
		~KxVersion();

	public:
		bool IsOK() const
		{
			return m_Type != KxVERSION_INVALID;
		}
		bool IsDefault() const
		{
			return m_Type == KxVERSION_DEFAULT;
		}
		bool IsDateTime() const
		{
			return m_Type == KxVERSION_DATETIME;
		}
		bool IsInteger() const
		{
			return m_Type == KxVERSION_INTEGER;
		}
		
		KxVersionType GetType() const
		{
			return m_Type;
		}
		int GetPartsCount() const;
		
		wxString ToString() const;
		operator wxString() const
		{
			return ToString();
		}
		wxVersionInfo ToWxVersionInfo(const wxString& name = {}, const wxString& description = {}, const wxString& copyright = {}) const;
		DateTimeT ToDateTime() const
		{
			return IsDateTime() ? GetDateTime() : wxDefaultDateTime;
		}
		IntegerT ToInteger() const
		{
			return IsInteger() ? GetInteger() : -1;
		}

	public:
		bool operator==(const KxVersion& other) const;
		bool operator!=(const KxVersion& other) const;
		bool operator<(const KxVersion& other) const;
		bool operator<=(const KxVersion& other) const;
		bool operator>(const KxVersion& other) const;
		bool operator>=(const KxVersion& other) const;
};

extern KX_API const KxVersion KxNullVersion;
