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
			static FSPath FromStringUnchecked(const wxString& string, FSPathNamespace ns = FSPathNamespace::None);

		protected:
			wxString m_Path;
			FSPathNamespace m_Namespace = FSPathNamespace::None;
			bool m_SearchMaksAllowed = false;

		protected:
			bool AssignFromPath(const wxString& path);
			void ProcessNamespace();
			void Normalize();

			bool CheckIsLegacyVolume(const wxString& path) const;
			bool CheckIsVolumeGUID(const wxString& path) const;
			size_t DetectNamespacePrefix(const wxString& path, KxFramework::FSPathNamespace& ns) const;

			bool CheckStringOnInitialAssign(const wxString& path) const;
			bool CheckStringOnAssignPath(const wxString& path) const;
			bool CheckStringOnAssignName(const wxString& name) const;

		public:
			FSPath() = default;
			FSPath(FSPath&&) = default;
			FSPath(const FSPath&) = default;
			FSPath(const wxString& path)
			{
				AssignFromPath(path);
			}
			FSPath(const char* path)
				:FSPath(wxString(path))
			{
			}
			FSPath(const wchar_t* path)
				:FSPath(wxString(path))
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
			bool ContainsCharacters(const wxString& characters) const
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
			size_t ForEachComponent(std::function<bool(const wxString&)> func) const;

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
			
			wxString GetFullPath(FSPathNamespace withNamespace = FSPathNamespace::None, FSPathFormat format = FSPathFormat::None) const;
			wxString GetFullPathWithNS(FSPathNamespace withNamespace = FSPathNamespace::None, FSPathFormat format = FSPathFormat::None) const
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

			wxString GetPath() const;
			FSPath& SetPath(const wxString& path);

			wxString GetName() const;
			FSPath& SetName(const wxString& name);

			wxString GetExtension() const;
			FSPath& SetExtension(const wxString& ext);

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

			operator const wxString&() const
			{
				// Without any formatting options we can just return normalized internal representation
				return m_Path;
			}

			bool operator==(const FSPath& other) const
			{
				return IsSameAs(other, false);
			}
			bool operator==(const wxString& other) const
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

			bool operator!=(const wxString& other) const
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
			FSPath& operator+=(const wxString& other)
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
			FSPath& operator/=(const wxString& other)
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
			FSPath& operator=(const wxString& path)
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
	FSPath operator+(const FSPath& left, T&& right)
	{
		return FSPath(left).Concat(std::forward<T>(right));
	}

	template<class T>
	FSPath operator/(const FSPath& left, T&& right)
	{
		return FSPath(left).Append(std::forward<T>(right));
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
			FSPathQuery(const wxString& path)
				:FSPathQuery()
			{
				AssignFromPath(path);
			}
			FSPathQuery(const char* path)
				:FSPathQuery(wxString(path))
			{
			}
			FSPathQuery(const wchar_t* path)
				:FSPathQuery(wxString(path))
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
	};

	template<class T>
	FSPathQuery operator+(const FSPathQuery& left, T&& right)
	{
		return FSPathQuery(left).Concat(std::forward<T>(right));
	}

	template<class T>
	FSPathQuery operator/(const FSPathQuery& left, T&& right)
	{
		return FSPathQuery(left).Append(std::forward<T>(right));
	}
}
