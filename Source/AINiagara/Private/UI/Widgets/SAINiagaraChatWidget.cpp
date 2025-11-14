// Copyright Epic Games, Inc. All Rights Reserved.

#include "UI/Widgets/SAINiagaraChatWidget.h"
#include "Core/GeminiAPIClient.h"
#include "Core/ConversationHistoryManager.h"
#include "Core/VFXDSLParser.h"
#include "Core/VFXPromptBuilder.h"
#include "Core/NiagaraSystemGenerator.h"
#include "Core/NiagaraSystemToDSLConverter.h"
#include "Core/CascadeSystemGenerator.h"
#include "Core/CascadeSystemToDSLConverter.h"
#include "Core/VFXDSLValidator.h"
#include "Core/PreviewSystemManager.h"
#include "Core/VFXDSLDiff.h"
#include "Tools/TextureGenerationHandler.h"
#include "Tools/TextureMaterialHelper.h"
#include "Particles/ParticleSystem.h"
#include "Core/VFXDSL.h"
#include "NiagaraSystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Widgets/Layout/SBorder.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/AppStyle.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"
#include "Misc/DateTime.h"

void SAINiagaraChatWidget::Construct(const FArguments& InArgs)
{
	// Set asset path from arguments or try to get from editor
	CurrentAssetPath = InArgs._AssetPath;
	if (CurrentAssetPath.IsEmpty())
	{
		CurrentAssetPath = GetCurrentAssetPath();
	}

	// Initialize preview manager
	PreviewManager = UPreviewSystemManager::Get();

	ChildSlot
	[
		SNew(SVerticalBox)
		
		// Message history area
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(5.0f)
		[
			SAssignNew(MessageHistoryBox, SScrollBox)
			.Orientation(Orient_Vertical)
		]
		
		// Loading indicator area
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(SVerticalBox)
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(LoadingBar, SProgressBar)
				.Visibility(EVisibility::Collapsed)
				.Percent(1.0f)
			]
			
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(2.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(LoadingText, STextBlock)
				.Visibility(EVisibility::Collapsed)
				.Text(NSLOCTEXT("AINiagara", "LoadingText", "Processing..."))
				.ColorAndOpacity(FLinearColor(0.7f, 0.7f, 0.7f))
			]
		]
		
		// Input area
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(5.0f)
		[
			SNew(SHorizontalBox)
			
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SAssignNew(InputTextBox, SEditableTextBox)
				.HintText(NSLOCTEXT("AINiagara", "ChatInputHint", "Describe the VFX effect you want to create..."))
				.OnTextCommitted(this, &SAINiagaraChatWidget::OnInputTextCommitted)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ExportDSLButton, SButton)
				.Text(NSLOCTEXT("AINiagara", "ExportDSLButton", "Export DSL"))
				.ToolTipText(NSLOCTEXT("AINiagara", "ExportDSLTooltip", "Export current system to DSL format"))
				.OnClicked(this, &SAINiagaraChatWidget::OnExportDSLClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(ImportDSLButton, SButton)
				.Text(NSLOCTEXT("AINiagara", "ImportDSLButton", "Import DSL"))
				.ToolTipText(NSLOCTEXT("AINiagara", "ImportDSLTooltip", "Import DSL from JSON file"))
				.OnClicked(this, &SAINiagaraChatWidget::OnImportDSLClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(RegenerateDSLButton, SButton)
				.Text(NSLOCTEXT("AINiagara", "RegenerateDSLButton", "Regenerate"))
				.ToolTipText(NSLOCTEXT("AINiagara", "RegenerateDSLTooltip", "Regenerate system from loaded DSL"))
				.OnClicked(this, &SAINiagaraChatWidget::OnRegenerateDSLClicked)
				.IsEnabled(this, &SAINiagaraChatWidget::IsRegenerateEnabled)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(PreviewToggleButton, SButton)
				.Text(this, &SAINiagaraChatWidget::GetPreviewToggleText)
				.ToolTipText(NSLOCTEXT("AINiagara", "PreviewToggleTooltip", "Toggle real-time preview"))
				.OnClicked(this, &SAINiagaraChatWidget::OnPreviewToggleClicked)
			]
			
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5.0f, 0.0f, 0.0f, 0.0f)
			[
				SAssignNew(SendButton, SButton)
				.Text(NSLOCTEXT("AINiagara", "SendButton", "Send"))
				.OnClicked(this, &SAINiagaraChatWidget::OnSendClicked)
			]
		]
	];

	// Load conversation history for current asset
	LoadConversationHistory();
}

