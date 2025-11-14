// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AINiagara : ModuleRules
{
	public AINiagara(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"EditorFramework",
				"EditorStyle",
				"UnrealEd",
				"LevelEditor",
				"InteractiveToolsFramework",
				"EditorInteractiveToolsFramework",
				"HTTP",
				"Json",
				"JsonUtilities",
				"Niagara",
				"NiagaraEditor",
				"Engine",
				"ToolMenus",
				"WorkspaceMenuStructure",
				"PropertyEditor",
				"AssetRegistry",
				"AssetTools",
				"EditorStyle",
				"ContentBrowser",
				"AutomationTest",
				"UnrealEd"
				// ... add private dependencies that you statically link with here ...	
			}
		);

		// Enable tests in editor builds
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.Add("AutomationTest");
		}
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
