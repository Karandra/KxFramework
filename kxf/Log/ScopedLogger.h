#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DateTime.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/System/SystemThread.h"
#include "kxf/Threading/ThreadLocalSlot.h"
#include "kxf/Utility/Common.h"
#include <source_location>
#include <atomic>
#include <vector>
#include <map>

namespace kxf
{
	class ScopedLoggerTLS;
	class ScopedLoggerScope;
	class ScopedLoggerGlobalContext;
	class ScopedMessageLogger;
}

namespace kxf
{
	class IScopedLoggerTarget: public RTTI::Interface<IScopedLoggerTarget>
	{
		KxRTTI_DeclareIID(IScopedLoggerTarget, {0xddebd088, 0x6198, 0x46aa, {0x9f, 0xe9, 0xc2, 0xda, 0xc5, 0xb4, 0x60, 0x9a}});

		public:
			virtual ~IScopedLoggerTarget() = default;

		public:
			virtual void Write(LogLevel logLevel, StringView str) = 0;
			virtual void Flush() = 0;

			virtual String FormatRecord(const ScopedLoggerTLS& tls, LogLevel logLevel, DateTime timestamp, StringView message) const
			{
				return {};
			}
	};

	class IScopedLoggerContext: public RTTI::Interface<IScopedLoggerContext>
	{
		KxRTTI_DeclareIID(IScopedLoggerContext, {0x7dd1c550, 0xe0a1, 0x4f92, {0x8c, 0x3c, 0x3f, 0xd8, 0x46, 0x3c, 0x80, 0xd2 }});

		public:
			virtual ~IScopedLoggerContext() = default;

		public:
			virtual std::shared_ptr<IScopedLoggerTarget> CreateLogTarget(ScopedLoggerTLS& tls) = 0;
	};

}

namespace kxf
{
	class ScopedLoggerGlobalContext final
	{
		public:
			static ScopedLoggerGlobalContext& Initialize(std::shared_ptr<IScopedLoggerContext> userContext);
			static ScopedLoggerGlobalContext& GetInstance() noexcept;

		private:
			std::shared_ptr<IScopedLoggerContext> m_UserContext;
			std::atomic<TimeZoneOffset> m_TimeOffset;
			std::atomic<LogLevel> m_LogLevel = LogLevel::Unknown;

			std::unique_ptr<ScopedLoggerTLS> m_UnknownContext;
			FiberLocalSlot m_TLSIndex;

		private:
			void Initialize();
			void Destroy();

		private:
			ScopedLoggerGlobalContext(std::shared_ptr<IScopedLoggerContext> userContext)
				:m_UserContext(std::move(userContext))
			{
				Initialize();
			}
			ScopedLoggerGlobalContext(const ScopedLoggerGlobalContext&) = delete;
			~ScopedLoggerGlobalContext()
			{
				Destroy();
			}

		public:
			std::shared_ptr<IScopedLoggerContext> GetUserContext() const noexcept
			{
				return m_UserContext;
			}

			ScopedLoggerTLS* QueryThreadContext() const noexcept;
			ScopedLoggerTLS& GetThreadContext();
			ScopedLoggerTLS& GetUnknownThreadContext() const noexcept
			{
				return *m_UnknownContext;
			}

			LogLevel GetLogLevel() const noexcept
			{
				return m_LogLevel;
			}
			void SetLogLevel(LogLevel logLevel) noexcept
			{
				if (ToInt(logLevel) >= 0)
				{
					m_LogLevel = logLevel;
				}
			}
			bool CanLogLevel(LogLevel logLevel) const noexcept;

			TimeZoneOffset GetTimeOffset() const noexcept
			{
				return m_TimeOffset;
			}
			void SetTimeOffset(const TimeZoneOffset& timeOffset)
			{
				m_TimeOffset = timeOffset;
			}
	};

	class ScopedLoggerTLS
	{
		friend class ScopedLogger;
		friend class ScopedLoggerGlobalContext;

		protected:
			ScopedLoggerGlobalContext& m_GlobalContext;
			std::shared_ptr<IScopedLoggerTarget> m_LogTarget;
			std::vector<ScopedLogger*> m_ScopeStack;
			SystemThread m_Thread;
			SystemProcess m_Process;
			DateTime m_TimeStamp;
			size_t m_ScopeLevel = 0;
			bool m_Terminated = false;

		protected:
			virtual void Initialize();
			virtual void Destroy();

			void LogOpenClose(bool open);
			String FormatRecord(LogLevel logLevel, DateTime timestamp, StringView str) const;
			std::shared_ptr<IScopedLoggerTarget> CreateLogTarget();