FReply SAINiagaraChatWidget::OnSendClicked()
{
	if (!InputTextBox.IsValid())
	{
		return FReply::Handled();
	}

	FText InputText = InputTextBox->GetText();
	if (InputText.IsEmpty())
	{
		return FReply::Handled();
	}

	FString UserMessage = InputText.ToString();
	
	// Add user message to history
	AddMessageToHistory(TEXT("user"), UserMessage);
	
	// Save to conversation history
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		HistoryManager->AddMessage(CurrentAssetPath, TEXT("user"), UserMessage);
	}

	// Clear input
	ClearInput();
	
	// Show loading
	ShowLoading(true, TEXT("Preparing request..."));
	
	// Get conversation history
	TArray<FConversationMessage> ConversationHistory;
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		ConversationHistory = HistoryManager->GetHistory(CurrentAssetPath);
	}

	// Create API client
	FGeminiAPIClient APIClient;
	
	// Build available tools
	TArray<FVFXToolFunction> AvailableTools = UVFXPromptBuilder::GetAvailableTools();
	
	// Add system prompt as first message if conversation is empty
	TArray<FConversationMessage> MessagesWithSystemPrompt = ConversationHistory;
	if (MessagesWithSystemPrompt.Num() == 0)
	{
		FConversationMessage SystemMessage;
		SystemMessage.Role = TEXT("system");
		SystemMessage.Content = UVFXPromptBuilder::BuildSystemPrompt();
		MessagesWithSystemPrompt.Insert(SystemMessage, 0);
	}
	
	// Show loading with message
	ShowLoading(true, TEXT("Sending request to AI..."));

	// Send request to Gemini API
	APIClient.SendChatCompletion(
		UserMessage,
		MessagesWithSystemPrompt,
		AvailableTools,
		FOnGeminiResponse::CreateLambda([this](const FString& ResponseText)
		{
			// Hide loading
			ShowLoading(false);
			
			// Add assistant response to history
			AddMessageToHistory(TEXT("assistant"), ResponseText);
			
			// Save to conversation history
			if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
			{
				HistoryManager->AddMessage(CurrentAssetPath, TEXT("assistant"), ResponseText);
			}
			
			// First, check if response contains tool calls
			if (ProcessToolCalls(ResponseText))
			{
				// Tool call was processed, don't try to parse as DSL
				return;
			}
			
			// Try to parse DSL from response
			FVFXDSL DSL;
			FString ParseError;
			if (UVFXDSLParser::ParseFromJSON(ResponseText, DSL, ParseError))
			{
				ShowLoading(true, TEXT("Validating DSL..."));
				
				// Validate DSL
				FVFXDSLValidationResult ValidationResult = UVFXDSLValidator::Validate(DSL);
				
				ShowLoading(false);
				
				if (ValidationResult.bIsValid)
				{
					// Update preview if enabled (shows in editor viewport)
					UpdatePreview(DSL);
					
					// Generate Niagara/Cascade system from DSL
					if (DSL.Effect.Type == EVFXEffectType::Niagara)
					{
						ShowLoading(true, TEXT("Generating Niagara system..."));
						
						// Determine package path
						FString PackagePath = TEXT("/Game/VFX");
						if (!CurrentAssetPath.IsEmpty())
						{
							// Extract package path from asset path
							int32 LastSlash = CurrentAssetPath.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
							if (LastSlash != INDEX_NONE)
							{
								PackagePath = CurrentAssetPath.Left(LastSlash);
							}
						}
						
						// Generate system name from first emitter name or use default
						FString SystemName = TEXT("AINiagaraSystem");
						if (DSL.Emitters.Num() > 0 && !DSL.Emitters[0].Name.IsEmpty())
						{
							SystemName = DSL.Emitters[0].Name + TEXT("_System");
						}
						
						UNiagaraSystem* GeneratedSystem = nullptr;
						FString GenerationError;
						
						if (UNiagaraSystemGenerator::CreateSystemFromDSL(DSL, PackagePath, SystemName, GeneratedSystem, GenerationError))
						{
							ShowLoading(false);
							FString SuccessMessage = FString::Printf(
								TEXT("Niagara system '%s' generated successfully at %s/%s!"),
								*SystemName,
								*PackagePath,
								*SystemName
							);
							ShowSuccessNotification(SuccessMessage);
						}
						else
						{
							ShowLoading(false);
							FString ErrorMessage = FString::Printf(
								TEXT("Failed to generate Niagara system: %s"),
								*GenerationError
							);
							ShowErrorNotification(ErrorMessage);
						}
					}
					else if (DSL.Effect.Type == EVFXEffectType::Cascade)
					{
						ShowLoading(true, TEXT("Generating Cascade system..."));
						
						// Determine package path
						FString PackagePath = TEXT("/Game/VFX");
						if (!CurrentAssetPath.IsEmpty())
						{
							// Extract package path from asset path
							int32 LastSlash = CurrentAssetPath.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
							if (LastSlash != INDEX_NONE)
							{
								PackagePath = CurrentAssetPath.Left(LastSlash);
							}
						}
						
						// Generate system name from first emitter name or use default
						FString SystemName = TEXT("AICascadeSystem");
						if (DSL.Emitters.Num() > 0 && !DSL.Emitters[0].Name.IsEmpty())
						{
							SystemName = DSL.Emitters[0].Name + TEXT("_System");
						}
						
						UParticleSystem* GeneratedSystem = nullptr;
						FString GenerationError;
						
						if (UCascadeSystemGenerator::CreateSystemFromDSL(DSL, PackagePath, SystemName, GeneratedSystem, GenerationError))
						{
							ShowLoading(false);
							FString SuccessMessage = FString::Printf(
								TEXT("Cascade system '%s' generated successfully at %s/%s!"),
								*SystemName,
								*PackagePath,
								*SystemName
							);
							ShowSuccessNotification(SuccessMessage);
						}
						else
						{
							ShowLoading(false);
							FString ErrorMessage = FString::Printf(
								TEXT("Failed to generate Cascade system: %s"),
								*GenerationError
							);
							ShowErrorNotification(ErrorMessage);
						}
					}
				}
				else
				{
					// Show validation errors
					FString ErrorMessage = TEXT("DSL validation failed:\n");
					for (const FString& Error : ValidationResult.ErrorMessages)
					{
						ErrorMessage += Error + TEXT("\n");
					}
					ShowErrorNotification(ErrorMessage);
				}
			}
			else
			{
				// Response is not DSL, just show it
				// This might be a tool call or other response
			}
		}),
		FOnGeminiError::CreateLambda([this](int32 ErrorCode, const FString& ErrorMessage)
		{
			// Hide loading
			ShowLoading(false);
			
			// Show error message
			FString FullErrorMessage = FString::Printf(TEXT("API Error (%d): %s"), ErrorCode, *ErrorMessage);
			ShowErrorNotification(FullErrorMessage);
		})
	);

	return FReply::Handled();
}

