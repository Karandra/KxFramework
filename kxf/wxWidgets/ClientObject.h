#pragma once
#include "Common.h"
#include <wx/object.h>

namespace kxf::wxWidgets
{
	template<class T>
	class ClientObject: public wxObject
	{
		private:
			T m_Object;

		public:
			ClientObject(T object)
				:m_Object(std::move(object))
			{
			}
			ClientObject(ClientObject&&) noexcept = default;
			ClientObject(const ClientObject&) = default;

		public:
			const T& Get() const&
			{
				return m_Object;
			}
			T Get() &&
			{
				return std::move(m_Object);
			}

		public:
			ClientObject& operator=(ClientObject&&) noexcept = default;
			ClientObject& operator=(const ClientObject&) = default;
	};
}
