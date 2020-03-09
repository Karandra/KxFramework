#pragma once
#include "Common.h"
#include "LegacyDrive.h"

namespace KxFileSystem
{
	class KX_API KxFSPath final
	{
		public:
			KxFSPath FromStringUnchecked(const wxString& string);

		private:
			wxString m_Path;
			PathNamespace m_Namespace = PathNamespace::None;

		private:
			bool AssignFromPath(const wxString& path);
			void ProcessNamespace();
			void Normalize();

		public:
			KxFSPath() = default;
			KxFSPath(const wxString& path)
			{
				AssignFromPath(path);
			}
			KxFSPath(const char* path)
			{
				AssignFromPath(path);
			}
			KxFSPath(const wchar_t* path)
			{
				AssignFromPath(path);
			}
			KxFSPath(const KxFSPath&) = default;
			KxFSPath(KxFSPath&&) = default;
			~KxFSPath() = default;

		public:
			bool IsValid() const;
			bool IsSameAs(const KxFSPath& other, bool caseSensitive = false) const;
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
			KxFSPath& SetNamespace(PathNamespace ns)
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
			KxFSPath& SetDrive(const LegacyDrive& drive);

			wxString GetPath() const;
			KxFSPath& SetPath(const wxString& path);

			wxString GetName() const;
			KxFSPath& SetName(const wxString& name);

			wxString GetExtension() const;
			KxFSPath& SetExtension(const wxString& ext);

			KxFSPath GetParent() const;
			KxFSPath& RemoveLast();

			KxFSPath& Append(const KxFSPath& other);
			KxFSPath& Append(const char* other)
			{
				return Append(KxFSPath(other));
			}
			KxFSPath& Append(const wchar_t* other)
			{
				return Append(KxFSPath(other));
			}

			KxFSPath& Concat(const KxFSPath& other);
			KxFSPath& Concat(const char* other)
			{
				return Concat(KxFSPath(other));
			}
			KxFSPath& Concat(const wchar_t* other)
			{
				return Concat(KxFSPath(other));
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

			bool operator==(const KxFSPath& other) const
			{
				return IsSameAs(other, false);
			}
			bool operator!=(const KxFSPath& other) const
			{
				return !(*this == other);
			}

			KxFSPath& operator+(const KxFSPath& other)
			{
				return Concat(other);
			}
			KxFSPath& operator+(const char* other)
			{
				return Concat(KxFSPath(other));
			}
			KxFSPath& operator+(const wchar_t* other)
			{
				return Concat(KxFSPath(other));
			}
		
			KxFSPath& operator/(const KxFSPath& other)
			{
				return Append(other);
			}
			KxFSPath& operator/(const char* other)
			{
				return Append(KxFSPath(other));
			}
			KxFSPath& operator/(const wchar_t* other)
			{
				return Append(KxFSPath(other));
			}

			KxFSPath& operator=(const KxFSPath&) = default;
			KxFSPath& operator=(KxFSPath&&) = default;
			KxFSPath& operator=(const wxString& path)
			{
				AssignFromPath(path);
				return *this;
			}
	};
}