void SAINiagaraChatWidget::OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitType)
{
	if (CommitType == ETextCommit::OnEnter)
	{
		OnSendClicked();
	}
}

void SAINiagaraChatWidget::AddMessageToHistory(const FString& Role, const FString& Content, bool bIsError, bool bIsSuccess)
{
	if (!MessageHistoryBox.IsValid())
	{
		return;
	}

	// Create formatted message widget
	TSharedRef<SWidget> MessageWidget = CreateMessageWidget(Role, Content, bIsError, bIsSuccess);

	// Add to scroll box
	MessageHistoryBox->AddSlot()
		.Padding(5.0f)
		[
			MessageWidget
		];

	// Scroll to bottom
	MessageHistoryBox->ScrollToEnd();
}

TSharedRef<SWidget> SAINiagaraChatWidget::CreateMessageWidget(const FString& Role, const FString& Content, bool bIsError, bool bIsSuccess)
{
	// Determine colors and styling based on role and status
	FLinearColor BackgroundColor;
	FLinearColor TextColor;
	FString RoleLabel;
	float BorderThickness = 1.0f;

	if (bIsError)
	{
		BackgroundColor = FLinearColor(0.3f, 0.1f, 0.1f, 0.3f);
		TextColor = FLinearColor(1.0f, 0.5f, 0.5f);
		RoleLabel = TEXT("ERROR");
		BorderThickness = 2.0f;
	}
	else if (bIsSuccess)
	{
		BackgroundColor = FLinearColor(0.1f, 0.3f, 0.1f, 0.3f);
		TextColor = FLinearColor(0.5f, 1.0f, 0.5f);
		RoleLabel = TEXT("SUCCESS");
		BorderThickness = 2.0f;
	}
	else if (Role == TEXT("user"))
	{
		BackgroundColor = FLinearColor(0.15f, 0.15f, 0.25f, 0.5f);
		TextColor = FLinearColor(0.9f, 0.9f, 1.0f);
		RoleLabel = TEXT("You");
	}
	else if (Role == TEXT("assistant"))
	{
		BackgroundColor = FLinearColor(0.1f, 0.15f, 0.2f, 0.5f);
		TextColor = FLinearColor(0.7f, 0.9f, 1.0f);
		RoleLabel = TEXT("AI");
	}
	else // system
	{
		BackgroundColor = FLinearColor(0.2f, 0.2f, 0.1f, 0.3f);
		TextColor = FLinearColor(1.0f, 1.0f, 0.7f);
		RoleLabel = TEXT("System");
	}

	// Format timestamp
	FString Timestamp = FDateTime::Now().ToString(TEXT("%H:%M:%S"));

	// Create message widget with border and padding
	return SNew(SBorder)
		.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(BackgroundColor)
		.Padding(8.0f)
		[
			SNew(SVerticalBox)
			
			// Role label and timestamp
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 0.0f, 0.0f, 4.0f)
			[
				SNew(SHorizontalBox)
				
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(RoleLabel))
					.TextStyle(FAppStyle::Get(), "BoldFont")
					.ColorAndOpacity(TextColor)
				]
				
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.HAlign(HAlign_Right)
				[
					SNew(STextBlock)
					.Text(FText::FromString(Timestamp))
					.ColorAndOpacity(FLinearColor(0.6f, 0.6f, 0.6f))
					.TextStyle(FAppStyle::Get(), "SmallFont")
				]
			]
			
			// Message content
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString(Content))
				.AutoWrapText(true)
				.WrapTextAt(550.0f)
				.ColorAndOpacity(TextColor)
			]
		];
}

