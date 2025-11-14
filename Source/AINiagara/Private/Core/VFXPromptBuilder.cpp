// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/VFXPromptBuilder.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"

FString UVFXPromptBuilder::BuildSystemPrompt()
{
	FString SystemPrompt;
	
	SystemPrompt += BuildExpertPersonaInstructions();
	SystemPrompt += TEXT("\n\n");
	SystemPrompt += BuildDSLFormatInstructions();
	SystemPrompt += TEXT("\n\n");
	SystemPrompt += BuildToolUsageInstructions();
	SystemPrompt += TEXT("\n\n");
	SystemPrompt += Build3DModelInstructions();
	
	return SystemPrompt;
}

FString UVFXPromptBuilder::BuildUserPrompt(
	const FString& UserRequest,
	const TArray<FConversationMessage>& ConversationHistory
)
{
	FString UserPrompt = UserRequest;
	
	// Add context from conversation history if available
	if (ConversationHistory.Num() > 0)
	{
		UserPrompt += TEXT("\n\n");
		UserPrompt += TEXT("Previous conversation context:\n");
		
		for (const FConversationMessage& Message : ConversationHistory)
		{
			UserPrompt += FString::Printf(TEXT("[%s]: %s\n"), *Message.Role, *Message.Content);
		}
	}
	
	return UserPrompt;
}

TArray<FVFXToolFunction> UVFXPromptBuilder::GetAvailableTools()
{
	TArray<FVFXToolFunction> Tools;

	// tool:texture
	FVFXToolFunction TextureTool;
	TextureTool.Name = TEXT("tool:texture");
	TextureTool.Description = TEXT("Generate a custom texture for the VFX effect. Use this when you need textures like noise, fire, smoke, sparks, or distortion effects.");
	TextureTool.Parameters.Add(TEXT("type"), TEXT("string")); // noise, fire, smoke, sparks, distortion
	TextureTool.Parameters.Add(TEXT("resolution"), TEXT("number"));
	TextureTool.Parameters.Add(TEXT("colorScheme"), TEXT("string"));
	TextureTool.Parameters.Add(TEXT("frames"), TEXT("number")); // For flipbooks
	Tools.Add(TextureTool);

	// tool:shader
	FVFXToolFunction ShaderTool;
	ShaderTool.Name = TEXT("tool:shader");
	ShaderTool.Description = TEXT("Generate custom shader code for special visual effects. Use this when standard materials are not sufficient.");
	ShaderTool.Parameters.Add(TEXT("specifications"), TEXT("string"));
	ShaderTool.Parameters.Add(TEXT("functionality"), TEXT("string"));
	Tools.Add(ShaderTool);

	// tool:material
	FVFXToolFunction MaterialTool;
	MaterialTool.Name = TEXT("tool:material");
	MaterialTool.Description = TEXT("Create or modify a material for the particle system. Use this to set up material properties, shader references, and texture bindings.");
	MaterialTool.Parameters.Add(TEXT("properties"), TEXT("object"));
	MaterialTool.Parameters.Add(TEXT("shaderReference"), TEXT("string"));
	MaterialTool.Parameters.Add(TEXT("textureBindings"), TEXT("object"));
	Tools.Add(MaterialTool);

	return Tools;
}

FString UVFXPromptBuilder::GetDSLFormatSpecification()
{
	return TEXT(
		"DSL Format Specification:\n"
		"{\n"
		"  \"effect\": {\n"
		"    \"type\": \"Niagara\" | \"Cascade\",\n"
		"    \"duration\": number,\n"
		"    \"looping\": boolean\n"
		"  },\n"
		"  \"emitters\": [\n"
		"    {\n"
		"      \"name\": \"string\",\n"
		"      \"spawners\": {\n"
		"        \"burst\": {\n"
		"          \"count\": number,\n"
		"          \"time\": number,\n"
		"          \"intervals\": number[]\n"
		"        },\n"
		"        \"rate\": {\n"
		"          \"spawnRate\": number,\n"
		"          \"scaleOverTime\": number\n"
		"        }\n"
		"      },\n"
		"      \"initialization\": {\n"
		"        \"color\": { \"r\": number, \"g\": number, \"b\": number, \"a\": number },\n"
		"        \"size\": { \"min\": number, \"max\": number },\n"
		"        \"velocity\": { \"x\": number, \"y\": number, \"z\": number }\n"
		"      },\n"
		"      \"update\": {\n"
		"        \"forces\": {\n"
		"          \"gravity\": number,\n"
		"          \"wind\": { \"x\": number, \"y\": number, \"z\": number }\n"
		"        },\n"
		"        \"drag\": number,\n"
		"        \"collision\": {\n"
		"          \"enabled\": boolean,\n"
		"          \"bounce\": number\n"
		"        }\n"
		"      },\n"
		"      \"render\": {\n"
		"        \"material\": \"string\",\n"
		"        \"texture\": \"string\",\n"
		"        \"blendMode\": \"string\",\n"
		"        \"sort\": \"string\"\n"
		"      }\n"
		"    }\n"
		"  ]\n"
		"}"
	);
}

