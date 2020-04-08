#pragma once
#include "Common.h"
#include "Utility/HandleWrapper.h"
#include <utility>

namespace KxFramework::Sciter
{
	class Element;
	struct NodeHandle;
}

namespace KxFramework::Sciter
{
	class KX_API Node final: public HandleWrapper<Node, NodeHandle>
	{
		friend class HandleWrapper<Node, NodeHandle>;

		public:
			static Node CreateTextNode(const String& value);
			static Node CreateCommentNode(const String& value);

		private:
			NodeHandle* m_Handle = nullptr;

		private:
			bool DoAcquire(NodeHandle* handle);
			void DoRelease();

		public:
			Node() = default;
			Node(NodeHandle* handle)
				:HandleWrapper(handle)
			{
			}
			Node(const Node& other)
				:HandleWrapper(other)
			{
			}
			Node(Node&& other)
				:HandleWrapper(std::move(other))
			{
			}

		public:
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

			String GetValue() const;
			bool SetValue(StringView value) const;

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
	};
}
