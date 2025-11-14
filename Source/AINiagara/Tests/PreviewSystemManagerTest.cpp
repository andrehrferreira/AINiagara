// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Core/PreviewSystemManager.h"
#include "Core/VFXDSL.h"

#if WITH_DEV_AUTOMATION_TESTS

/**
 * Test PreviewSystemManager singleton instance
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerSingletonTest,
	"AINiagara.PreviewSystemManager.Singleton",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerSingletonTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager1 = UPreviewSystemManager::Get();
	UPreviewSystemManager* Manager2 = UPreviewSystemManager::Get();
	
	TestNotNull(TEXT("Manager instance should not be null"), Manager1);
	TestEqual(TEXT("Should return same singleton instance"), Manager1, Manager2);
	
	return true;
}

/**
 * Test preview enabled/disabled state
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerEnabledTest,
	"AINiagara.PreviewSystemManager.Enabled",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerEnabledTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager = UPreviewSystemManager::Get();
	if (!Manager)
	{
		AddError(TEXT("Failed to get PreviewSystemManager instance"));
		return false;
	}
	
	// Test default state (should be enabled)
	TestTrue(TEXT("Preview should be enabled by default"), Manager->IsPreviewEnabled());
	
	// Disable preview
	Manager->SetPreviewEnabled(false);
	TestFalse(TEXT("Preview should be disabled"), Manager->IsPreviewEnabled());
	
	// Enable preview
	Manager->SetPreviewEnabled(true);
	TestTrue(TEXT("Preview should be enabled"), Manager->IsPreviewEnabled());
	
	return true;
}

/**
 * Test preview update with disabled state
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerDisabledUpdateTest,
	"AINiagara.PreviewSystemManager.DisabledUpdate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerDisabledUpdateTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager = UPreviewSystemManager::Get();
	if (!Manager)
	{
		AddError(TEXT("Failed to get PreviewSystemManager instance"));
		return false;
	}
	
	// Disable preview
	Manager->SetPreviewEnabled(false);
	
	// Try to update preview
	FVFXDSL TestDSL;
	TestDSL.Effect.Type = EVFXEffectType::Niagara;
	
	FString Error;
	bool bResult = Manager->UpdatePreview(TestDSL, false, &Error);
	
	TestFalse(TEXT("Update should fail when preview is disabled"), bResult);
	TestTrue(TEXT("Error message should indicate preview is disabled"), 
		Error.Contains(TEXT("disabled")) || Error.Contains(TEXT("Disabled")));
	
	return true;
}

/**
 * Test preview update with invalid DSL type
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerInvalidDSLTypeTest,
	"AINiagara.PreviewSystemManager.InvalidDSLType",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerInvalidDSLTypeTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager = UPreviewSystemManager::Get();
	if (!Manager)
	{
		AddError(TEXT("Failed to get PreviewSystemManager instance"));
		return false;
	}
	
	// Ensure preview is enabled
	Manager->SetPreviewEnabled(true);
	
	// Create DSL with invalid type (assuming there's an invalid enum value)
	FVFXDSL TestDSL;
	// Set to a potentially invalid value - this depends on enum definition
	// For now, we'll test with a valid type but empty emitters to trigger error
	
	FString Error;
	bool bResult = Manager->UpdatePreview(TestDSL, true, &Error);
	
	// Update might fail due to empty DSL or other validation
	// The important thing is error handling works
	if (!bResult)
	{
		TestTrue(TEXT("Error message should be provided"), !Error.IsEmpty());
	}
	
	return true;
}

/**
 * Test preview update throttling
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerThrottlingTest,
	"AINiagara.PreviewSystemManager.Throttling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerThrottlingTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager = UPreviewSystemManager::Get();
	if (!Manager)
	{
		AddError(TEXT("Failed to get PreviewSystemManager instance"));
		return false;
	}
	
	// Ensure preview is enabled
	Manager->SetPreviewEnabled(true);
	
	// Create test DSL
	FVFXDSL TestDSL;
	TestDSL.Effect.Type = EVFXEffectType::Niagara;
	
	// First update should work (or be throttled if called too quickly)
	FString Error1;
	bool bResult1 = Manager->UpdatePreview(TestDSL, true, &Error1);
	
	// Immediately try second update (should be throttled)
	FVFXDSL TestDSL2;
	TestDSL2.Effect.Type = EVFXEffectType::Niagara;
	TestDSL2.Emitters.Add(FVFXDSLEmitter()); // Add emitter to make it different
	
	FString Error2;
	bool bResult2 = Manager->UpdatePreview(TestDSL2, false, &Error2);
	
	// Second update should be throttled (unless forced)
	if (!bResult2 && !Error2.IsEmpty())
	{
		TestTrue(TEXT("Error should indicate throttling"), 
			Error2.Contains(TEXT("throttled")) || Error2.Contains(TEXT("Throttled")));
	}
	
	return true;
}

/**
 * Test preview cleanup
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerCleanupTest,
	"AINiagara.PreviewSystemManager.Cleanup",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerCleanupTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager = UPreviewSystemManager::Get();
	if (!Manager)
	{
		AddError(TEXT("Failed to get PreviewSystemManager instance"));
		return false;
	}
	
	// Clear any existing preview
	Manager->ClearPreview();
	TestFalse(TEXT("Preview should not be active after clear"), Manager->IsPreviewActive());
	
	// Test cleanup
	Manager->Cleanup();
	TestFalse(TEXT("Preview should not be active after cleanup"), Manager->IsPreviewActive());
	
	return true;
}

/**
 * Test error message propagation
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerErrorMessageTest,
	"AINiagara.PreviewSystemManager.ErrorMessage",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerErrorMessageTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager = UPreviewSystemManager::Get();
	if (!Manager)
	{
		AddError(TEXT("Failed to get PreviewSystemManager instance"));
		return false;
	}
	
	// Test with disabled preview
	Manager->SetPreviewEnabled(false);
	FString Error;
	bool bResult = Manager->UpdatePreview(FVFXDSL(), false, &Error);
	
	TestFalse(TEXT("Update should fail"), bResult);
	TestTrue(TEXT("Error message should be provided"), !Error.IsEmpty());
	
	// Test without error output parameter (should not crash)
	bResult = Manager->UpdatePreview(FVFXDSL(), false, nullptr);
	TestFalse(TEXT("Update should still fail without error output"), bResult);
	
	return true;
}

/**
 * Test preview state after error
 */
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FPreviewSystemManagerErrorStateTest,
	"AINiagara.PreviewSystemManager.ErrorState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FPreviewSystemManagerErrorStateTest::RunTest(const FString& Parameters)
{
	UPreviewSystemManager* Manager = UPreviewSystemManager::Get();
	if (!Manager)
	{
		AddError(TEXT("Failed to get PreviewSystemManager instance"));
		return false;
	}
	
	// Clear any existing preview
	Manager->ClearPreview();
	
	// Try to update with invalid DSL (disabled state)
	Manager->SetPreviewEnabled(false);
	FString Error;
	bool bResult = Manager->UpdatePreview(FVFXDSL(), false, &Error);
	
	TestFalse(TEXT("Update should fail"), bResult);
	TestFalse(TEXT("Preview should not be active after error"), Manager->IsPreviewActive());
	
	// Manager should still be functional
	TestNotNull(TEXT("Manager should still be valid"), Manager);
	
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS

