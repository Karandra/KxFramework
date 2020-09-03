#include "stdafx.h"
#include "IVariablesCollection.h"
#include "kxf/Application/ICoreApplication.h"
#include "kxf/System/ShellOperations.h"
#include "kxf/System/SystemInformation.h"
#include "kxf/Utility/CallAtScopeExit.h"

namespace
{
	kxf::String DoExpandStdVariables(const kxf::IVariablesCollection& collection, const kxf::String& source, bool expandStdOnly)
	{
		using namespace kxf;

		return ExpandVariables(collection, source, [&](const String& ns, const String& name) -> String
		{
			if (ns == wxS('T'))
			{
				if (auto app = ICoreApplication::GetInstance())
				{
					return app->GetLocalizationPackage().GetItem(name).GetString();
				}
			}
			else if (ns == wxS("ENV"))
			{
				return System::GetEnvironmentVariable(name);
			}
			else if (ns == wxS("SHDir"))
			{
				KnownDirectoryID directoryID = KnownDirectoryID::None;
				Shell::EnumKnownDirectories([&](KnownDirectoryID id, String directoryName)
				{
					if (directoryName == name)
					{
						directoryID = id;
						return false;
					}
					return true;
				});

				return Shell::GetKnownDirectory(directoryID);
			}
			else if (!expandStdOnly && ns.IsEmpty())
			{
				return collection.GetItem(name).As<String>();
			}
			return {};
		});
	}
}

namespace kxf
{
	String ExpandVariables(const IVariablesCollection& collection, const String& source, std::function<String(const String& ns, const String& name)> onVariable)
	{
		if (!source.IsEmpty())
		{
			String result = source.Clone();
			size_t entryStartPos = String::npos;
			size_t varNameStartPos = String::npos;

			for (size_t i = 0; i < result.length(); i++)
			{
				// Find the variable anchor
				if (result[i] == wxS('$'))
				{
					entryStartPos = i;
				}

				// We're at the beginning of the variable name
				if (entryStartPos != String::npos && result[i] == wxS('('))
				{
					varNameStartPos = i + 1;
				}

				// We've found the end of the variable, extract namespace and call the provided callback
				if (entryStartPos != String::npos && varNameStartPos != String::npos && result[i] == wxS(')'))
				{
					Utility::CallAtScopeExit atExit = [&]()
					{
						// Reset parser state
						entryStartPos = String::npos;
						varNameStartPos = String::npos;
					};

					const size_t varEntryLength = i - entryStartPos + 1;
					const size_t varNameLength = i - varNameStartPos;
					if (varNameLength != 0 && varNameStartPos >= 2)
					{
						const String varNamespace = result.SubString(entryStartPos + 1, varNameStartPos - 2);
						const String varName = result.Mid(varNameStartPos, varNameLength);

						String value = std::invoke(onVariable, varNamespace, varName);
						if (!value.IsEmpty())
						{
							result.GetWxString().replace(entryStartPos, varEntryLength, value);

							// Set 'i' value to end of replacement
							i = entryStartPos + value.length() - 1;
						}
					}
				}
			}
			return result;
		}
		return source;
	}

	String ExpandStdVariables(const String& source)
	{
		class DummyVariablesCollection final: public IVariablesCollection
		{
			public:
				size_t GetItemCount() const override
				{
					return 0;
				}

				String Expand(const String& variables) const override
				{
					return {};
				}
				size_t EnumItems(std::function<bool(const Any&)> func) const override
				{
					return 0;
				}
				size_t EnumItems(std::function<bool(Any&)> func) override
				{
					return 0;
				}

				bool HasItem(const String& id) const override
				{
					return false;
				}
				Any GetItem(const String& id) const override
				{
					return {};
				}
				void SetItem(const String& id, Any item) override
				{
				}
		} dummyCollection;
		return DoExpandStdVariables(dummyCollection, source, true);
	}
	String ExpandStdVariables(const IVariablesCollection& collection, const String& source)
	{
		return DoExpandStdVariables(collection, source, false);
	}
}