void SAINiagaraChatWidget::ShowLoading(bool bShow, const FString& LoadingMessage)
{
	if (LoadingBar.IsValid())
	{
		LoadingBar->SetVisibility(bShow ? EVisibility::Visible : EVisibility::Collapsed);
	}

	if (LoadingText.IsValid())
	{
		LoadingText->SetVisibility(bShow ? EVisibility::Visible : EVisibility::Collapsed);
		if (!LoadingMessage.IsEmpty())
		{
			LoadingText->SetText(FText::FromString(LoadingMessage));
		}
	}

	if (SendButton.IsValid())
	{
		SendButton->SetEnabled(!bShow);
	}

	if (InputTextBox.IsValid())
	{
		InputTextBox->SetEnabled(!bShow);
	}
}

void SAINiagaraChatWidget::ClearInput()
{
	if (InputTextBox.IsValid())
	{
		InputTextBox->SetText(FText::GetEmpty());
		FSlateApplication::Get().SetKeyboardFocus(InputTextBox.ToSharedRef());
	}
}

void SAINiagaraChatWidget::LoadConversationHistory()
{
	if (CurrentAssetPath.IsEmpty())
	{
		return;
	}

	// Try to load history from disk
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		if (HistoryManager->LoadHistory(CurrentAssetPath))
		{
			// Get loaded history
			TArray<FConversationMessage> History = HistoryManager->GetHistory(CurrentAssetPath);
			
			// Display all messages except system messages (they're added automatically)
			for (const FConversationMessage& Message : History)
			{
				if (Message.Role != TEXT("system"))
				{
					AddMessageToHistory(Message.Role, Message.Content);
				}
			}
		}
	}
}

FString SAINiagaraChatWidget::GetCurrentAssetPath() const
{
	// Try to get asset path from active editor
	// This is a simplified version - in a real implementation, you'd need to
	// check which editor is active (Niagara or Cascade) and get the asset path
	// For now, we'll use a default path that can be set via the widget arguments
	
	// TODO: Implement proper asset path detection from active editor
	// This would require accessing the Niagara or Cascade editor toolkit
	// and getting the currently edited asset path
	
	return FString();
}

void SAINiagaraChatWidget::RequestDSLCorrection(const FString& InvalidDSL, const FVFXDSLValidationResult& ValidationResult)
{
	// TODO: Implement DSL correction request to LLM
	// This would send the invalid DSL and validation errors back to the LLM
	// asking for corrections
}

void SAINiagaraChatWidget::ShowSuccessNotification(const FString& Message)
{
	AddMessageToHistory(TEXT("system"), Message, false, true);
	
	// Save to conversation history
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		HistoryManager->AddMessage(CurrentAssetPath, TEXT("system"), Message);
	}
}

void SAINiagaraChatWidget::ShowErrorNotification(const FString& Message)
{
	AddMessageToHistory(TEXT("system"), Message, true, false);
	
	// Save to conversation history
	if (UConversationHistoryManager* HistoryManager = UConversationHistoryManager::Get())
	{
		HistoryManager->AddMessage(CurrentAssetPath, TEXT("system"), Message);
	}
}

