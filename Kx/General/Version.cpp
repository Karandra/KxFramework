#include "KxStdAfx.h"
#include "Version.h"
#include <wx/versioninfo.h>

namespace
{
	template<KxFramework::VersionType type, class T>
	auto&& GetItem(T&& items)
	{
		using namespace KxFramework;

		return std::get<ToInt(type)>(items);
	}
}

namespace KxFramework
{
	bool Version::Parse(const wxString& source, VersionType type)
	{
		switch (type)
		{
			case VersionType::Default:
			{
				return Private::Version::Parse(source, GetItem<VersionType::Default>(m_Value), m_ComponentCount);
			}
			case VersionType::DateTime:
			{
				return Private::Version::Parse(source, GetItem<VersionType::DateTime>(m_Value));
			}
		};
		return false;
	}
	VersionType Version::ParseUnknown(const wxString& source)
	{
		// Order is important here
		if (Private::Version::Parse(source, GetItem<VersionType::DateTime>(m_Value)))
		{
			return VersionType::DateTime;
		}
		else if (Private::Version::Parse(source, GetItem<VersionType::Default>(m_Value), m_ComponentCount))
		{
			return VersionType::Default;
		}
		return VersionType::None;
	}

	Version::Version(const wxVersionInfo& versionInfo)
		:m_Type(VersionType::Default), m_ComponentCount(3)
	{
		auto& version = AssignDefault();
		version[0].m_Numeric = versionInfo.GetMajor();
		version[1].m_Numeric = versionInfo.GetMinor();
		version[2].m_Numeric = versionInfo.GetMicro();
	}

	size_t Version::GetComponentCount() const
	{
		switch (m_Type)
		{
			case VersionType::Default:
			{
				return m_ComponentCount;
			}
			case VersionType::DateTime:
			{
				return Private::Version::HasTimePart(GetItem<VersionType::DateTime>(m_Value)) ? 2 : 1;
			}
		};
		return 0;
	}
	wxString Version::ToString() const
	{
		switch (m_Type)
		{
			case VersionType::Default:
			{
				return Private::Version::Format(GetItem<VersionType::Default>(m_Value), m_ComponentCount);
			}
			case VersionType::DateTime:
			{
				return Private::Version::Format(GetItem<VersionType::DateTime>(m_Value));
			}
		};
		return {};
	}
	wxVersionInfo Version::ToWxVersionInfo(const wxString& name, const wxString& description, const wxString& copyright) const
	{
		const auto& items = GetItem<VersionType::Default>(m_Value);

		auto GetPart = [&](size_t index)
		{
			int value = items[index].m_Numeric;
			return value >= 0 ? value : 0;
		};
		return wxVersionInfo(name, GetPart(0), GetPart(1), GetPart(2), description, copyright);
	}
	wxDateTime Version::ToDateTime() const
	{
		if (m_Type == VersionType::DateTime)
		{
			return GetItem<VersionType::DateTime>(m_Value);
		}
		return {};
	}

	bool Version::operator==(const Version& other) const
	{
		using namespace Private::Version;

		return Compare(*this, other) == Cmp::EQ;
	}
	bool Version::operator<(const Version& other) const
	{
		using namespace Private::Version;

		return Compare(*this, other) == Cmp::LT;
	}
	bool Version::operator>(const Version& other) const
	{
		using namespace Private::Version;

		return Compare(*this, other) == Cmp::GT;
	}
}
