#pragma once
#include <winnt.h>
#include "UndefWindows.h"

namespace KxFramework
{
	enum class LockWorkstationCommand
	{
		LockScreen,
		ScreenOn,
		ScreenOff,
		ScreenSleep,
		ScreenWait,
		ScreenSaver
	};
	enum class ExitWorkstationCommand
	{
		None = 0,

		LogOff = 1 << 0,
		PowerOff = 1 << 1,
		Shutdown = 1 << 2,
		Reboot = 1 << 3,
		QuickResolve = 1 << 4,
		RestartApps = 1 << 5,
		HybridShutdown = 1 << 6,
		BootOptions = 1 << 7,
		Force = 1 << 8,
		ForceHung = 1 << 9,
	};
	enum class DisplayDeviceFlag
	{
		None = 0,

		Active = 1 << 0,
		Primary = 1 << 1,
		Removable = 1 << 2,
		VGACompatible = 1 << 3,
		MirroringDriver = 1 << 4,
	};

	enum class SystemType
	{
		Unknown = -1,

		Server = VER_NT_SERVER,
		Workstation = VER_NT_WORKSTATION,
		DomainController = VER_NT_DOMAIN_CONTROLLER,
	};
	enum class SystemPlatformID
	{
		Unknown = -1,

		Win3x = VER_PLATFORM_WIN32s,
		Win9x = VER_PLATFORM_WIN32_WINDOWS,
		WinNT = VER_PLATFORM_WIN32_NT,
	};
	enum class SystemProductType: uint32_t
	{
		Unknown = PRODUCT_UNDEFINED, // An unknown product