			void OnScopeEnter(ScopedLogger& scope)
			{
				m_ScopeLevel++;
				m_ScopeStack.emplace_back(&scope);
			}
			void OnScopeLeave(ScopedLogger& scope)
			{
				m_ScopeLevel--;
				m_ScopeStack.pop_back();
			}
			void OnTermination()
			{
				if (!m_Terminated)
				{
					m_Terminated = true;
					Destroy();
				}
			}

		public:
			ScopedLoggerTLS(ScopedLoggerGlobalContext& globalContext)
				:m_GlobalContext(globalContext)
			{
			}
			virtual ~ScopedLoggerTLS()
			{
				Destroy();
			}

		public:
			ScopedLoggerGlobalContext& GetGlobalContext() const
			{
				return m_GlobalContext;
			}
			ScopedLogger* GetCurrentScope() const
			{
				if (!m_ScopeStack.empty())
				{
					return m_ScopeStack.back();
				}
				return nullptr;
			}

			SystemProcess GetProcess() const noexcept
			{
				return m_Process;
			}
			SystemThread GetThread() const noexcept
			{
				return m_Thread;
			}
			DateTime GetTimestamp() const noexcept
			{
				return m_TimeStamp;
			}
			size_t GetScopeLevel() const noexcept
			{
				return m_ScopeLevel;
			}
			bool IsTerminated() const noexcept
			{
				return m_Terminated;
			}
			bool IsUnknown() const noexcept
			{
				return m_Thread.IsNull();
			}

			void Flush();
			void Write(LogLevel logLevel, DateTime timestamp, StringView message);
	};
}

namespace kxf
{
	class ScopedMessageLogger final
	{
		friend class ScopedLoggerTLS;
		friend class ScopedLoggerUnknownTLS;

		private:
			ScopedLogger* m_Scope = nullptr;
			String m_Buffer;
			String m_Separator;
			bool m_SeparatorAllowed = false;
			DateTime m_TimeStamp;
			LogLevel m_LogLevel = LogLevel::Unknown;

		private:
			void Reset()
			{
				m_Buffer.clear();
				m_Separator.clear();
				m_SeparatorAllowed = false;
				m_TimeStamp = DateTime::Now();
			}
			bool CanLog() const
			{
				return m_LogLevel != LogLevel::Disabled;
			}

			template<class T>
			void DoLog(const std::basic_string<T>& str)
			{
				DoLog(StringViewOf(str));
			}

			template<class T>
			void DoLog(std::basic_string_view<T> str)
			{
				if (!str.empty() && CanLog())
				{
					ProcessSeparator();
					if constexpr(std::is_same_v<T, wchar_t>)
					{
						m_Buffer += str;
					}
					else
					{
						m_Buffer += String::FromUTF8(str);
					}
				}
			}

			void DoLog(const String& str)
			{
				DoLog(str.xc_view());
			}
			void DoLog(const char* str)
			{
				DoLog(StringViewOf(str));
			}
			void DoLog(const wchar_t* str)
			{
				DoLog(StringViewOf(str));
			}
			void ProcessSeparator()
			{
				if (m_SeparatorAllowed && !m_Separator.empty() && !m_Buffer.empty())
				{
					m_Buffer += m_Separator;
					m_SeparatorAllowed = false;
				}
			}

			template<class... Args>
			void DoLogArgs(String separator, Args&&... arg)
			{
				SetSeparator(std::move(separator));
				std::initializer_list<int> list{(*this << std::forward<Args>(arg), Sep(), 0) ...};
			}

		public:
			ScopedMessageLogger(LogLevel logLevel = LogLevel::Unknown)
				:m_LogLevel(logLevel)
			{
				Reset();
			}
			ScopedMessageLogger(ScopedLogger& scope, LogLevel logLevel = LogLevel::Unknown)
				:m_Scope(&scope), m_LogLevel(logLevel)
			{
				Reset();
			}
			ScopedMessageLogger(const ScopedMessageLogger&) = delete;
			ScopedMessageLogger(ScopedMessageLogger&&) = default;
			~ScopedMessageLogger()
			{
				Write();
			}

		public:
			ScopedLogger* GetScope() const noexcept
			{
				return m_Scope;
			}
			DateTime GetTimeStamp() const noexcept
			{
				return m_TimeStamp;
			}

			ScopedLogger* DetachScope()
			{
				auto ptr = m_Scope;
				m_Scope = nullptr;

				return ptr;
			}
			StringView ToString() const noexcept
			{
				return m_Buffer.xc_str();
			}

			void Write();
			void Write(ScopedLoggerTLS& tls)
			{
				tls.Write(m_LogLevel, m_TimeStamp, m_Buffer.xc_view());
			}

			void Finalize()
			{
				Write();
				Reset();
			}
			void Finalize(ScopedLoggerTLS& tls)
			{
				Write(tls);
				Reset();
			}

