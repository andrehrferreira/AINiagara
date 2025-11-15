// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class SEditableTextBox;
class SButton;
class STextBlock;

/**
 * API Key configuration dialog widget
 */
class AINIAGARA_API SAINiagaraAPIKeyDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAINiagaraAPIKeyDialog)
	{}
		SLATE_EVENT(FSimpleDelegate, OnAPIKeyConfigured)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	/** Delegate for when API key is successfully configured */
	FSimpleDelegate OnAPIKeyConfigured;

	/** Delegate for when dialog is cancelled */
	DECLARE_DELEGATE(FOnDialogCancelled);
	FOnDialogCancelled OnDialogCancelled;

	/** Check if widget is still valid */
	bool IsValid() const { return bIsValid; }

	/** Mark widget as invalid (called when widget is being destroyed) */
	void Invalidate() { bIsValid = false; }

private:
	/** API key input field */
	TSharedPtr<SEditableTextBox> APIKeyInputBox;

	/** Test button */
	TSharedPtr<SButton> TestButton;

	/** Save button */
	TSharedPtr<SButton> SaveButton;

	/** Cancel button */
	TSharedPtr<SButton> CancelButton;

	/** Status message text */
	TSharedPtr<STextBlock> StatusTextBlock;

	/** Current API key being tested */
	FString CurrentAPIKey;

	/** Flag to track if widget is still valid */
	bool bIsValid;

	/** Weak pointer to self for safe callbacks */
	TWeakPtr<SAINiagaraAPIKeyDialog> WeakSelf;

	/**
	 * Handle test button click
	 */
	FReply OnTestClicked();

	/**
	 * Handle save button click
	 */
	FReply OnSaveClicked();

	/**
	 * Handle cancel button click
	 */
	FReply OnCancelClicked();

	/**
	 * Update status message
	 */
	void UpdateStatus(const FString& Message, bool bIsError = false);

	/**
	 * Enable/disable buttons during API test
	 */
	void SetButtonsEnabled(bool bEnabled);
};

