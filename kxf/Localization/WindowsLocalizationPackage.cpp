#include "KxfPCH.h"
#include "WindowsLocalizationPackage.h"
#include "kxf/Serialization/XML.h"

namespace kxf
{
	bool WindowsLocalizationPackage::DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme)
	{
		if (loadingScheme.ExtractConsecutive(LoadingScheme::CONSECUTIVE_MASK) == LoadingScheme::Replace)
		{
			m_Items.clear();
		}

		if (XMLNode rootNode = xml.QueryElement("root"))
		{
			m_Items.reserve(rootNode.GetChildrenCount());

			size_t count = 0;
			for (XMLNode itemNode: rootNode.EnumChildElements("data"))
			{
				auto AddItem = [&](ResourceID id, LocalizationItem item)
				{
					if (item)
					{
						if (auto comment = itemNode.GetFirstChildElement("comment").QueryValue())
						{
							item.SetComment(std::move(*comment));
						}

						if (loadingScheme.Contains(LoadingScheme::OverwriteExisting))
						{
							m_Items.insert_or_assign(std::move(id), std::move(item));
							count++;
						}
						else
						{
							if (m_Items.emplace(std::move(id), std::move(item)).second)
							{
								count++;
							}
						}
						return true;
					}
					return false;
				};

				AddItem(itemNode.GetAttribute("name"), LocalizationItem(*this, itemNode.GetFirstChildElement("value").GetValue(), LocalizationItemFlag::Translatable));
			}
			return count != 0;
		}
		return false;
	}

	Enumerator<String> WindowsLocalizationPackage::EnumFileExtensions() const
	{
		return [count = 0]() mutable -> std::optional<String>
		{
			switch (count)
			{
				case 0:
				{
					count++;
					return "resx";
				}
				case 1:
				{
					count++;
					return "resw";
				}
			};
			return {};
		};
	}
}
