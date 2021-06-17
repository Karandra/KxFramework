#include "KxfPCH.h"
#include "AndroidLocalizationPackage.h"
#include "kxf/Serialization/XML.h"

namespace kxf
{
	bool AndroidLocalizationPackage::DoLoadXML(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme)
	{
		if (loadingScheme.ExtractConsecutive(LoadingScheme::CONSECUTIVE_MASK) == LoadingScheme::Replace)
		{
			m_Items.clear();
		}

		if (XMLNode resourcesNode = xml.QueryElement("resources"))
		{
			m_Items.reserve(resourcesNode.GetChildrenCount());

			size_t count = 0;
			for (XMLNode itemNode: resourcesNode.EnumChildElements())
			{
				auto AddItem = [&](ResourceID id, LocalizationItem item)
				{
					if (item)
					{
						if (auto maxLength = itemNode.QueryAttributeInt("maxLength"))
						{
							item.SetMaxLength(*maxLength);
						}
						if (auto comment = itemNode.QueryAttribute("comment"))
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

				FlagSet<LocalizationItemFlag> flags;
				flags.Mod(LocalizationItemFlag::Translatable, itemNode.GetAttributeBool("translatable", true));

				const String itemName = itemNode.GetName();
				if (itemName == "string")
				{
					AddItem(itemNode.GetAttribute("name"), LocalizationItem(*this, itemNode.GetValue(), flags));
				}
				else if (itemName == "string-array")
				{
					LocalizationItem::TMultipleItems items;
					items.reserve(itemNode.GetChildrenCount());

					for (XMLNode node: itemNode.EnumChildElements("item"))
					{
						if (items.emplace_back(node.GetValue()).IsEmpty())
						{
							items.pop_back();
						}
					}
					AddItem(itemNode.GetAttribute("name"), LocalizationItem(*this, items, flags));
				}
				else if (itemName == "plurals")
				{
					LocalizationItem::TPlurals plurals;
					for (XMLNode node : itemNode.EnumChildElements("item"))
					{
						const String name = node.GetAttribute("quantity");
						if (name == "one")
						{
							plurals.emplace(LocalizationItemQuantity::One, node.GetValue());
							return true;
						}
						else if (name == "few")
						{
							plurals.emplace(LocalizationItemQuantity::Few, node.GetValue());
							return true;
						}
						else if (name == "many")
						{
							plurals.emplace(LocalizationItemQuantity::Many, node.GetValue());
							return true;
						}
						else if (name == "other")
						{
							plurals.emplace(LocalizationItemQuantity::Other, node.GetValue());
							return true;
						}
						return false;
					}
					AddItem(itemNode.GetAttribute("name"), LocalizationItem(*this, plurals, flags));
				}
				return true;
			}
			return count != 0;
		}
		return false;
	}

	Enumerator<String> AndroidLocalizationPackage::EnumFileExtensions() const
	{
		return [done = false]() mutable -> std::optional<String>
		{
			if (!done)
			{
				done = true;
				return "xml";
			}
			return {};
		};
	}
}
