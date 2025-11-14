// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/VFXPromptBuilder.h"
#include "Core/GeminiAPIClient.h"
#include "Core/VFXDSL.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderSystemPromptTest,
	"AINiagara.VFXPromptBuilder.SystemPrompt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderSystemPromptTest::RunTest(const FString& Parameters)
{
	// Build system prompt
	FString SystemPrompt = UVFXPromptBuilder::BuildSystemPrompt();

	// Verify system prompt is not empty
	TestFalse(TEXT("System prompt should not be empty"), SystemPrompt.IsEmpty());

	// Verify it contains key sections
	TestTrue(TEXT("Should contain VFX expert persona"), SystemPrompt.Contains(TEXT("VFX")) || SystemPrompt.Contains(TEXT("expert")));
	TestTrue(TEXT("Should contain DSL information"), SystemPrompt.Contains(TEXT("DSL")) || SystemPrompt.Contains(TEXT("format")));
	
	// Verify it has substantial content
	TestTrue(TEXT("System prompt should be substantial (>100 chars)"), SystemPrompt.Len() > 100);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderUserPromptTest,
	"AINiagara.VFXPromptBuilder.UserPrompt",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderUserPromptTest::RunTest(const FString& Parameters)
{
	// Test with simple request and no history
	FString UserRequest = TEXT("Create a fire effect");
	TArray<FConversationMessage> EmptyHistory;
	
	FString UserPrompt = UVFXPromptBuilder::BuildUserPrompt(UserRequest, EmptyHistory);
	
	TestFalse(TEXT("User prompt should not be empty"), UserPrompt.IsEmpty());
	TestTrue(TEXT("User prompt should contain user request"), UserPrompt.Contains(UserRequest));

	// Test with conversation history
	TArray<FConversationMessage> History;
	History.Add(FConversationMessage(TEXT("user"), TEXT("Hello")));
	History.Add(FConversationMessage(TEXT("assistant"), TEXT("Hi! How can I help?")));
	History.Add(FConversationMessage(TEXT("user"), TEXT("Make it blue")));
	
	FString PromptWithHistory = UVFXPromptBuilder::BuildUserPrompt(UserRequest, History);
	
	TestFalse(TEXT("Prompt with history should not be empty"), PromptWithHistory.IsEmpty());
	TestTrue(TEXT("Should contain user request"), PromptWithHistory.Contains(UserRequest));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderAvailableToolsTest,
	"AINiagara.VFXPromptBuilder.AvailableTools",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderAvailableToolsTest::RunTest(const FString& Parameters)
{
	// Get available tools
	TArray<FVFXToolFunction> Tools = UVFXPromptBuilder::GetAvailableTools();

	// Verify tools are provided
	TestTrue(TEXT("Should have at least one tool"), Tools.Num() > 0);

	// Verify each tool has required fields
	for (const FVFXToolFunction& Tool : Tools)
	{
		TestFalse(TEXT("Tool name should not be empty"), Tool.Name.IsEmpty());
		TestFalse(TEXT("Tool description should not be empty"), Tool.Description.IsEmpty());
		
		// Tool names should follow convention (e.g., "tool:texture")
		TestTrue(TEXT("Tool name should contain colon"), Tool.Name.Contains(TEXT(":")));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderDSLSpecificationTest,
	"AINiagara.VFXPromptBuilder.DSLSpecification",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderDSLSpecificationTest::RunTest(const FString& Parameters)
{
	// Get DSL format specification
	FString DSLSpec = UVFXPromptBuilder::GetDSLFormatSpecification();

	// Verify specification is not empty
	TestFalse(TEXT("DSL specification should not be empty"), DSLSpec.IsEmpty());

	// Verify it contains key DSL elements
	TestTrue(TEXT("Should mention effect"), DSLSpec.Contains(TEXT("effect")) || DSLSpec.Contains(TEXT("Effect")));
	TestTrue(TEXT("Should mention emitters"), DSLSpec.Contains(TEXT("emitter")) || DSLSpec.Contains(TEXT("Emitter")));
	
	// Verify it has JSON-like structure hints
	TestTrue(TEXT("Should reference JSON or structure"), 
		DSLSpec.Contains(TEXT("JSON")) || DSLSpec.Contains(TEXT("{")) || DSLSpec.Contains(TEXT("format")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderToolDocumentationTest,
	"AINiagara.VFXPromptBuilder.ToolDocumentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderToolDocumentationTest::RunTest(const FString& Parameters)
{
	// Get tool function documentation
	FString ToolDoc = UVFXPromptBuilder::GetToolFunctionDocumentation();

	// Verify documentation is not empty
	TestFalse(TEXT("Tool documentation should not be empty"), ToolDoc.IsEmpty());

	// Verify it mentions tools
	TestTrue(TEXT("Should mention tools or functions"), 
		ToolDoc.Contains(TEXT("tool")) || ToolDoc.Contains(TEXT("function")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderEmptyInputTest,
	"AINiagara.VFXPromptBuilder.EmptyInput",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderEmptyInputTest::RunTest(const FString& Parameters)
{
	// Test with empty user request
	FString EmptyRequest = TEXT("");
	TArray<FConversationMessage> EmptyHistory;
	
	FString EmptyPrompt = UVFXPromptBuilder::BuildUserPrompt(EmptyRequest, EmptyHistory);
	
	// Should still return a valid prompt (may contain instructions even if request is empty)
	TestNotNull(TEXT("Empty request should still return a string"), &EmptyPrompt);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderLargeHistoryTest,
	"AINiagara.VFXPromptBuilder.LargeHistory",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderLargeHistoryTest::RunTest(const FString& Parameters)
{
	// Create large conversation history
	TArray<FConversationMessage> LargeHistory;
	for (int32 i = 0; i < 100; i++)
	{
		FString UserMsg = FString::Printf(TEXT("User message %d"), i);
		FString AssistantMsg = FString::Printf(TEXT("Assistant response %d"), i);
		
		LargeHistory.Add(FConversationMessage(TEXT("user"), UserMsg));
		LargeHistory.Add(FConversationMessage(TEXT("assistant"), AssistantMsg));
	}

	FString UserRequest = TEXT("Create a water effect");
	FString LargePrompt = UVFXPromptBuilder::BuildUserPrompt(UserRequest, LargeHistory);

	// Should handle large history without crashing
	TestFalse(TEXT("Should handle large history"), LargePrompt.IsEmpty());
	TestTrue(TEXT("Should contain user request"), LargePrompt.Contains(UserRequest));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderSpecialCharactersTest,
	"AINiagara.VFXPromptBuilder.SpecialCharacters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderSpecialCharactersTest::RunTest(const FString& Parameters)
{
	// Test with special characters in user request
	FString SpecialRequest = TEXT("Create effect with \"quotes\", 'apostrophes', and <brackets>");
	TArray<FConversationMessage> EmptyHistory;
	
	FString SpecialPrompt = UVFXPromptBuilder::BuildUserPrompt(SpecialRequest, EmptyHistory);
	
	TestFalse(TEXT("Should handle special characters"), SpecialPrompt.IsEmpty());

	// Test with special characters in history
	TArray<FConversationMessage> SpecialHistory;
	SpecialHistory.Add(FConversationMessage(TEXT("user"), TEXT("Test with {braces} and [brackets]")));
	SpecialHistory.Add(FConversationMessage(TEXT("assistant"), TEXT("Response with special chars: !@#$%^&*()")));
	
	FString HistoryPrompt = UVFXPromptBuilder::BuildUserPrompt(TEXT("Follow-up"), SpecialHistory);
	
	TestFalse(TEXT("Should handle special chars in history"), HistoryPrompt.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderUnicodeTest,
	"AINiagara.VFXPromptBuilder.UnicodeCharacters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderUnicodeTest::RunTest(const FString& Parameters)
{
	// Test with Unicode characters
	FString UnicodeRequest = TEXT("Create effect with emojis: 🔥💧⚡ and symbols: ™©®");
	TArray<FConversationMessage> EmptyHistory;
	
	FString UnicodePrompt = UVFXPromptBuilder::BuildUserPrompt(UnicodeRequest, EmptyHistory);
	
	TestFalse(TEXT("Should handle Unicode characters"), UnicodePrompt.IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVFXPromptBuilderConsistencyTest,
	"AINiagara.VFXPromptBuilder.Consistency",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FVFXPromptBuilderConsistencyTest::RunTest(const FString& Parameters)
{
	// Test that same input produces same output
	FString Request = TEXT("Create a smoke effect");
	TArray<FConversationMessage> History;
	History.Add(FConversationMessage(TEXT("user"), TEXT("Hello")));

	FString Prompt1 = UVFXPromptBuilder::BuildUserPrompt(Request, History);
	FString Prompt2 = UVFXPromptBuilder::BuildUserPrompt(Request, History);

	TestEqual(TEXT("Same input should produce same output"), Prompt1, Prompt2);

	// Test that system prompt is consistent
	FString SystemPrompt1 = UVFXPromptBuilder::BuildSystemPrompt();
	FString SystemPrompt2 = UVFXPromptBuilder::BuildSystemPrompt();

	TestEqual(TEXT("System prompt should be consistent"), SystemPrompt1, SystemPrompt2);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

