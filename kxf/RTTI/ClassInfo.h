#pragma once
#include "Common.h"
#include "IObject.h"
#include "kxf/General/FlagSet.h"
#include <variant>

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
		Abstract = 1 << 9
	};
}

namespace kxf::RTTI
{
	class KX_API ClassInfo: public IObject
	{
		KxRTTI_DeclareIID(ClassInfo, {0xccefbb7f, 0xe63e, 0x4a32, {0x95, 0xfa, 0xce, 0x15, 0x13, 0xe3, 0x5c, 0xfc}});

		friend const ClassInfo* GetClassInfoByName(std::string_view) noexcept;
		friend const ClassInfo* GetClassInfoByName(const kxf::String&) noexcept;

		public:
			static const ClassInfo* GetFirst() noexcept;

		protected:
			ClassInfo* m_Next = nullptr;

			std::string_view m_FullyQualifiedName;
			size_t m_Size = 0;
			size_t m_Alignment = 0;
			FlagSet<ClassTrait> m_Traits;
			const std::type_info* m_TypeInfo = nullptr;

		private:
			void OnCreate() noexcept;
			void OnDestroy() noexcept;

		protected:
			void Initialize(std::string_view name, size_t size, size_t alignment, FlagSet<ClassTrait> traits, const std::type_info& typeInfo);
			bool IsInitialized() const;

		protected:
			// IObject
			RTTI::QueryInfo DoQueryInterface(const kxf::IID& iid) noexcept override;

			// ClassInfo
			std::string_view ParseToFullyQualifiedName(std::string_view name, size_t index) const;

		protected:
			ClassInfo() noexcept
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

			FlagSet<ClassTrait> GetTraits() const noexcept
			{
				return m_Traits;
			}
			const std::type_info& GetTypeInfo() const noexcept
			{
				return *m_TypeInfo;
			}

			virtual IID GetInterfaceID() const = 0;
			virtual size_t EnumBaseClassInfo(std::function<bool(const ClassInfo&)> func) const = 0;

		public:
			const ClassInfo* GetNext() const noexcept
			{
				return m_Next;
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
			FlagSet<ClassTrait> CollectTraits() const noexcept
			{
				FlagSet<ClassTrait> traits;

				traits.Add(ClassTrait::DefaultConstructible, std::is_default_constructible_v<T>);
				traits.Add(ClassTrait::CopyConstructible, std::is_copy_constructible_v<T>);
				traits.Add(ClassTrait::CopyAssignable, std::is_copy_assignable_v<T>);
				traits.Add(ClassTrait::MoveConstructible, std::is_move_constructible_v<T>);
				traits.Add(ClassTrait::MoveAssignable, std::is_move_assignable_v<T>);

				traits.Add(ClassTrait::Abstract, std::is_abstract_v<T>);
				traits.Add(ClassTrait::Final, std::is_final_v<T>);

				return traits;
			}

		public:
			ClassInfoOfCommon() noexcept
			{
				ClassInfo::Initialize(__FUNCTION__, sizeof(T), alignof(T), CollectTraits(), typeid(T));

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

		public:
			size_t EnumBaseClassInfo(std::function<bool(const ClassInfo&)> func) const override
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
	};
}

namespace kxf::RTTI
{
	template<class T, class... TBase>
	class ClassInfoOf: public Private::ClassInfoOfCommon<T, TBase...>
	{
		public:
			IID GetInterfaceID() const override
			{
				return RTTI::GetInterfaceID<T>();
			}
	};

	template<class T, class... TBase>
	class ClassInfoOfImplementation: public Private::ClassInfoOfCommon<T, TBase...>
	{
		public:
			IID GetInterfaceID() const override
			{
				return {};
			}
	};
}

namespace kxf
{
	KxFlagSet_Declare(RTTI::ClassTrait);
}
