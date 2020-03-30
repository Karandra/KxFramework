#pragma once
#include "Common.h"

namespace KxFramework
{
	class LegacyDrive;
}

namespace KxFramework
{
	class KX_API FSPath final
	{
		public:
			static FSPath FromStringUnchecked(const wxString& string, FSPathNamespace ns = FSPathNamespace::None);

		private:
			wxString m_Path;
			FSPathNamespace m_Namespace = FSPathNamespace::None;

		private:
			bool AssignFromPath(const wxString& path);
			void ProcessNamespace();
			void Normalize();

		public:
			FSPath() = default;
			FSPath(const wxString& path)
			{
				AssignFromPath(path);
			}
			FSPath(const char* path)
			{
				AssignFromPath(path);
			}
			FSPath(const wchar_t* path)
			{
				AssignFromPath(path);
			}
			FSPath(const FSPath&) = default;
			FSPath(FSPath&&) = default;
			~FSPath() = default;

		public:
			bool IsValid() const;
			bool IsSameAs(const FSPath& other, bool caseSensitive = false) const;
			bool IsAbsolute() const;
			bool IsRelative() const;
			bool IsUNCPath() const
			{
				return m_Namespace == FSPathNamespace::Win32FileUNC || m_Namespace == FSPathNamespace::NetworkUNC;
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
			
			wxString GetFullPath(FSPathNamespace withNamespace = FSPathNamespace::None) const;
			wxString GetFullPathWithNS(FSPathNamespace withNamespace = FSPathNamespace::None) const
			{
				return GetFullPath(m_Namespace != FSPathNamespace::None ? m_Namespace : withNamespace);
			}

			bool HasDrive() const;
			LegacyDrive GetDrive() const;
			FSPath& SetDrive(const LegacyDrive& drive);

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

			bool operator==(const FSPath& other) const
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
			bool operator!=(const FSPath& other) const
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
			FSPath& operator/=(const char* other)
			{
				return Append(FSPath(other));
			}
			FSPath& operator/=(const wchar_t* other)
			{
				return Append(FSPath(other));
			}

			FSPath& operator=(const FSPath&) = default;
			FSPath& operator=(FSPath&&) = default;
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
