#pragma once
#include "Common.h"

namespace KxFramework
{
	class LegacyVolume;
	class StorageVolume;

	enum class FSPathFormat
	{
		None = 0,
		TrailingSeparator = 1 << 0,
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(FSPathFormat);
	}
}

namespace KxFramework
{
	class KX_API FSPath
	{
		friend class FSPathQuery;

		public:
			static FSPath FromStringUnchecked(String string, FSPathNamespace ns = FSPathNamespace::None);

		protected:
			String m_Path;
			FSPathNamespace m_Namespace = FSPathNamespace::None;
			bool m_SearchMaksAllowed = false;

		protected:
			void AssignFromPath(String path);
			void ProcessNamespace();
			void Normalize();

			bool CheckIsLegacyVolume(const String& path) const;
			bool CheckIsVolumeGUID(const String& path) const;
			size_t DetectNamespacePrefix(const String& path, KxFramework::FSPathNamespace& ns) const;

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
			FSPath(wxString path)
			{
				AssignFromPath(std::move(path));
			}
			FSPath(const char* path)
				:FSPath(String(path))
			{
			}
			FSPath(const wchar_t* path)
				:FSPath(String(path))
			{
			}
			virtual ~FSPath() = default;

		public:
			bool IsValid() const;
			bool IsSameAs(const FSPath& other, bool caseSensitive = false) const;
			bool IsAbsolute() const;
			bool IsRelative() const;
			bool IsUNCPath() const
			{
				return m_Namespace == FSPathNamespace::Win32FileUNC || m_Namespace == FSPathNamespace::NetworkUNC;
			}
			
			bool Contains(const FSPath& path) const;
			bool ContainsCharacters(const String& characters) const
			{
				return m_Path.Contains(characters);
			}
			bool ContainsSearchMask() const
			{
				return m_SearchMaksAllowed && (m_Path.Contains(wxS('*')) || m_Path.Contains(wxS('?')));
			}

			size_t GetPathLength() const
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
			
			String GetFullPath(FSPathNamespace withNamespace = FSPathNamespace::None, FSPathFormat format = FSPathFormat::None) const;
			String GetFullPathWithNS(FSPathNamespace withNamespace = FSPathNamespace::None, FSPathFormat format = FSPathFormat::None) const
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
			LegacyVolume GetLegacyVolume() const;
			FSPath& SetVolume(const LegacyVolume& volume);
			FSPath& SetVolume(const StorageVolume& volume);

			String GetPath() const;
			FSPath& SetPath(const String& path);

			String GetName() const;
			FSPath& SetName(const String& name);

			String GetExtension() const;
			FSPath& SetExtension(const String& ext);

			FSPath GetAfter(const FSPath& start) const;
			FSPath GetBefore(const FSPath& end) const;
			FSPath GetParent() const;
			FSPath& RemoveLastPart();

			FSPath& Append(const FSPath& other);
			FSPath& Append(const char* other)
			{
				return Append(FSPath(other));
			}
			FSPath& Append(const wchar_t* other)
			{
				return Append(FSPath(other));
			}

			FSPath& Concat(const FSPath& other);
			FSPath& Concat(const char* other)
			{
				return Concat(FSPath(other));
			}
			FSPath& Concat(const wchar_t* other)
			{
				return Concat(FSPath(other));
			}

		public:
			explicit operator bool() const noexcept
			{
				return IsValid();
			}
			bool operator!() const noexcept
			{
				return !IsValid();
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
			bool operator==(const wxString& other) const
			{
				return IsSameAs(other, false);
			}
			bool operator==(const String& other) const
			{
				return IsSameAs(other, false);
			}
			bool operator==(const char* other) const
			{
				return IsSameAs(other, false);
			}
			bool operator==(const wchar_t* other) const
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
			bool operator!=(const char* other) const
			{
				return !(*this == other);
			}
			bool operator!=(const wchar_t* other) const
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
			FSPath& operator+=(const char* other)
			{
				return Concat(FSPath(other));
			}
			FSPath& operator+=(const wchar_t* other)
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
			FSPath& operator/=(const char* other)
			{
				return Append(FSPath(other));
			}
			FSPath& operator/=(const wchar_t* other)
			{
				return Append(FSPath(other));
			}

			FSPath& operator=(FSPath&&) = default;
			FSPath& operator=(const FSPath&) = default;
			FSPath& operator=(const String& path)
			{
				AssignFromPath(path);
				return *this;
			}
			FSPath& operator=(const char* path)
			{
				AssignFromPath(path);
				return *this;
			}
			FSPath& operator=(const wchar_t* path)
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

namespace KxFramework
{
	class KX_API FSPathQuery: public FSPath
	{
		private:
			void Init()
			{
				m_SearchMaksAllowed = true;
			}

		public:
			FSPathQuery()
			{
				Init();
			}
			FSPathQuery(FSPathQuery&&) = default;
			FSPathQuery(const FSPathQuery&) = default;
			FSPathQuery(String path)
				:FSPathQuery()
			{
				AssignFromPath(std::move(path));
			}
			FSPathQuery(const char* path)
				:FSPathQuery(String(path))
			{
			}
			FSPathQuery(const wchar_t* path)
				:FSPathQuery(String(path))
			{
			}
			FSPathQuery(const FSPath& path)
				:FSPathQuery()
			{
				m_Path = path.m_Path;
				m_Namespace = path.m_Namespace;
			}
	
		public:
			FSPathQuery GetAfter(const FSPath& start) const
			{
				return FSPath::GetAfter(start);
			}
			FSPathQuery GetBefore(const FSPath& end) const
			{
				return FSPath::GetBefore(end);
			}
			FSPathQuery GetParent() const
			{
				return FSPath::GetParent();
			}

		public:
			FSPathQuery& operator=(FSPathQuery&&) = default;
			FSPathQuery& operator=(const FSPathQuery&) = default;
	};

	template<class T>
	FSPathQuery operator+(FSPathQuery left, T&& right)
	{
		return left.Concat(std::forward<T>(right));
	}

	template<class T>
	FSPathQuery operator/(FSPathQuery left, T&& right)
	{
		return left.Append(std::forward<T>(right));
	}
}
