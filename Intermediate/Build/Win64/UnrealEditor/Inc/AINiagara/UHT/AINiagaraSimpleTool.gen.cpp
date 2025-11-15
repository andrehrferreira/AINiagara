// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AINiagara/Private/Tools/AINiagaraSimpleTool.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAINiagaraSimpleTool() {}
// Cross Module References
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraSimpleTool();
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraSimpleTool_NoRegister();
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraSimpleToolBuilder();
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraSimpleToolBuilder_NoRegister();
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraSimpleToolProperties();
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraSimpleToolProperties_NoRegister();
	INTERACTIVETOOLSFRAMEWORK_API UClass* Z_Construct_UClass_UInteractiveToolBuilder();
	INTERACTIVETOOLSFRAMEWORK_API UClass* Z_Construct_UClass_UInteractiveToolPropertySet();
	INTERACTIVETOOLSFRAMEWORK_API UClass* Z_Construct_UClass_USingleClickTool();
	UPackage* Z_Construct_UPackage__Script_AINiagara();
// End Cross Module References
	void UAINiagaraSimpleToolBuilder::StaticRegisterNativesUAINiagaraSimpleToolBuilder()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAINiagaraSimpleToolBuilder);
	UClass* Z_Construct_UClass_UAINiagaraSimpleToolBuilder_NoRegister()
	{
		return UAINiagaraSimpleToolBuilder::StaticClass();
	}
	struct Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UInteractiveToolBuilder,
		(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Builder for UAINiagaraSimpleTool\n */" },
		{ "IncludePath", "Tools/AINiagaraSimpleTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
		{ "ToolTip", "Builder for UAINiagaraSimpleTool" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraSimpleToolBuilder>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::ClassParams = {
		&UAINiagaraSimpleToolBuilder::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x001000A8u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::Class_MetaDataParams), Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::Class_MetaDataParams)
	};
	UClass* Z_Construct_UClass_UAINiagaraSimpleToolBuilder()
	{
		if (!Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder.OuterSingleton, Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder.OuterSingleton;
	}
	template<> AINIAGARA_API UClass* StaticClass<UAINiagaraSimpleToolBuilder>()
	{
		return UAINiagaraSimpleToolBuilder::StaticClass();
	}
	UAINiagaraSimpleToolBuilder::UAINiagaraSimpleToolBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraSimpleToolBuilder);
	UAINiagaraSimpleToolBuilder::~UAINiagaraSimpleToolBuilder() {}
	void UAINiagaraSimpleToolProperties::StaticRegisterNativesUAINiagaraSimpleToolProperties()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAINiagaraSimpleToolProperties);
	UClass* Z_Construct_UClass_UAINiagaraSimpleToolProperties_NoRegister()
	{
		return UAINiagaraSimpleToolProperties::StaticClass();
	}
	struct Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ShowExtendedInfo_MetaData[];
