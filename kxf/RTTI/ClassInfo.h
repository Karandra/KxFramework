#pragma once
#include "Common.h"
#include "IObject.h"
#include "kxf/General/FlagSet.h"
#include <variant>
#include <wx/msgdlg.h>

namespace kxf
{
	class IObject;
	class String;
}

namespace kxf::RTTI
{
	enum class ClassTrait: uint64_t
	{
		None = 0,

		DefaultConstructible = 1 << 0,
		CopyConstructible = 1 << 1,
		CopyAssignable = 1 << 2,
		MoveConstructible = 1 << 3,
		MoveAssignable = 1 << 4,

		Final = 1 << 8,
		Abstract = 1 << 10,

		Interface = 1 << 16,
		Implementation = 1 << 17,
		Dynamic = 1 << 18
	};
}
namespace kxf
{
	KxFlagSet_Declare(RTTI::ClassTrait);
}

namespace kxf::RTTI
{
	class KX_API ClassInfo: public IObject
	{
		KxRTTI_DeclareIID(ClassInfo, {0xccefbb7f, 0xe63e, 0x4a32, {0x95, 0xfa, 0xce, 0x15, 0x13, 0xe3, 0x5c, 0xfc}});

		friend const ClassInfo* GetClassInfoByName(std::string_view) noexcept;
		friend const ClassInfo* GetClassInfoByName(const kxf::String&) noexcept;

		public:
			static const ClassInfo* GetFirstClassInfo() noexcept;

		private:
			ClassInfo* m_NextClassInfo = nullptr;

			std::string_view m_FullyQualifiedName;
			size_t m_Size = 0;
			size_t m_Alignment = 0;
			FlagSet<ClassTrait> m_Traits;
			const std::type_info* m_TypeInfo = nullptr;

		private:
			void OnCreate() noexcept;
			void OnDestroy() noexcept;

		protected:
			// IObject
			RTTI::QueryInfo DoQueryInterface(const kxf::IID& iid) noexcept override;

			// ClassInfo
			std::string_view ParseToFullyQualifiedName(std::string_view name, size_t index) const noexcept;

			virtual IID DoGetInterfaceID() const noexcept = 0;
			virtual size_t DoEnumBaseClasses(std::function<bool(const ClassInfo&)> func) const noexcept = 0;
			virtual std::unique_ptr<IObject> DoCreateObjectInstance() const = 0;

		protected:
			ClassInfo(std::string_view name, size_t size, size_t alignment, FlagSet<ClassTrait> traits, const std::type_info& typeInfo) noexcept
				:m_FullyQualifiedName(ParseToFullyQualifiedName(name, 0)), m_Size(size), m_Alignment(alignment), m_Traits(traits), m_TypeInfo(&typeInfo)
			{
				OnCreate();
			}
			virtual ~ClassInfo()
			{
				OnDestroy();
			}

		public:
			kxf::String GetClassName() const;
			kxf::String GetNamespace() const;
			kxf::String GetFullyQualifiedName() const;

			size_t GetSize() const noexcept
			{
				return m_Size;
			}
			size_t GetAlignment() const noexcept
			{
				return m_Alignment;
			}

			IID GetInterfaceID() const noexcept
			{
				return DoGetInterfaceID();
			}
			FlagSet<ClassTrait> GetTraits() const noexcept
			{
				return m_Traits;
			}
			const std::type_info& GetStdTypeInfo() const noexcept
			{
				return *m_TypeInfo;
			}

			size_t EnumBaseClasses(std::function<bool(const ClassInfo&)> func) const noexcept
			{
				return DoEnumBaseClasses(std::move(func));
			}
			size_t EnumDerivedClasses(std::function<bool(const ClassInfo&)> func) const noexcept;
			bool IsBaseOf(const ClassInfo& other) const noexcept;
			bool IsSameAs(const ClassInfo& other) const noexcept;
			bool IsNull() const noexcept;

			template<class T = IObject>
			std::unique_ptr<T> CreateObjectInstance() const
			{
				return RTTI::dynamic_cast_unique_ptr<T>(DoCreateObjectInstance());
			}

