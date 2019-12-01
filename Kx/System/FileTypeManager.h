#pragma once
#include "Common.h"
#include "FileType.h"
#include "FileTypeInfo.h"
#include <wx/mimetype.h>

class KX_API KxFileTypeManager
{
	public:
		static bool IsOfType(const wxString& mimeType, const wxString& wildcard)
		{
			return wxMimeTypesManager::IsOfType(mimeType, wildcard);
		}
		static wxString NormalizeFileExtension(const wxString& extension);

	private:
		mutable wxMimeTypesManager m_Manager;

	public:
		KxFileType FileTypeFromExtension(const wxString& extension) const;
		KxFileType FileTypeFromMimeType(const wxString& mimeType) const;

		KxFileType Associate(const KxFileTypeInfo& fileTypeInfo);
		bool IsAssociatedWith(const KxFileType& fileType, const wxString& executablePath) const;
		bool Unassociate(KxFileType& fileType);

		wxArrayString EnumAllFileTypes() const
		{
			wxArrayString mimetypes;
			m_Manager.EnumAllFileTypes(mimetypes);
			return mimetypes;
		}
		template<class TFunc> size_t EnumFileTypes(TFunc&& func) const
		{
			using T = std::invoke_result_t<TFunc, const wxString&>;

			size_t counter = 0;
			wxArrayString mimetypes = EnumAllFileTypes();

			for (const wxString& mime: mimetypes)
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

		void AddFallback(const KxFileTypeInfo& fileTypeInfo)
		{
			m_Manager.AddFallback(fileTypeInfo.AsWxFileTypeInfo());
		}
};