		Business = PRODUCT_BUSINESS, // Business
		BusinessN = PRODUCT_BUSINESS_N, // Business N
		ClusterServer = PRODUCT_CLUSTER_SERVER, // HPC Edition
		ClusterServerV = PRODUCT_CLUSTER_SERVER_V, // Server Hyper Core V
		Core = PRODUCT_CORE, // Windows 10 Home
		CoreN = PRODUCT_CORE_N, // Windows 10 Home N
		CoreCountrySpecific = PRODUCT_CORE_COUNTRYSPECIFIC, // Windows 10 Home China
		CoreSingleLanguage = PRODUCT_CORE_SINGLELANGUAGE, // Windows 10 Home Single Language
		DatacenterEvaluationServer = PRODUCT_DATACENTER_EVALUATION_SERVER, // Server Datacenter (evaluation installation)
		DatacenterAServerCore = PRODUCT_DATACENTER_A_SERVER_CORE, // Server Datacenter, Semi-Annual Channel (core installation)
		StandardAServerCore = PRODUCT_STANDARD_A_SERVER_CORE, // Server Standard, Semi-Annual Channel (core installation)
		DatacenterServer = PRODUCT_DATACENTER_SERVER, // Server Datacenter (full installation. For Server Core installations of Windows Server 2012 and later, use the method, Determining whether Server Core is running.)
		DatacenterServerCore = PRODUCT_DATACENTER_SERVER_CORE, // Server Datacenter (core installation, Windows Server 2008 R2 and earlier)
		DatacenterServerCoreV = PRODUCT_DATACENTER_SERVER_CORE_V, // Server Datacenter without Hyper-V (core installation)
		DatacenterServerV = PRODUCT_DATACENTER_SERVER_V, // Server Datacenter without Hyper-V (full installation)
		Education = PRODUCT_EDUCATION, // Windows 10 Education
		EducationN = PRODUCT_EDUCATION_N, // Windows 10 Education N
		Enterprise = PRODUCT_ENTERPRISE, // Windows 10 Enterprise
		EnterpriseE = PRODUCT_ENTERPRISE_E, // Windows 10 Enterprise E
		EnterpriseEvaluation = PRODUCT_ENTERPRISE_EVALUATION, // Windows 10 Enterprise Evaluation
		EnterpriseN = PRODUCT_ENTERPRISE_N, // Windows 10 Enterprise N
		EnterpriseNEvaluation = PRODUCT_ENTERPRISE_N_EVALUATION, // Windows 10 Enterprise N Evaluation
		EnterpriseS = PRODUCT_ENTERPRISE_S, // Windows 10 Enterprise 2015 LTSB
		EnterpriseSEvaluation = PRODUCT_ENTERPRISE_S_EVALUATION, // Windows 10 Enterprise 2015 LTSB Evaluation
		EnterpriseSN = PRODUCT_ENTERPRISE_S_N, // Windows 10 Enterprise 2015 LTSB N
		EnterpriseSNEvaluation = PRODUCT_ENTERPRISE_S_N_EVALUATION, // Windows 10 Enterprise 2015 LTSB N Evaluation
		EnterpriseServer = PRODUCT_ENTERPRISE_SERVER, // Server Enterprise (full installation)
		EnterpriseServerCore = PRODUCT_ENTERPRISE_SERVER_CORE, // Server Enterprise (core installation)
		EnterpriseServerCoreV = PRODUCT_ENTERPRISE_SERVER_CORE_V, // Server Enterprise without Hyper-V (core installation)
		EnterpriseServer_IA64 = PRODUCT_ENTERPRISE_SERVER_IA64, // Server Enterprise for Itanium-based Systems
		EnterpriseServerV = PRODUCT_ENTERPRISE_SERVER_V, // Server Enterprise without Hyper-V (full installation)
		EssentialbusinessServerADDL = PRODUCT_ESSENTIALBUSINESS_SERVER_ADDL, // Windows Essential Server Solution Additional
		EssentialbusinessServerADDLSVC = PRODUCT_ESSENTIALBUSINESS_SERVER_ADDLSVC, // Windows Essential Server Solution Additional SVC
		EssentialbusinessServerMGMT = PRODUCT_ESSENTIALBUSINESS_SERVER_MGMT, // Windows Essential Server Solution Management
		EssentialbusinessServerMGMTSVC = PRODUCT_ESSENTIALBUSINESS_SERVER_MGMTSVC, // Windows Essential Server Solution Management SVC
		HomeBasic = PRODUCT_HOME_BASIC, // Home Basic
		HomeBasicE = PRODUCT_HOME_BASIC_E, // Not supported
		HomeBasicN = PRODUCT_HOME_BASIC_N, // Home Basic N
		HomePremium = PRODUCT_HOME_PREMIUM, // Home Premium
		HomePremiumE = PRODUCT_HOME_PREMIUM_E, // Not supported
		HomePremiumN = PRODUCT_HOME_PREMIUM_N, // Home Premium N
		HomePremiumServer = PRODUCT_HOME_PREMIUM_SERVER, // Windows Home Server 2011
		HomeServer = PRODUCT_HOME_SERVER, // Windows Storage Server 2008 R2 Essentials
		HyperV = PRODUCT_HYPERV, // Microsoft Hyper-V Server
		IoTUAP = PRODUCT_IOTUAP, // Windows 10 IoT Core
		IoTUAPCommercial = 0x00000083, // PRODUCT_IOTUAPCOMMERCIAL Windows 10 IoT Core Commercial
		MediumBusinessServerManagement = PRODUCT_MEDIUMBUSINESS_SERVER_MANAGEMENT, // Windows Essential Business Server Management Server
		MediumBusinessServerMessaging = PRODUCT_MEDIUMBUSINESS_SERVER_MESSAGING, // Windows Essential Business Server Messaging Server
		MediumBusinessServerSecurity = PRODUCT_MEDIUMBUSINESS_SERVER_SECURITY, // Windows Essential Business Server Security Server
		MobileCore = 0x00000068, // PRODUCT_MOBILE_CORE Windows 10 Mobile
		MobileEnterprise = 0x00000085, // PRODUCT_MOBILE_ENTERPRISE Windows 10 Mobile Enterprise
		MultipointPremiumServer = PRODUCT_MULTIPOINT_PREMIUM_SERVER, // Windows MultiPoint Server Premium (full installation)
		MultipointStandardServer = PRODUCT_MULTIPOINT_STANDARD_SERVER, // Windows MultiPoint Server Standard (full installation)
		ProWorkstation = PRODUCT_PRO_WORKSTATION, // Windows 10 Pro for Workstations
		ProWorkstationN = PRODUCT_PRO_WORKSTATION_N, // Windows 10 Pro for Workstations N
		Professional = PRODUCT_PROFESSIONAL, // Windows 10 Pro
		ProfessionalE = PRODUCT_PROFESSIONAL_E, // Not supported
		ProfessionalN = PRODUCT_PROFESSIONAL_N, // Windows 10 Pro N
		ProfessionalWMC = PRODUCT_PROFESSIONAL_WMC, // Professional with Media Center
		SbSolutionServer = PRODUCT_SB_SOLUTION_SERVER, // Windows Small Business Server 2011 Essentials
		SbSolutionServerEM = PRODUCT_SB_SOLUTION_SERVER_EM, // Server For SB Solutions EM
		ServerForSbSolutions = PRODUCT_SERVER_FOR_SB_SOLUTIONS, // Server For SB Solutions
		ServerForSbSolutionsEM = PRODUCT_SERVER_FOR_SB_SOLUTIONS_EM, // Server For SB Solutions EM
		ServerForSmallBusiness = PRODUCT_SERVER_FOR_SMALLBUSINESS, // Windows Server 2008 for Windows Essential Server Solutions
		ServerForSmallBusinessV = PRODUCT_SERVER_FOR_SMALLBUSINESS_V, // Windows Server 2008 without Hyper-V for Windows Essential Server Solutions
		ServerFoundation = PRODUCT_SERVER_FOUNDATION, // Server Foundation
		SmallBusinessServer = PRODUCT_SMALLBUSINESS_SERVER, // Windows Small Business Server
		SmallBusinessServerPremium = PRODUCT_SMALLBUSINESS_SERVER_PREMIUM, // Small Business Server Premium
		SmallBusinessServerPremiumCore = PRODUCT_SMALLBUSINESS_SERVER_PREMIUM_CORE, // Small Business Server Premium (core installation)
		SolutionEmbeddedserver = PRODUCT_SOLUTION_EMBEDDEDSERVER, // Windows MultiPoint Server
		StandardEvaluationServer = PRODUCT_STANDARD_EVALUATION_SERVER, // Server Standard (evaluation installation)
		StandardServer = PRODUCT_STANDARD_SERVER, // Server Standard (full installation. For Server Core installations of Windows Server 2012 and later, use the method, Determining whether Server Core is running.)
		StandardServerCore = PRODUCT_STANDARD_SERVER_CORE, // Server Standard (core installation, Windows Server 2008 R2 and earlier)
		StandardServerCoreV = PRODUCT_STANDARD_SERVER_CORE_V, // Server Standard without Hyper-V (core installation)
		StandardServerV = PRODUCT_STANDARD_SERVER_V, // Server Standard without Hyper-V
		StandardServerSolutions = PRODUCT_STANDARD_SERVER_SOLUTIONS, // Server Solutions Premium
		StandardServerSolutionsCore = PRODUCT_STANDARD_SERVER_SOLUTIONS_CORE, // Server Solutions Premium (core installation)
		Starter = PRODUCT_STARTER, // Starter
		StarterE = PRODUCT_STARTER_E, // Not supported
		StarterN = PRODUCT_STARTER_N, // Starter N
		StorageEnterpriseServer = PRODUCT_STORAGE_ENTERPRISE_SERVER, // Storage Server Enterprise
		StorageEnterpriseServerCore = PRODUCT_STORAGE_ENTERPRISE_SERVER_CORE, // Storage Server Enterprise (core installation)
		StorageExpressServer = PRODUCT_STORAGE_EXPRESS_SERVER, // Storage Server Express
		StorageExpressServerCore = PRODUCT_STORAGE_EXPRESS_SERVER_CORE, // Storage Server Express (core installation)
		StorageStandardEvaluationServer = PRODUCT_STORAGE_STANDARD_EVALUATION_SERVER, // Storage Server Standard (evaluation installation)
		StorageStandardServer = PRODUCT_STORAGE_STANDARD_SERVER, // Storage Server Standard
		StorageStandardServerCore = PRODUCT_STORAGE_STANDARD_SERVER_CORE, // Storage Server Standard (core installation)
		StorageWorkgroupEvaluationServer = PRODUCT_STORAGE_WORKGROUP_EVALUATION_SERVER, // Storage Server Workgroup (evaluation installation)
		StorageWorkgroupServer = PRODUCT_STORAGE_WORKGROUP_SERVER, // Storage Server Workgroup
		StorageWorkgroupServerCore = PRODUCT_STORAGE_WORKGROUP_SERVER_CORE, // Storage Server Workgroup (core installation)
		Ultimate = PRODUCT_ULTIMATE, // Ultimate
		UltimateE = PRODUCT_ULTIMATE_E, // Not supported
		UltimateN = PRODUCT_ULTIMATE_N, // Ultimate N
		WebServer = PRODUCT_WEB_SERVER, // Web Server (full installation)
		WebServerCore = PRODUCT_WEB_SERVER_CORE // Web Server (core installation)
	};
	enum class SystemProductSuite: uint32_t
	{
		None = 0,

