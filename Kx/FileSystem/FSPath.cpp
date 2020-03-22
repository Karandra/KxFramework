#include "KxStdAfx.h"
#include "FSPath.h"
#include "NamespacePrefix.h"
#include <KxFramework/KxComparator.h>

namespace KxFileSystem
{
	wxString ConcatWithNamespace(const wxString& path, PathNamespace withNamespace)
	{
		if (withNamespace != PathNamespace::None && !path.IsEmpty())
		{
			return KxFileSystem::GetNamespaceString(withNamespace) + path;
		}
		return path;
	}
	size_t DetectNamespacePrefix(const wxString& path, PathNamespace& ns)
	{
		// Test for every namespace starting from the longest prefix

		// 10
		if (path.StartsWith(NamespacePrefix::Win32Volume))
		{
			ns = PathNamespace::Win32Volume;
			return std::size(NamespacePrefix::Win32Volume) - 1;
		}

		// 8
		if (path.StartsWith(NamespacePrefix::Win32FileUNC))
		{
			ns = PathNamespace::Win32FileUNC;
			return std::size(NamespacePrefix::Win32FileUNC) - 1;
		}
		else if (path.StartsWith(NamespacePrefix::NetworkUNC))
		{
			ns = PathNamespace::NetworkUNC;
			return std::size(NamespacePrefix::NetworkUNC) - 1;
		}

		// 4
		if (path.StartsWith(NamespacePrefix::Win32File))
		{
			ns = PathNamespace::Win32File;
			return std::size(NamespacePrefix::Win32File) - 1;
		}
		else if (path.StartsWith(NamespacePrefix::Win32Device))
		{
			ns = PathNamespace::Win32Device;
			return std::size(NamespacePrefix::Win32Device) - 1;
		}

		// 2
		if (path.StartsWith(NamespacePrefix::Network))
		{
			ns = PathNamespace::Network;
			return std::size(NamespacePrefix::Network) - 1;
		}

		// 1
		if (path.StartsWith(NamespacePrefix::NT))
		{
			ns = PathNamespace::NT;
			return std::size(NamespacePrefix::NT) - 1;
		}

		return 0;
	}

	size_t FindChar(const wxString& path, wxChar c, bool reverse = false)
	{
		return reverse ? path.rfind(c) : path.find(c);
	}
	wxString ExtractBefore(const wxString& path, wxChar c, bool reverse = false)
	{
		const size_t pos = FindChar(path, c, reverse);
		if (pos != wxString::npos)
		{
			return path.Left(pos);
		}
		return wxEmptyString;
	}
	wxString ExtractAfter(const wxString& path, wxChar c, size_t count = wxString::npos, bool reverse = false)
	{
		const size_t pos = FindChar(path, c, reverse);
		if (pos != wxString::npos && pos + 1 < path.length())
		{
			return path.Mid(pos + 1, count);
		}
		return wxEmptyString;
	}
	size_t RemoveLeadingSpaces(wxString& path)
	{
		const std::locale locale;
		size_t removedCount = 0;
		bool removeSpaces = true;

		for (size_t i = 0; i < path.length(); i++)
		{
			// Remove any leading space characters
			if (removeSpaces)
			{
				if (std::isspace(static_cast<wxChar>(path[i]), locale))
				{
					path.Remove(i, 1);
					removedCount++;
					i--;
				}
				else
				{
					removeSpaces = false;
				}
			}
		}
		return removedCount;
	}
}

namespace KxFileSystem
{
	FSPath FSPath::FromStringUnchecked(const wxString& string)
	{
		FSPath path;
		path.m_Path = string;
		return path;
	}

	bool FSPath::AssignFromPath(const wxString& path)
	{
		m_Path = path;
		if (!m_Path.IsEmpty())
		{
			// It's important to process namespace before normalization,
			// because normalization can remove some namespace symbols.
			ProcessNamespace();
			Normalize();
		}

		return IsValid();
	}
	void FSPath::ProcessNamespace()
	{
		// We need to remove any leading spaces before searching for a namespace.
		// This is duplicated in the 'Normalize' function but anyway we need it here.
		RemoveLeadingSpaces(m_Path);

		const size_t namespacePrefixLength = DetectNamespacePrefix(m_Path, m_Namespace);
		if (namespacePrefixLength != 0)
		{
			m_Path.Remove(0, namespacePrefixLength);
		}
	}
	void FSPath::Normalize()
	{
		const std::locale locale;

		bool removeSpaces = true;
		bool removeNextSlash = false;
		for (size_t i = 0; i < m_Path.length(); i++)
		{
			wxUniCharRef c = m_Path[i];

			// Replace forward slashes with backward slashes
			if (c == wxS('/'))
			{
				c = wxS('\\');
			}

			// Remove any leading space characters
			if (removeSpaces)
			{
				if (std::isspace(static_cast<wxChar>(c), locale))
				{
					m_Path.Remove(i, 1);
					i--;
				}
				else
				{
					removeSpaces = false;
				}
			}

			// Remove any duplicating slashes
			if (c == wxS('\\'))
			{
				if (removeNextSlash || i + 1 == m_Path.length())
				{
					m_Path.Remove(i, 1);
					i--;
				}
				else
				{
					removeNextSlash = true;
				}
			}
			else
			{
				removeNextSlash = false;
			}
		}
	}

