#pragma once
#include "Common.h"

namespace kxf::Utility
{
	template<class TItem_>
	class VectorCallbackAdapter final
	{
		public:
			using TItem = TItem_;

		private:
			std::vector<TItem>* m_Container = nullptr;

		public:
			VectorCallbackAdapter(std::vector<TItem>& ref, size_t reserve = 0) noexcept
				:m_Container(&ref)
			{
				if (reserve != 0 && reserve != std::numeric_limits<size_t>::max())
				{
					ref.reserve(reserve);
				}
			}
			VectorCallbackAdapter(const VectorCallbackAdapter&) = default;
			VectorCallbackAdapter(VectorCallbackAdapter&&) = default;

		public:
			CallbackCommand operator()(TItem&& item)
			{
				m_Container->emplace_back(std::move(item));
				return CallbackCommand::Continue;
			}

		public:
			VectorCallbackAdapter& operator=(const VectorCallbackAdapter&) = default;
			VectorCallbackAdapter& operator=(VectorCallbackAdapter&&) = default;
	};
}