		ServerNT = VER_SERVER_NT,
		WorkstationNT = VER_WORKSTATION_NT,
		Communications = VER_SUITE_COMMUNICATIONS,
		EmbeddedRestricted = VER_SUITE_EMBEDDED_RESTRICTED,
		SecurityAppliance = VER_SUITE_SECURITY_APPLIANCE,

		// Windows Server 2008 Enterprise, Windows Server 2003, Enterprise Edition,
		// or Windows 2000 Advanced Server is installed.
		Enterprise = VER_SUITE_ENTERPRISE,

		// Microsoft BackOffice components are installed.
		Backoffice = VER_SUITE_BACKOFFICE,

		// Terminal Services is installed. This value is always set. If VER_SUITE_TERMINAL is set
		// but VER_SUITE_SINGLEUSERTS is not set, the system is running in application server mode.
		Terminal = VER_SUITE_TERMINAL,

		// Microsoft Small Business Server was once installed on the system, but may have been upgraded
		// to another version of Windows.
		SmallBusiness = VER_SUITE_SMALLBUSINESS,

		// Microsoft Small Business Server is installed with the restrictive client license in force.
		SmallBusinessRestricted = VER_SUITE_SMALLBUSINESS_RESTRICTED,

		// Windows XP Embedded is installed.
		EmbeddedNT = VER_SUITE_EMBEDDEDNT,

