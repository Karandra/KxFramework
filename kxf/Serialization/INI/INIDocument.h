#pragma once
#include "../Common.h"
#include "../XDocument.h"
#include "kxf/General/Version.h"
#include "kxf/IO/IStream.h"
#include <map>

namespace SimpleINI
{
	#include "SimpleIni.h"
}

namespace kxf
{
	class KX_API INIDocument: private XDocument::XNode<INIDocument>
	{
		friend class XDocument::XNode<INIDocument>;

		public:
			static String GetLibraryName();
			static Version GetLibraryVersion();

		private:
			using TDocument = SimpleINI::CSimpleIniA;

		private:
			TDocument m_Document;

		private:
			void Init();
			void DoLoad(const char* ini, size_t length);
			void DoUnload();

		protected:
			std::optional<String> DoGetValue() const override;
			bool DoSetValue(const String& value, WriteEmpty writeEmpty, AsCDATA asCDATA) override;

			std::optional<String> DoGetAttribute(const String& name) const override;
			bool DoSetAttribute(const String& name, const String& value, WriteEmpty writeEmpty) override;

			std::optional<String> IniGetValue(const String& sectionName, const String& keyName) const;
			bool IniSetValue(const String& sectionName, const String& keyName, const String& value, WriteEmpty writeEmpty = WriteEmpty::Always);

		private:
			INIDocument(const std::string_view& ini)
				:INIDocument()
			{
				if (!ini.empty())
				{
					DoLoad(ini.data(), ini.length());
				}
			}

		public:
			INIDocument()
				:m_Document(true, false, false)
			{
				Init();
			}
			INIDocument(const String& ini)
				:INIDocument()
			{
				if (!ini.IsEmpty())
				{
					Load(ini);
				}
			}
			INIDocument(IInputStream& stream)
				:INIDocument()
			{
				Load(stream);
			}
			INIDocument(INIDocument&&) = delete;
			INIDocument(const INIDocument&) = delete;

		public:
			// General
			bool IsNull() const override
			{
				return m_Document.IsEmpty();
			}

			bool Load(const String& ini);
			bool Load(IInputStream& stream);
			bool Save(IOutputStream& stream) const;
			String Save() const;
			INIDocument Clone() const;

			// Navigation
			size_t GetSectionNames(std::function<bool(String)> func) const;
			size_t GetKeyNames(const String& sectionName, std::function<bool(String)> func) const;

			// Value
			bool HasSection(const String& sectionName) const;
			bool HasValue(const String& sectionName, const String& keyName) const;

			String GetValue(const String& sectionName, const String& keyName, const String& defaultValue = {}) const
			{
				return IniGetValue(sectionName, keyName).value_or(defaultValue);
			}
			int64_t GetValueInt(const String& sectionName, const String& keyName, int64_t defaultValue = 0) const
			{
				return GetValueIntWithBase(sectionName, keyName, 10, defaultValue);
			}
			int64_t GetValueIntWithBase(const String& sectionName, const String& keyName, int base, int64_t defaultValue = 0) const
			{
				if (auto value = IniGetValue(sectionName, keyName))
				{
					if (auto iValue = ParseInt(*value, base))
					{
						return *iValue;
					}
				}
				return defaultValue;
			}
			double GetValueFloat(const String& sectionName, const String& keyName, double defaultValue = 0.0) const
			{
				if (auto value = IniGetValue(sectionName, keyName))
				{
					if (auto iValue = ParseFloat(*value))
					{
						return *iValue;
					}
				}
				return defaultValue;
			}
			bool GetValueBool(const String& sectionName, const String& keyName, bool defaultValue = false) const
			{
				if (auto value = IniGetValue(sectionName, keyName))
				{
					if (auto bValue = ParseBool(*value))
					{
						return *bValue;
					}
				}
				return defaultValue;
			}

			bool SetValue(const String& sectionName, const String& keyName, const String& value, WriteEmpty writeEmpty = WriteEmpty::Always)
			{
				return IniSetValue(sectionName, keyName, value, writeEmpty);
			}
			bool SetValue(const String& sectionName, const String& keyName, const char* value, WriteEmpty writeEmpty = WriteEmpty::Always)
			{
				return IniSetValue(sectionName, keyName, String::FromUTF8(value), writeEmpty);
			}
			bool SetValue(const String& sectionName, const String& keyName, const wchar_t* value, WriteEmpty writeEmpty = WriteEmpty::Always)
			{
				return IniSetValue(sectionName, keyName, String(value), writeEmpty);
			}
			bool SetValue(const String& sectionName, const String& keyName, int64_t value)
			{
				return IniSetValue(sectionName, keyName, FormatInt(value), WriteEmpty::Always);
			}
			bool SetValue(const String& sectionName, const String& keyName, int value)
			{
				return IniSetValue(sectionName, keyName, FormatInt(value), WriteEmpty::Always);
			}
			bool SetValue(const String& sectionName, const String& keyName, double value, int precision = -1)
			{
				return IniSetValue(sectionName, keyName, FormatFloat(value, precision), WriteEmpty::Always);
			}
			bool SetValue(const String& sectionName, const String& keyName, float value, int precision = -1)
			{
				return IniSetValue(sectionName, keyName, FormatFloat((double)value, precision), WriteEmpty::Always);
			}
			bool SetValue(const String& sectionName, const String& keyName, bool value)
			{
				return IniSetValue(sectionName, keyName, FormatBool(value), WriteEmpty::Always);
			}

			// Deletion
			bool RemoveSection(const String& sectionName);
			bool RemoveValue(const String& sectionName, const String& keyName);

		public:
			INIDocument& operator=(const INIDocument&) = delete;
			INIDocument& operator=(INIDocument&&) = delete;
	};
}

namespace kxf
{
	template<>
	struct BinarySerializer<INIDocument> final
	{
		uint64_t Serialize(IOutputStream& stream, const INIDocument& value) const
		{
			return BinarySerializer<String>().Serialize(stream, value.Save());
		}
		uint64_t Deserialize(IInputStream& stream, INIDocument& value) const
		{
			String buffer;
			auto read = BinarySerializer<String>().Deserialize(stream, buffer);

			value.Load(buffer);
			return read;
		}
	};
}
