#pragma once
#include "Common.h"
#include "kxf/General/String.h"
#include "kxf/General/DateTime/TimeSpan.h"

namespace kxf::Private
{
	class KX_API BasicMutex
	{
		private:
			void* m_Handle = nullptr;

		protected:
			bool Create(const String& name, bool acquireInitially, bool isGlobal) noexcept;
			bool Open(const String& name, bool isGlobal) noexcept;

		protected:
			BasicMutex() noexcept = default;
			BasicMutex(const BasicMutex&) = delete;
			BasicMutex(BasicMutex&& other) noexcept
				:m_Handle(other.m_Handle)
			{
				other.m_Handle = nullptr;
			}
			~BasicMutex() noexcept
			{
				Destroy();
			}

		public:
			bool IsNull() const noexcept
			{
				return m_Handle == nullptr;
			}
			void* GetHandle() const noexcept
			{
				return m_Handle;
			}

			void Destroy() noexcept;
			bool Acquire(const TimeSpan& timeout = {}) noexcept;
			bool Release() noexcept;

		public:
			explicit operator bool() const noexcept
			{
				return !IsNull();
			}
			bool operator!() const noexcept
			{
				return IsNull();
			}

			BasicMutex& operator=(const BasicMutex&) = delete;
			BasicMutex& operator=(BasicMutex&& other) noexcept
			{
				Destroy();

				m_Handle = other.m_Handle;
				other.m_Handle = nullptr;

				return *this;
			}
	};
}

namespace kxf
{
	class LocalMutex final: public Private::BasicMutex
	{
		public:
			LocalMutex() noexcept = default;

		public:
			bool Create(const String& name = {})
			{
				return BasicMutex::Create(name, false, false);
			}
			bool CreateAcquired(const String& name = {})
			{
				return BasicMutex::Create(name, true, false);
			}
			bool Open(const String& name)
			{
				return BasicMutex::Open(name, false);
			}
	};

	class GlobalMutex final: public Private::BasicMutex
	{
		public:
			GlobalMutex() noexcept = default;

		public:
			bool Create(const String& name = {})
			{
				return BasicMutex::Create(name, false, true);
			}
			bool CreateAcquired(const String& name = {})
			{
				return BasicMutex::Create(name, true, true);
			}
			bool Open(const String& name)
			{
				return BasicMutex::Open(name, true);
			}
	};
}