		// Windows Server 2008 Datacenter, Windows Server 2003, Datacenter Edition,
		// or Windows 2000 Datacenter Server is installed.
		DataCenter = VER_SUITE_DATACENTER,

		// Remote Desktop is supported, but only one interactive session is supported.
		// This value is set unless the system is running in application server mode.
		SingleUserTS = VER_SUITE_SINGLEUSERTS,

		// AppServer mode is enabled.
		MultiUserTS = VER_SUITE_MULTIUSERTS,

		// Windows Vista Home Premium, Windows Vista Home Basic, or Windows XP Home Edition is installed.
		Personal = VER_SUITE_PERSONAL,

		// Windows Server 2003, Web Edition is installed.
		Blade = VER_SUITE_BLADE,

		// Windows Storage Server 2003 R2 or Windows Storage Server 2003is installed.
		StorageServer = VER_SUITE_STORAGE_SERVER,

		// Windows Server 2003, Compute Cluster Edition is installed.
		ComputeServer = VER_SUITE_COMPUTE_SERVER,

		// Windows Home Server is installed.
		HomeServer = VER_SUITE_WH_SERVER
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(ExitWorkstationCommand);
		Kx_EnumClass_AllowEverything(DisplayDeviceFlag);

		Kx_EnumClass_AllowCast(SystemType);
		Kx_EnumClass_AllowCast(SystemPlatformID);
		Kx_EnumClass_AllowEverything(SystemProductType);
		Kx_EnumClass_AllowEverything(SystemProductSuite);
	}
}
