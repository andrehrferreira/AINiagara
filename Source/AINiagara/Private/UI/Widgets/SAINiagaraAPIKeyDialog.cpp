// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Widgets/SAINiagaraAPIKeyDialog.h"
#include "Core/AINiagaraSettings.h"
#include "Core/GeminiAPIClient.h"
#include "Core/VFXDSL.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SBorder.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"
#include "Misc/DateTime.h"
#include "HAL/PlatformProcess.h"
#include "Containers/Ticker.h"
#include "Async/Async.h"
#include "HAL/ThreadHeartBeat.h"

void SAINiagaraAPIKeyDialog::Construct(const FArguments& InArgs)
{
	OnAPIKeyConfigured = InArgs._OnAPIKeyConfigured;
	
	// Initialize validity flag
	bIsValid = true;
	
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("AINiagara", "APIKeyDialogTitle", "Configure Gemini API Key"))
			.TextStyle(FAppStyle::Get(), "LargeText")
		]
		
		// Description
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 0.0f, 10.0f, 10.0f)
		[
			SNew(STextBlock)
			.Text(NSLOCTEXT("AINiagara", "APIKeyDialogDescription", "Enter your Google Gemini API key to enable AI-powered VFX generation. Your API key will be stored securely in your project settings."))
			.AutoWrapText(true)
			.WrapTextAt(500.0f)
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SSeparator)
		]
		
		// API Key input
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 5.0f)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("AINiagara", "APIKeyLabel", "API Key:"))
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(APIKeyInputBox, SEditableTextBox)
				.HintText(NSLOCTEXT("AINiagara", "APIKeyHint", "Enter your Gemini API key"))
				.IsPassword(true) // Mask the API key input
				.MinDesiredWidth(400.0f)
			]
		]
		
		// Status message
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 5.0f)
		[
			SAssignNew(StatusTextBlock, STextBlock)
			.Visibility(EVisibility::Collapsed)
			.AutoWrapText(true)
			.WrapTextAt(500.0f)
		]
		
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SSeparator)
		]
		
		// Buttons
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(0.0f, 0.0f, 5.0f, 0.0f)
			[
				SAssignNew(TestButton, SButton)
				.Text(NSLOCTEXT("AINiagara", "TestButton", "Test"))
				.OnClicked(this, &SAINiagaraAPIKeyDialog::OnTestClicked)
			]
			
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNullWidget::NullWidget
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 5.0f, 0.0f)
			[
				SAssignNew(SaveButton, SButton)
				.Text(NSLOCTEXT("AINiagara", "SaveButton", "Save"))
				.OnClicked(this, &SAINiagaraAPIKeyDialog::OnSaveClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(CancelButton, SButton)
				.Text(NSLOCTEXT("AINiagara", "CancelButton", "Cancel"))
				.OnClicked(this, &SAINiagaraAPIKeyDialog::OnCancelClicked)
			]
		]
	];

	// Load existing API key if configured (masked)
	if (UAINiagaraSettings* Settings = UAINiagaraSettings::Get())
	{
		if (Settings->IsAPIKeyConfigured())
		{
			FString MaskedKey = Settings->GetMaskedAPIKey();
			APIKeyInputBox->SetText(FText::FromString(MaskedKey));
		}
	}

	// Focus on input box
	FSlateApplication::Get().SetKeyboardFocus(APIKeyInputBox.ToSharedRef());
	
	// Create weak pointer to self for safe callbacks (after widget is fully constructed)
	WeakSelf = SharedThis(this);
}

