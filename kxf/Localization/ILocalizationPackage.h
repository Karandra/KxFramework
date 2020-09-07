#pragma once
#include "Common.h"
#include "Locale.h"
#include "LocalizationItem.h"
#include "kxf/RTTI/QueryInterface.h"
#include <wx/stream.h>

namespace kxf::Localization
{
	enum class LoadingScheme: uint32_t
	{
		Append = 1,
		Replace = 2,

		OverwriteExisting = 1 << 16,

		CONSECUTIVE_MASK = 0xFFFF
	};
}
namespace kxf
{
	KxDeclareFlagSet(Localization::LoadingScheme);
}

namespace kxf
{
	class KX_API ILocalizationPackage: public RTTI::Interface<ILocalizationPackage>
	{
		KxRTTI_DeclareIID(ILocalizationPackage, {0xd52888ce, 0x2c58, 0x438e, {0x95, 0x95, 0x20, 0x2, 0x84, 0xd, 0x6a, 0x51}});

		friend class LocalizationItem;

		public:
			using LoadingScheme = Localization::LoadingScheme;

		protected:
			virtual const String& GetPluralStringForNumber(const LocalizationItem& item, int value) const;

		public:
			virtual ~ILocalizationPackage() = default;

		public:
			virtual Locale GetLocale() const = 0;
			virtual size_t GetItemCount() const = 0;
			bool IsEmpty() const
			{
				return GetItemCount() == 0;
			}

			virtual const LocalizationItem& GetItem(const ResourceID& id) const = 0;
			virtual size_t EnumItems(std::function<bool(const ResourceID&, const LocalizationItem&)> func) const = 0;

			virtual bool Load(wxInputStream& stream, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) = 0;
			virtual bool Load(const DynamicLibrary& library, const FSPath& name, const Locale& locale, FlagSet<LoadingScheme> loadingScheme = LoadingScheme::Replace) = 0;

		public:
			explicit operator bool() const
			{
				return !IsEmpty();
			}
			bool operator!() const
			{
				return IsEmpty();
			}
	};
}
