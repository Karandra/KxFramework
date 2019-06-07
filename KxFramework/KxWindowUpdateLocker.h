#pragma once
#include "KxFramework/KxFramework.h"

class KxWindowUpdateLocker final
{
	private:
		wxWindow* m_Window = nullptr;

	public:
		KxWindowUpdateLocker(wxWindow* window)
			:m_Window(window)
		{
			if (m_Window)
			{
				m_Window->Freeze();
			}
		}
		KxWindowUpdateLocker(const KxWindowUpdateLocker&) = delete;
		KxWindowUpdateLocker(KxWindowUpdateLocker&& other)
		{
			*this = std::move(other);
		}
		~KxWindowUpdateLocker()
		{
			if (m_Window)
			{
				m_Window->Thaw();
			}
		}

	public:
		KxWindowUpdateLocker& operator=(const KxWindowUpdateLocker&) = delete;
		KxWindowUpdateLocker& operator=(KxWindowUpdateLocker&& other)
		{
			m_Window = other.m_Window;
			other.m_Window = nullptr;

			return *this;
		}
};