		public:
			template<class... Args>
			ScopedMessageLogger& Log(Args&&... arg)
			{
				DoLogArgs({}, std::forward<Args>(arg)...);
				return *this;
			}

			template<class... Args>
			ScopedMessageLogger& Print(Args&&... arg)
			{
				DoLogArgs(", ", std::forward<Args>(arg)...);
				return *this;
			}

			template<class... Args>
			ScopedMessageLogger& PrintSep(String separator, Args&&... arg)
			{
				DoLogArgs(separator, std::forward<Args>(arg)...);
				return *this;
			}

			template<class TFormat, class... Args>
			ScopedMessageLogger& Format(const TFormat& format, Args&&... arg)
			{
				ProcessSeparator();
				m_Buffer.Format(format, std::forward<Args>(arg)...);
				return *this;
			}

			ScopedMessageLogger& LineBreak()
			{
				DoLog("\n");
				return *this;
			}
			ScopedMessageLogger& Sep()
			{
				m_SeparatorAllowed = true;
				return *this;
			}
			ScopedMessageLogger& SetSeparator(String separator)
			{
				m_Separator = std::move(separator);
				return *this;
			}

			ScopedMessageLogger& operator<<(const String& str)
			{
				DoLog(StringViewOf(str));
				return *this;
			}
			ScopedMessageLogger& operator<<(std::string_view str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(std::wstring_view str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const std::string& str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const std::wstring& str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const char* str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(const wchar_t* str)
			{
				DoLog(str);
				return *this;
			}
			ScopedMessageLogger& operator<<(char c)
			{
				DoLog(std::string_view(&c, 1));
				return *this;
			}
			ScopedMessageLogger& operator<<(wchar_t c)
			{
				DoLog(std::wstring_view(&c, 1));
				return *this;
			}
			ScopedMessageLogger& operator<<(bool b)
			{
				DoLog(b ? "true" : "false");
				return *this;
			}

			template<class T> requires(std::is_arithmetic_v<T>)
			ScopedMessageLogger& operator<<(T value)
			{
				DoLog(std::to_wstring(value));
				return *this;
			}

			template<class T> requires(std::is_enum_v<T>)
			ScopedMessageLogger& operator<<(T value)
			{
				DoLog(std::to_wstring(static_cast<std::underlying_type_t<T>>(value)));
				return *this;
			}

			template<class T> requires(std::is_pointer_v<T>)
			ScopedMessageLogger& operator<<(T value)
			{
				DoLog(String().Format("0x{:0{}x}", reinterpret_cast<intptr_t>(value), sizeof(void*) * 2));
				return *this;
			}

			template<class T>
			ScopedMessageLogger& operator<<(const std::optional<T>& opt)
			{
				if (opt)
				{
					*this << "optional(" << *opt << ')';
				}
				else
				{
					DoLog("optional(nullopt)");
				}
				return *this;
			}

		public:
			ScopedMessageLogger& operator=(const ScopedMessageLogger&) = delete;
			ScopedMessageLogger& operator=(ScopedMessageLogger&&) = default;
	};
}

namespace kxf
{
	class ScopedLogger
	{
		protected:
			ScopedLoggerTLS& m_ScopeTLS;
			uint32_t m_Line = 0;
			uint32_t m_Column = 0;
			String m_FileName;
			String m_Function;
			String m_ReturnValue;
			bool m_IsSuccess = false;
			bool m_IsVoid = false;

		protected:
			void FromSourceLocation(const std::source_location& sourceLocation)
			{
				m_Line = sourceLocation.line();
				m_Column = sourceLocation.column();
				m_FileName = sourceLocation.file_name();
				m_Function = sourceLocation.function_name();
			}
			bool CanEnterLeave() const noexcept
			{
				return !m_Function.IsEmpty();
			}

			void OnEnter(StringView serializedParameters = {});
			void OnLeave();

			ScopedMessageLogger FlowControl()
			{
				return ScopedMessageLogger(*this, LogLevel::FlowControl);
			}

		protected:
			ScopedLogger(ScopedLoggerTLS& tls)
				:m_ScopeTLS(tls)
			{
			}

			template<class... Args>
			ScopedLogger(ScopedLoggerTLS& tls, const std::source_location& sourceLocation, Args&&... arg)
				:m_ScopeTLS(tls)
			{
				FromSourceLocation(sourceLocation);

				if (!Utility::IsEmptyParameterPack<Args...>())
				{
					ScopedMessageLogger logger;
					logger.PrintSep(", ", std::forward<Args>(arg)...);
					OnEnter(logger.ToString());
				}
				else
				{
					OnEnter();
				}
			}

			~ScopedLogger()
			{
				OnLeave();
			}

