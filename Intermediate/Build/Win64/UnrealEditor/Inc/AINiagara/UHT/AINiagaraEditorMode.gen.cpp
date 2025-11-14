// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AINiagaraEditorMode.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS

void EmptyLinkFunctionForGeneratedCodeAINiagaraEditorMode() {}

// ********** Begin Cross Module References ********************************************************
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraEditorMode();
AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraEditorMode_NoRegister();
UNREALED_API UClass* Z_Construct_UClass_UEdMode();
UPackage* Z_Construct_UPackage__Script_AINiagara();
// ********** End Cross Module References **********************************************************

// ********** Begin Class UAINiagaraEditorMode *****************************************************
void UAINiagaraEditorMode::StaticRegisterNativesUAINiagaraEditorMode()
{
}
FClassRegistrationInfo Z_Registration_Info_UClass_UAINiagaraEditorMode;
UClass* UAINiagaraEditorMode::GetPrivateStaticClass()
{
	using TClass = UAINiagaraEditorMode;
	if (!Z_Registration_Info_UClass_UAINiagaraEditorMode.InnerSingleton)
	{
		GetPrivateStaticClassBody(
			StaticPackage(),
			TEXT("AINiagaraEditorMode"),
			Z_Registration_Info_UClass_UAINiagaraEditorMode.InnerSingleton,
			StaticRegisterNativesUAINiagaraEditorMode,
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
	return Z_Registration_Info_UClass_UAINiagaraEditorMode.InnerSingleton;
}
UClass* Z_Construct_UClass_UAINiagaraEditorMode_NoRegister()
{
	return UAINiagaraEditorMode::GetPrivateStaticClass();
}
struct Z_Construct_UClass_UAINiagaraEditorMode_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
#if !UE_BUILD_SHIPPING
		{ "Comment", "/**\n * This class provides an example of how to extend a UEdMode to add some simple tools\n * using the InteractiveTools framework. The various UEdMode input event handlers (see UEdMode.h)\n * forward events to a UEdModeInteractiveToolsContext instance, which\n * has all the logic for interacting with the InputRouter, ToolManager, etc.\n * The functions provided here are the minimum to get started inserting some custom behavior.\n * Take a look at the UEdMode markup for more extensibility options.\n */" },
#endif
		{ "IncludePath", "AINiagaraEditorMode.h" },
		{ "ModuleRelativePath", "Public/AINiagaraEditorMode.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "This class provides an example of how to extend a UEdMode to add some simple tools\nusing the InteractiveTools framework. The various UEdMode input event handlers (see UEdMode.h)\nforward events to a UEdModeInteractiveToolsContext instance, which\nhas all the logic for interacting with the InputRouter, ToolManager, etc.\nThe functions provided here are the minimum to get started inserting some custom behavior.\nTake a look at the UEdMode markup for more extensibility options." },
#endif
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraEditorMode>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_UAINiagaraEditorMode_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UEdMode,
	(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraEditorMode_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_UAINiagaraEditorMode_Statics::ClassParams = {
	&UAINiagaraEditorMode::StaticClass,
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
	0x000000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraEditorMode_Statics::Class_MetaDataParams), Z_Construct_UClass_UAINiagaraEditorMode_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_UAINiagaraEditorMode()
{
	if (!Z_Registration_Info_UClass_UAINiagaraEditorMode.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAINiagaraEditorMode.OuterSingleton, Z_Construct_UClass_UAINiagaraEditorMode_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_UAINiagaraEditorMode.OuterSingleton;
}
DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraEditorMode);
// ********** End Class UAINiagaraEditorMode *******************************************************

// ********** Begin Registration *******************************************************************
struct Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h__Script_AINiagara_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_UAINiagaraEditorMode, UAINiagaraEditorMode::StaticClass, TEXT("UAINiagaraEditorMode"), &Z_Registration_Info_UClass_UAINiagaraEditorMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraEditorMode), 3898058132U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h__Script_AINiagara_2870774845(TEXT("/Script/AINiagara"),
	Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h__Script_AINiagara_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_NetworkSample_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h__Script_AINiagara_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// ********** End Registration *********************************************************************

PRAGMA_ENABLE_DEPRECATION_WARNINGS
