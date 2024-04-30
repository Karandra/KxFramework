#include "KxfPCH.h"
#include "QtLocalizationPackage.h"
#include "kxf/Serialization/XML.h"

namespace kxf
{
	bool QtLocalizationPackage::DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme)
	{
		if (loadingScheme.ExtractConsecutive(LoadingScheme::CONSECUTIVE_MASK) == LoadingScheme::Replace)
		{
			m_Items.clear();
		}

		if (XMLNode tsNode = xml.QueryElement("TS"))
		{
			m_Locale = tsNode.GetAttribute("language");
			m_Version = tsNode.GetAttribute("version");

			size_t count = 0;
			for (const XMLNode& contextNode: tsNode.EnumChildElements("context"))
			{
				String name = contextNode.GetFirstChildElement("name").GetValue();
				for (const XMLNode& messageNode: contextNode.EnumChildElements("message"))
				{
					auto AddItem = [&](ResourceID id, LocalizationItem item)
					{
						if (item)
						{
							if (auto locationNode = messageNode.GetFirstChildElement("location"))
							{
								item.SetComment(Format("[Context={}][FileName={}][Line={}]", name, locationNode.GetAttribute("filename"), locationNode.GetAttribute("line")));
							}

							if (loadingScheme.Contains(LoadingScheme::OverwriteExisting))
							{
								m_Items.insert_or_assign(std::move(id), std::move(item));
								count++;
							}
							else if (m_Items.emplace(std::move(id), std::move(item)).second)
							{
								count++;
							}
							return true;
						}
						return false;
					};

					AddItem(messageNode.GetFirstChildElement("source").GetValue(), LocalizationItem(*this, messageNode.GetFirstChildElement("translation").GetValue(), LocalizationItemFlag::Translatable));
					return true;
				}
				return true;
			}
			return count != 0;
		}
		return false;
	}

	Enumerator<String> QtLocalizationPackage::EnumFileExtensions() const
	{
		return [done = false]() mutable -> std::optional<String>
		{
			if (!done)
			{
				done = true;
				return "ts";
			}
			return {};
		};
	}
}
