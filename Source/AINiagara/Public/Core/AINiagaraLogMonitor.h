// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/Platform.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/IConsoleManager.h"
#include "Logging/LogMacros.h"
#include "Misc/DateTime.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Delegates/Delegate.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAINiagaraDebug, Log, All);

/**
 * Monitors Unreal Editor output logs and Live Coding events
 * Writes structured debug information to a JSON file for external tools
 */
class FAINiagaraLogMonitor
{
public:
	/** Get the singleton instance */
	static FAINiagaraLogMonitor& Get();

	FAINiagaraLogMonitor();
	~FAINiagaraLogMonitor();

	/** Initialize the log monitor */
	void Initialize();

	/** Shutdown the log monitor */
	void Shutdown();

	/** Check if monitoring is active */
	bool IsMonitoring() const { return bIsMonitoring; }

	/** Get the path to the debug output file */
	FString GetDebugFilePath() const;

	/** Manually process a log message (called from log hook) */
	void ProcessLogMessage(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category);

	/** Check for Live Coding events in the log file */
	void CheckLiveCodingStatus();

private:
	/** Start monitoring the log file */
	void StartMonitoring();

	/** Stop monitoring */
	void StopMonitoring();

	/** Process the log file and extract relevant messages */
	void ProcessLogFile();

	/** Write debug info to JSON file */
	void WriteDebugInfo();

	/** Get the latest editor log file path */
	FString GetLatestLogFilePath() const;

	/** Tick function for periodic checks */
	bool Tick(float DeltaTime);

	/** Console command to manually trigger log check */
	static void ConsoleCommand_CheckLogs(const TArray<FString>& Args);

	/** Console command to get debug file path */
	static void ConsoleCommand_GetDebugPath(const TArray<FString>& Args);

private:
	bool bIsMonitoring;
	FString LastLogFilePath;
	int64 LastLogFileSize;
	// Note: We don't store the ticker handle - the ticker will stop when Tick() returns false
	
	// Structured debug data
	TArray<TSharedPtr<FJsonObject>> RecentMessages;
	TSharedPtr<FJsonObject> LiveCodingStatus;
	FDateTime LastUpdateTime;
	
	// Filters
	TArray<FString> RelevantCategories;
	TArray<FString> RelevantKeywords;
	
	// Debug output file path
	FString DebugOutputPath;
	
	// Max messages to keep in memory
	static const int32 MaxStoredMessages = 100;
};