FReply SAINiagaraChatWidget::OnExportDSLClicked()
{
	// Get current asset path
	FString AssetPath = CurrentAssetPath;
	if (AssetPath.IsEmpty())
	{
		AssetPath = GetCurrentAssetPath();
	}

	if (AssetPath.IsEmpty())
	{
		ShowErrorNotification(TEXT("No asset selected. Please open a Niagara system first."));
		return FReply::Handled();
	}

	// Try to load the Niagara system
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(*AssetPath);

	if (!AssetData.IsValid())
	{
		ShowErrorNotification(FString::Printf(TEXT("Could not find asset: %s"), *AssetPath));
		return FReply::Handled();
	}

	// Try Niagara system first
	UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(AssetData.GetAsset());
	UParticleSystem* ParticleSystem = nullptr;
	
	bool bConverted = false;
	FVFXDSL DSL;
	FString ConversionError;
	FString SystemName;

	if (NiagaraSystem)
	{
		SystemName = NiagaraSystem->GetName();
		bConverted = UNiagaraSystemToDSLConverter::ConvertSystemToDSL(
			NiagaraSystem,
			DSL,
			ConversionError
		);
	}
	else
	{
		// Try Cascade system
		ParticleSystem = Cast<UParticleSystem>(AssetData.GetAsset());
		if (!ParticleSystem)
		{
			ShowErrorNotification(TEXT("Selected asset is not a Niagara or Cascade system."));
			return FReply::Handled();
		}

		SystemName = ParticleSystem->GetName();
		bConverted = UCascadeSystemToDSLConverter::ConvertSystemToDSL(
			ParticleSystem,
			DSL,
			ConversionError
		);
	}

	if (!bConverted)
	{
		ShowErrorNotification(FString::Printf(TEXT("Failed to convert system to DSL: %s"), *ConversionError));
		return FReply::Handled();
	}

	// Export to JSON
	FString JsonOutput;
	if (!UNiagaraSystemToDSLConverter::ExportDSLToJSON(DSL, JsonOutput, true))
	{
		ShowErrorNotification(TEXT("Failed to export DSL to JSON."));
		return FReply::Handled();
	}

	// Show save file dialog
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		ShowErrorNotification(TEXT("Desktop platform not available."));
		return FReply::Handled();
	}

	TArray<FString> OutFilenames;
	FString DefaultPath = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("Exports");
	FString DefaultFile = SystemName + TEXT("_DSL.json");

	if (DesktopPlatform->SaveFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		NSLOCTEXT("AINiagara", "ExportDSLDialogTitle", "Export DSL").ToString(),
		DefaultPath,
		DefaultFile,
		TEXT("JSON files (*.json)|*.json|All files (*.*)|*.*"),
		EFileDialogFlags::None,
		OutFilenames))
	{
		if (OutFilenames.Num() > 0)
		{
			FString SavePath = OutFilenames[0];
			if (UNiagaraSystemToDSLConverter::ExportDSLToFile(DSL, SavePath))
			{
				FString SuccessMessage = FString::Printf(
					TEXT("DSL exported successfully to:\n%s"),
					*SavePath
				);
				ShowSuccessNotification(SuccessMessage);
				
				// Also add to chat history
				AddMessageToHistory(TEXT("system"), FString::Printf(TEXT("Exported DSL:\n\n%s"), *JsonOutput), false, false);
			}
			else
			{
				ShowErrorNotification(TEXT("Failed to save DSL file."));
			}
		}
	}

	return FReply::Handled();
}

FReply SAINiagaraChatWidget::OnImportDSLClicked()
{
	// Show open file dialog
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		ShowErrorNotification(TEXT("Desktop platform not available."));
		return FReply::Handled();
	}

	TArray<FString> OutFilenames;
	FString DefaultPath = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("Exports");

	if (DesktopPlatform->OpenFileDialog(
		FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
		NSLOCTEXT("AINiagara", "ImportDSLDialogTitle", "Import DSL").ToString(),
		DefaultPath,
		TEXT(""),
		TEXT("JSON files (*.json)|*.json|All files (*.*)|*.*"),
		EFileDialogFlags::None,
		OutFilenames))
	{
		if (OutFilenames.Num() > 0)
		{
			FString FilePath = OutFilenames[0];
			
			// Read JSON file
			FString JsonContent;
			if (!FFileHelper::LoadFileToString(JsonContent, *FilePath))
			{
				ShowErrorNotification(TEXT("Failed to read DSL file."));
				return FReply::Handled();
			}

			// Parse DSL
			FVFXDSL ParsedDSL;
			FString ParseError;
			if (!UVFXDSLParser::ParseFromJSON(JsonContent, ParsedDSL, ParseError))
			{
				ShowErrorNotification(FString::Printf(TEXT("Failed to parse DSL: %s"), *ParseError));
				return FReply::Handled();
			}

			// Validate DSL
			FVFXDSLValidationResult ValidationResult = UVFXDSLValidator::Validate(ParsedDSL);
			if (!ValidationResult.bIsValid)
			{
				FString ErrorMessage = TEXT("DSL validation failed:\n");
				for (const FString& Error : ValidationResult.ErrorMessages)
				{
					ErrorMessage += Error + TEXT("\n");
				}
				ShowErrorNotification(ErrorMessage);
				return FReply::Handled();
			}

			// Store loaded DSL
			LoadedDSL = ParsedDSL;
			bHasLoadedDSL = true;

			// Show success message
			FString SuccessMessage = FString::Printf(
				TEXT("DSL imported successfully from:\n%s\n\nSystem type: %s\nEmitters: %d\n\nClick 'Regenerate' to create system from this DSL."),
				*FilePath,
				ParsedDSL.Effect.Type == EVFXEffectType::Niagara ? TEXT("Niagara") : TEXT("Cascade"),
				ParsedDSL.Emitters.Num()
			);
			ShowSuccessNotification(SuccessMessage);

			// Add DSL preview to chat
			FString JsonOutput;
			if (UNiagaraSystemToDSLConverter::ExportDSLToJSON(ParsedDSL, JsonOutput, true))
			{
				AddMessageToHistory(TEXT("system"), FString::Printf(TEXT("Loaded DSL:\n\n%s"), *JsonOutput), false, false);
			}
		}
	}

	return FReply::Handled();
}