#endif
		static void NewProp_ShowExtendedInfo_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ShowExtendedInfo;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UInteractiveToolPropertySet,
		(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * Settings UObject for UAINiagaraSimpleTool. This UClass inherits from UInteractiveToolPropertySet,\n * which provides an OnModified delegate that the Tool will listen to for changes in property values.\n */" },
		{ "IncludePath", "Tools/AINiagaraSimpleTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
		{ "ToolTip", "Settings UObject for UAINiagaraSimpleTool. This UClass inherits from UInteractiveToolPropertySet,\nwhich provides an OnModified delegate that the Tool will listen to for changes in property values." },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo_MetaData[] = {
		{ "Category", "Options" },
		{ "Comment", "/** If enabled, dialog should display extended information about the actor clicked on. Otherwise, only basic info will be shown. */" },
		{ "DisplayName", "Show Extended Info" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
		{ "ToolTip", "If enabled, dialog should display extended information about the actor clicked on. Otherwise, only basic info will be shown." },
	};
#endif
	void Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo_SetBit(void* Obj)
	{
		((UAINiagaraSimpleToolProperties*)Obj)->ShowExtendedInfo = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo = { "ShowExtendedInfo", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UAINiagaraSimpleToolProperties), &Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo_MetaData), Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo_MetaData) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraSimpleToolProperties>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::ClassParams = {
		&UAINiagaraSimpleToolProperties::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::PropPointers),
		0,
		0x001000A8u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::Class_MetaDataParams), Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UAINiagaraSimpleToolProperties()
	{
		if (!Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.OuterSingleton, Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.OuterSingleton;
	}
	template<> AINIAGARA_API UClass* StaticClass<UAINiagaraSimpleToolProperties>()
	{
		return UAINiagaraSimpleToolProperties::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraSimpleToolProperties);
	UAINiagaraSimpleToolProperties::~UAINiagaraSimpleToolProperties() {}
	void UAINiagaraSimpleTool::StaticRegisterNativesUAINiagaraSimpleTool()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAINiagaraSimpleTool);
	UClass* Z_Construct_UClass_UAINiagaraSimpleTool_NoRegister()
	{
		return UAINiagaraSimpleTool::StaticClass();
	}
	struct Z_Construct_UClass_UAINiagaraSimpleTool_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Properties_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPtrPropertyParams NewProp_Properties;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAINiagaraSimpleTool_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_USingleClickTool,
		(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleTool_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAINiagaraSimpleTool_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * UAINiagaraSimpleTool is an example Tool that opens a message box displaying info about an actor that the user\n * clicks left mouse button. All the action is in the ::OnClicked handler.\n */" },
		{ "IncludePath", "Tools/AINiagaraSimpleTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
		{ "ToolTip", "UAINiagaraSimpleTool is an example Tool that opens a message box displaying info about an actor that the user\nclicks left mouse button. All the action is in the ::OnClicked handler." },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAINiagaraSimpleTool_Statics::NewProp_Properties_MetaData[] = {
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
	};
#endif
	const UECodeGen_Private::FObjectPtrPropertyParams Z_Construct_UClass_UAINiagaraSimpleTool_Statics::NewProp_Properties = { "Properties", nullptr, (EPropertyFlags)0x0024080000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAINiagaraSimpleTool, Properties), Z_Construct_UClass_UAINiagaraSimpleToolProperties_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleTool_Statics::NewProp_Properties_MetaData), Z_Construct_UClass_UAINiagaraSimpleTool_Statics::NewProp_Properties_MetaData) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAINiagaraSimpleTool_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraSimpleTool_Statics::NewProp_Properties,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAINiagaraSimpleTool_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraSimpleTool>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAINiagaraSimpleTool_Statics::ClassParams = {
		&UAINiagaraSimpleTool::StaticClass,
		nullptr,
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UAINiagaraSimpleTool_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleTool_Statics::PropPointers),
		0,
		0x001000A8u,
		METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleTool_Statics::Class_MetaDataParams), Z_Construct_UClass_UAINiagaraSimpleTool_Statics::Class_MetaDataParams)
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleTool_Statics::PropPointers) < 2048);
	UClass* Z_Construct_UClass_UAINiagaraSimpleTool()
	{
		if (!Z_Registration_Info_UClass_UAINiagaraSimpleTool.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraSimpleTool.OuterSingleton, Z_Construct_UClass_UAINiagaraSimpleTool_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAINiagaraSimpleTool.OuterSingleton;
	}
	template<> AINIAGARA_API UClass* StaticClass<UAINiagaraSimpleTool>()
	{
		return UAINiagaraSimpleTool::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraSimpleTool);
	UAINiagaraSimpleTool::~UAINiagaraSimpleTool() {}
	struct Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UAINiagaraSimpleToolBuilder, UAINiagaraSimpleToolBuilder::StaticClass, TEXT("UAINiagaraSimpleToolBuilder"), &Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraSimpleToolBuilder), 2244814397U) },
		{ Z_Construct_UClass_UAINiagaraSimpleToolProperties, UAINiagaraSimpleToolProperties::StaticClass, TEXT("UAINiagaraSimpleToolProperties"), &Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraSimpleToolProperties), 2307472668U) },
		{ Z_Construct_UClass_UAINiagaraSimpleTool, UAINiagaraSimpleTool::StaticClass, TEXT("UAINiagaraSimpleTool"), &Z_Registration_Info_UClass_UAINiagaraSimpleTool, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraSimpleTool), 3914410782U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h_706106855(TEXT("/Script/AINiagara"),
		Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
