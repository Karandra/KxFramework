#include "stdafx.h"
#include "AndroidLocalizationPackage.h"
#include "Private/LocalizationResources.h"
#include "kxf/System/DynamicLibrary.h"
#include "kxf/Serialization/XML.h"

namespace kxf
{
	bool AndroidLocalizationPackage::DoLoad(const XMLDocument& xml, FlagSet<LoadingScheme> loadingScheme)
	{
		if (loadingScheme.ExtractConsecutive(LoadingScheme::CONSECUTIVE_MASK) == LoadingScheme::Replace)
		{
			m_Items.clear();
		}

		if (XMLNode resourcesNode = xml.QueryElement(wxS("resources")))
		{
			size_t count = 0;
			resourcesNode.EnumChildElements([&](XMLNode itemNode)
			{
				auto AddItem = [&](ResourceID id, LocalizationItem item)
				{
					if (item)
					{
						if (auto maxLength = itemNode.QueryAttributeInt(wxS("maxLength")))
						{
							item.SetMaxLength(*maxLength);
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
				flags.Mod(LocalizationItemFlag::Translatable, itemNode.GetAttributeBool(wxS("translatable"), true));

				const String itemName = itemNode.GetName();
				if (itemName == wxS("string"))
				{
					AddItem(itemNode.GetAttribute(wxS("name")), LocalizationItem(itemNode.GetValue(), flags));
				}
				else if (itemName == wxS("string-array"))
				{
					LocalizationItem::TMultipleItems items;
					items.reserve(itemNode.GetChildrenCount());

					itemNode.EnumChildElements([&](XMLNode node)
					{
						if (items.emplace_back(node.GetValue()).IsEmpty())
						{
							items.pop_back();
						}
						return true;
					}, wxS("item"));
					AddItem(itemNode.GetAttribute(wxS("name")), LocalizationItem(items, flags));
				}
				else if (itemName == wxS("plurals"))
				{
					LocalizationItem::TPlurals plurals;
					itemNode.EnumChildElements([&](XMLNode node)
					{
						const String name = node.GetAttribute(wxS("quantity"));
						if (name == wxS("one"))
						{
							plurals.emplace(LocalizationItemQuantity::One, node.GetValue());
							return true;
						}
						else if (name == wxS("few"))
						{
							plurals.emplace(LocalizationItemQuantity::Few, node.GetValue());
							return true;
						}
						else if (name == wxS("many"))
						{
							plurals.emplace(LocalizationItemQuantity::Many, node.GetValue());
							return true;
						}
						else if (name == wxS("other"))
						{
							plurals.emplace(LocalizationItemQuantity::Other, node.GetValue());
							return true;
						}
						return false;
					}, wxS("item"));
					AddItem(itemNode.GetAttribute(wxS("name")), LocalizationItem(plurals, flags));
				}
				return true;
			});
			return count != 0;
		}
		return false;
	}

	bool AndroidLocalizationPackage::Load(wxInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme)
	{
		if (stream.IsOk() && DoLoad(XMLDocument(stream), loadingScheme))
		{
			m_Locale = locale;
			return true;
		}
		return false;
	}
	bool AndroidLocalizationPackage::Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme)
	{
		if (auto data = library.GetResource(Localization::Private::EmbeddedResourceType, name.GetName()))
		{
			Locale usedLcoale = locale;
			if (!usedLcoale)
			{
				usedLcoale = Localization::Private::LocaleFromFileName(name);
			}
			if (DoLoad(String::FromUTF8(data), loadingScheme))
			{
				m_Locale = std::move(usedLcoale);
				return true;
			}
		}
		return false;
	}
}
