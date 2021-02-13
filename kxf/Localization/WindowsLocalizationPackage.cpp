#include "stdafx.h"
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

		if (XMLNode rootNode = xml.QueryElement(wxS("root")))
		{
			m_Items.reserve(rootNode.GetChildrenCount());

			size_t count = 0;
			for (XMLNode itemNode: rootNode.EnumChildElements(wxS("data")))
			{
				auto AddItem = [&](ResourceID id, LocalizationItem item)
				{
					if (item)
					{
						if (auto comment = itemNode.GetFirstChildElement(wxS("comment")).QueryValue())
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

				AddItem(itemNode.GetAttribute(wxS("name")), LocalizationItem(*this, itemNode.GetFirstChildElement(wxS("value")).GetValue(), LocalizationItemFlag::Translatable));
			}
			return count != 0;
		}
		return false;
	}
}
