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

	private:
		mutable wxMimeTypesManager m_Manager;

	public:
		KxFileType FileTypeFromExtension(const wxString& extension) const
		{
			return m_Manager.GetFileTypeFromExtension(extension);
		}
		KxFileType FileTypeFromMimeType(const wxString& mimeType) const
		{
			return m_Manager.GetFileTypeFromMimeType(mimeType);
		}

		KxFileType Associate(const KxFileTypeInfo& fileTypeInfo)
		{
			return m_Manager.Associate(fileTypeInfo.AsWxFileTypeInfo());
		}
		bool Unassociate(KxFileType& fileType)
		{
			if (fileType)
			{
				return m_Manager.Unassociate(&fileType.AsWxFileType());
			}
			return false;
		}

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