FReply SAINiagaraAPIKeyDialog::OnTestClicked()
{
	if (!APIKeyInputBox.IsValid() || !bIsValid)
	{
		return FReply::Handled();
	}

	FString APIKey = APIKeyInputBox->GetText().ToString().TrimStartAndEnd();
	
	if (APIKey.IsEmpty())
	{
		UpdateStatus(TEXT("Please enter an API key"), true);
		return FReply::Handled();
	}

	// Disable buttons during test
	SetButtonsEnabled(false);
	UpdateStatus(TEXT("Testing API key..."));

	// Capture weak pointer for safe callbacks
	TWeakPtr<SAINiagaraAPIKeyDialog> WeakDialog = SharedThis(this);

	// Test API key
	FGeminiAPIClient APIClient;
	
	// Always use AsyncTask to ensure we're on the game thread
	// HTTP callbacks are executed on HTTP thread, not game thread
	APIClient.TestAPIKey(
		APIKey,
		FOnGeminiResponse::CreateLambda([WeakDialog, APIKey](const FString& ResponseText)
		{
			UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey response callback executed (outside AsyncTask)"));
			
			// Always execute on game thread - HandleRequestCompleteOnGameThread already handles thread safety
			// But we still need to ensure UI updates are on game thread
			if (IsInGameThread())
			{
				UE_LOG(LogTemp, Log, TEXT("AINiagara: Already on game thread, updating UI directly"));
				TSharedPtr<SAINiagaraAPIKeyDialog> Dialog = WeakDialog.Pin();
				if (Dialog.IsValid() && Dialog->IsValid() && FSlateApplication::IsInitialized())
				{
					UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey response - widget is valid, updating UI"));
					Dialog->SetButtonsEnabled(true);
					Dialog->CurrentAPIKey = APIKey;
					Dialog->UpdateStatus(TEXT("API key is valid! Click Save to store it."), false);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AINiagara: TestAPIKey response - widget is invalid or Slate not initialized"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("AINiagara: Not on game thread, scheduling AsyncTask"));
				AsyncTask(ENamedThreads::GameThread, [WeakDialog, APIKey, ResponseText]()
				{
					UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey response callback executed (inside AsyncTask)"));
					
					// Check if widget still exists using weak pointer
					TSharedPtr<SAINiagaraAPIKeyDialog> Dialog = WeakDialog.Pin();
					if (Dialog.IsValid() && Dialog->IsValid() && FSlateApplication::IsInitialized())
					{
						UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey response - widget is valid, updating UI"));
						Dialog->SetButtonsEnabled(true);
						Dialog->CurrentAPIKey = APIKey;
						Dialog->UpdateStatus(TEXT("API key is valid! Click Save to store it."), false);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("AINiagara: TestAPIKey response - widget is invalid or Slate not initialized"));
					}
				});
			}
		}),
		FOnGeminiError::CreateLambda([WeakDialog](int32 ErrorCode, const FString& ErrorMessage)
		{
			UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey error callback executed (outside AsyncTask): %d - %s"), ErrorCode, *ErrorMessage);
			
			// Always execute on game thread - HandleRequestCompleteOnGameThread already handles thread safety
			// But we still need to ensure UI updates are on game thread
			if (IsInGameThread())
			{
				UE_LOG(LogTemp, Log, TEXT("AINiagara: Already on game thread, updating UI directly"));
				TSharedPtr<SAINiagaraAPIKeyDialog> Dialog = WeakDialog.Pin();
				if (Dialog.IsValid() && Dialog->IsValid() && FSlateApplication::IsInitialized())
				{
					UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey error - widget is valid, updating UI"));
					Dialog->SetButtonsEnabled(true);
					FString ErrorMsg = FString::Printf(TEXT("API key test failed (%d): %s"), ErrorCode, *ErrorMessage);
					Dialog->UpdateStatus(ErrorMsg, true);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("AINiagara: TestAPIKey error - widget is invalid or Slate not initialized"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("AINiagara: Not on game thread, scheduling AsyncTask"));
				AsyncTask(ENamedThreads::GameThread, [WeakDialog, ErrorCode, ErrorMessage]()
				{
					UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey error callback executed (inside AsyncTask): %d - %s"), ErrorCode, *ErrorMessage);
					
					// Check if widget still exists using weak pointer
					TSharedPtr<SAINiagaraAPIKeyDialog> Dialog = WeakDialog.Pin();
					if (Dialog.IsValid() && Dialog->IsValid() && FSlateApplication::IsInitialized())
					{
						UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey error - widget is valid, updating UI"));
						Dialog->SetButtonsEnabled(true);
						FString ErrorMsg = FString::Printf(TEXT("API key test failed (%d): %s"), ErrorCode, *ErrorMessage);
						Dialog->UpdateStatus(ErrorMsg, true);
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("AINiagara: TestAPIKey error - widget is invalid or Slate not initialized"));
					}
				});
			}
		})
	);
	
	UE_LOG(LogTemp, Log, TEXT("AINiagara: TestAPIKey request sent"));

	return FReply::Handled();
}

