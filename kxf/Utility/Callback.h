#pragma once
#include "Common.h"

namespace kxf::Utility
{
	template<class TItem_>
	class MoveToVectorCallback final
	{
		public:
			using TItem = TItem_;

		private:
			std::vector<TItem>* m_Container = nullptr;

		public:
			MoveToVectorCallback(std::vector<TItem>& ref, size_t reserve = 0) noexcept
				:m_Container(&m_Container)
			{
				if (reserve != 0 && reserve != std::numeric_limits<size_t>::max())
				{
					ref.reserve(reserve);
				}
			}
			MoveToVectorCallback(const MoveToVectorCallback&) = delete;
			MoveToVectorCallback(MoveToVectorCallback&&) = default;

		public:
			CallbackCommand operator()(TItem&& item)
			{
				m_Container->emplace_back(std::move(item));
				return CallbackCommand::Continue;
			}

		public:
			MoveToVectorCallback& operator=(const MoveToVectorCallback&) = delete;
			MoveToVectorCallback& operator=(MoveToVectorCallback&&) = default;
	};
}
