#include "KxStdAfx.h"
#include "FSPath.h"
#include "LegacyVolume.h"
#include "StorageVolume.h"
#include "Kx/Utility/CallAtScopeExit.h"
#include "Private/NamespacePrefix.h"

namespace
{
	constexpr size_t g_GUIDLength = 36;
	constexpr size_t g_VolumePathPrefixLength = 6;
	constexpr size_t g_VolumePathTotalLength = g_VolumePathPrefixLength + g_GUIDLength + 2;
}
namespace
{
	KxFramework::String ConcatWithNamespace(const KxFramework::String& path, KxFramework::FSPathNamespace withNamespace)
	{
		using namespace KxFramework;

		if (withNamespace != FSPathNamespace::None && !path.IsEmpty())
		{
			return FileSystem::GetNamespaceString(withNamespace) + path;
		}
		return path;
	}
	size_t FindChar(const KxFramework::String& path, wxChar c, bool reverse = false)
	{
		using namespace KxFramework;

		return path.Find(c, 0, reverse ? StringOpFlag::FromEnd : StringOpFlag::None);
	}
	KxFramework::String ExtractBefore(const KxFramework::String& path, wxChar c, bool reverse = false)
	{
		using namespace KxFramework;

		const size_t pos = FindChar(path, c, reverse);
		if (pos != String::npos)
		{
			String result = path.Left(pos);
			if (!result.IsEmpty() && result.back() == wxS('\\'))
			{
				result.RemoveFromEnd(1);
			}
			return result;
		}
		return wxEmptyString;
	}
	KxFramework::String ExtractAfter(const KxFramework::String& path, wxChar c, size_t count = KxFramework::String::npos, bool reverse = false)
	{
		using namespace KxFramework;

		const size_t pos = FindChar(path, c, reverse);
		if (pos != String::npos && pos + 1 < path.length())
		{
			String result = path.Mid(pos + 1, count);
			if (!result.IsEmpty() && result[0] == wxS('\\'))
			{
				result.Remove(0, 1);
			}
			return result;
		}
		return wxEmptyString;
	}
	size_t RemoveLeadingSpaces(KxFramework::String& path)
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
	FSPath FSPath::FromStringUnchecked(String string, FSPathNamespace ns)
	{
		FSPath path;
		path.m_Path = std::move(string);
		path.m_Namespace = ns;

		return path;
	}