		public:
			const ClassInfo* GetNextClassInfo() const noexcept
			{
				return m_NextClassInfo;
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

			bool operator==(const ClassInfo& other) const noexcept
			{
				return IsSameAs(other);
			}
			bool operator!=(const ClassInfo& other) const noexcept
			{
				return !IsSameAs(other);
			}

	};
}

namespace kxf::RTTI::Private
{
	template<class T, class... TBase>
	class ClassInfoOfCommon: public ClassInfo
	{
		private:
			mutable std::array<std::variant<const ClassInfo*, std::string_view>, Utility::CountOfParameterPack<TBase...>()> m_BaseClassInfo;

		private:
			FlagSet<ClassTrait> CollectTraits(FlagSet<ClassTrait> traits) const noexcept
			{
				traits.Add(ClassTrait::DefaultConstructible, std::is_default_constructible_v<T>);
				traits.Add(ClassTrait::CopyConstructible, std::is_copy_constructible_v<T>);
				traits.Add(ClassTrait::CopyAssignable, std::is_copy_assignable_v<T>);
				traits.Add(ClassTrait::MoveConstructible, std::is_move_constructible_v<T>);
				traits.Add(ClassTrait::MoveAssignable, std::is_move_assignable_v<T>);

				traits.Add(ClassTrait::Abstract, std::is_abstract_v<T>);
				traits.Add(ClassTrait::Final, std::is_final_v<T>);

				return traits;
			}

		protected:
			// ClassInfo
			size_t DoEnumBaseClasses(std::function<bool(const ClassInfo&)> func) const noexcept override
			{
				size_t count = 0;
				for (auto& item: m_BaseClassInfo)
				{
					if (auto classInfo = std::get_if<const ClassInfo*>(&item))
					{
						count++;
						if (func && !std::invoke(func, **classInfo))
						{
							break;
						}
					}
					else if (auto name = std::get_if<std::string_view>(&item))
					{
						if (auto classInfo = RTTI::GetClassInfoByName(*name))
						{
							item = classInfo;

							count++;
							if (func && !std::invoke(func, *classInfo))
							{
								break;
							}
						}
					}
				}
				return count;
			}

		public:
			ClassInfoOfCommon(FlagSet<ClassTrait> classTraits = {}) noexcept
				:ClassInfo(__FUNCTION__, sizeof(T), alignof(T), CollectTraits(classTraits), typeid(T))
			{
				for (size_t i = 0; i < m_BaseClassInfo.size(); i++)
				{
					auto name = ParseToFullyQualifiedName(__FUNCTION__, i + 1);
					if (auto classInfo = RTTI::GetClassInfoByName(name))
					{
						m_BaseClassInfo[i] = classInfo;
					}
					else
					{
						m_BaseClassInfo[i] = std::move(name);
					}
				}
			}
	};
}

namespace kxf::RTTI
{
	template<class T, class... TBase>
	class InterfaceClassInfo: public Private::ClassInfoOfCommon<T, TBase...>
	{
		protected:
			// ClassInfo
			IID DoGetInterfaceID() const noexcept override
			{
				return RTTI::GetInterfaceID<T>();
			}
			std::unique_ptr<IObject> DoCreateObjectInstance() const override
			{
				return nullptr;
			}

		public:
			InterfaceClassInfo()
				:Private::ClassInfoOfCommon<T, TBase...>(ClassTrait::Interface)
			{
			}
	};

	template<class T, class... TBase>
	class ImplementationClassInfo: public Private::ClassInfoOfCommon<T, TBase...>
	{
		protected:
			// ClassInfo
			IID DoGetInterfaceID() const noexcept override
			{
				return {};
			}
			std::unique_ptr<IObject> DoCreateObjectInstance() const override
			{
				return nullptr;
			}

		public:
			ImplementationClassInfo()
				:Private::ClassInfoOfCommon<T, TBase...>(ClassTrait::Implementation)
			{
			}
	};

	template<class T, class... TBase>
	class DynamicImplementationClassInfo: public Private::ClassInfoOfCommon<T, TBase...>
	{
		protected:
			// ClassInfo
			IID DoGetInterfaceID() const noexcept override
			{
				return RTTI::GetInterfaceID<T>();
			}
			std::unique_ptr<IObject> DoCreateObjectInstance() const override
			{
				return RTTI::new_object<T>();
			}

		public:
			DynamicImplementationClassInfo()
				:Private::ClassInfoOfCommon<T, TBase...>(ClassTrait::Implementation|ClassTrait::Dynamic)
			{
			}
	};
}
