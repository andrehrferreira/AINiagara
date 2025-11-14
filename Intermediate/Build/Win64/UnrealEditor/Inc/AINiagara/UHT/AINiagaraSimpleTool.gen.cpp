// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Tools/AINiagaraSimpleTool.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeAINiagaraSimpleTool() {}

// ********** Begin Cross Module References ********************************************************
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
// ********** End Cross Module References **********************************************************

// ********** Begin Class UAINiagaraSimpleToolBuilder **********************************************
void UAINiagaraSimpleToolBuilder::StaticRegisterNativesUAINiagaraSimpleToolBuilder()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder;
UClass* UAINiagaraSimpleToolBuilder::GetPrivateStaticClass()
{
	using TClass = UAINiagaraSimpleToolBuilder;
	if (!Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("AINiagaraSimpleToolBuilder"),
			Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder.InnerSingleton,
			StaticRegisterNativesUAINiagaraSimpleToolBuilder,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder.InnerSingleton;
}
UClass* Z_Construct_UClass_UAINiagaraSimpleToolBuilder_NoRegister()
{
	return UAINiagaraSimpleToolBuilder::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Builder for UAINiagaraSimpleTool\n */" },
#endif
		{ "IncludePath", "Tools/AINiagaraSimpleTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Builder for UAINiagaraSimpleTool" },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraSimpleToolBuilder>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UInteractiveToolBuilder,
	(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolBuilder_Statics::DependentSingletons) < 16);
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
UAINiagaraSimpleToolBuilder::UAINiagaraSimpleToolBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraSimpleToolBuilder);
UAINiagaraSimpleToolBuilder::~UAINiagaraSimpleToolBuilder() {}
// ********** End Class UAINiagaraSimpleToolBuilder ************************************************