FReply SAINiagaraAPIKeyDialog::OnSaveClicked()
{
	if (!APIKeyInputBox.IsValid())
	{
		return FReply::Handled();
	}

	FString APIKey = APIKeyInputBox->GetText().ToString().TrimStartAndEnd();
	
	// If we tested the key, use the tested one
	if (!CurrentAPIKey.IsEmpty())
	{
		APIKey = CurrentAPIKey;
	}
	
	if (APIKey.IsEmpty())
	{
		UpdateStatus(TEXT("Please enter an API key"), true);
		return FReply::Handled();
	}

	// Save API key
	if (UAINiagaraSettings* Settings = UAINiagaraSettings::Get())
	{
		Settings->SetGeminiAPIKey(APIKey, true);
		UpdateStatus(TEXT("API key saved successfully!"), false);
		
		// Notify that configuration is complete
		OnAPIKeyConfigured.ExecuteIfBound();
	}

	return FReply::Handled();
}

FReply SAINiagaraAPIKeyDialog::OnCancelClicked()
{
	// Invalidate widget before closing
	Invalidate();
	OnDialogCancelled.ExecuteIfBound();
	return FReply::Handled();
}

void SAINiagaraAPIKeyDialog::UpdateStatus(const FString& Message, bool bIsError)
{
	if (StatusTextBlock.IsValid())
	{
		StatusTextBlock->SetText(FText::FromString(Message));
		StatusTextBlock->SetColorAndOpacity(bIsError ? FLinearColor::Red : FLinearColor::Green);
		StatusTextBlock->SetVisibility(EVisibility::Visible);
	}
}

void SAINiagaraAPIKeyDialog::SetButtonsEnabled(bool bEnabled)
{
	// Check if widget is still valid before accessing members
	if (!bIsValid)
	{
		return;
	}

	// Ensure Slate is initialized before accessing widgets
	if (!FSlateApplication::IsInitialized())
	{
		return;
	}

	// Ensure we're on the game thread before accessing Slate widgets
	if (!IsInGameThread())
	{
		UE_LOG(LogTemp, Error, TEXT("AINiagara: SetButtonsEnabled called from non-game thread!"));
		return;
	}

	// Safely enable/disable buttons with additional safety checks
	// Wrap in try-catch to handle any potential crashes from invalid Slate attributes
	try
	{
		// Check each widget individually before modifying
		// Use IsValid() to ensure the widget pointer is valid
		if (TestButton.IsValid() && TestButton.Get() != nullptr)
		{
			TestButton->SetEnabled(bEnabled);
		}
		if (SaveButton.IsValid() && SaveButton.Get() != nullptr)
		{
			SaveButton->SetEnabled(bEnabled);
		}
		if (CancelButton.IsValid() && CancelButton.Get() != nullptr)
		{
			CancelButton->SetEnabled(bEnabled);
		}
		if (APIKeyInputBox.IsValid() && APIKeyInputBox.Get() != nullptr)
		{
			APIKeyInputBox->SetEnabled(bEnabled);
		}
	}
	catch (...)
	{
		// Widget was destroyed - mark as invalid and silently ignore
		UE_LOG(LogTemp, Warning, TEXT("AINiagara: SetButtonsEnabled failed - widget may have been destroyed"));
		bIsValid = false; // Mark as invalid to prevent further attempts
	}
}

