#pragma once
#include "Common.h"
#include "Kx/Utility/Common.h"
#include <wx/mimetype.h>
#include <wx/iconloc.h>

class KX_API KxFileTypeInfo final
{
	private:
		wxFileTypeInfo m_FileTypeInfo;

	public:
		KxFileTypeInfo() = default;
		KxFileTypeInfo(const wxString& mimeType)
			:m_FileTypeInfo(mimeType)
		{
		}

		template<class... Args>
		KxFileTypeInfo(const wxString& mimeType,
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

		KxFileTypeInfo(const wxFileTypeInfo& other)
			:m_FileTypeInfo(other)
		{
		}
		KxFileTypeInfo(wxFileTypeInfo&& other)
			:m_FileTypeInfo(std::move(other))
		{
		}
		KxFileTypeInfo(const KxFileTypeInfo&) = delete;
		KxFileTypeInfo(KxFileTypeInfo&&) = delete;

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
		KxFileTypeInfo& SetDescription(const wxString& value)
		{
			m_FileTypeInfo.SetDescription(value);
			return *this;
		}
		
		const wxString& GetShortDescription() const
		{
			return m_FileTypeInfo.GetShortDesc();
		}
		KxFileTypeInfo& SetShortDescription(const wxString& value)
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
		KxFileTypeInfo& SetOpenCommand(const wxString& value)
		{
			m_FileTypeInfo.SetOpenCommand(value);
			return *this;
		}

		const wxString& GetPrintCommand() const
		{
			return m_FileTypeInfo.GetPrintCommand();
		}
		KxFileTypeInfo& SetPrintCommand(const wxString& value)
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
		KxFileTypeInfo& AddExtension(const wxString& ext)
		{
			m_FileTypeInfo.AddExtension(ext);
			return *this;
		}
		template<class... Args> KxFileTypeInfo& AddExtensions(Args&&... arg)
		{
			KxUtility::ForEachParameterInPack([this](const wxString& ext)
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
		KxFileTypeInfo& SetIcon(const wxIconLocation& icon)
		{
			m_FileTypeInfo.SetIcon(icon.GetFileName(), icon.GetIndex());
			return *this;
		}
		KxFileTypeInfo& SetIcon(const wxString& filePath, int index = 0)
		{
			m_FileTypeInfo.SetIcon(filePath, index);
			return *this;
		}

	public:
		explicit operator bool() const noexcept
		{
			return m_FileTypeInfo.IsValid();
		}
		bool operator!() const noexcept
		{
			return !m_FileTypeInfo.IsValid();
		}

		KxFileTypeInfo& operator=(const KxFileTypeInfo&) = delete;
		KxFileTypeInfo& operator=(KxFileTypeInfo&&) = delete;

		KxFileTypeInfo& operator=(const wxFileTypeInfo& other)
		{
			m_FileTypeInfo = other;
			return *this;
		}
		KxFileTypeInfo& operator=(wxFileTypeInfo&& other)
		{
			m_FileTypeInfo = std::move(other);
			return *this;
		}
};