// ********** Begin Class UAINiagaraSimpleToolProperties *******************************************
void UAINiagaraSimpleToolProperties::StaticRegisterNativesUAINiagaraSimpleToolProperties()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties;
UClass* UAINiagaraSimpleToolProperties::GetPrivateStaticClass()
{
	using TClass = UAINiagaraSimpleToolProperties;
	if (!Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("AINiagaraSimpleToolProperties"),
			Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.InnerSingleton,
			StaticRegisterNativesUAINiagaraSimpleToolProperties,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.InnerSingleton;
}
UClass* Z_Construct_UClass_UAINiagaraSimpleToolProperties_NoRegister()
{
	return UAINiagaraSimpleToolProperties::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Settings UObject for UAINiagaraSimpleTool. This UClass inherits from UInteractiveToolPropertySet,\n * which provides an OnModified delegate that the Tool will listen to for changes in property values.\n */" },
#endif
		{ "IncludePath", "Tools/AINiagaraSimpleTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Settings UObject for UAINiagaraSimpleTool. This UClass inherits from UInteractiveToolPropertySet,\nwhich provides an OnModified delegate that the Tool will listen to for changes in property values." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_ShowExtendedInfo_MetaData[] = {
		{ "Category", "Options" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** If enabled, dialog should display extended information about the actor clicked on. Otherwise, only basic info will be shown. */" },
#endif
		{ "DisplayName", "Show Extended Info" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "If enabled, dialog should display extended information about the actor clicked on. Otherwise, only basic info will be shown." },
#endif
	};
#endif // WITH_METADATA
	static void NewProp_ShowExtendedInfo_SetBit(void* Obj);
	static const UECodeGen_Private::FBoolPropertyParams NewProp_ShowExtendedInfo;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraSimpleToolProperties>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
void Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo_SetBit(void* Obj)
{
	((UAINiagaraSimpleToolProperties*)Obj)->ShowExtendedInfo = 1;
}
const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo = { "ShowExtendedInfo", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, sizeof(bool), sizeof(UAINiagaraSimpleToolProperties), &Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo_SetBit, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_ShowExtendedInfo_MetaData), NewProp_ShowExtendedInfo_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::NewProp_ShowExtendedInfo,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UInteractiveToolPropertySet,
	(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::DependentSingletons) < 16);
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
UClass* Z_Construct_UClass_UAINiagaraSimpleToolProperties()
{
	if (!Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.OuterSingleton, Z_Construct_UClass_UAINiagaraSimpleToolProperties_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraSimpleToolProperties);
UAINiagaraSimpleToolProperties::~UAINiagaraSimpleToolProperties() {}
// ********** End Class UAINiagaraSimpleToolProperties *********************************************

// ********** Begin Class UAINiagaraSimpleTool *****************************************************
void UAINiagaraSimpleTool::StaticRegisterNativesUAINiagaraSimpleTool()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UAINiagaraSimpleTool;
UClass* UAINiagaraSimpleTool::GetPrivateStaticClass()
{
	using TClass = UAINiagaraSimpleTool;
	if (!Z_Registration_Info_UClass_UAINiagaraSimpleTool.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("AINiagaraSimpleTool"),
			Z_Registration_Info_UClass_UAINiagaraSimpleTool.InnerSingleton,
			StaticRegisterNativesUAINiagaraSimpleTool,
			sizeof(TClass),
			alignof(TClass),
			TClass::StaticClassFlags,
			TClass::StaticClassCastFlags(),
			TClass::StaticConfigName(),
			(UClass::ClassConstructorType)InternalConstructor<TClass>,
			(UClass::ClassVTableHelperCtorCallerType)InternalVTableHelperCtorCaller<TClass>,
			UOBJECT_CPPCLASS_STATICFUNCTIONS_FORCLASS(TClass),
			&TClass::Super::StaticClass,
			&TClass::WithinClass::StaticClass
		);
	}
	return Z_Registration_Info_UClass_UAINiagaraSimpleTool.InnerSingleton;
}
UClass* Z_Construct_UClass_UAINiagaraSimpleTool_NoRegister()
{
	return UAINiagaraSimpleTool::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAINiagaraSimpleTool_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * UAINiagaraSimpleTool is an example Tool that opens a message box displaying info about an actor that the user\n * clicks left mouse button. All the action is in the ::OnClicked handler.\n */" },
#endif
		{ "IncludePath", "Tools/AINiagaraSimpleTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "UAINiagaraSimpleTool is an example Tool that opens a message box displaying info about an actor that the user\nclicks left mouse button. All the action is in the ::OnClicked handler." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Properties_MetaData[] = {
		{ "ModuleRelativePath", "Private/Tools/AINiagaraSimpleTool.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Properties;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraSimpleTool>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAINiagaraSimpleTool_Statics::NewProp_Properties = { "Properties", nullptr, (EPropertyFlags)0x0124080000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAINiagaraSimpleTool, Properties), Z_Construct_UClass_UAINiagaraSimpleToolProperties_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Properties_MetaData), NewProp_Properties_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAINiagaraSimpleTool_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraSimpleTool_Statics::NewProp_Properties,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleTool_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UAINiagaraSimpleTool_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_USingleClickTool,
	(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraSimpleTool_Statics::DependentSingletons) < 16);
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
UClass* Z_Construct_UClass_UAINiagaraSimpleTool()
{
	if (!Z_Registration_Info_UClass_UAINiagaraSimpleTool.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraSimpleTool.OuterSingleton, Z_Construct_UClass_UAINiagaraSimpleTool_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAINiagaraSimpleTool.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraSimpleTool);
UAINiagaraSimpleTool::~UAINiagaraSimpleTool() {}
// ********** End Class UAINiagaraSimpleTool *******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h__Script_AINiagara_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAINiagaraSimpleToolBuilder, UAINiagaraSimpleToolBuilder::StaticClass, TEXT("UAINiagaraSimpleToolBuilder"), &Z_Registration_Info_UClass_UAINiagaraSimpleToolBuilder, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraSimpleToolBuilder), 3698830850U) },
		{ Z_Construct_UClass_UAINiagaraSimpleToolProperties, UAINiagaraSimpleToolProperties::StaticClass, TEXT("UAINiagaraSimpleToolProperties"), &Z_Registration_Info_UClass_UAINiagaraSimpleToolProperties, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraSimpleToolProperties), 234160580U) },
		{ Z_Construct_UClass_UAINiagaraSimpleTool, UAINiagaraSimpleTool::StaticClass, TEXT("UAINiagaraSimpleTool"), &Z_Registration_Info_UClass_UAINiagaraSimpleTool, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraSimpleTool), 3718747776U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h__Script_AINiagara_270741513(TEXT("/Script/AINiagara"),
	Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h__Script_AINiagara_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraSimpleTool_h__Script_AINiagara_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
