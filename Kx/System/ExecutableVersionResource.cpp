#include "stdafx.h"
#include "ExecutableVersionResource.h"
#include "Kx/General/StringFormater.h"

namespace
{
	struct VerQueryValueLangInfo final
	{
		WORD Language;
		WORD CodePage;
	};

	KxFramework::Version UnpackNumericVersion(DWORD mostSignificant, DWORD leastSignificant)
	{
		return {static_cast<int>(HIWORD(mostSignificant)), static_cast<int>(LOWORD(mostSignificant)), static_cast<int>(HIWORD(leastSignificant)), static_cast<int>(LOWORD(leastSignificant))};
	}
	VS_FIXEDFILEINFO* GetFixedFileVersionInfo(const uint8_t* buffer) noexcept
	{
		void* fixedInfo = nullptr;
		UINT size = 0;
		if (::VerQueryValueW(buffer, L"\\", &fixedInfo, &size) && fixedInfo)
		{
			return reinterpret_cast<VS_FIXEDFILEINFO*>(fixedInfo);
		}
		return nullptr;
	}
}

namespace KxFramework
{
	void ExecutableVersionResource::Load(const FSPath& filePath)
	{
		String filePathString = filePath.GetFullPathWithNS(FSPathNamespace::Win32File);

		DWORD structSize = ::GetFileVersionInfoSizeW(filePathString.wc_str(), nullptr);
		if (structSize != 0)
		{
			m_Buffer.resize(structSize);
			if (::GetFileVersionInfoW(filePathString.wc_str(), 0, structSize, m_Buffer.data()))
			{
				void* fixedInfo = nullptr;
				UINT fixedSize = 0;
				if (::VerQueryValueW(m_Buffer.data(), L"\\", &fixedInfo, &fixedSize))
				{
					// Language
					void* langInfo = nullptr;
					UINT langInfoSize = 0;
					if (::VerQueryValueW(m_Buffer.data(), L"\\VarFileInfo\\Translation", &langInfo, &langInfoSize))
					{
						auto* langInfoHeader = reinterpret_cast<VerQueryValueLangInfo*>(langInfo);
						m_LangID = langInfoHeader->Language;
						m_CodePage = langInfoHeader->CodePage;
					}
				}
			}
		}
	}

	bool ExecutableVersionResource::IsNull() const noexcept
	{
		return m_Buffer.empty() || m_LangID == 0;
	}

	Version ExecutableVersionResource::GetFileVersion() const
	{
		if (const VS_FIXEDFILEINFO* fixedInfo = GetFixedFileVersionInfo(m_Buffer.data()))
		{
			return UnpackNumericVersion(fixedInfo->dwFileVersionMS, fixedInfo->dwFileVersionLS);
		}
		return {};
	}
	Version ExecutableVersionResource::GetProductVersion() const
	{
		if (const VS_FIXEDFILEINFO* fixedInfo = GetFixedFileVersionInfo(m_Buffer.data()))
		{
			return UnpackNumericVersion(fixedInfo->dwProductVersionMS, fixedInfo->dwProductVersionLS);
		}
		return {};
	}
	wxDateTime ExecutableVersionResource::GetFileDate() const
	{
		if (const VS_FIXEDFILEINFO* fixedInfo = GetFixedFileVersionInfo(m_Buffer.data()))
		{
			FILETIME time = {};
			time.dwHighDateTime = fixedInfo->dwFileDateLS;
			time.dwLowDateTime = fixedInfo->dwFileDateMS;

			// TODO: Check if the time is in UTC or local
			SYSTEMTIME systemTime = {};
			if (::FileTimeToSystemTime(&time, &systemTime))
			{
				return wxDateTime().SetFromMSWSysTime(systemTime);
			}
		}
		return {};
	}
	String ExecutableVersionResource::GetStringField(ExecutableVersionField fieldName) const
	{
		if (!IsNull())
		{
			String queryTemplate = wxString::Format(wxS("\\StringFileInfo\\%04x%04x"), static_cast<int>(m_LangID), static_cast<int>(m_CodePage));
			auto GetField = [&](const wchar_t* fieldName) -> String
			{
				String query = String::Format(wxS("%1\\%2"), queryTemplate, fieldName);

				UINT size = 0;
				LPWSTR stringInfo = nullptr;
				if (::VerQueryValueW(m_Buffer.data(), query.wc_str(), reinterpret_cast<void**>(&stringInfo), &size))
				{
					return String(stringInfo, size);
				}
				return {};
			};

			switch (fieldName)
			{
				case ExecutableVersionField::FileVersion:
				{
					return GetField(L"FileVersion");
				}
				case ExecutableVersionField::ProductVersion:
				{
					return GetField(L"ProductVersion");
				}
				case ExecutableVersionField::FileDescription:
				{
					return GetField(L"FileDescription");
				}
				case ExecutableVersionField::InternalName:
				{
					return GetField(L"InternalName");
				}
				case ExecutableVersionField::OriginalFileName:
				{
					return GetField(L"OriginalFilename");
				}
				case ExecutableVersionField::CompanyName:
				{
					return GetField(L"CompanyName");
				}
				case ExecutableVersionField::ProductName:
				{
					return GetField(L"ProductName");
				}
				case ExecutableVersionField::LegalCopyright:
				{
					return GetField(L"LegalCopyright");
				}
				case ExecutableVersionField::LegalTrademarks:
				{
					return GetField(L"LegalTrademarks");
				}
				case ExecutableVersionField::PrivateBuild:
				{
					return GetField(L"PrivateBuild");
				}
				case ExecutableVersionField::SpecialBuild:
				{
					return GetField(L"SpecialBuild");
				}
			};
		}
		return {};
	}

	String ExecutableVersionResource::GetAnyVersion() const
	{
		String result = GetAnyProductVersion();
		if (!result.IsEmpty())
		{
			return result;
		}
		return GetAnyFileVersion();
	}
	String ExecutableVersionResource::GetAnyFileVersion() const
	{
		String result = GetStringField(ExecutableVersionField::FileVersion);
		if (!result.IsEmpty())
		{
			return result;
		}
		return GetFileVersion();
	}
	String ExecutableVersionResource::GetAnyProductVersion() const
	{
		String result = GetStringField(ExecutableVersionField::ProductVersion);
		if (!result.IsEmpty())
		{
			return result;
		}
		return GetProductVersion();
	}
}