FReply SAINiagaraChatWidget::OnRegenerateDSLClicked()
{
	if (!bHasLoadedDSL)
	{
		ShowErrorNotification(TEXT("No DSL loaded. Please import a DSL file first."));
		return FReply::Handled();
	}

	// Determine package path
	FString PackagePath = TEXT("/Game/VFX");
	if (!CurrentAssetPath.IsEmpty())
	{
		int32 LastSlash = CurrentAssetPath.Find(TEXT("/"), ESearchCase::CaseSensitive, ESearchDir::FromEnd);
		if (LastSlash != INDEX_NONE)
		{
			PackagePath = CurrentAssetPath.Left(LastSlash);
		}
	}

	// Generate system name from first emitter name or use default
	FString SystemName = TEXT("AIRegeneratedSystem");
	if (LoadedDSL.Emitters.Num() > 0 && !LoadedDSL.Emitters[0].Name.IsEmpty())
	{
		SystemName = LoadedDSL.Emitters[0].Name + TEXT("_System");
	}

	// Generate system based on type
	if (LoadedDSL.Effect.Type == EVFXEffectType::Niagara)
	{
		ShowLoading(true, TEXT("Regenerating Niagara system from DSL..."));

		UNiagaraSystem* GeneratedSystem = nullptr;
		FString GenerationError;

		if (UNiagaraSystemGenerator::CreateSystemFromDSL(LoadedDSL, PackagePath, SystemName, GeneratedSystem, GenerationError))
		{
			ShowLoading(false);
			FString SuccessMessage = FString::Printf(
				TEXT("Niagara system '%s' regenerated successfully at %s/%s!"),
				*SystemName,
				*PackagePath,
				*SystemName
			);
			ShowSuccessNotification(SuccessMessage);
		}
		else
		{
			ShowLoading(false);
			FString ErrorMessage = FString::Printf(
				TEXT("Failed to regenerate Niagara system: %s"),
				*GenerationError
			);
			ShowErrorNotification(ErrorMessage);
		}
	}
	else if (LoadedDSL.Effect.Type == EVFXEffectType::Cascade)
	{
		ShowLoading(true, TEXT("Regenerating Cascade system from DSL..."));

		UParticleSystem* GeneratedSystem = nullptr;
		FString GenerationError;

		if (UCascadeSystemGenerator::CreateSystemFromDSL(LoadedDSL, PackagePath, SystemName, GeneratedSystem, GenerationError))
		{
			ShowLoading(false);
			FString SuccessMessage = FString::Printf(
				TEXT("Cascade system '%s' regenerated successfully at %s/%s!"),
				*SystemName,
				*PackagePath,
				*SystemName
			);
			ShowSuccessNotification(SuccessMessage);
		}
		else
		{
			ShowLoading(false);
			FString ErrorMessage = FString::Printf(
				TEXT("Failed to regenerate Cascade system: %s"),
				*GenerationError
			);
			ShowErrorNotification(ErrorMessage);
		}
	}
	else
	{
		ShowErrorNotification(TEXT("Invalid DSL type for regeneration."));
	}

	return FReply::Handled();
}

FReply SAINiagaraChatWidget::OnPreviewToggleClicked()
{
	if (PreviewManager)
	{
		bool bCurrentlyEnabled = PreviewManager->IsPreviewEnabled();
		PreviewManager->SetPreviewEnabled(!bCurrentlyEnabled);
		
		if (!bCurrentlyEnabled)
		{
			ShowSuccessNotification(TEXT("Preview enabled - systems will update in real-time"));
		}
		else
		{
			ShowSuccessNotification(TEXT("Preview disabled"));
		}
	}
	
	return FReply::Handled();
}

void SAINiagaraChatWidget::UpdatePreview(const FVFXDSL& DSL)
{
	if (!PreviewManager)
	{
		return;
	}
	
	if (!PreviewManager->IsPreviewEnabled())
	{
		return; // Preview disabled, skip update
	}
	
	// Get diff before updating (only if there's a current preview)
	FVFXDSLDiffResult Diff;
	if (PreviewManager->IsPreviewActive())
	{
		Diff = PreviewManager->GetDSLDiff(DSL);
	}
	
	FString PreviewError;
	bool bSuccess = PreviewManager->UpdatePreview(DSL, false, &PreviewError);
	
	if (bSuccess)
	{
		// Preview updated successfully - show diff if there are changes
		if (Diff.bHasChanges && Diff.Changes.Num() > 0)
		{
			FString DiffMessage = TEXT("Preview updated. Changes:\n");
			int32 MaxChangesToShow = 5;
			for (int32 i = 0; i < FMath::Min(Diff.Changes.Num(), MaxChangesToShow); i++)
			{
				DiffMessage += FString::Printf(TEXT("  • %s\n"), *Diff.Changes[i].Description);
			}
			if (Diff.Changes.Num() > MaxChangesToShow)
			{
				DiffMessage += FString::Printf(TEXT("  ... and %d more change(s)\n"), Diff.Changes.Num() - MaxChangesToShow);
			}
			AddMessageToHistory(TEXT("system"), DiffMessage, false, true);
		}
		else if (!PreviewManager->IsPreviewActive())
		{
			// First preview created
			AddMessageToHistory(TEXT("system"), TEXT("Preview created successfully"), false, true);
		}
	}
	else
	{
		// Preview update failed - show error but don't break the chat flow
		// Only show error if it's a real error (not throttling, no change, or disabled)
		if (!PreviewError.IsEmpty())
		{
			bool bShouldShowError = true;
			
			// Filter out expected/non-error cases
			FString LowerError = PreviewError.ToLower();
			if (LowerError.Contains(TEXT("throttled")) ||
			    LowerError.Contains(TEXT("not changed")) ||
			    LowerError.Contains(TEXT("disabled")) ||
			    LowerError.Contains(TEXT("has not changed")))
			{
				bShouldShowError = false;
			}
			
			if (bShouldShowError)
			{
				FString ErrorMessage = FString::Printf(
					TEXT("Preview update failed: %s (Previous preview maintained)"),
					*PreviewError
				);
				ShowErrorNotification(ErrorMessage);
			}
		}
		else
		{
			// No error message provided, but update failed
			// This shouldn't happen, but handle gracefully
			ShowErrorNotification(TEXT("Preview update failed (unknown error). Previous preview maintained."));
		}
	}
}