FString UVFXPromptBuilder::GetToolFunctionDocumentation()
{
	FString Documentation;
	
	Documentation += TEXT("Available Tool Functions:\n\n");
	
	Documentation += TEXT("tool:texture - Generate custom textures\n");
	Documentation += TEXT("  Parameters:\n");
	Documentation += TEXT("    - type: \"noise\" | \"fire\" | \"smoke\" | \"sparks\" | \"distortion\"\n");
	Documentation += TEXT("    - resolution: number (e.g., 512, 1024)\n");
	Documentation += TEXT("    - colorScheme: string description\n");
	Documentation += TEXT("    - frames: number (for flipbook textures)\n\n");
	
	Documentation += TEXT("tool:shader - Generate custom shader code\n");
	Documentation += TEXT("  Parameters:\n");
	Documentation += TEXT("    - specifications: string description\n");
	Documentation += TEXT("    - functionality: string description\n\n");
	
	Documentation += TEXT("tool:material - Create or modify materials\n");
	Documentation += TEXT("  Parameters:\n");
	Documentation += TEXT("    - properties: object with material properties\n");
	Documentation += TEXT("    - shaderReference: string path to shader\n");
	Documentation += TEXT("    - textureBindings: object with texture paths\n");
	
	return Documentation;
}

FString UVFXPromptBuilder::BuildExpertPersonaInstructions()
{
	return TEXT(
		"You are an expert VFX artist specializing in creating particle effects for Unreal Engine 5.\n"
		"You have deep knowledge of both Niagara and Cascade particle systems.\n"
		"Your expertise includes:\n"
		"- Creating realistic and stylized visual effects\n"
		"- Optimizing particle systems for performance\n"
		"- Understanding particle behavior, forces, and rendering\n"
		"- Material and shader creation for particle effects\n"
		"- Texture generation for VFX\n\n"
		"Your task is to interpret user requests and generate accurate DSL specifications that can be converted into functional particle systems."
	);
}

FString UVFXPromptBuilder::BuildDSLFormatInstructions()
{
	FString Instructions;
	
	Instructions += TEXT("You MUST generate your response in the DSL (Domain-Specific Language) format specified below.\n");
	Instructions += TEXT("The DSL is a JSON structure that describes the complete particle system configuration.\n\n");
	
	Instructions += GetDSLFormatSpecification();
	
	Instructions += TEXT("\n\n");
	Instructions += TEXT("Important DSL Rules:\n");
	Instructions += TEXT("- Effect type MUST be either \"Niagara\" or \"Cascade\"\n");
	Instructions += TEXT("- At least one emitter is REQUIRED\n");
	Instructions += TEXT("- Each emitter MUST contain all sections: spawners, initialization, update, render\n");
	Instructions += TEXT("- Color values MUST be between 0.0 and 1.0\n");
	Instructions += TEXT("- Size values MUST be positive numbers\n");
	Instructions += TEXT("- Duration MUST be a positive number\n");
	Instructions += TEXT("- All numeric values MUST be valid numbers (not strings)\n");
	
	return Instructions;
}

FString UVFXPromptBuilder::BuildToolUsageInstructions()
{
	FString Instructions;
	
	Instructions += TEXT("When generating VFX, you may need to use tool functions for custom assets:\n\n");
	
	Instructions += GetToolFunctionDocumentation();
	
	Instructions += TEXT("\n\n");
	Instructions += TEXT("Tool Usage Guidelines:\n");
	Instructions += TEXT("- Use tool:texture when you need custom textures (noise, fire, smoke, sparks, distortion)\n");
	Instructions += TEXT("- Use tool:shader when standard materials cannot achieve the desired effect\n");
	Instructions += TEXT("- Use tool:material to create materials with specific properties\n");
	Instructions += TEXT("- When calling a tool, include all required parameters\n");
	Instructions += TEXT("- After calling a tool, reference the generated asset in your DSL (e.g., in render.texture or render.material)\n");
	
	return Instructions;
}

FString UVFXPromptBuilder::Build3DModelInstructions()
{
	return TEXT(
		"3D Model Handling:\n"
		"- For 80% of gameplay VFX, use sprites/flipbooks or simple meshes (billboards, cones, spheres)\n"
		"- When a custom 3D model is required, you MUST request the user to import the static mesh\n"
		"- Inform the user: \"Please import the static mesh for [description] and provide the asset path\"\n"
		"- Once the user provides the mesh path, use it in your DSL with appropriate scale and rotation\n"
		"- DO NOT attempt to generate 3D models automatically\n"
		"- Simple meshes (billboards, cones, spheres) can be selected automatically - specify which one to use\n"
	);
}

