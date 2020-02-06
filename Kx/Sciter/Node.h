#pragma once
#include "Common.h"
#include <utility>

namespace KxSciter
{
	class Element;

	struct NodeHandle;
}

namespace KxSciter
{
	class KX_API Node final
	{
		public:
			static Node CreateTextNode(const wxString& value);
			static Node CreateCommentNode(const wxString& value);

		private:
			NodeHandle* m_Handle = nullptr;

		private:
			void Acquire(NodeHandle* handle);
			void Release();

			void CopyFrom(const Node& other);
			void CopyFrom(NodeHandle* handle)
			{
				Release();
				Acquire(handle);
			}
			void MoveFrom(Node& other);

		public:
			Node() = default;
			Node(NodeHandle* handle)
			{
				Acquire(handle);
			}
			Node(const Node& other)
			{
				*this = other;
			}
			Node(Node&& other)
			{
				*this = std::move(other);
			}
			~Node()
			{
				Release();
			}

		public:
			bool IsOk() const
			{
				return m_Handle != nullptr;
			}
			NodeHandle* GetHandle() const
			{
				return m_Handle;
			}
			void MakeNull()
			{
				Release();
			}

			bool AttachHandle(NodeHandle* handle);
			NodeHandle* DetachHandle();
			bool Detach();
			bool Remove();

			bool IsText() const;
			bool IsElement() const;
			bool IsComment() const;

			Element ToElement() const;

			Node GetParent() const;
			Node GetPrevSibling() const;
			Node GetNextSibling() const;
			Node GetFirstChild() const;
			Node GetLastChild() const;

			size_t GetIndexWithinParent() const;
			size_t GetChildrenCount() const;
			Node GetChildAt(size_t index) const;
			Node operator[](size_t index) const
			{
				return GetChildAt(index);
			}

			bool Append(const Node& node);
			bool Prepend(const Node& node);
			bool InsertBefore(const Node& node);
			bool InsertAfter(const Node& node);

			wxString GetValue() const;
			bool SetValue(wxStringView value) const;

		public:
			Node& operator=(const Node& other)
			{
				CopyFrom(other);
				return *this;
			}
			Node& operator=(Node&& other)
			{
				MoveFrom(other);
				return *this;
			}
			Node& operator=(NodeHandle* handle)
			{
				CopyFrom(handle);
				return *this;
			}

			bool operator==(const Node& other) const
			{
				return m_Handle == other.m_Handle;
			}
			bool operator!=(const Node& other) const
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
