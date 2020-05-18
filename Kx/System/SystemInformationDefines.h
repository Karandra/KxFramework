#pragma once
#include "Common.h"

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

		Server,
		Workstation,
		DomainController,
	};
	enum class SystemPlatformID
	{
		Unknown = -1,

		Win3x,
		Win9x,
		WinNT,
	};
	enum class SystemProductType
	{
		Unknown = -2, // An unknown product
		Unlicensed = -1, // An unlicensed product

		Business, // Business
		BusinessN, // Business N
		ClusterServer, // HPC Edition
		ClusterServerV, // Server Hyper Core V
		Core, // Windows 10 Home
		CoreN, // Windows 10 Home N
		CoreCountrySpecific, // Windows 10 Home China
		CoreSingleLanguage, // Windows 10 Home Single Language
		DatacenterEvaluationServer, // Server Datacenter (evaluation installation)
		DatacenterAServerCore, // Server Datacenter, Semi-Annual Channel (core installation)
		StandardAServerCore, // Server Standard, Semi-Annual Channel (core installation)
		DatacenterServer, // Server Datacenter (full installation. For Server Core installations of Windows Server 2012 and later, use the method, Determining whether Server Core is running.)
		DatacenterServerCore, // Server Datacenter (core installation, Windows Server 2008 R2 and earlier)
		DatacenterServerCoreV, // Server Datacenter without Hyper-V (core installation)
		DatacenterServerV, // Server Datacenter without Hyper-V (full installation)
		Education, // Windows 10 Education
		EducationN, // Windows 10 Education N
		Enterprise, // Windows 10 Enterprise
		EnterpriseE, // Windows 10 Enterprise E
		EnterpriseEvaluation, // Windows 10 Enterprise Evaluation
		EnterpriseN, // Windows 10 Enterprise N
		EnterpriseNEvaluation, // Windows 10 Enterprise N Evaluation
		EnterpriseS, // Windows 10 Enterprise 2015 LTSB
		EnterpriseSEvaluation, // Windows 10 Enterprise 2015 LTSB Evaluation
		EnterpriseSN, // Windows 10 Enterprise 2015 LTSB N
		EnterpriseSNEvaluation, // Windows 10 Enterprise 2015 LTSB N Evaluation
		EnterpriseServer, // Server Enterprise (full installation)
		EnterpriseServerCore, // Server Enterprise (core installation)
		EnterpriseServerCoreV, // Server Enterprise without Hyper-V (core installation)
		EnterpriseServer_IA64, // Server Enterprise for Itanium-based Systems
		EnterpriseServerV, // Server Enterprise without Hyper-V (full installation)
		EssentialbusinessServerADDL, // Windows Essential Server Solution Additional
		EssentialbusinessServerADDLSVC, // Windows Essential Server Solution Additional SVC
		EssentialbusinessServerMGMT, // Windows Essential Server Solution Management
		EssentialbusinessServerMGMTSVC, // Windows Essential Server Solution Management SVC
		HomeBasic, // Home Basic
		HomeBasicE, // Not supported
		HomeBasicN, // Home Basic N
		HomePremium, // Home Premium
		HomePremiumE, // Not supported
		HomePremiumN, // Home Premium N
		HomePremiumServer, // Windows Home Server 2011
		HomeServer, // Windows Storage Server 2008 R2 Essentials
		HyperV, // Microsoft Hyper-V Server
		IoTUAP, // Windows 10 IoT Core
		IoTUAPCommercial, // PRODUCT_IOTUAPCOMMERCIAL Windows 10 IoT Core Commercial
		MediumBusinessServerManagement, // Windows Essential Business Server Management Server
		MediumBusinessServerMessaging, // Windows Essential Business Server Messaging Server
		MediumBusinessServerSecurity, // Windows Essential Business Server Security Server
		MobileCore, // PRODUCT_MOBILE_CORE Windows 10 Mobile
		MobileEnterprise, // PRODUCT_MOBILE_ENTERPRISE Windows 10 Mobile Enterprise
		MultipointPremiumServer, // Windows MultiPoint Server Premium (full installation)
		MultipointStandardServer, // Windows MultiPoint Server Standard (full installation)
		ProWorkstation, // Windows 10 Pro for Workstations
		ProWorkstationN, // Windows 10 Pro for Workstations N
		Professional, // Windows 10 Pro
		ProfessionalE, // Not supported
		ProfessionalN, // Windows 10 Pro N
		ProfessionalWMC, // Professional with Media Center
		SbSolutionServer, // Windows Small Business Server 2011 Essentials
		SbSolutionServerEM, // Server For SB Solutions EM
		ServerForSbSolutions, // Server For SB Solutions
		ServerForSbSolutionsEM, // Server For SB Solutions EM
		ServerForSmallBusiness, // Windows Server 2008 for Windows Essential Server Solutions
		ServerForSmallBusinessV, // Windows Server 2008 without Hyper-V for Windows Essential Server Solutions
		ServerFoundation, // Server Foundation
		SmallBusinessServer, // Windows Small Business Server
		SmallBusinessServerPremium, // Small Business Server Premium
		SmallBusinessServerPremiumCore, // Small Business Server Premium (core installation)
		SolutionEmbeddedserver, // Windows MultiPoint Server
		StandardEvaluationServer, // Server Standard (evaluation installation)
		StandardServer, // Server Standard (full installation. For Server Core installations of Windows Server 2012 and later, use the method, Determining whether Server Core is running.)
		StandardServerCore, // Server Standard (core installation, Windows Server 2008 R2 and earlier)
		StandardServerCoreV, // Server Standard without Hyper-V (core installation)
		StandardServerV, // Server Standard without Hyper-V
		StandardServerSolutions, // Server Solutions Premium
		StandardServerSolutionsCore, // Server Solutions Premium (core installation)
		Starter, // Starter
		StarterE, // Not supported
		StarterN, // Starter N
		StorageEnterpriseServer, // Storage Server Enterprise
		StorageEnterpriseServerCore, // Storage Server Enterprise (core installation)
		StorageExpressServer, // Storage Server Express
		StorageExpressServerCore, // Storage Server Express (core installation)
		StorageStandardEvaluationServer, // Storage Server Standard (evaluation installation)
		StorageStandardServer, // Storage Server Standard
		StorageStandardServerCore, // Storage Server Standard (core installation)
		StorageWorkgroupEvaluationServer, // Storage Server Workgroup (evaluation installation)
		StorageWorkgroupServer, // Storage Server Workgroup
		StorageWorkgroupServerCore, // Storage Server Workgroup (core installation)
		Ultimate, // Ultimate
		UltimateE, // Not supported
		UltimateN, // Ultimate N
		WebServer, // Web Server (full installation)
		WebServerCore // Web Server (core installation)
	};
	enum class SystemProductSuite: uint32_t
	{
		None = 0,

		ServerNT = 1 << 0,
		WorkstationNT = 1 << 1,
		Communications = 1 << 2,
		SecurityAppliance = 1 << 3,

		// Windows Server 2008 Enterprise, Windows Server 2003, Enterprise Edition,
		// or Windows 2000 Advanced Server is installed.
		Enterprise = 1 << 4,

		// Microsoft BackOffice components are installed.
		BackOffice = 1 << 5,

		// Terminal Services is installed. This value is always set. If VER_SUITE_TERMINAL is set
		// but VER_SUITE_SINGLEUSERTS is not set, the system is running in application server mode.
		Terminal = 1 << 6,

		// Microsoft Small Business Server was once installed on the system, but may have been upgraded
		// to another version of Windows.
		SmallBusiness = 1 << 7,

		// Microsoft Small Business Server is installed with the restrictive client license in force.
		SmallBusinessRestricted = 1 << 8,

		// Windows XP Embedded is installed.
		EmbeddedNT = 1 << 9,
		EmbeddedRestricted = 1 << 10,

		// Windows Server 2008 Datacenter, Windows Server 2003, Datacenter Edition,
		// or Windows 2000 Datacenter Server is installed.
		DataCenter = 1 << 11,

		// Remote Desktop is supported, but only one interactive session is supported.
		// This value is set unless the system is running in application server mode.
		SingleUserTS = 1 << 12,

		// AppServer mode is enabled.
		MultiUserTS = 1 << 13,

		// Windows Vista Home Premium, Windows Vista Home Basic, or Windows XP Home Edition is installed.
		Personal = 1 << 14,

		// Windows Server 2003, Web Edition is installed.
		Blade = 1 << 15,

		// Windows Storage Server 2003 R2 or Windows Storage Server 2003is installed.
		StorageServer = 1 << 16,

		// Windows Server 2003, Compute Cluster Edition is installed.
		ComputeServer = 1 << 17,

		// Windows Home Server is installed.
		HomeServer = 1 << 18
	};

	namespace EnumClass
	{
		Kx_EnumClass_AllowEverything(ExitWorkstationCommand);
		Kx_EnumClass_AllowEverything(DisplayDeviceFlag);
		Kx_EnumClass_AllowEverything(SystemProductSuite);
	}
}
