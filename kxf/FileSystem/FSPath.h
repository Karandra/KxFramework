#pragma once
#include "Common.h"

namespace kxf
{
	class IFileSystem;
	class LegacyVolume;
	class StorageVolume;

	enum class FSPathFormat: uint32_t
	{
		None = 0,
		TrailingSeparator = 1 << 0,
	};
	KxFlagSet_Declare(FSPathFormat);
}

namespace kxf
{
	class KX_API FSPath final
	{
		friend class FSPath;

		public:
			static FSPath FromStringUnchecked(String string, FSPathNamespace ns = FSPathNamespace::None);

		private:
			String m_Path;
			FSPathNamespace m_Namespace = FSPathNamespace::None;

		private:
			void AssignFromPath(String path);
			void ProcessNamespace();
			void Normalize();

			bool CheckIsLegacyVolume(const String& path) const;
			bool CheckIsVolumeGUID(const String& path) const;
			size_t DetectNamespacePrefix(const String& path, kxf::FSPathNamespace& ns) const;

			bool CheckStringOnInitialAssign(const String& path) const;
			bool CheckStringOnAssignPath(const String& path) const;
			bool CheckStringOnAssignName(const String& name) const;

		public:
			FSPath() = default;
			FSPath(FSPath&&) = default;
			FSPath(const FSPath&) = default;
			FSPath(String path)
			{
				AssignFromPath(std::move(path));
			}
			FSPath(const char* path)
			{
				AssignFromPath(path);
			}
			FSPath(const wchar_t* path)
			{
				AssignFromPath(path);
			}
			FSPath(const wxString& path)
				:FSPath(String(path))
			{
			}
			FSPath(wxString&& path)
				:FSPath(String(std::move(path)))
			{
			}
			~FSPath() = default;

		public:
			bool IsNull() const;
			bool IsSameAs(const FSPath& other, bool caseSensitive = false) const;
			bool IsAbsolute() const;
			bool IsRelative() const;
			bool IsUNCPath() const
			{
				return m_Namespace == FSPathNamespace::Win32FileUNC || m_Namespace == FSPathNamespace::NetworkUNC;
			}
			
			bool ContainsPath(const FSPath& path, bool caseSensitive = false) const;
			bool ContainsAnyOfCharacters(const String& characters, bool caseSensitive = false) const
			{
				return m_Path.ContainsAnyOfCharacters(characters, caseSensitive ? StringOpFlag::None : StringOpFlag::IgnoreCase);
			}
			bool ContainsSearchMask() const
			{
				return m_Path.ContainsAnyOfCharacters(wxS("*?"));
			}
			bool MatchesWildcards(const String& expression, FlagSet<StringOpFlag> flags = {}) const
			{
				return m_Path.MatchesWildcards(expression, flags);
			}

			void ReserveLength(size_t capacity)
			{
				m_Path.reserve(std::clamp<size_t>(capacity, 0, std::numeric_limits<int16_t>::max()));
			}
			size_t GetLength() const
			{
				return m_Path.length();
			}
			size_t GetComponentCount() const;
			size_t ForEachComponent(std::function<bool(String)> func) const;

			bool HasNamespace() const
			{
				return m_Namespace != FSPathNamespace::None;
			}
			FSPathNamespace GetNamespace() const
			{
				return m_Namespace;
			}
			FSPath& SetNamespace(FSPathNamespace ns)
			{
				m_Namespace = ns;
				return *this;
			}
			FSPath& EnsureNamespaceSet(FSPathNamespace ns)
			{
				if (m_Namespace == FSPathNamespace::None)
				{
					m_Namespace = ns;
				}
				return *this;
			}
			
			String GetFullPath(FSPathNamespace withNamespace = FSPathNamespace::None, FlagSet<FSPathFormat> format = {}) const;
			String GetFullPathWithNS(FSPathNamespace withNamespace = FSPathNamespace::None, FlagSet<FSPathFormat> format = {}) const
			{
				return GetFullPath(m_Namespace != FSPathNamespace::None ? m_Namespace : withNamespace, format);
			}

			bool HasAnyVolume() const
			{
				return HasVolume() || HasLegacyVolume();
			}
			bool HasVolume() const;
			bool HasLegacyVolume() const;
			StorageVolume GetVolume() const;
			StorageVolume GetAsVolume() const;
			LegacyVolume GetLegacyVolume() const;
			LegacyVolume GetAsLegacyVolume() const;
			FSPath& SetVolume(const LegacyVolume& volume);
			FSPath& SetVolume(const StorageVolume& volume);

			String GetPath() const;
			FSPath& SetPath(const String& path);
			FSPath& SimplifyPath();

			String GetName() const;
			FSPath& SetName(const String& name);

			String GetExtension() const;
			FSPath& SetExtension(const String& ext);

			FSPath GetAfter(const FSPath& start) const;
			FSPath GetBefore(const FSPath& end) const;
			FSPath GetParent() const;
			FSPath& RemoveLastPart();

			FSPath& Append(const FSPath& other);
			FSPath& Append(const XChar* other)
			{
				return Append(FSPath(other));
			}

			FSPath& Concat(const FSPath& other);
			FSPath& Concat(const XChar* other)
			{
				return Concat(FSPath(other));
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			operator const String&() const&
			{
				// Without any formatting options we can just return normalized internal representation
				return m_Path;
			}
			operator String() const&&
			{
				return std::move(m_Path);
			}

			bool operator==(const FSPath& other) const
			{
				return IsSameAs(other, false);
			}
			bool operator==(const String& other) const
			{
				return IsSameAs(other, false);
			}
			bool operator==(const XChar* other) const
			{
				return IsSameAs(other, false);
			}
			
			template<class T>
			bool operator!=(T&& other) const
			{
				return !(*this == other);
			}

			bool operator!=(const String& other) const
			{
				return !(*this == other);
			}
			bool operator!=(const XChar* other) const
			{
				return !(*this == other);
			}

			FSPath& operator+=(const FSPath& other)
			{
				return Concat(other);
			}
			FSPath& operator+=(const String& other)
			{
				return Concat(other);
			}
			FSPath& operator+=(const XChar* other)
			{
				return Concat(FSPath(other));
			}

			FSPath& operator/=(const FSPath& other)
			{
				return Append(other);
			}
			FSPath& operator/=(const String& other)
			{
				return Append(other);
			}
			FSPath& operator/=(const XChar* other)
			{
				return Append(other);
			}

			FSPath& operator=(FSPath&&) = default;
			FSPath& operator=(const FSPath&) = default;
			FSPath& operator=(const String& path)
			{
				AssignFromPath(path);
				return *this;
			}
	};

	template<class T>
	FSPath operator+(FSPath left, T&& right)
	{
		return left.Concat(std::forward<T>(right));
	}

	template<class T>
	FSPath operator/(FSPath left, T&& right)
	{
		return left.Append(std::forward<T>(right));
	}
}
