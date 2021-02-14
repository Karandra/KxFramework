#pragma once
#include "Common.h"
#include "kxf/EventSystem/EventID.h"
#include <wx/object.h>
#include <wx/event.h>

namespace kxf::wxWidgets
{
	template<class T>
	class EventTagRef final
	{
		private:
			const wxEventTypeTag<T>& m_ID;

		private:
			EventTag<T> DoGet() const
			{
				return static_cast<wxEventType>(m_ID);
			}

		public:
			EventTagRef(const wxEventTypeTag<T>& id) noexcept
				:m_ID(id)
			{
			}
			EventTagRef(EventTagRef&&) = delete;
			EventTagRef(const EventTagRef&) = delete;

		public:
			bool IsNull() const noexcept
			{
				return DoGet().IsNull();
			}

			const wxEventTypeTag<T>& ToWxTag() const noexcept
			{
				return m_ID;
			}
			EventTag<T> operator*() const noexcept
			{
				return DoGet();
			}

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			operator const wxEventTypeTag<T>&() const noexcept
			{
				return m_ID;
			}
			operator EventTag<T>() const noexcept
			{
				return DoGet();
			}

			bool operator==(const EventTagRef& other) const noexcept
			{
				return m_ID == other.m_ID;
			}
			bool operator!=(const EventTagRef& other) const noexcept
			{
				return m_ID != other.m_ID;
			}

			bool operator==(const EventTag<T>& other) const noexcept
			{
				return DoGet() == other;
			}
			bool operator!=(const EventTag<T>& other) const noexcept
			{
				return DoGet() != other;
			}

			EventTagRef& operator=(EventTagRef&&) = delete;
			EventTagRef& operator=(const EventTagRef&) = delete;
	};
}
