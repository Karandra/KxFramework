#pragma once
#include "Common.h"
#include "LegacyDrive.h"

namespace KxFileSystem
{
	class KX_API FSPath final
	{
		public:
			FSPath FromStringUnchecked(const wxString& string);

		private:
			wxString m_Path;
			PathNamespace m_Namespace = PathNamespace::None;

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
				return m_Namespace == PathNamespace::Win32FileUNC || m_Namespace == PathNamespace::NetworkUNC;
			}
			size_t GetComponentCount() const;

			bool HasNamespace() const
			{
				return m_Namespace != PathNamespace::None;
			}
			PathNamespace GetNamespace() const
			{
				return m_Namespace;
			}
			FSPath& SetNamespace(PathNamespace ns)
			{
				m_Namespace = ns;
				return *this;
			}
		
			wxString GetFullPath(PathNamespace withNamespace = PathNamespace::None) const;
			wxString GetFullPathWithNS() const
			{
				return GetFullPath(m_Namespace);
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

			FSPath GetParent() const;
			FSPath& RemoveLast();

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
			operator wxString() const
			{
				return GetFullPath();
			}
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
			bool operator!=(const FSPath& other) const
			{
				return !(*this == other);
			}

			FSPath& operator+(const FSPath& other)
			{
				return Concat(other);
			}
			FSPath& operator+(const char* other)
			{
				return Concat(FSPath(other));
			}
			FSPath& operator+(const wchar_t* other)
			{
				return Concat(FSPath(other));
			}
		
			FSPath& operator/(const FSPath& other)
			{
				return Append(other);
			}
			FSPath& operator/(const char* other)
			{
				return Append(FSPath(other));
			}
			FSPath& operator/(const wchar_t* other)
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
	};
}
