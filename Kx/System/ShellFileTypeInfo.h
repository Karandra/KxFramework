#pragma once
#include "Common.h"
#include "Kx/Utility/Common.h"
#include <KxFramework/KxComparator.h>
#include <wx/mimetype.h>
#include <wx/iconloc.h>

namespace KxFramework
{
	class KX_API ShellFileTypeInfo final
	{
		private:
			wxFileTypeInfo m_FileTypeInfo;
			KxComparator::UMapNoCase<wxString, bool> m_URLProtocolMap;

		public:
			ShellFileTypeInfo() = default;
			ShellFileTypeInfo(const wxString& mimeType)
				:m_FileTypeInfo(mimeType)
			{
			}

			template<class... Args>
			ShellFileTypeInfo(const wxString& mimeType,
						   const wxString& openCommand,
						   const wxString& printCommand,
						   const wxString& description,
						   const wxString& extension,
						   Args&& ... arg
			)
				:m_FileTypeInfo(mimeType, openCommand, printCommand, description, extension)
			{
				AddExtensions(std::forward<Args>(arg)...);
			}

			ShellFileTypeInfo(const wxFileTypeInfo& other)
				:m_FileTypeInfo(other)
			{
			}
			ShellFileTypeInfo(wxFileTypeInfo&& other)
				:m_FileTypeInfo(std::move(other))
			{
			}
			ShellFileTypeInfo(const ShellFileTypeInfo&) = delete;
			ShellFileTypeInfo(ShellFileTypeInfo&&) = delete;

		public:
			const wxFileTypeInfo& AsWxFileTypeInfo() const noexcept
			{
				return m_FileTypeInfo;
			}
			wxFileTypeInfo& AsWxFileTypeInfo() noexcept
			{
				return m_FileTypeInfo;
			}

		public:
			const wxString& GetDescription() const
			{
				return m_FileTypeInfo.GetDescription();
			}
			ShellFileTypeInfo& SetDescription(const wxString& value)
			{
				m_FileTypeInfo.SetDescription(value);
				return *this;
			}
		
			const wxString& GetShortDescription() const
			{
				return m_FileTypeInfo.GetShortDesc();
			}
			ShellFileTypeInfo& SetShortDescription(const wxString& value)
			{
				m_FileTypeInfo.SetShortDesc(value);
				return *this;
			}
		
			const wxString& GetMimeType() const
			{
				return m_FileTypeInfo.GetMimeType();
			}

			const wxString& GetOpenCommand() const
			{
				return m_FileTypeInfo.GetOpenCommand();
			}
			ShellFileTypeInfo& SetOpenCommand(const wxString& value)
			{
				m_FileTypeInfo.SetOpenCommand(value);
				return *this;
			}

			const wxString& GetPrintCommand() const
			{
				return m_FileTypeInfo.GetPrintCommand();
			}
			ShellFileTypeInfo& SetPrintCommand(const wxString& value)
			{
				m_FileTypeInfo.SetPrintCommand(value);
				return *this;
			}

			const wxArrayString& GetExtensions() const
			{
				return m_FileTypeInfo.GetExtensions();
			}
			size_t GetExtensionsCount() const
			{
				return m_FileTypeInfo.GetExtensionsCount();
			}
			ShellFileTypeInfo& AddExtension(const wxString& ext, bool isURLProtocol = false)
			{
				m_FileTypeInfo.AddExtension(ext);
				m_URLProtocolMap.insert_or_assign(ext, isURLProtocol);
				return *this;
			}
			
			template<class... Args>
			ShellFileTypeInfo& AddExtensions(Args&&... arg)
			{
				Utility::ForEachParameterPackItem([this](const wxString& ext)
				{
					m_FileTypeInfo.AddExtension(ext);
				}, std::forward<Args>(arg)...);
				return *this;
			}

			int GetIconIndex() const
			{
				return m_FileTypeInfo.GetIconIndex();
			}
			const wxString& GetIconFile() const
			{
				return m_FileTypeInfo.GetIconFile();
			}
			wxIconLocation GetIcon() const
			{
				return wxIconLocation(m_FileTypeInfo.GetIconFile(), m_FileTypeInfo.GetIconIndex());
			}
			ShellFileTypeInfo& SetIcon(const wxIconLocation& icon)
			{
				m_FileTypeInfo.SetIcon(icon.GetFileName(), icon.GetIndex());
				return *this;
			}
			ShellFileTypeInfo& SetIcon(const wxString& filePath, int index = 0)
			{
				m_FileTypeInfo.SetIcon(filePath, index);
				return *this;
			}

			bool IsURLProtocol(const wxString& ext) const;
			ShellFileTypeInfo& SetURLProtocol(const wxString& ext, bool protocol = true);

		public:
			explicit operator bool() const noexcept
			{
				return m_FileTypeInfo.IsValid();
			}
			bool operator!() const noexcept
			{
				return !m_FileTypeInfo.IsValid();
			}

			ShellFileTypeInfo& operator=(const ShellFileTypeInfo&) = delete;
			ShellFileTypeInfo& operator=(ShellFileTypeInfo&&) = delete;

			ShellFileTypeInfo& operator=(const wxFileTypeInfo& other)
			{
				m_FileTypeInfo = other;
				return *this;
			}
			ShellFileTypeInfo& operator=(wxFileTypeInfo&& other)
			{
				m_FileTypeInfo = std::move(other);
				return *this;
			}
	};
}