		public:
			ScopedLoggerTLS& GetTLS() const noexcept
			{
				return m_ScopeTLS;
			}
			size_t GetScopeLevel() const noexcept
			{
				return m_ScopeTLS.GetScopeLevel();
			}

			ScopedMessageLogger Log()
			{
				return ScopedMessageLogger(*this, LogLevel::Unknown);
			}
			ScopedMessageLogger Trace()
			{
				return ScopedMessageLogger(*this, LogLevel::Trace);
			}
			ScopedMessageLogger Debug()
			{
				return ScopedMessageLogger(*this, LogLevel::Debug);
			}
			ScopedMessageLogger Info()
			{
				return ScopedMessageLogger(*this, LogLevel::Information);
			}
			ScopedMessageLogger Warning()
			{
				return ScopedMessageLogger(*this, LogLevel::Warning);
			}
			ScopedMessageLogger Error()
			{
				return ScopedMessageLogger(*this, LogLevel::Error);
			}
			ScopedMessageLogger Critical()
			{
				return ScopedMessageLogger(*this, LogLevel::Critical);
			}

			void SetSuccess(bool success = true) noexcept
			{
				m_IsSuccess = success;
			}
			void SetFail(bool fail = true) noexcept
			{
				m_IsSuccess = !fail;
			}

			template<class T>
			void LogReturn(const T& value, bool success = true)
			{
				m_IsSuccess = success;
				if (!m_IsVoid)
				{
					ScopedMessageLogger messageLogger(LogLevel::FlowControl);
					messageLogger << value;

					m_ReturnValue = messageLogger.ToString();
				}
			}

			template<class T>
			ScopedMessageLogger operator<<(T&& value)
			{
				ScopedMessageLogger messageLogger(*this, LogLevel::Unknown);
				messageLogger << std::forward<T>(value);

				return messageLogger;
			}
	};

	class ScopedLoggerScope final: public ScopedLogger
	{
		public:
			ScopedLoggerScope(const std::source_location& sourceLocation = std::source_location::current())
				:ScopedLogger(ScopedLoggerGlobalContext::GetInstance().GetThreadContext(), sourceLocation)
			{
			}

			template<class... Args>
			ScopedLoggerScope(const std::source_location& sourceLocation, Args&&... arg)
				: ScopedLogger(ScopedLoggerGlobalContext::GetInstance().GetThreadContext(), sourceLocation)
			{
			}
	};

	class ScopedLoggerAuto final: public ScopedLogger
	{
		private:
			static ScopedLoggerTLS& GetActiveTLS() noexcept
			{
				auto& global = ScopedLoggerGlobalContext::GetInstance();
				if (auto tls = global.QueryThreadContext())
				{
					return *tls;
				}
				else
				{
					return global.GetUnknownThreadContext();
				}
			}

		private:
			ScopedLogger* m_Scope = nullptr;

		public:
			ScopedLoggerAuto()
				:ScopedLogger(GetActiveTLS())
			{
				m_Scope = m_ScopeTLS.GetCurrentScope();
			}

		public:
			ScopedLogger& GetCurrentScope() const noexcept
			{
				return *m_Scope;
			}
	};
}

namespace kxf
{
	class ScopedLoggerUnknown final: public ScopedLogger
	{
		friend class ScopedLoggerUnknownTLS;

		private:
			void Initialize();
			void Destroy();

		private:
			ScopedLoggerUnknown(ScopedLoggerTLS& tls)
				:ScopedLogger(tls)
			{
			}
	};

	class ScopedLoggerUnknownTLS final: public ScopedLoggerTLS
	{
		private:
			ScopedLoggerUnknown m_Scope;

		private:
			void Initialize() override;
			void Destroy() override;

		public:
			ScopedLoggerUnknownTLS(ScopedLoggerGlobalContext& globalContext)
				:ScopedLoggerTLS(globalContext), m_Scope(*this)
			{
			}
	};
}

namespace kxf
{
	String ToString(LogLevel value);
}
namespace kxf::Log
{
	template<class TFormat, class... Args>
	void Critical(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAuto().Critical().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Error(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAuto().Error().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Warning(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAuto().Warning().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Info(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAuto().Info().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Debug(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAuto().Debug().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Trace(const TFormat& format, Args&&... arg)
	{
		ScopedLoggerAuto().Trace().Format(format, std::forward<Args>(arg)...);
	}
}

#define KX_SCOPEDLOG				scopedLogger_
#define KX_SCOPEDLOG_AUTO			kxf::ScopedLoggerAuto		KX_SCOPEDLOG;
#define KX_SCOPEDLOG_FUNC			kxf::ScopedLoggerScope		KX_SCOPEDLOG(std::source_location::current());
#define KX_SCOPEDLOG_ARGS(...)		kxf::ScopedLoggerScope		KX_SCOPEDLOG(std::source_location::current(), __VA_ARGS__);
