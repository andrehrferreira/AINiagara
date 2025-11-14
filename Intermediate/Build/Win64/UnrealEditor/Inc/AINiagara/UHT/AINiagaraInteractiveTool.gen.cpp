// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "Tools/AINiagaraInteractiveTool.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeAINiagaraInteractiveTool() {}

// ********** Begin Cross Module References ********************************************************
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraInteractiveTool();
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraInteractiveTool_NoRegister();
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraInteractiveToolBuilder();
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_NoRegister();
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraInteractiveToolProperties();
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraInteractiveToolProperties_NoRegister();
COREUOBJECT_API UScriptStruct* Z_Construct_UScriptStruct_FVector();
INTERACTIVETOOLSFRAMEWORK_API UClass* Z_Construct_UClass_UInteractiveTool();
INTERACTIVETOOLSFRAMEWORK_API UClass* Z_Construct_UClass_UInteractiveToolBuilder();
INTERACTIVETOOLSFRAMEWORK_API UClass* Z_Construct_UClass_UInteractiveToolPropertySet();
UPackage* Z_Construct_UPackage__Script_AINiagara();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UAINiagaraInteractiveToolBuilder *****************************************
void UAINiagaraInteractiveToolBuilder::StaticRegisterNativesUAINiagaraInteractiveToolBuilder()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder;
UClass* UAINiagaraInteractiveToolBuilder::GetPrivateStaticClass()
{
	using TClass = UAINiagaraInteractiveToolBuilder;
	if (!Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("AINiagaraInteractiveToolBuilder"),
			Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder.InnerSingleton,
			StaticRegisterNativesUAINiagaraInteractiveToolBuilder,
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
	return Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder.InnerSingleton;
}
UClass* Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_NoRegister()
{
	return UAINiagaraInteractiveToolBuilder::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Builder for UAINiagaraInteractiveTool\n */" },
#endif
		{ "IncludePath", "Tools/AINiagaraInteractiveTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraInteractiveTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Builder for UAINiagaraInteractiveTool" },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraInteractiveToolBuilder>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UInteractiveToolBuilder,
	(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_Statics::ClassParams = {
	&UAINiagaraInteractiveToolBuilder::StaticClass,
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
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_Statics::Class_MetaDataParams), Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAINiagaraInteractiveToolBuilder()
{
	if (!Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder.OuterSingleton, Z_Construct_UClass_UAINiagaraInteractiveToolBuilder_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder.OuterSingleton;
}
UAINiagaraInteractiveToolBuilder::UAINiagaraInteractiveToolBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraInteractiveToolBuilder);
UAINiagaraInteractiveToolBuilder::~UAINiagaraInteractiveToolBuilder() {}
// ********** End Class UAINiagaraInteractiveToolBuilder *******************************************

// ********** Begin Class UAINiagaraInteractiveToolProperties **************************************
void UAINiagaraInteractiveToolProperties::StaticRegisterNativesUAINiagaraInteractiveToolProperties()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties;
UClass* UAINiagaraInteractiveToolProperties::GetPrivateStaticClass()
{
	using TClass = UAINiagaraInteractiveToolProperties;
	if (!Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("AINiagaraInteractiveToolProperties"),
			Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties.InnerSingleton,
			StaticRegisterNativesUAINiagaraInteractiveToolProperties,
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
	return Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties.InnerSingleton;
}
UClass* Z_Construct_UClass_UAINiagaraInteractiveToolProperties_NoRegister()
{
	return UAINiagaraInteractiveToolProperties::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * Property set for the UAINiagaraInteractiveTool\n */" },
#endif
		{ "IncludePath", "Tools/AINiagaraInteractiveTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraInteractiveTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Property set for the UAINiagaraInteractiveTool" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StartPoint_MetaData[] = {
		{ "Category", "Options" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** First point of measurement */" },
#endif
		{ "ModuleRelativePath", "Private/Tools/AINiagaraInteractiveTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "First point of measurement" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_EndPoint_MetaData[] = {
		{ "Category", "Options" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Second point of measurement */" },
#endif
		{ "ModuleRelativePath", "Private/Tools/AINiagaraInteractiveTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Second point of measurement" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Distance_MetaData[] = {
		{ "Category", "Options" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Current distance measurement */" },
#endif
		{ "ModuleRelativePath", "Private/Tools/AINiagaraInteractiveTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Current distance measurement" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStructPropertyParams NewProp_StartPoint;
	static const UECodeGen_Private::FStructPropertyParams NewProp_EndPoint;
	static const UECodeGen_Private::FDoublePropertyParams NewProp_Distance;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraInteractiveToolProperties>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::NewProp_StartPoint = { "StartPoint", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAINiagaraInteractiveToolProperties, StartPoint), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StartPoint_MetaData), NewProp_StartPoint_MetaData) };
const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::NewProp_EndPoint = { "EndPoint", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAINiagaraInteractiveToolProperties, EndPoint), Z_Construct_UScriptStruct_FVector, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_EndPoint_MetaData), NewProp_EndPoint_MetaData) };
const UECodeGen_Private::FDoublePropertyParams Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::NewProp_Distance = { "Distance", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAINiagaraInteractiveToolProperties, Distance), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Distance_MetaData), NewProp_Distance_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::NewProp_StartPoint,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::NewProp_EndPoint,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::NewProp_Distance,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UInteractiveToolPropertySet,
	(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::ClassParams = {
	&UAINiagaraInteractiveToolProperties::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::PropPointers),
	0,
	0x001000A8u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::Class_MetaDataParams), Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAINiagaraInteractiveToolProperties()
{
	if (!Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties.OuterSingleton, Z_Construct_UClass_UAINiagaraInteractiveToolProperties_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraInteractiveToolProperties);
UAINiagaraInteractiveToolProperties::~UAINiagaraInteractiveToolProperties() {}
// ********** End Class UAINiagaraInteractiveToolProperties ****************************************

// ********** Begin Class UAINiagaraInteractiveTool ************************************************
void UAINiagaraInteractiveTool::StaticRegisterNativesUAINiagaraInteractiveTool()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UAINiagaraInteractiveTool;
UClass* UAINiagaraInteractiveTool::GetPrivateStaticClass()
{
	using TClass = UAINiagaraInteractiveTool;
	if (!Z_Registration_Info_UClass_UAINiagaraInteractiveTool.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("AINiagaraInteractiveTool"),
			Z_Registration_Info_UClass_UAINiagaraInteractiveTool.InnerSingleton,
			StaticRegisterNativesUAINiagaraInteractiveTool,
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
	return Z_Registration_Info_UClass_UAINiagaraInteractiveTool.InnerSingleton;
}
UClass* Z_Construct_UClass_UAINiagaraInteractiveTool_NoRegister()
{
	return UAINiagaraInteractiveTool::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAINiagaraInteractiveTool_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * UAINiagaraInteractiveTool is an example Tool that allows the user to measure the \n * distance between two points. The first point is set by click-dragging the mouse, and\n * the second point is set by shift-click-dragging the mouse.\n */" },
#endif
		{ "IncludePath", "Tools/AINiagaraInteractiveTool.h" },
		{ "ModuleRelativePath", "Private/Tools/AINiagaraInteractiveTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "UAINiagaraInteractiveTool is an example Tool that allows the user to measure the\ndistance between two points. The first point is set by click-dragging the mouse, and\nthe second point is set by shift-click-dragging the mouse." },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_Properties_MetaData[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/** Properties of the tool are stored here */" },
#endif
		{ "ModuleRelativePath", "Private/Tools/AINiagaraInteractiveTool.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Properties of the tool are stored here" },
#endif
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FObjectPropertyParams NewProp_Properties;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraInteractiveTool>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::NewProp_Properties = { "Properties", nullptr, (EPropertyFlags)0x0124080000000000, UECodeGen_Private::EPropertyGenFlags::Object | UECodeGen_Private::EPropertyGenFlags::ObjectPtr, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(UAINiagaraInteractiveTool, Properties), Z_Construct_UClass_UAINiagaraInteractiveToolProperties_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_Properties_MetaData), NewProp_Properties_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::NewProp_Properties,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UInteractiveTool,
	(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::ClassParams = {
	&UAINiagaraInteractiveTool::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::PropPointers),
	0,
	0x001000A8u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::Class_MetaDataParams), Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAINiagaraInteractiveTool()
{
	if (!Z_Registration_Info_UClass_UAINiagaraInteractiveTool.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraInteractiveTool.OuterSingleton, Z_Construct_UClass_UAINiagaraInteractiveTool_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAINiagaraInteractiveTool.OuterSingleton;
}
UAINiagaraInteractiveTool::UAINiagaraInteractiveTool() {}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraInteractiveTool);
UAINiagaraInteractiveTool::~UAINiagaraInteractiveTool() {}
// ********** End Class UAINiagaraInteractiveTool **************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraInteractiveTool_h__Script_AINiagara_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAINiagaraInteractiveToolBuilder, UAINiagaraInteractiveToolBuilder::StaticClass, TEXT("UAINiagaraInteractiveToolBuilder"), &Z_Registration_Info_UClass_UAINiagaraInteractiveToolBuilder, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraInteractiveToolBuilder), 3381007244U) },
		{ Z_Construct_UClass_UAINiagaraInteractiveToolProperties, UAINiagaraInteractiveToolProperties::StaticClass, TEXT("UAINiagaraInteractiveToolProperties"), &Z_Registration_Info_UClass_UAINiagaraInteractiveToolProperties, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraInteractiveToolProperties), 1489812250U) },
		{ Z_Construct_UClass_UAINiagaraInteractiveTool, UAINiagaraInteractiveTool::StaticClass, TEXT("UAINiagaraInteractiveTool"), &Z_Registration_Info_UClass_UAINiagaraInteractiveTool, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraInteractiveTool), 479793014U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraInteractiveTool_h__Script_AINiagara_179536085(TEXT("/Script/AINiagara"),
	Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraInteractiveTool_h__Script_AINiagara_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Private_Tools_AINiagaraInteractiveTool_h__Script_AINiagara_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