bool SAINiagaraChatWidget::ProcessToolCalls(const FString& ResponseText)
{
	// Try to parse response as JSON to check for tool calls
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseText);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		// Not JSON or invalid - no tool calls
		return false;
	}

	// Check for function calls in Gemini API format
	// Format: { "functionCall": { "name": "tool:texture", "args": {...} } }
	if (JsonObject->HasTypedField<EJson::Object>(TEXT("functionCall")))
	{
		TSharedPtr<FJsonObject> FunctionCallObj = JsonObject->GetObjectField(TEXT("functionCall"));
		if (FunctionCallObj.IsValid())
		{
			FString FunctionName = FunctionCallObj->GetStringField(TEXT("name"));
			
			if (FunctionName == TEXT("tool:texture"))
			{
				TSharedPtr<FJsonObject> ArgsObj = FunctionCallObj->GetObjectField(TEXT("args"));
				ProcessTextureGenerationTool(ArgsObj);
				return true;
			}
			else if (FunctionName == TEXT("tool:shader"))
			{
				// TODO: Implement shader generation (Phase 11)
				AddMessageToHistory(TEXT("system"), TEXT("Shader generation is not yet implemented (Phase 11)."), true, false);
				return true;
			}
			else if (FunctionName == TEXT("tool:material"))
			{
				// TODO: Implement material generation (Phase 11)
				AddMessageToHistory(TEXT("system"), TEXT("Material generation is not yet implemented (Phase 11)."), true, false);
				return true;
			}
		}
	}

	// Check for candidates format with function calls
	// Format: { "candidates": [{ "content": { "parts": [{ "functionCall": {...} }] } }] }
	if (JsonObject->HasTypedField<EJson::Array>(TEXT("candidates")))
	{
		const TArray<TSharedPtr<FJsonValue>>& Candidates = JsonObject->GetArrayField(TEXT("candidates"));
		if (Candidates.Num() > 0)
		{
			TSharedPtr<FJsonObject> CandidateObj = Candidates[0]->AsObject();
			if (CandidateObj.IsValid())
			{
				TSharedPtr<FJsonObject> ContentObj = CandidateObj->GetObjectField(TEXT("content"));
				if (ContentObj.IsValid())
				{
					const TArray<TSharedPtr<FJsonValue>>& Parts = ContentObj->GetArrayField(TEXT("parts"));
					for (const TSharedPtr<FJsonValue>& PartValue : Parts)
					{
						TSharedPtr<FJsonObject> PartObj = PartValue->AsObject();
						if (PartObj.IsValid() && PartObj->HasTypedField<EJson::Object>(TEXT("functionCall")))
						{
							TSharedPtr<FJsonObject> FunctionCallObj = PartObj->GetObjectField(TEXT("functionCall"));
							FString FunctionName = FunctionCallObj->GetStringField(TEXT("name"));
							
							if (FunctionName == TEXT("tool:texture"))
							{
								TSharedPtr<FJsonObject> ArgsObj = FunctionCallObj->GetObjectField(TEXT("args"));
								ProcessTextureGenerationTool(ArgsObj);
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

void SAINiagaraChatWidget::ProcessTextureGenerationTool(TSharedPtr<FJsonObject> ToolParameters)
{
	if (!ToolParameters.IsValid())
	{
		ShowErrorNotification(TEXT("Invalid texture generation parameters"));
		return;
	}

	// Extract parameters
	FTextureGenerationRequest Request;
	
	// Type (required)
	if (!ToolParameters->TryGetStringField(TEXT("type"), Request.TextureType))
	{
		ShowErrorNotification(TEXT("Texture type is required"));
		return;
	}

	// Prompt (build from context)
	FString PromptDescription;
	if (ToolParameters->TryGetStringField(TEXT("description"), PromptDescription))
	{
		Request.Prompt = PromptDescription;
	}
	else
	{
		Request.Prompt = FString::Printf(TEXT("Generate %s texture for VFX"), *Request.TextureType);
	}

	// Resolution (optional, default 512)
	int32 Resolution = 512;
	if (ToolParameters->TryGetNumberField(TEXT("resolution"), Resolution))
	{
		Request.Resolution = Resolution;
	}

	// Color scheme (optional)
	ToolParameters->TryGetStringField(TEXT("colorScheme"), Request.ColorScheme);

	// Frames (optional, default 1)
	int32 Frames = 1;
	if (ToolParameters->TryGetNumberField(TEXT("frames"), Frames))
	{
		Request.Frames = Frames;
	}

	// Target emitter (optional)
	ToolParameters->TryGetStringField(TEXT("targetEmitter"), Request.TargetEmitterName);

	// Show loading
	FString LoadingMessage = FString::Printf(
		TEXT("Generating %s texture (%dx%d, %d frame%s)..."),
		*Request.TextureType,
		Request.Resolution,
		Request.Resolution,
		Request.Frames,
		Request.Frames > 1 ? TEXT("s") : TEXT("")
	);
	ShowLoading(true, LoadingMessage);
	AddMessageToHistory(TEXT("system"), LoadingMessage, false, false);

	// Generate texture
	UTextureGenerationHandler::GenerateTexture(
		Request,
		FOnTextureGenerated::CreateLambda([this, Request](const FTextureGenerationResult& Result)
		{
			ShowLoading(false);

			if (Result.bSuccess && Result.Texture)
			{
				FString SuccessMessage;
				if (Result.FrameCount > 1)
				{
					SuccessMessage = FString::Printf(
						TEXT("✅ Generated %s flipbook texture (%d frames, %dx%d)!"),
						*Request.TextureType,
						Result.FrameCount,
						Result.Texture->GetSizeX(),
						Result.Texture->GetSizeY()
					);
				}
				else
				{
					SuccessMessage = FString::Printf(
						TEXT("✅ Generated %s texture (%dx%d)!"),
						*Request.TextureType,
						Result.Texture->GetSizeX(),
						Result.Texture->GetSizeY()
					);
				}
				
				AddMessageToHistory(TEXT("system"), SuccessMessage, false, true);
				ShowSuccessNotification(SuccessMessage);

				// Try to apply texture to target emitter/system if specified
				if (!Request.TargetEmitterName.IsEmpty())
				{
					// Try to get the current preview system
					UObject* PreviewSystem = nullptr;
					if (PreviewManager && PreviewManager->IsPreviewActive())
					{
						// Get preview system (Niagara or Cascade)
						UNiagaraSystem* NiagaraPreview = PreviewManager->GetNiagaraPreview();
						UParticleSystem* CascadePreview = PreviewManager->GetCascadePreview();

						if (CascadePreview)
						{
							// Apply texture to Cascade system
							FString ApplyError;
							if (UTextureMaterialHelper::ApplyTextureToCascadeSystem(
								CascadePreview,
								Result.Texture,
								Request.TargetEmitterName,
								FName(TEXT("Texture")),
								ApplyError))
							{
								AddMessageToHistory(TEXT("system"), 
									FString::Printf(TEXT("✅ Applied texture to Cascade emitter '%s'"), *Request.TargetEmitterName),
									false, true);
							}
							else
							{
								AddMessageToHistory(TEXT("system"), 
									FString::Printf(TEXT("⚠️ Could not apply texture to emitter: %s"), *ApplyError),
									false, false);
							}
						}
						else if (NiagaraPreview)
						{
							// Niagara texture application not yet fully implemented
							AddMessageToHistory(TEXT("system"), 
								TEXT("⚠️ Niagara texture application not yet fully implemented. Apply texture manually in the emitter's render settings."),
								false, false);
						}
					}
					else
					{
						AddMessageToHistory(TEXT("system"), 
							TEXT("⚠️ No active preview. Generate a VFX system first, then request texture generation with target emitter name."),
							false, false);
					}
				}
				else
				{
					// No target emitter specified
					AddMessageToHistory(TEXT("system"), 
						TEXT("💡 Tip: Specify a target emitter name in your request to automatically apply the texture."),
						false, false);
				}
			}
			else
			{
				FString ErrorMessage = FString::Printf(
					TEXT("❌ Texture generation failed: %s"),
					*Result.ErrorMessage
				);
				AddMessageToHistory(TEXT("system"), ErrorMessage, true, false);
				ShowErrorNotification(ErrorMessage);
			}
		})
	);
}

bool SAINiagaraChatWidget::IsPreviewEnabled() const
{
	return PreviewManager && PreviewManager->IsPreviewEnabled();
}

FText SAINiagaraChatWidget::GetPreviewToggleText() const
{
	if (IsPreviewEnabled())
	{
		return NSLOCTEXT("AINiagara", "PreviewToggleOn", "Preview: ON");
	}
	else
	{
		return NSLOCTEXT("AINiagara", "PreviewToggleOff", "Preview: OFF");
	}
}

