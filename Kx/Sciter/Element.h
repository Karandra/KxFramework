#pragma once
#include "Common.h"
#include <utility>

namespace KxSciter
{
	class Node;
}

namespace KxSciter
{
	class KX_API Element final
	{
		private:
			void* m_Handle = nullptr;

		private:
			void Acquire(void* handle);
			void Release();

			void CopyFrom(const Element& other);
			void CopyFrom(void* handle)
			{
				Release();
				Acquire(handle);
			}
			void MoveFrom(Element& other);

		public:
			Element() = default;
			explicit Element(void* handle)
			{
				Acquire(handle);
			}
			Element(const Element& other)
			{
				CopyFrom(other);
			}
			Element(Element&& other)
			{
				MoveFrom(other);
			}
			~Element()
			{
				Release();
			}
			
		public:
			bool IsOk() const
			{
				return m_Handle != nullptr;
			}
			void* GetHandle() const
			{
				return m_Handle;
			}
			void MakeNull()
			{
				Release();
			}

			bool Attach(void* handle);
			void* Detach();
			bool Remove();

			Node ToNode() const;
			Element Clone() const;

			Element GetParent() const;
			Element GetPrevSibling() const;
			Element GetNextSibling() const;
			Element GetFirstChild() const;
			Element GetLastChild() const;

			size_t GetIndexWithinParent() const;
			size_t GetChildrenCount() const;
			Element GetChildAt(size_t index) const;
			Element operator[](size_t index) const
			{
				return GetChildAt(index);
			}
			
			bool Append(const Element& node);
			bool Prepend(const Element& node);
			bool InsertAt(const Element& node, size_t index);
			bool InsertBefore(const Element& node);
			bool InsertAfter(const Element& node);

			wxString GetValue() const;
			bool SetValue(wxStringView value) const;

			wxString GetStyleAttribute(const wxString& name) const;
			bool SetStyleAttribute(const wxString& name, const wxString& value);

		public:
			Element& operator=(const Element& other)
			{
				CopyFrom(other);
				return *this;
			}
			Element& operator=(Element&& other)
			{
				MoveFrom(other);
				return *this;
			}
			Element& operator=(void* handle)
			{
				CopyFrom(handle);
				return *this;
			}
	
			bool operator==(const Element& other) const
			{
				return m_Handle == other.m_Handle;
			}
			bool operator!=(const Element& other) const
			{
				return !(*this == other);
			}

			explicit operator bool() const
			{
				return IsOk();
			}
			bool operator!() const
			{
				return !IsOk();
			}
	};
}
