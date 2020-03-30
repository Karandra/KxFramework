#include "KxStdAfx.h"
#include "FSPath.h"
#include "LegacyVolume.h"
#include "StorageVolume.h"
#include "NamespacePrefix.h"
#include <KxFramework/KxComparator.h>

namespace KxFramework
{
	wxString ConcatWithNamespace(const wxString& path, FSPathNamespace withNamespace)
	{
		if (withNamespace != FSPathNamespace::None && !path.IsEmpty())
		{
			return FileSystem::GetNamespaceString(withNamespace) + path;
		}
		return path;
	}
	size_t DetectNamespacePrefix(const wxString& path, FSPathNamespace& ns)
	{
		// All namespaces starts from at least one'\'
		if (path.IsEmpty() || path[0] != wxS('\\'))
		{
			ns = FSPathNamespace::None;
			return 0;
		}

		using namespace FileSystem;
		// Test for every namespace starting from the longest prefix

		// 10
		if (path.StartsWith(NamespacePrefix::Win32Volume))
		{
			ns = FSPathNamespace::Win32Volume;
			return std::size(NamespacePrefix::Win32Volume) - 1;
		}

		// 8
		if (path.StartsWith(NamespacePrefix::Win32FileUNC))
		{
			ns = FSPathNamespace::Win32FileUNC;
			return std::size(NamespacePrefix::Win32FileUNC) - 1;
		}
		else if (path.StartsWith(NamespacePrefix::NetworkUNC))
		{
			ns = FSPathNamespace::NetworkUNC;
			return std::size(NamespacePrefix::NetworkUNC) - 1;
		}

		// 4
		if (path.StartsWith(NamespacePrefix::Win32File))
		{
			ns = FSPathNamespace::Win32File;
			return std::size(NamespacePrefix::Win32File) - 1;
		}
		else if (path.StartsWith(NamespacePrefix::Win32Device))
		{
			ns = FSPathNamespace::Win32Device;
			return std::size(NamespacePrefix::Win32Device) - 1;
		}

		// 2
		if (path.StartsWith(NamespacePrefix::Network))
		{
			ns = FSPathNamespace::Network;
			return std::size(NamespacePrefix::Network) - 1;
		}

		// 1
		if (path.StartsWith(NamespacePrefix::NT))
		{
			ns = FSPathNamespace::NT;
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

namespace KxFramework
{
	FSPath FSPath::FromStringUnchecked(const wxString& string, FSPathNamespace ns)
	{
		FSPath path;
		path.m_Path = string;
		path.m_Namespace = ns;

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
		// Path is absolute if it has a namespace or starts with a volume (a disk designator)
		return IsValid() && (m_Namespace != FSPathNamespace::None || HasAnyVolume());
	}
	bool FSPath::IsRelative() const
	{
		return IsValid() && !IsAbsolute();
	}
	
	size_t FSPath::GetComponentCount() const
	{
		size_t count = 0;
		for (wxChar c: m_Path)
		{
			if (c == wxS('\\'))
			{
				count++;
			}
		}
		return count;
	}
	size_t FSPath::ForEachComponent(std::function<bool(const wxString&)> func) const
	{
		size_t count = 0;

		size_t start = 0;
		for (size_t i = 0; i < m_Path.length(); i++)
		{
			const bool isLastIndex = i + 1 == m_Path.length();
			if (m_Path[i] == wxS('\\') || isLastIndex)
			{
				size_t end = isLastIndex ? i : i - 1;
				if (func(m_Path.SubString(start, end)))
				{
					count++;
				}
				else
				{
					break;
				}

				// Set start to next character after the separator
				start++;
			}
		}
		return count;
	}

	wxString FSPath::GetFullPath(FSPathNamespace withNamespace) const
	{
		return ConcatWithNamespace(m_Path, withNamespace);
	}

	bool FSPath::HasVolume() const
	{
		return m_Namespace == FSPathNamespace::Win32Volume;
	}
	bool FSPath::HasLegacyVolume() const
	{
		return m_Namespace != FSPathNamespace::Win32Volume && m_Path.length() >= 2 && m_Path[1] == wxS(':');
	}
	StorageVolume FSPath::GetVolume() const
	{
		return {};
	}
	LegacyVolume FSPath::GetLegacyVolume() const
	{
		if (HasLegacyVolume())
		{
			return LegacyVolume::FromChar(ExtractBefore(m_Path, wxS(':')));
		}
		return {};
	}
	FSPath& FSPath::SetVolume(const LegacyVolume& drive)
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
	FSPath& FSPath::SetVolume(const StorageVolume& volume)
	{
		m_Namespace = FSPathNamespace::Win32Volume;
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
		wxString forbiddenChars = FileSystem::GetForbiddenChars();
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
				FSPathNamespace ns = m_Namespace;
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
		if (!name.Contains(FileSystem::GetForbiddenChars()))
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
		if (!ext.Contains(FileSystem::GetForbiddenChars()))
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

	FSPath FSPath::GetAfter(const FSPath& start) const
	{
		// this: C:\Program Files (x86)\Common Files\Microsoft
		// start: C:\Program Files (x86)
		// return: Common Files\Microsoft

		wxString fullPath = GetFullPath();
		if (KxComparator::IsEqual(fullPath.Left(start.GetPathLength()), start.m_Path, true))
		{
			fullPath = fullPath.Remove(0, start.GetPathLength());
		}
		return FSPath(fullPath).EnsureNamespaceSet(m_Namespace);
	}
	FSPath FSPath::GetBefore(const FSPath& end) const
	{
		// this: C:\Program Files (x86)\Common Files\Microsoft
		// end: Common Files\Microsoft
		// return: C:\Program Files (x86)

		wxString fullPath = GetFullPath();
		if (KxComparator::IsEqual(fullPath.Right(end.GetPathLength()), end.m_Path, true))
		{
			fullPath = fullPath.Remove(fullPath.length() - end.GetPathLength(), end.GetPathLength());
		}
		return FSPath(fullPath).EnsureNamespaceSet(m_Namespace);
	}
	FSPath FSPath::GetParent() const
	{
		return FSPath(ExtractBefore(m_Path, wxS('\\'), true)).EnsureNamespaceSet(m_Namespace);
	}
	FSPath& FSPath::RemoveLastPart()
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
