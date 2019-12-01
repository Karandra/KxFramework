#pragma once
#include "Common.h"
#include <wx/mimetype.h>
#include <wx/iconloc.h>

class KX_API KxFileType final
{
	public:
		using MessageParameters = wxFileType::MessageParameters;

	public:
		static wxString ExpandCommand(const wxString &command, const MessageParameters& parameters)
		{
			return wxFileType::ExpandCommand(command, parameters);
		}

	private:
		std::unique_ptr<wxFileType> m_FileType;

	public:
		KxFileType() = default;
		KxFileType(wxFileType* fileType) noexcept
			:m_FileType(fileType)
		{
		}
		KxFileType(KxFileType&& other) noexcept
		{
			*this = std::move(other);
		}
		KxFileType(const KxFileType&) = delete;

	public:
		const wxFileType& AsWxFileType() const
		{
			return *m_FileType;
		}
		wxFileType& AsWxFileType()
		{
			return *m_FileType;
		}

	public:
		wxString GetDescription() const
		{
			wxString value;
			m_FileType->GetDescription(&value);
			return value;
		}
		wxString GetExtension() const
		{
			wxArrayString extensions;
			return m_FileType->GetExtensions(extensions) ? extensions.front() : wxEmptyString;
		}
		wxArrayString GetAllExtensions() const
		{
			wxArrayString extensions;
			m_FileType->GetExtensions(extensions);
			return extensions;
		}
		
		wxString GetMimeType() const
		{
			wxString value;
			m_FileType->GetMimeType(&value);
			return value;
		}
		wxArrayString GetAllMimeTypes() const
		{
			wxArrayString mimeTypes;
			m_FileType->GetMimeTypes(mimeTypes);
			return mimeTypes;
		}

		wxIconLocation GetIcon() const
		{
			wxIconLocation icon;
			m_FileType->GetIcon(&icon);
			return icon;
		}
		wxIconLocation GetIcon(const MessageParameters& parameters) const
		{
			wxIconLocation icon;
			m_FileType->GetIcon(&icon, parameters);
			return icon;
		}

		wxString GetCommand(const MessageParameters& parameters, const wxString& action) const
		{
			wxString value;
			m_FileType->GetExpandedCommand(action, parameters);
			return value;
		}
		wxString GetCommand(const wxString& filePath, const wxString& action) const
		{
			return GetCommand(MessageParameters(filePath), action);
		}

		wxString GetOpenCommand(const MessageParameters& parameters) const
		{
			wxString value;
			m_FileType->GetOpenCommand(&value, parameters);
			return value;
		}
		wxString GetOpenCommand(const wxString& filePath) const
		{
			return GetOpenCommand(MessageParameters(filePath));
		}
		wxString GetOpenExecutable() const;

		wxString GetPrintCommand(const MessageParameters& parameters) const
		{
			wxString value;
			m_FileType->GetPrintCommand(&value, parameters);
			return value;
		}
		wxString GetPrintCommand(const wxString& filePath) const
		{
			return GetPrintCommand(MessageParameters(filePath));
		}
		
		size_t GetAllCommands(wxArrayString& verbs, wxArrayString& commands, const MessageParameters& parameters) const
		{
			return m_FileType->GetAllCommands(&verbs, &commands, parameters);
		}
		bool IsURLProtocol(const wxString& extension) const;

	public:
		explicit operator bool() const noexcept
		{
			return m_FileType != nullptr;
		}
		bool operator!() const noexcept
		{
			return m_FileType == nullptr;
		}

		KxFileType& operator=(KxFileType&& other) noexcept
		{
			m_FileType = std::move(other.m_FileType);
			return *this;
		}
		KxFileType& operator=(const KxFileType&) = delete;
};
