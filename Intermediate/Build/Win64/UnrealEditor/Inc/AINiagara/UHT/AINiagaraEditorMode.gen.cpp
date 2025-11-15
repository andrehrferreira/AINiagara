// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AINiagara/Public/AINiagaraEditorMode.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAINiagaraEditorMode() {}
// Cross Module References
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraEditorMode();
	AINIAGARA_API UClass* Z_Construct_UClass_UAINiagaraEditorMode_NoRegister();
	UNREALED_API UClass* Z_Construct_UClass_UEdMode();
	UPackage* Z_Construct_UPackage__Script_AINiagara();
// End Cross Module References
	void UAINiagaraEditorMode::StaticRegisterNativesUAINiagaraEditorMode()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAINiagaraEditorMode);
	UClass* Z_Construct_UClass_UAINiagaraEditorMode_NoRegister()
	{
		return UAINiagaraEditorMode::StaticClass();
	}
	struct Z_Construct_UClass_UAINiagaraEditorMode_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAINiagaraEditorMode_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UEdMode,
		(UObject* (*)())Z_Construct_UPackage__Script_AINiagara,
	};
	static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_UAINiagaraEditorMode_Statics::DependentSingletons) < 16);
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAINiagaraEditorMode_Statics::Class_MetaDataParams[] = {
		{ "Comment", "/**\n * This class provides an example of how to extend a UEdMode to add some simple tools\n * using the InteractiveTools framework. The various UEdMode input event handlers (see UEdMode.h)\n * forward events to a UEdModeInteractiveToolsContext instance, which\n * has all the logic for interacting with the InputRouter, ToolManager, etc.\n * The functions provided here are the minimum to get started inserting some custom behavior.\n * Take a look at the UEdMode markup for more extensibility options.\n */" },
		{ "IncludePath", "AINiagaraEditorMode.h" },
		{ "ModuleRelativePath", "Public/AINiagaraEditorMode.h" },
		{ "ToolTip", "This class provides an example of how to extend a UEdMode to add some simple tools\nusing the InteractiveTools framework. The various UEdMode input event handlers (see UEdMode.h)\nforward events to a UEdModeInteractiveToolsContext instance, which\nhas all the logic for interacting with the InputRouter, ToolManager, etc.\nThe functions provided here are the minimum to get started inserting some custom behavior.\nTake a look at the UEdMode markup for more extensibility options." },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAINiagaraEditorMode_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAINiagaraEditorMode>::IsAbstract,
	};
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
	template<> AINIAGARA_API UClass* StaticClass<UAINiagaraEditorMode>()
	{
		return UAINiagaraEditorMode::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAINiagaraEditorMode);
	struct Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UAINiagaraEditorMode, UAINiagaraEditorMode::StaticClass, TEXT("UAINiagaraEditorMode"), &Z_Registration_Info_UClass_UAINiagaraEditorMode, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAINiagaraEditorMode), 2015245212U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h_548190254(TEXT("/Script/AINiagara"),
		Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_G__ToS_Plugins_AINiagara_Source_AINiagara_Public_AINiagaraEditorMode_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
