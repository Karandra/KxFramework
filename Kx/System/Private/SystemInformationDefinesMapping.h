#pragma once
#include "../SystemInformationDefines.h"
#include "Kx/Utility/Common.h"
#include <Windows.h>
#include "Kx/System/UndefWindows.h"

namespace KxFramework::System::Private
{
	constexpr SystemType MapSystemType(DWORD nativeType) noexcept
	{
		switch (nativeType)
		{
			case VER_NT_SERVER:
			{
				return SystemType::Server;
			}
			case VER_NT_WORKSTATION:
			{
				return SystemType::Workstation;
			}
			case VER_NT_DOMAIN_CONTROLLER:
			{
				return SystemType::DomainController;
			}
		};
		return SystemType::Unknown;
	}
	constexpr SystemPlatformID MapSystemPlatformID(DWORD nativeType) noexcept
	{
		switch (nativeType)
		{
			case VER_PLATFORM_WIN32s:
			{
				return SystemPlatformID::Win3x;
			}
			case VER_PLATFORM_WIN32_WINDOWS:
			{
				return SystemPlatformID::Win9x;
			}
			case VER_PLATFORM_WIN32_NT:
			{
				return SystemPlatformID::WinNT;
			}
		};
		return SystemPlatformID::Unknown;
	}
	constexpr SystemProductType MapSystemProductType(DWORD nativeType) noexcept
	{
		switch (nativeType)
		{
			case PRODUCT_UNLICENSED:
			{
				return SystemProductType::Unlicensed;
			}
			case PRODUCT_BUSINESS:
			{
				return SystemProductType::Business;
			}
			case PRODUCT_BUSINESS_N:
			{
				return SystemProductType::BusinessN;
			}
			case PRODUCT_CLUSTER_SERVER:
			{
				return SystemProductType::ClusterServer;
			}
			case PRODUCT_CLUSTER_SERVER_V:
			{
				return SystemProductType::ClusterServerV;
			}
			case PRODUCT_CORE:
			{
				return SystemProductType::Core;
			}
			case PRODUCT_CORE_N:
			{
				return SystemProductType::CoreN;
			}
			case PRODUCT_CORE_COUNTRYSPECIFIC:
			{
				return SystemProductType::CoreCountrySpecific;
			}
			case PRODUCT_CORE_SINGLELANGUAGE:
			{
				return SystemProductType::CoreSingleLanguage;
			}
			case PRODUCT_DATACENTER_EVALUATION_SERVER:
			{
				return SystemProductType::DatacenterEvaluationServer;
			}
			case PRODUCT_DATACENTER_A_SERVER_CORE:
			{
				return SystemProductType::DatacenterAServerCore;
			}
			case PRODUCT_STANDARD_A_SERVER_CORE:
			{
				return SystemProductType::StandardAServerCore;
			}
			case PRODUCT_DATACENTER_SERVER:
			{
				return SystemProductType::DatacenterServer;
			}
			case PRODUCT_DATACENTER_SERVER_CORE:
			{
				return SystemProductType::DatacenterServerCore;
			}
			case PRODUCT_DATACENTER_SERVER_CORE_V:
			{
				return SystemProductType::DatacenterServerCoreV;
			}
			case PRODUCT_DATACENTER_SERVER_V:
			{
				return SystemProductType::DatacenterServerV;
			}
			case PRODUCT_EDUCATION:
			{
				return SystemProductType::Education;
			}
			case PRODUCT_EDUCATION_N:
			{
				return SystemProductType::EducationN;
			}
			case PRODUCT_ENTERPRISE:
			{
				return SystemProductType::Enterprise;
			}
			case PRODUCT_ENTERPRISE_E:
			{
				return SystemProductType::EnterpriseE;
			}
			case PRODUCT_ENTERPRISE_EVALUATION:
			{
				return SystemProductType::EnterpriseEvaluation;
			}
			case PRODUCT_ENTERPRISE_N:
			{
				return SystemProductType::EnterpriseN;
			}
			case PRODUCT_ENTERPRISE_N_EVALUATION:
			{
				return SystemProductType::EnterpriseNEvaluation;
			}
			case PRODUCT_ENTERPRISE_S:
			{
				return SystemProductType::EnterpriseS;
			}
			case PRODUCT_ENTERPRISE_S_EVALUATION:
			{
				return SystemProductType::EnterpriseSEvaluation;
			}
			case PRODUCT_ENTERPRISE_S_N:
			{
				return SystemProductType::EnterpriseSN;
			}
			case PRODUCT_ENTERPRISE_S_N_EVALUATION:
			{
				return SystemProductType::EnterpriseSNEvaluation;
			}
			case PRODUCT_ENTERPRISE_SERVER:
			{
				return SystemProductType::EnterpriseServer;
			}
			case PRODUCT_ENTERPRISE_SERVER_CORE:
			{
				return SystemProductType::EnterpriseServerCore;
			}
			case PRODUCT_ENTERPRISE_SERVER_CORE_V:
			{
				return SystemProductType::EnterpriseServerCoreV;
			}
			case PRODUCT_ENTERPRISE_SERVER_IA64:
			{
				return SystemProductType::EnterpriseServer_IA64;
			}
			case PRODUCT_ENTERPRISE_SERVER_V:
			{
				return SystemProductType::EnterpriseServerV;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL:
			{
				return SystemProductType::EssentialbusinessServerADDL;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC:
			{
				return SystemProductType::EssentialbusinessServerADDLSVC;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT:
			{
				return SystemProductType::EssentialbusinessServerMGMT;
			}
			case PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC:
			{
				return SystemProductType::EssentialbusinessServerMGMTSVC;
			}
			case PRODUCT_HOME_BASIC:
			{
				return SystemProductType::HomeBasic;
			}
			case PRODUCT_HOME_BASIC_E:
			{
				return SystemProductType::HomeBasicE;
			}
			case PRODUCT_HOME_BASIC_N:
			{
				return SystemProductType::HomeBasicN;
			}
			case PRODUCT_HOME_PREMIUM:
			{
				return SystemProductType::HomePremium;
			}
			case PRODUCT_HOME_PREMIUM_E:
			{
				return SystemProductType::HomePremiumE;
			}
			case PRODUCT_HOME_PREMIUM_N:
			{
				return SystemProductType::HomePremiumN;
			}
			case PRODUCT_HOME_PREMIUM_SERVER:
			{
				return SystemProductType::HomePremiumServer;
			}
			case PRODUCT_HOME_SERVER:
			{
				return SystemProductType::HomeServer;
			}
			case PRODUCT_HYPERV:
			{
				return SystemProductType::HyperV;
			}
			case PRODUCT_IOTUAP:
			{
				return SystemProductType::IoTUAP;
			}
			case PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT:
			{
				return SystemProductType::MediumBusinessServerManagement;
			}
			case PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING:
			{
				return SystemProductType::MediumBusinessServerMessaging;
			}
			case PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY:
			{
				return SystemProductType::MediumBusinessServerSecurity;
			}
			case PRODUCT_MULTIPOINT_PREMIUM_SERVER:
			{
				return SystemProductType::MultipointPremiumServer;
			}
			case PRODUCT_MULTIPOINT_STANDARD_SERVER:
			{
				return SystemProductType::MultipointStandardServer;
			}
			case PRODUCT_PRO_WORKSTATION:
			{
				return SystemProductType::ProWorkstation;
			}
			case PRODUCT_PRO_WORKSTATION_N:
			{
				return SystemProductType::ProWorkstationN;
			}
			case PRODUCT_PROFESSIONAL:
			{
				return SystemProductType::Professional;
			}
			case PRODUCT_PROFESSIONAL_E:
			{
				return SystemProductType::ProfessionalE;
			}
			case PRODUCT_PROFESSIONAL_N:
			{
				return SystemProductType::ProfessionalN;
			}
			case PRODUCT_PROFESSIONAL_WMC:
			{
				return SystemProductType::ProfessionalWMC;
			}
			case PRODUCT_SB_SOLUTION_SERVER:
			{
				return SystemProductType::SbSolutionServer;
			}
			case PRODUCT_SB_SOLUTION_SERVER_EM:
			{
				return SystemProductType::SbSolutionServerEM;
			}
			case PRODUCT_SERVER_FOR_SB_SOLUTIONS:
			{
				return SystemProductType::ServerForSbSolutions;
			}
			case PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM:
			{
				return SystemProductType::ServerForSbSolutionsEM;
			}
			case PRODUCT_SERVER_FOR_SMALLBUSINESS:
			{
				return SystemProductType::ServerForSmallBusiness;
			}
			case PRODUCT_SERVER_FOR_SMALLBUSINESS_V:
			{
				return SystemProductType::ServerForSmallBusinessV;
			}
			case PRODUCT_SERVER_FOUNDATION:
			{
				return SystemProductType::ServerFoundation;
			}
			case PRODUCT_SMALLBUSINESS_SERVER:
			{
				return SystemProductType::SmallBusinessServer;
			}
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:
			{
				return SystemProductType::SmallBusinessServerPremium;
			}
			case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE:
			{
				return SystemProductType::SmallBusinessServerPremiumCore;
			}
			case PRODUCT_SOLUTION_EMBEDDEDSERVER:
			{
				return SystemProductType::SolutionEmbeddedserver;
			}
			case PRODUCT_STANDARD_EVALUATION_SERVER:
			{
				return SystemProductType::StandardEvaluationServer;
			}
			case PRODUCT_STANDARD_SERVER:
			{
				return SystemProductType::StandardServer;
			}
			case PRODUCT_STANDARD_SERVER_CORE:
			{
				return SystemProductType::StandardServerCore;
			}
			case PRODUCT_STANDARD_SERVER_CORE_V:
			{
				return SystemProductType::StandardServerCoreV;
			}
			case PRODUCT_STANDARD_SERVER_V:
			{
				return SystemProductType::StandardServerV;
			}
			case PRODUCT_STANDARD_SERVER_SOLUTIONS:
			{
				return SystemProductType::StandardServerSolutions;
			}
			case PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE:
			{
				return SystemProductType::StandardServerSolutionsCore;
			}
			case PRODUCT_STARTER:
			{
				return SystemProductType::Starter;
			}
			case PRODUCT_STARTER_E:
			{
				return SystemProductType::StarterE;
			}
			case PRODUCT_STARTER_N:
			{
				return SystemProductType::StarterN;
			}
			case PRODUCT_STORAGE_ENTERPRISE_SERVER:
			{
				return SystemProductType::StorageEnterpriseServer;
			}
			case PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE:
			{
				return SystemProductType::StorageEnterpriseServerCore;
			}
			case PRODUCT_STORAGE_EXPRESS_SERVER:
			{
				return SystemProductType::StorageExpressServer;
			}
			case PRODUCT_STORAGE_EXPRESS_SERVER_CORE:
			{
				return SystemProductType::StorageExpressServerCore;
			}
			case PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER:
			{
				return SystemProductType::StorageStandardEvaluationServer;
			}
			case PRODUCT_STORAGE_STANDARD_SERVER:
			{
				return SystemProductType::StorageStandardServer;
			}
			case PRODUCT_STORAGE_STANDARD_SERVER_CORE:
			{
				return SystemProductType::StorageStandardServerCore;
			}
			case PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER:
			{
				return SystemProductType::StorageWorkgroupEvaluationServer;
			}
			case PRODUCT_STORAGE_WORKGROUP_SERVER:
			{
				return SystemProductType::StorageWorkgroupServer;
			}
			case PRODUCT_STORAGE_WORKGROUP_SERVER_CORE:
			{
				return SystemProductType::StorageWorkgroupServerCore;
			}
			case PRODUCT_ULTIMATE:
			{
				return SystemProductType::Ultimate;
			}
			case PRODUCT_ULTIMATE_E:
			{
				return SystemProductType::UltimateE;
			}
			case PRODUCT_ULTIMATE_N:
			{
				return SystemProductType::UltimateN;
			}
			case PRODUCT_WEB_SERVER:
			{
				return SystemProductType::WebServer;
			}
			case PRODUCT_WEB_SERVER_CORE:
			{
				return SystemProductType::WebServerCore;
			}
		};
		return SystemProductType::Unknown;

		SystemProductType type = SystemProductType::Unknown;
		Utility::AddFlagRef(type, SystemProductType::Business, nativeType & PRODUCT_BUSINESS);
		Utility::AddFlagRef(type, SystemProductType::BusinessN, nativeType & PRODUCT_BUSINESS_N);
		Utility::AddFlagRef(type, SystemProductType::ClusterServer, nativeType & PRODUCT_CLUSTER_SERVER);
		Utility::AddFlagRef(type, SystemProductType::ClusterServerV, nativeType & PRODUCT_CLUSTER_SERVER_V);
		Utility::AddFlagRef(type, SystemProductType::Core, nativeType & PRODUCT_CORE);
		Utility::AddFlagRef(type, SystemProductType::CoreN, nativeType & PRODUCT_CORE_N);
		Utility::AddFlagRef(type, SystemProductType::CoreCountrySpecific, nativeType & PRODUCT_CORE_COUNTRYSPECIFIC);
		Utility::AddFlagRef(type, SystemProductType::CoreSingleLanguage, nativeType & PRODUCT_CORE_SINGLELANGUAGE);
		Utility::AddFlagRef(type, SystemProductType::DatacenterEvaluationServer, nativeType & PRODUCT_DATACENTER_EVALUATION_SERVER);
		Utility::AddFlagRef(type, SystemProductType::DatacenterAServerCore, nativeType & PRODUCT_DATACENTER_A_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::StandardAServerCore, nativeType & PRODUCT_STANDARD_A_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::DatacenterServer, nativeType & PRODUCT_DATACENTER_SERVER);
		Utility::AddFlagRef(type, SystemProductType::DatacenterServerCore, nativeType & PRODUCT_DATACENTER_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::DatacenterServerCoreV, nativeType & PRODUCT_DATACENTER_SERVER_CORE_V);
		Utility::AddFlagRef(type, SystemProductType::DatacenterServerV, nativeType & PRODUCT_DATACENTER_SERVER_V);
		Utility::AddFlagRef(type, SystemProductType::Education, nativeType & PRODUCT_EDUCATION);
		Utility::AddFlagRef(type, SystemProductType::EducationN, nativeType & PRODUCT_EDUCATION_N);
		Utility::AddFlagRef(type, SystemProductType::Enterprise, nativeType & PRODUCT_ENTERPRISE);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseE, nativeType & PRODUCT_ENTERPRISE_E);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseEvaluation, nativeType & PRODUCT_ENTERPRISE_EVALUATION);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseN, nativeType & PRODUCT_ENTERPRISE_N);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseNEvaluation, nativeType & PRODUCT_ENTERPRISE_N_EVALUATION);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseS, nativeType & PRODUCT_ENTERPRISE_S);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseSEvaluation, nativeType & PRODUCT_ENTERPRISE_S_EVALUATION);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseSN, nativeType & PRODUCT_ENTERPRISE_S_N);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseSNEvaluation, nativeType & PRODUCT_ENTERPRISE_S_N_EVALUATION);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseServer, nativeType & PRODUCT_ENTERPRISE_SERVER);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseServerCore, nativeType & PRODUCT_ENTERPRISE_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseServerCoreV, nativeType & PRODUCT_ENTERPRISE_SERVER_CORE_V);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseServer_IA64, nativeType & PRODUCT_ENTERPRISE_SERVER_IA64);
		Utility::AddFlagRef(type, SystemProductType::EnterpriseServerV, nativeType & PRODUCT_ENTERPRISE_SERVER_V);
		Utility::AddFlagRef(type, SystemProductType::EssentialbusinessServerADDL, nativeType & PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL);
		Utility::AddFlagRef(type, SystemProductType::EssentialbusinessServerADDLSVC, nativeType & PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC);
		Utility::AddFlagRef(type, SystemProductType::EssentialbusinessServerMGMT, nativeType & PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT);
		Utility::AddFlagRef(type, SystemProductType::EssentialbusinessServerMGMTSVC, nativeType & PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC);
		Utility::AddFlagRef(type, SystemProductType::HomeBasic, nativeType & PRODUCT_HOME_BASIC);
		Utility::AddFlagRef(type, SystemProductType::HomeBasicE, nativeType & PRODUCT_HOME_BASIC_E);
		Utility::AddFlagRef(type, SystemProductType::HomeBasicN, nativeType & PRODUCT_HOME_BASIC_N);
		Utility::AddFlagRef(type, SystemProductType::HomePremium, nativeType & PRODUCT_HOME_PREMIUM);
		Utility::AddFlagRef(type, SystemProductType::HomePremiumE, nativeType & PRODUCT_HOME_PREMIUM_E);
		Utility::AddFlagRef(type, SystemProductType::HomePremiumN, nativeType & PRODUCT_HOME_PREMIUM_N);
		Utility::AddFlagRef(type, SystemProductType::HomePremiumServer, nativeType & PRODUCT_HOME_PREMIUM_SERVER);
		Utility::AddFlagRef(type, SystemProductType::HomeServer, nativeType & PRODUCT_HOME_SERVER);
		Utility::AddFlagRef(type, SystemProductType::HyperV, nativeType & PRODUCT_HYPERV);
		Utility::AddFlagRef(type, SystemProductType::IoTUAP, nativeType & PRODUCT_IOTUAP);
		Utility::AddFlagRef(type, SystemProductType::MediumBusinessServerManagement, nativeType & PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT);
		Utility::AddFlagRef(type, SystemProductType::MediumBusinessServerMessaging, nativeType & PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING);
		Utility::AddFlagRef(type, SystemProductType::MediumBusinessServerSecurity, nativeType & PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY);
		Utility::AddFlagRef(type, SystemProductType::MultipointPremiumServer, nativeType & PRODUCT_MULTIPOINT_PREMIUM_SERVER);
		Utility::AddFlagRef(type, SystemProductType::MultipointStandardServer, nativeType & PRODUCT_MULTIPOINT_STANDARD_SERVER);
		Utility::AddFlagRef(type, SystemProductType::ProWorkstation, nativeType & PRODUCT_PRO_WORKSTATION);
		Utility::AddFlagRef(type, SystemProductType::ProWorkstationN, nativeType & PRODUCT_PRO_WORKSTATION_N);
		Utility::AddFlagRef(type, SystemProductType::Professional, nativeType & PRODUCT_PROFESSIONAL);
		Utility::AddFlagRef(type, SystemProductType::ProfessionalE, nativeType & PRODUCT_PROFESSIONAL_E);
		Utility::AddFlagRef(type, SystemProductType::ProfessionalN, nativeType & PRODUCT_PROFESSIONAL_N);
		Utility::AddFlagRef(type, SystemProductType::ProfessionalWMC, nativeType & PRODUCT_PROFESSIONAL_WMC);
		Utility::AddFlagRef(type, SystemProductType::SbSolutionServer, nativeType & PRODUCT_SB_SOLUTION_SERVER);
		Utility::AddFlagRef(type, SystemProductType::SbSolutionServerEM, nativeType & PRODUCT_SB_SOLUTION_SERVER_EM);
		Utility::AddFlagRef(type, SystemProductType::ServerForSbSolutions, nativeType & PRODUCT_SERVER_FOR_SB_SOLUTIONS);
		Utility::AddFlagRef(type, SystemProductType::ServerForSbSolutionsEM, nativeType & PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM);
		Utility::AddFlagRef(type, SystemProductType::ServerForSmallBusiness, nativeType & PRODUCT_SERVER_FOR_SMALLBUSINESS);
		Utility::AddFlagRef(type, SystemProductType::ServerForSmallBusinessV, nativeType & PRODUCT_SERVER_FOR_SMALLBUSINESS_V);
		Utility::AddFlagRef(type, SystemProductType::ServerFoundation, nativeType & PRODUCT_SERVER_FOUNDATION);
		Utility::AddFlagRef(type, SystemProductType::SmallBusinessServer, nativeType & PRODUCT_SMALLBUSINESS_SERVER);
		Utility::AddFlagRef(type, SystemProductType::SmallBusinessServerPremium, nativeType & PRODUCT_SMALLBUSINESS_SERVER_PREMIUM);
		Utility::AddFlagRef(type, SystemProductType::SmallBusinessServerPremiumCore, nativeType & PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE);
		Utility::AddFlagRef(type, SystemProductType::SolutionEmbeddedserver, nativeType & PRODUCT_SOLUTION_EMBEDDEDSERVER);
		Utility::AddFlagRef(type, SystemProductType::StandardEvaluationServer, nativeType & PRODUCT_STANDARD_EVALUATION_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StandardServer, nativeType & PRODUCT_STANDARD_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StandardServerCore, nativeType & PRODUCT_STANDARD_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::StandardServerCoreV, nativeType & PRODUCT_STANDARD_SERVER_CORE_V);
		Utility::AddFlagRef(type, SystemProductType::StandardServerV, nativeType & PRODUCT_STANDARD_SERVER_V);
		Utility::AddFlagRef(type, SystemProductType::StandardServerSolutions, nativeType & PRODUCT_STANDARD_SERVER_SOLUTIONS);
		Utility::AddFlagRef(type, SystemProductType::StandardServerSolutionsCore, nativeType & PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE);
		Utility::AddFlagRef(type, SystemProductType::Starter, nativeType & PRODUCT_STARTER);
		Utility::AddFlagRef(type, SystemProductType::StarterE, nativeType & PRODUCT_STARTER_E);
		Utility::AddFlagRef(type, SystemProductType::StarterN, nativeType & PRODUCT_STARTER_N);
		Utility::AddFlagRef(type, SystemProductType::StorageEnterpriseServer, nativeType & PRODUCT_STORAGE_ENTERPRISE_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StorageEnterpriseServerCore, nativeType & PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::StorageExpressServer, nativeType & PRODUCT_STORAGE_EXPRESS_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StorageExpressServerCore, nativeType & PRODUCT_STORAGE_EXPRESS_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::StorageStandardEvaluationServer, nativeType & PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StorageStandardServer, nativeType & PRODUCT_STORAGE_STANDARD_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StorageStandardServerCore, nativeType & PRODUCT_STORAGE_STANDARD_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::StorageWorkgroupEvaluationServer, nativeType & PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StorageWorkgroupServer, nativeType & PRODUCT_STORAGE_WORKGROUP_SERVER);
		Utility::AddFlagRef(type, SystemProductType::StorageWorkgroupServerCore, nativeType & PRODUCT_STORAGE_WORKGROUP_SERVER_CORE);
		Utility::AddFlagRef(type, SystemProductType::Ultimate, nativeType & PRODUCT_ULTIMATE);
		Utility::AddFlagRef(type, SystemProductType::UltimateE, nativeType & PRODUCT_ULTIMATE_E);
		Utility::AddFlagRef(type, SystemProductType::UltimateN, nativeType & PRODUCT_ULTIMATE_N);
		Utility::AddFlagRef(type, SystemProductType::WebServer, nativeType & PRODUCT_WEB_SERVER);
		Utility::AddFlagRef(type, SystemProductType::WebServerCore, nativeType & PRODUCT_WEB_SERVER_CORE);

		return type;
	}
	constexpr SystemProductSuite MapSystemProductSuite(DWORD nativeType) noexcept
	{
		SystemProductSuite suite = SystemProductSuite::None;
		Utility::AddFlagRef(suite, SystemProductSuite::ServerNT, nativeType & VER_SERVER_NT);
		Utility::AddFlagRef(suite, SystemProductSuite::WorkstationNT, nativeType & VER_WORKSTATION_NT);
		Utility::AddFlagRef(suite, SystemProductSuite::Communications, nativeType & VER_SUITE_COMMUNICATIONS);
		Utility::AddFlagRef(suite, SystemProductSuite::SecurityAppliance, nativeType & VER_SUITE_SECURITY_APPLIANCE);
		Utility::AddFlagRef(suite, SystemProductSuite::Enterprise, nativeType & VER_SUITE_ENTERPRISE);
		Utility::AddFlagRef(suite, SystemProductSuite::BackOffice, nativeType & VER_SUITE_BACKOFFICE);
		Utility::AddFlagRef(suite, SystemProductSuite::Terminal, nativeType & VER_SUITE_TERMINAL);
		Utility::AddFlagRef(suite, SystemProductSuite::SmallBusiness, nativeType & VER_SUITE_SMALLBUSINESS);
		Utility::AddFlagRef(suite, SystemProductSuite::SmallBusinessRestricted, nativeType & VER_SUITE_SMALLBUSINESS_RESTRICTED);
		Utility::AddFlagRef(suite, SystemProductSuite::EmbeddedNT, nativeType & VER_SUITE_EMBEDDEDNT);
		Utility::AddFlagRef(suite, SystemProductSuite::EmbeddedRestricted, nativeType & VER_SUITE_EMBEDDED_RESTRICTED);
		Utility::AddFlagRef(suite, SystemProductSuite::DataCenter, nativeType & VER_SUITE_DATACENTER);
		Utility::AddFlagRef(suite, SystemProductSuite::SingleUserTS, nativeType & VER_SUITE_SINGLEUSERTS);
		Utility::AddFlagRef(suite, SystemProductSuite::MultiUserTS, nativeType & VER_SUITE_MULTIUSERTS);
		Utility::AddFlagRef(suite, SystemProductSuite::Personal, nativeType & VER_SUITE_PERSONAL);
		Utility::AddFlagRef(suite, SystemProductSuite::Blade, nativeType & VER_SUITE_BLADE);
		Utility::AddFlagRef(suite, SystemProductSuite::StorageServer, nativeType & VER_SUITE_STORAGE_SERVER);
		Utility::AddFlagRef(suite, SystemProductSuite::ComputeServer, nativeType & VER_SUITE_COMPUTE_SERVER);
		Utility::AddFlagRef(suite, SystemProductSuite::HomeServer, nativeType & VER_SUITE_WH_SERVER);

		return suite;
	}
}
