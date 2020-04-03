#pragma once
#include "Common.h"
#include "Private/VersionImpl.h"
class wxVersionInfo;

namespace KxFramework
{
	class KX_API Version final
	{
		friend Private::Version::Cmp Private::Version::Compare(const KxFramework::Version& left, const KxFramework::Version& right);

		public:
			using TDefaultItem = Private::Version::DefaultFormat::Array;

		private:
			std::variant<TDefaultItem, wxDateTime> m_Value;
			VersionType m_Type = VersionType::None;
			size_t m_ComponentCount = 0;

		private:
			bool Parse(const wxString& source, VersionType type);
			VersionType ParseUnknown(const wxString& source);

			TDefaultItem& AssignDefault(TDefaultItem value = {})
			{
				m_Value = std::move(value);
				return std::get<ToInt(VersionType::Default)>(m_Value);
			}
			wxDateTime& AssignDateTime(wxDateTime value = {})
			{
				m_Value = std::move(value);
				return std::get<ToInt(VersionType::DateTime)>(m_Value);
			}

		public:
			Version() = default;
			Version(const wxString& source, VersionType type = VersionType::None)
			{
				if (type == VersionType::None)
				{
					m_Type = ParseUnknown(source);
				}
				else if (Parse(source, type))
				{
					m_Type = type;
				}
			}
			Version(const char* data, VersionType type = VersionType::None)
				:Version(wxString(data), type)
			{
			}
			Version(const wchar_t* data, VersionType type = VersionType::None)
				:Version(wxString(data), type)
			{
			}
			Version(const wxDateTime& dateTime)
			{
				if (dateTime.IsValid())
				{
					m_Value = dateTime;
					m_Type = VersionType::DateTime;
					m_ComponentCount = GetComponentCount();
				}
			}
			Version(const wxVersionInfo& versionInfo);

		public:
			bool IsEmpty() const
			{
				return m_Type == VersionType::None || m_ComponentCount == 0 || m_Value.valueless_by_exception();
			}
			VersionType GetType() const
			{
				return m_Type;
			}
			size_t GetComponentCount() const;
			
			wxString ToString() const;
			wxVersionInfo ToWxVersionInfo(const wxString& name = {}, const wxString& description = {}, const wxString& copyright = {}) const;
			wxDateTime ToDateTime() const;

		public:
			explicit operator bool() const noexcept
			{
				return !IsEmpty();
			}
			bool operator!() const noexcept
			{
				return IsEmpty();
			}

			bool operator==(const Version& other) const;
			bool operator!=(const Version& other) const
			{
				return !(*this == other);
			}
			bool operator<(const Version& other) const;
			bool operator<=(const Version& other) const
			{
				return (*this < other) || (*this == other);
			}
			bool operator>(const Version& other) const;
			bool operator>=(const Version& other) const
			{
				return (*this > other) || (*this == other);
			}

			operator wxString() const
			{
				return ToString();
			}
	};
}
