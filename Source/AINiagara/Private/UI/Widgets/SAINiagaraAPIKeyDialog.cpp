// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Widgets/SAINiagaraAPIKeyDialog.h"
#include "Core/AINiagaraSettings.h"
#include "Core/GeminiAPIClient.h"
#include "Widgets/Layout/SVerticalBox.h"
#include "Widgets/Layout/SHorizontalBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SSeparator.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"

void SAINiagaraAPIKeyDialog::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Title
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("APIKeyDialogTitle", "Configure Gemini API Key"))
			.TextStyle(FAppStyle::Get(), "LargeText")
		]
		
		// Description
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f, 0.0f, 10.0f, 10.0f)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("APIKeyDialogDescription", "Enter your Google Gemini API key to enable AI-powered VFX generation. Your API key will be stored securely in your project settings."))
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
				.Text(LOCTEXT("APIKeyLabel", "API Key:"))
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(APIKeyInputBox, SEditableTextBox)
				.HintText(LOCTEXT("APIKeyHint", "Enter your Gemini API key"))
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
				.Text(LOCTEXT("TestButton", "Test"))
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
				.Text(LOCTEXT("SaveButton", "Save"))
				.OnClicked(this, &SAINiagaraAPIKeyDialog::OnSaveClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(CancelButton, SButton)
				.Text(LOCTEXT("CancelButton", "Cancel"))
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
}

FReply SAINiagaraAPIKeyDialog::OnTestClicked()
{
	if (!APIKeyInputBox.IsValid())
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

	// Test API key
	FGeminiAPIClient APIClient;
	APIClient.TestAPIKey(
		APIKey,
		FOnGeminiResponse::CreateLambda([this, APIKey](const FString& ResponseText)
		{
			SetButtonsEnabled(true);
			CurrentAPIKey = APIKey;
			UpdateStatus(TEXT("API key is valid! Click Save to store it."), false);
		}),
		FOnGeminiError::CreateLambda([this](int32 ErrorCode, const FString& ErrorMessage)
		{
			SetButtonsEnabled(true);
			FString ErrorMsg = FString::Printf(TEXT("API key test failed (%d): %s"), ErrorCode, *ErrorMessage);
			UpdateStatus(ErrorMsg, true);
		})
	);

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
	if (TestButton.IsValid())
	{
		TestButton->SetEnabled(bEnabled);
	}
	if (SaveButton.IsValid())
	{
		SaveButton->SetEnabled(bEnabled);
	}
	if (CancelButton.IsValid())
	{
		CancelButton->SetEnabled(bEnabled);
	}
	if (APIKeyInputBox.IsValid())
	{
		APIKeyInputBox->SetEnabled(bEnabled);
	}
}

