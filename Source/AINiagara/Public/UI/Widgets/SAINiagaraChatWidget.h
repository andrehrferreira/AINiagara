// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SScrollBox;
class SEditableTextBox;
class SButton;
class SProgressBar;
class STextBlock;
class SBorder;
class UPreviewSystemManager;

/**
 * Chat widget for AI VFX generation
 */
class AINIAGARA_API SAINiagaraChatWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAINiagaraChatWidget)
		: _AssetPath(FString())
	{}
		SLATE_ARGUMENT(FString, AssetPath)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	/** Message history scroll box */
	TSharedPtr<SScrollBox> MessageHistoryBox;

	/** Input text box */
	TSharedPtr<SEditableTextBox> InputTextBox;

	/** Send button */
	TSharedPtr<SButton> SendButton;

	/** Export DSL button */
	TSharedPtr<SButton> ExportDSLButton;

	/** Import DSL button */
	TSharedPtr<SButton> ImportDSLButton;

	/** Regenerate from DSL button */
	TSharedPtr<SButton> RegenerateDSLButton;

	/** Preview toggle button */
	TSharedPtr<SButton> PreviewToggleButton;

	/** Loading progress bar */
	TSharedPtr<SProgressBar> LoadingBar;

	/** Loading text block */
	TSharedPtr<STextBlock> LoadingText;

	/** Current asset path */
	FString CurrentAssetPath;

	/**
	 * Handle send button click
	 */
	FReply OnSendClicked();

	/**
	 * Handle export DSL button click
	 */
	FReply OnExportDSLClicked();

	/**
	 * Handle import DSL button click
	 */
	FReply OnImportDSLClicked();

	/**
	 * Handle regenerate from DSL button click
	 */
	FReply OnRegenerateDSLClicked();

	/**
	 * Handle preview toggle button click
	 */
	FReply OnPreviewToggleClicked();

	/**
	 * Update preview system from DSL
	 */
	void UpdatePreview(const FVFXDSL& DSL);

	/**
	 * Current DSL loaded from file (for regeneration)
	 */
	FVFXDSL LoadedDSL;

	/**
	 * Whether a DSL is currently loaded
	 */
	bool bHasLoadedDSL = false;

	/**
	 * Handle enter key in input box
	 */
	void OnInputTextCommitted(const FText& Text, ETextCommit::Type CommitType);

	/**
	 * Add message to history display with improved formatting
	 */
	void AddMessageToHistory(const FString& Role, const FString& Content, bool bIsError = false, bool bIsSuccess = false);

	/**
	 * Create a formatted message widget
	 */
	TSharedRef<SWidget> CreateMessageWidget(const FString& Role, const FString& Content, bool bIsError = false, bool bIsSuccess = false);

	/**
	 * Show loading state with optional message
	 */
	void ShowLoading(bool bShow, const FString& LoadingMessage = TEXT(""));

	/**
	 * Clear input field
	 */
	void ClearInput();

	/**
	 * Load conversation history for current asset
	 */
	void LoadConversationHistory();

	/**
	 * Get current asset path from editor context
	 */
	FString GetCurrentAssetPath() const;

	/**
	 * Request DSL correction from LLM
	 */
	void RequestDSLCorrection(const FString& InvalidDSL, const FVFXDSLValidationResult& ValidationResult);

	/**
	 * Show success notification
	 */
	void ShowSuccessNotification(const FString& Message);

	/**
	 * Show error notification
	 */
	void ShowErrorNotification(const FString& Message);

	/**
	 * Check if regenerate button should be enabled
	 */
	bool IsRegenerateEnabled() const { return bHasLoadedDSL; }

	/**
	 * Check if preview is enabled
	 */
	bool IsPreviewEnabled() const;

	/**
	 * Get preview toggle button text
	 */
	FText GetPreviewToggleText() const;

	/**
	 * Detect and process tool calls in AI response
	 * @param ResponseText AI response text
	 * @return True if tool call was detected and processed
	 */
	bool ProcessToolCalls(const FString& ResponseText);

	/**
	 * Process texture generation tool call
	 * @param ToolParameters Tool call parameters JSON
	 */
	void ProcessTextureGenerationTool(TSharedPtr<FJsonObject> ToolParameters);

	/**
	 * Process shader generation tool call
	 * @param ToolParameters Tool call parameters JSON
	 */
	void ProcessShaderGenerationTool(TSharedPtr<FJsonObject> ToolParameters);

	/**
	 * Process material generation tool call
	 * @param ToolParameters Tool call parameters JSON
	 */
	void ProcessMaterialGenerationTool(TSharedPtr<FJsonObject> ToolParameters);

	/**
	 * Preview system manager instance
	 */
	UPROPERTY()
	UPreviewSystemManager* PreviewManager;
};

