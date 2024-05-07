#pragma once
#include "Common.h"
#include "kxf/RTTI/RTTI.h"
#include "kxf/Core/String.h"
#include "kxf/Core/DateTime.h"
#include "kxf/System/SystemProcess.h"
#include "kxf/System/SystemThread.h"
#include "kxf/Threading/ThreadLocalSlot.h"
#include <source_location>
#include <atomic>
#include <vector>
#include <map>

namespace kxf
{
	class ScopedLogger;
	class ScopedLoggerTLS;
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
			static ScopedLoggerGlobalContext& GetInstance() noexcept;

		private:
			std::atomic<std::shared_ptr<IScopedLoggerContext>> m_UserContext;
			std::atomic<TimeZoneOffset> m_TimeOffset;
			std::atomic<LogLevel> m_LogLevel = LogLevel::Unknown;

			std::unique_ptr<ScopedLoggerTLS> m_UnknownContext;
			FiberLocalSlot m_TLSIndex;

		private:
			void Initialize();
			void Destroy();

		private:
			ScopedLoggerGlobalContext()
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
			void SetUserContext(std::shared_ptr<IScopedLoggerContext> userContext) noexcept
			{
				m_UserContext = std::move(userContext);
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
			size_t m_ScopeLevel = 0;
			bool m_Terminated = false;

		protected:
			void Initialize();
			void Destroy();

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
		friend class ScopedLogger;
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
			void Initialize()
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
				Initialize();
			}
			ScopedMessageLogger(ScopedLogger& scope, LogLevel logLevel = LogLevel::Unknown)
				:m_Scope(&scope), m_LogLevel(logLevel)
			{
				Initialize();
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
				Initialize();
			}
			void Finalize(ScopedLoggerTLS& tls)
			{
				Write(tls);
				Initialize();
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
	class ScopedLogger final
	{
		friend class ScopedMessageLogger;

		private:
			ScopedLoggerTLS& m_ScopeTLS;
			uint32_t m_Line = 0;
			uint32_t m_Column = 0;
			String m_FileName;
			String m_Function;
			String m_ReturnValue;
			bool m_IsSuccess = false;
			bool m_IsVoid = false;

		private:
			void Write(LogLevel logLevel, DateTime timestamp, StringView str)
			{
				m_ScopeTLS.Write(logLevel, timestamp, str);
			}
			void FromSourceLocation(const std::source_location& sourceLocation)
			{
				m_Line = sourceLocation.line();
				m_Column = sourceLocation.column();
				m_FileName = sourceLocation.file_name();
				m_Function = sourceLocation.function_name();
			}

			void OnEnter(StringView parameters = {});
			void OnLeave();

			ScopedMessageLogger FlowControl()
			{
				return ScopedMessageLogger(*this, LogLevel::FlowControl);
			}

		public:
			ScopedLogger(ScopedLoggerTLS& tls, String function)
				:m_ScopeTLS(tls), m_Function(std::move(function))
			{
				OnEnter();
			}

			ScopedLogger(const std::source_location& sourceLocation = std::source_location::current())
				:m_ScopeTLS(ScopedLoggerGlobalContext::GetInstance().GetThreadContext())
			{
				FromSourceLocation(sourceLocation);
				OnEnter();
			}

			template<class... Args>
			ScopedLogger(const std::source_location& sourceLocation, Args&&... arg)
				:m_ScopeTLS(ScopedLoggerGlobalContext::GetInstance().GetThreadContext())
			{
				FromSourceLocation(sourceLocation);

				ScopedMessageLogger logger;
				logger.PrintSep(", ", std::forward<Args>(arg)...);
				OnEnter(logger.ToString());
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

	class AutoScopedLogger final
	{
		private:
			ScopedLoggerTLS* m_TLS = nullptr;
			ScopedLogger* m_Scope = nullptr;

		public:
			AutoScopedLogger() noexcept
			{
				auto& global = ScopedLoggerGlobalContext::GetInstance();
				if (auto tls = global.QueryThreadContext())
				{
					m_TLS = tls;
				}
				else
				{
					m_TLS = &global.GetUnknownThreadContext();
				}
				m_Scope = m_TLS->GetCurrentScope();
			}

		public:
			ScopedLoggerTLS& GetTLS() const noexcept
			{
				return *m_TLS;
			}
			ScopedLogger& GetScope() const noexcept
			{
				return *m_Scope;
			}

			ScopedLogger* operator->() const noexcept
			{
				return m_Scope;
			}
	};

	class ScopedLoggerUnknownTLS final: public ScopedLoggerTLS
	{
		private:
			std::optional<ScopedLogger> m_Scope;

		private:
			void Initialize();

		public:
			ScopedLoggerUnknownTLS(ScopedLoggerGlobalContext& globalContext)
				:ScopedLoggerTLS(globalContext)
			{
				Initialize();
			}
	};
}

namespace kxf::Log
{
	template<class TFormat, class... Args>
	void Critical(const TFormat& format, Args&&... arg)
	{
		AutoScopedLogger()->Critical().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Error(const TFormat& format, Args&&... arg)
	{
		AutoScopedLogger()->Error().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Warning(const TFormat& format, Args&&... arg)
	{
		AutoScopedLogger()->Warning().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Info(const TFormat& format, Args&&... arg)
	{
		AutoScopedLogger()->Info().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Debug(const TFormat& format, Args&&... arg)
	{
		AutoScopedLogger()->Debug().Format(format, std::forward<Args>(arg)...);
	}

	template<class TFormat, class... Args>
	void Trace(const TFormat& format, Args&&... arg)
	{
		AutoScopedLogger()->Trace().Format(format, std::forward<Args>(arg)...);
	}
}

#define KX_SCOPEDLOG				scopedLogger_
#define KX_SCOPEDLOG_AUTO			kxf::AutoScopedLogger	KX_SCOPEDLOG;
#define KX_SCOPEDLOG_FUNC			kxf::ScopedLogger		KX_SCOPEDLOG(std::source_location::current());
#define KX_SCOPEDLOG_ARGS(...)		kxf::ScopedLogger		KX_SCOPEDLOG(std::source_location::current(), __VA_ARGS__);