	bool FSPath::IsValid() const
	{
		return !m_Path.IsEmpty();
	}
	bool FSPath::IsSameAs(const FSPath& other, bool caseSensitive) const
	{
		return m_Namespace == other.m_Namespace && KxComparator::IsEqual(m_Path, other.m_Path, !caseSensitive);
	}
	bool FSPath::IsAbsolute() const
	{
		// Path is absolute if it has a namespace or starts with a disk designator
		return m_Namespace != KxFileSystem::PathNamespace::None || HasDrive();
	}
	bool FSPath::IsRelative() const
	{
		return IsValid() && !IsAbsolute();
	}
	size_t FSPath::GetComponentCount() const
	{
		size_t count = 0;
		for (wxChar c : m_Path)
		{
			if (c == wxS('\\'))
			{
				count++;
			}
		}
		return count;
	}

	wxString FSPath::GetFullPath(KxFileSystem::PathNamespace withNamespace) const
	{
		return ConcatWithNamespace(m_Path, withNamespace);
	}

	bool FSPath::HasDrive() const
	{
		return m_Path.length() >= 2 && m_Path[1] == wxS(':');
	}
	LegacyDrive FSPath::GetDrive() const
	{
		return LegacyDrive::FromChar(ExtractBefore(m_Path, wxS(':')));
	}
	FSPath& FSPath::SetDrive(const LegacyDrive& drive)
	{
		const size_t pos = m_Path.find(wxS(':'));
		if (pos != wxString::npos)
		{
			// Replace the disk designator
			m_Path[pos - 1] = drive.GetChar();
		}
		else
		{
			// Perpend a new disk designator
			m_Path.Prepend(wxS(":\\"));
			m_Path.Prepend(drive.GetChar());
		}
		return *this;
	}

	wxString FSPath::GetPath() const
	{
		// Return after drive designator or the path itself
		wxString path = ExtractAfter(m_Path, wxS(':'));
		return path.IsEmpty() ? m_Path : path;
	}
	FSPath& FSPath::SetPath(const wxString& path)
	{
		// Don't check for '\' and '/' here
		wxString forbiddenChars = KxFileSystem::GetForbiddenChars();
		forbiddenChars.Replace(wxS('\\'), wxEmptyString);
		forbiddenChars.Replace(wxS('/'), wxEmptyString);

		if (!path.Contains(forbiddenChars))
		{
			const size_t pos = m_Path.find(wxS(':'));
			if (pos != wxString::npos)
			{
				// Replace after the disk designator
				m_Path.Remove(pos + 1, wxString::npos);
				m_Path += wxS("\\");
				m_Path += path;

				Normalize();
			}
			else
			{
				// Replace full path
				m_Path = path;
				Normalize();

				// It's possible to pass a path with a namespace here. It shouldn't be possible to change the namespace here.
				PathNamespace ns = m_Namespace;
				ProcessNamespace();
				m_Namespace = ns;
			}

		}
		return *this;
	}

	wxString FSPath::GetName() const
	{
		// Return everything after last path delimiter or itself
		wxString path = ExtractAfter(m_Path, wxS('\\'), wxString::npos, true);
		return path.IsEmpty() ? m_Path : path;
	}
	FSPath& FSPath::SetName(const wxString& name)
	{
		if (!name.Contains(KxFileSystem::GetForbiddenChars()))
		{
			const size_t pos = m_Path.rfind(wxS('\\'));
			if (pos != wxString::npos)
			{
				const size_t dot = m_Path.find(wxS('.'), pos);
				if (dot != wxString::npos)
				{
					m_Path.replace(pos + 1, dot - pos, name);
				}
				else
				{
					m_Path.replace(pos + 1, m_Path.length() - pos, name);
				}
				Normalize();
			}
		}
		return *this;
	}

	wxString FSPath::GetExtension() const
	{
		// Return extension without a dot
		return ExtractAfter(m_Path, wxS('.'), wxString::npos, true);
	}
	FSPath& FSPath::SetExtension(const wxString& ext)
	{
		if (!ext.Contains(KxFileSystem::GetForbiddenChars()))
		{
			auto Replace = [this](const wxString& ext)
			{
				const size_t pos = m_Path.rfind(wxS('.'));
				if (pos != wxString::npos)
				{
					m_Path.replace(pos + 1, m_Path.length() - pos, ext);
				}
				else
				{
					m_Path += wxS('.');
					m_Path += ext;
				}
			};

			if (wxString extWithoutDot; ext.StartsWith(wxS('.'), &extWithoutDot))
			{
				Replace(extWithoutDot);
			}
			else
			{
				Replace(ext);
			}
			Normalize();
		}
		return *this;
	}

	FSPath FSPath::GetParent() const
	{
		return ExtractBefore(m_Path, wxS('\\'), true);
	}
	FSPath& FSPath::RemoveLast()
	{
		*this = GetParent();
		return *this;
	}
	FSPath& FSPath::Append(const FSPath& other)
	{
		if (!IsValid() || other.IsRelative())
		{
			m_Path += wxS('\\');
			m_Path += other.m_Path;
		}
		return *this;
	}
	FSPath& FSPath::Concat(const FSPath& other)
	{
		if (!IsValid() || other.IsRelative())
		{
			m_Path += other.m_Path;
		}
		return *this;
	}
}