	void FSPath::AssignFromPath(String path)
	{
		Utility::CallAtScopeExit atExit([&]()
		{
			if (!CheckStringOnInitialAssign(m_Path))
			{
				m_Path.Clear();
			}
		});

		m_Path = std::move(path);
		if (!m_Path.IsEmpty())
		{
			// It's important to process namespace before normalization,
			// because normalization can remove some namespace symbols.
			ProcessNamespace();
			Normalize();
		}
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

	bool FSPath::CheckIsLegacyVolume(const String& path) const
	{
		if (path.length() >= 2 && path[1] == wxS(':'))
		{
			return LegacyVolume::FromChar(path[0]).IsValid();
		}
		return false;
	}
	bool FSPath::CheckIsVolumeGUID(const String& path) const
	{
		// Format: '\\?\Volume{66843779-55ae-45c5-9abe-b67ccee14079}\', but we're store path without namespace and trailing separators
		// so it'll be this instead: 'Volume{66843779-55ae-45c5-9abe-b67ccee14079}'.
		if (path.length() >= g_VolumePathTotalLength)
		{
			const bool prefixCorrect = std::char_traits<wxChar>::compare(path.wc_str(), wxS("Volume"), g_VolumePathPrefixLength) == 0;
			const bool bracesCorrect = path[g_VolumePathPrefixLength] == wxS('{') && path[g_VolumePathTotalLength - 1] == wxS('}');
			return prefixCorrect && bracesCorrect;
		}
		return false;
	}
	size_t FSPath::DetectNamespacePrefix(const String& path, KxFramework::FSPathNamespace& ns) const
	{
		using namespace FileSystem::Private;

		// All namespaces starts from at least one'\'
		if (path.IsEmpty() || path[0] != wxS('\\'))
		{
			ns = FSPathNamespace::None;
			return 0;
		}

		// Test for every namespace starting from the longest prefix
		// Length: 8
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

		// Length: 4
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

		// Length: 2
		if (path.StartsWith(NamespacePrefix::Network))
		{
			ns = FSPathNamespace::Network;
			return std::size(NamespacePrefix::Network) - 1;
		}

		// Length: 1
		if (path.StartsWith(NamespacePrefix::NT))
		{
			ns = FSPathNamespace::NT;
			return std::size(NamespacePrefix::NT) - 1;
		}

		return 0;
	}

	bool FSPath::CheckStringOnInitialAssign(const String& path) const
	{
		if (path.Contains(FileSystem::GetForbiddenChars(m_SearchMaksAllowed ? wxS("\\/*?") : wxS("\\/"))))
		{
			return CheckIsLegacyVolume(path);
		}
		return true;
	}
	bool FSPath::CheckStringOnAssignPath(const String& path) const
	{
		FSPathNamespace ns = FSPathNamespace::None;
		return !path.Contains(FileSystem::GetForbiddenChars(m_SearchMaksAllowed ? wxS("*?") : wxEmptyString)) &&
			!CheckIsLegacyVolume(path) &&
			!CheckIsVolumeGUID(path) &&
			DetectNamespacePrefix(path, ns) == 0;
	}
	bool FSPath::CheckStringOnAssignName(const String& name) const
	{
		return CheckStringOnAssignPath(name);
	}

	bool FSPath::IsValid() const
	{
		return !m_Path.IsEmpty();
	}
	bool FSPath::IsSameAs(const FSPath& other, bool caseSensitive) const
	{
		return m_Namespace == other.m_Namespace && m_Path.IsSameAs(other.m_Path, caseSensitive ? StringOpFlag::None : StringOpFlag::IgnoreCase);
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
	bool FSPath::Contains(const FSPath& path) const
	{
		return m_Path.Contains(path.GetFullPath(), StringOpFlag::IgnoreCase);
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
	size_t FSPath::ForEachComponent(std::function<bool(const String&)> func) const
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
	String FSPath::GetFullPath(FSPathNamespace withNamespace, FSPathFormat format) const
	{
		String result = ConcatWithNamespace(m_Path, withNamespace);
		if (!result.IsEmpty())
		{
			if (format & FSPathFormat::TrailingSeparator)
			{
				result += wxS('\\');
			}
		}
		return result;
	}

	bool FSPath::HasVolume() const
	{
		return CheckIsVolumeGUID(m_Path);
	}
	bool FSPath::HasLegacyVolume() const
	{
		return CheckIsLegacyVolume(m_Path);
	}
	StorageVolume FSPath::GetVolume() const
	{
		// StorageVolume constructor does the validity check and extracts volume path
		return *this;
	}
	LegacyVolume FSPath::GetLegacyVolume() const
	{
		if (HasLegacyVolume())
		{
			return LegacyVolume::FromChar(m_Path[0]);
		}
		return {};
	}
	FSPath& FSPath::SetVolume(const LegacyVolume& drive)
	{
		if (HasLegacyVolume())
		{
			if (drive)
			{
				// Replace the disk designator
				m_Path[0] = drive.GetChar();
			}
			else
			{
				// Remove the disk designator
				m_Path.Remove(0, 2);
				Normalize();
			}
		}
		else if (HasVolume())
		{
			if (drive)
			{
				// Replace with legacy drive path
				char disk[] = "\0:\\";
				disk[0] = drive.GetChar();
				m_Path.GetWxString().replace(0, g_VolumePathTotalLength, disk, std::size(disk) - 1);
			}
			else
			{
				// Remove the volume
				m_Path.Remove(0, g_VolumePathTotalLength);
			}
			Normalize();
		}
		else
		{
			// Perpend a new disk designator
			char disk[] = "\0:\\";
			disk[0] = drive.GetChar();
			m_Path.Prepend(disk);

			Normalize();
		}
		return *this;
	}
	FSPath& FSPath::SetVolume(const StorageVolume& volume)
	{
		if (HasLegacyVolume())
		{
			if (volume)
			{
				// Replace with volume path
				String path = volume.GetPath().GetFullPath(FSPathNamespace::None, FSPathFormat::TrailingSeparator);
				m_Path.GetWxString().replace(0, 2, path);
			}
			else
			{
				// Remove the disk designator
				m_Path.Remove(0, 2);
			}
			Normalize();
		}
		else if (HasVolume())
		{
			if (volume)
			{
				// Replace with a new volume path
				String path = volume.GetPath().GetFullPath(FSPathNamespace::None, FSPathFormat::TrailingSeparator);
				m_Path.GetWxString().replace(0, g_VolumePathTotalLength, path);
			}
			else
			{
				// Remove the volume
				m_Path.Remove(0, g_VolumePathTotalLength);
			}
			Normalize();
		}
		else
		{
			// Prepend a new volume path
			String path = volume.GetPath().GetFullPath(FSPathNamespace::None, FSPathFormat::TrailingSeparator);
			m_Path.Prepend(std::move(path));

			Normalize();
		}
		return *this;
	}

	String FSPath::GetPath() const
	{
		if (HasLegacyVolume())
		{
			// Return after the disk designator
			return m_Path.Mid(2);
		}
		else if (HasVolume())
		{
			// Return after GUID path
			return m_Path.Mid(g_VolumePathTotalLength);
		}
		else
		{
			// Return the path itself
			return m_Path;
		}
	}
	FSPath& FSPath::SetPath(const String& path)
	{
		if (CheckStringOnAssignPath(path))
		{
			if (HasLegacyVolume())
			{
				// Replace after the disk designator
				m_Path.Remove(2, String::npos);
				m_Path += wxS('\\');
				m_Path += path;
			}
			else if (HasVolume())
			{
				// Replace after GUID path
				m_Path.Truncate(g_VolumePathTotalLength);
				m_Path += wxS('\\');
				m_Path += path;
			}
			else
			{
				// Replace the full path
				m_Path = path;
			}
			Normalize();
		}
		return *this;
	}

	String FSPath::GetName() const
	{
		// Return everything after last path delimiter or itself
		String path = ExtractAfter(m_Path, wxS('\\'), String::npos, true);
		return path.IsEmpty() ? m_Path : path;
	}
	FSPath& FSPath::SetName(const String& name)
	{
		if (CheckStringOnAssignName(name))
		{
			const size_t pos = m_Path.Find(wxS('\\'), 0, StringOpFlag::FromEnd);
			if (pos != String::npos)
			{
				const size_t dot = m_Path.Find(wxS('.'), pos);
				if (dot != String::npos)
				{
					m_Path.GetWxString().replace(pos + 1, dot - pos, name);
				}
				else
				{
					m_Path.GetWxString().replace(pos + 1, m_Path.length() - pos, name);
				}
				Normalize();
			}
		}
		return *this;
	}

	String FSPath::GetExtension() const
	{
		// Return extension without a dot
		return ExtractAfter(m_Path, wxS('.'), String::npos, true);
	}
	FSPath& FSPath::SetExtension(const String& ext)
	{
		if (!ext.Contains(FileSystem::GetForbiddenChars()))
		{
			auto Replace = [this](const String& ext)
			{
				const size_t pos = m_Path.Find(wxS('.'), 0, StringOpFlag::FromEnd);
				if (pos != String::npos)
				{
					m_Path.GetWxString().replace(pos + 1, m_Path.length() - pos, ext);
				}
				else
				{
					m_Path += wxS('.');
					m_Path += ext;
				}
			};

			if (String extWithoutDot; ext.StartsWith(wxS('.'), &extWithoutDot))
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

		String fullPath = GetFullPath();
		if (fullPath.Left(start.GetPathLength()).IsSameAs(start.m_Path, StringOpFlag::IgnoreCase))
		{
			fullPath = fullPath.Remove(0, start.GetPathLength());
		}
		return FSPath(std::move(fullPath)).EnsureNamespaceSet(m_Namespace);
	}
	FSPath FSPath::GetBefore(const FSPath& end) const
	{
		// this: C:\Program Files (x86)\Common Files\Microsoft
		// end: Common Files\Microsoft
		// return: C:\Program Files (x86)

		String fullPath = GetFullPath();
		if (fullPath.Right(end.GetPathLength()).IsSameAs(end.m_Path, StringOpFlag::IgnoreCase))
		{
			fullPath = fullPath.Remove(fullPath.length() - end.GetPathLength(), end.GetPathLength());
		}
		return FSPath(std::move(fullPath)).EnsureNamespaceSet(m_Namespace);
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
