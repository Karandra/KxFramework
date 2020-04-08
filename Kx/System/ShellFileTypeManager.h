#pragma once
#include "Common.h"
#include "ShellFileType.h"
#include "ShellFileTypeInfo.h"
#include "Kx/General/String.h"
#include "Kx/FileSystem/FSPath.h"
#include <wx/mimetype.h>

namespace KxFramework
{
	class KX_API ShellFileTypeManager
	{
		public:
			static bool IsOfType(const String& mimeType, const String& wildcard)
			{
				return wxMimeTypesManager::IsOfType(mimeType, wildcard);
			}
			static String NormalizeFileExtension(const String& extension);

		private:
			mutable wxMimeTypesManager m_Manager;

		public:
			ShellFileType FileTypeFromExtension(const String& extension) const;
			ShellFileType FileTypeFromMimeType(const String& mimeType) const;

			ShellFileType Associate(const ShellFileTypeInfo& fileTypeInfo);
			bool IsAssociatedWith(const ShellFileType& fileType, const String& executablePath) const;
			bool Unassociate(ShellFileType& fileType);

			wxArrayString EnumAllFileTypes() const
			{
				wxArrayString mimetypes;
				m_Manager.EnumAllFileTypes(mimetypes);
				return mimetypes;
			}
			template<class TFunc> size_t EnumFileTypes(TFunc&& func) const
			{
				using T = std::invoke_result_t<TFunc, const String&>;

				size_t counter = 0;
				wxArrayString mimetypes = EnumAllFileTypes();

				for (const String& mime: mimetypes)
				{
					if constexpr(std::is_same_v<T, bool>)
					{
						if (!func(mime))
						{
							break;
						}
						counter++;
						return counter;
					}
					if constexpr(std::is_same_v<T, void>)
					{
						func(mime);
					}
					else
					{
						static_assert(false, "invalid return value");
					}
				}
				return mimetypes.size();
			}

			void AddFallback(const ShellFileTypeInfo& fileTypeInfo)
			{
				m_Manager.AddFallback(fileTypeInfo.AsWxFileTypeInfo());
			}
	};
}
