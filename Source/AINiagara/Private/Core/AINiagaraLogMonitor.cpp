// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/AINiagaraLogMonitor.h"
#include "HAL/PlatformFilemanager.h"
#include "HAL/Platform.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/IConsoleManager.h"
#include "Misc/App.h"
#include "Misc/DateTime.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "HAL/FileManager.h"

DEFINE_LOG_CATEGORY(LogAINiagaraDebug);

FAINiagaraLogMonitor& FAINiagaraLogMonitor::Get()
{
	static FAINiagaraLogMonitor Instance;
	return Instance;
}

FAINiagaraLogMonitor::FAINiagaraLogMonitor()
	: bIsMonitoring(false)
	, LastLogFileSize(0)
{
	// Initialize relevant categories to monitor
	RelevantCategories.Add(TEXT("LogTemp"));
	RelevantCategories.Add(TEXT("LogAINiagara"));
	RelevantCategories.Add(TEXT("LogAINiagaraDebug"));
	RelevantCategories.Add(TEXT("LogLiveCoding"));
	RelevantCategories.Add(TEXT("LogToolMenus"));
	
	// Initialize relevant keywords
	RelevantKeywords.Add(TEXT("AINiagara"));
	RelevantKeywords.Add(TEXT("RegisterMenuExtensions"));
	RelevantKeywords.Add(TEXT("RegisterNiagaraToolbarButton"));
	RelevantKeywords.Add(TEXT("Toolbar Initialize"));
	RelevantKeywords.Add(TEXT("Live Coding"));
	RelevantKeywords.Add(TEXT("Live coding"));
	RelevantKeywords.Add(TEXT("ToolMenus"));
	
	// Initialize Live Coding status
	LiveCodingStatus = MakeShareable(new FJsonObject);
	LiveCodingStatus->SetStringField(TEXT("status"), TEXT("unknown"));
	LiveCodingStatus->SetStringField(TEXT("lastEvent"), TEXT(""));
	LiveCodingStatus->SetStringField(TEXT("lastEventTime"), TEXT(""));
	
	// Set debug output path (in plugin directory)
	// Try multiple possible paths
	FString PluginBasePath;
	
	// First try: ProjectPluginsDir
	FString ProjectPluginsPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("AINiagara"));
	if (FPaths::DirectoryExists(ProjectPluginsPath))
	{
		PluginBasePath = ProjectPluginsPath;
	}
	else
	{
		// Second try: EnginePluginsDir
		FString EnginePluginsPath = FPaths::Combine(FPaths::EnginePluginsDir(), TEXT("AINiagara"));
		if (FPaths::DirectoryExists(EnginePluginsPath))
		{
			PluginBasePath = EnginePluginsPath;
		}
		else
		{
			// Fallback: Use Saved directory
			PluginBasePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("AINiagara"));
		}
	}
	
	DebugOutputPath = FPaths::Combine(PluginBasePath, TEXT("debug_output.json"));
	
	// Ensure directory exists
	FString DebugDir = FPaths::GetPath(DebugOutputPath);
	IFileManager::Get().MakeDirectory(*DebugDir, true);
}

FAINiagaraLogMonitor::~FAINiagaraLogMonitor()
{
	Shutdown();
}

void FAINiagaraLogMonitor::Initialize()
{
	if (bIsMonitoring)
	{
		return;
	}

	UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Initializing log monitor"));
	UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Debug output will be written to: %s"), *DebugOutputPath);

	// Register console commands
	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("AINiagara.CheckLogs"),
		TEXT("Manually check and process editor logs"),
		FConsoleCommandWithArgsDelegate::CreateStatic(&FAINiagaraLogMonitor::ConsoleCommand_CheckLogs),
		ECVF_Default
	);

	IConsoleManager::Get().RegisterConsoleCommand(
		TEXT("AINiagara.GetDebugPath"),
		TEXT("Get the path to the debug output JSON file"),
		FConsoleCommandWithArgsDelegate::CreateStatic(&FAINiagaraLogMonitor::ConsoleCommand_GetDebugPath),
		ECVF_Default
	);

	// Start monitoring
	StartMonitoring();
	
	// Write initial debug info
	WriteDebugInfo();
}

void FAINiagaraLogMonitor::Shutdown()
{
	if (!bIsMonitoring)
	{
		return;
	}

	StopMonitoring();
	
	// Write final debug info
	WriteDebugInfo();
	
	UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Shutdown complete"));
}

FString FAINiagaraLogMonitor::GetDebugFilePath() const
{
	return DebugOutputPath;
}

void FAINiagaraLogMonitor::StartMonitoring()
{
	if (bIsMonitoring)
	{
		return;
	}

	bIsMonitoring = true;
	LastLogFileSize = 0;
	LastUpdateTime = FDateTime::Now();
	
	// Register tick function to check logs periodically
	// We don't store the handle - the ticker will stop automatically when Tick() returns false
	FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateRaw(this, &FAINiagaraLogMonitor::Tick),
		1.0f // Check every second
	);
	
	UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Monitoring started"));
}

void FAINiagaraLogMonitor::StopMonitoring()
{
	if (!bIsMonitoring)
	{
		return;
	}

	bIsMonitoring = false;
	// Note: The ticker will stop automatically when Tick() returns false
	// We don't need to manually remove it
	
	UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Monitoring stopped"));
}

FString FAINiagaraLogMonitor::GetLatestLogFilePath() const
{
	// Get the project's log directory
	FString LogsDir = FPaths::Combine(
		FPaths::ProjectSavedDir(),
		TEXT("Logs")
	);

	// Find the latest log file
	TArray<FString> LogFiles;
	IFileManager::Get().FindFiles(LogFiles, *(LogsDir / TEXT("*.log")), true, false);

	if (LogFiles.Num() == 0)
	{
		return FString();
	}

	// Sort by modification time (newest first)
	LogFiles.Sort([&LogsDir](const FString& A, const FString& B)
	{
		FString PathA = LogsDir / A;
		FString PathB = LogsDir / B;
		FDateTime TimeA = IFileManager::Get().GetTimeStamp(*PathA);
		FDateTime TimeB = IFileManager::Get().GetTimeStamp(*PathB);
		return TimeA > TimeB;
	});

	return LogsDir / LogFiles[0];
}

void FAINiagaraLogMonitor::ProcessLogFile()
{
	FString LogFilePath = GetLatestLogFilePath();
	
	if (LogFilePath.IsEmpty())
	{
		return;
	}

	// Check if file changed
	int64 CurrentFileSize = IFileManager::Get().FileSize(*LogFilePath);
	if (LogFilePath == LastLogFilePath && CurrentFileSize == LastLogFileSize)
	{
		return; // No changes
	}

	// Read new content from the log file
	if (CurrentFileSize > LastLogFileSize)
	{
		TArray<FString> Lines;
		FFileHelper::LoadFileToStringArray(Lines, *LogFilePath);
		
		// Process only new lines
		int32 StartLine = 0;
		if (LogFilePath == LastLogFilePath && LastLogFileSize > 0)
		{
			// Estimate line count based on file size (rough approximation)
			// In practice, we'd need to track line numbers, but for now we'll process recent lines
			StartLine = FMath::Max(0, Lines.Num() - 50); // Process last 50 lines
		}

		for (int32 i = StartLine; i < Lines.Num(); ++i)
		{
			const FString& Line = Lines[i];
			
			// Check if line contains relevant keywords
			bool bIsRelevant = false;
			for (const FString& Keyword : RelevantKeywords)
			{
				if (Line.Contains(Keyword, ESearchCase::IgnoreCase))
				{
					bIsRelevant = true;
					break;
				}
			}

			// Check for Live Coding events
			if (Line.Contains(TEXT("LogLiveCoding"), ESearchCase::IgnoreCase))
			{
				if (Line.Contains(TEXT("Starting Live Coding compile"), ESearchCase::IgnoreCase))
				{
					LiveCodingStatus->SetStringField(TEXT("status"), TEXT("compiling"));
					LiveCodingStatus->SetStringField(TEXT("lastEvent"), TEXT("compile_started"));
					LiveCodingStatus->SetStringField(TEXT("lastEventTime"), FDateTime::Now().ToString());
				}
				else if (Line.Contains(TEXT("Live coding succeeded"), ESearchCase::IgnoreCase))
				{
					LiveCodingStatus->SetStringField(TEXT("status"), TEXT("success"));
					LiveCodingStatus->SetStringField(TEXT("lastEvent"), TEXT("compile_succeeded"));
					LiveCodingStatus->SetStringField(TEXT("lastEventTime"), FDateTime::Now().ToString());
				}
				else if (Line.Contains(TEXT("Live coding failed"), ESearchCase::IgnoreCase))
				{
					LiveCodingStatus->SetStringField(TEXT("status"), TEXT("failed"));
					LiveCodingStatus->SetStringField(TEXT("lastEvent"), TEXT("compile_failed"));
					LiveCodingStatus->SetStringField(TEXT("lastEventTime"), FDateTime::Now().ToString());
				}
			}

			if (bIsRelevant)
			{
				// Create message object
				TSharedPtr<FJsonObject> MessageObj = MakeShareable(new FJsonObject);
				MessageObj->SetStringField(TEXT("message"), Line);
				MessageObj->SetStringField(TEXT("timestamp"), FDateTime::Now().ToString());
				MessageObj->SetNumberField(TEXT("lineNumber"), i + 1);
				
				// Determine verbosity
				FString Verbosity = TEXT("Log");
				if (Line.Contains(TEXT("Error"), ESearchCase::CaseSensitive))
				{
					Verbosity = TEXT("Error");
				}
				else if (Line.Contains(TEXT("Warning"), ESearchCase::CaseSensitive))
				{
					Verbosity = TEXT("Warning");
				}
				MessageObj->SetStringField(TEXT("verbosity"), Verbosity);
				
				RecentMessages.Add(MessageObj);
				
				// Keep only recent messages
				if (RecentMessages.Num() > MaxStoredMessages)
				{
					RecentMessages.RemoveAt(0);
				}
			}
		}
	}

	LastLogFilePath = LogFilePath;
	LastLogFileSize = CurrentFileSize;
	LastUpdateTime = FDateTime::Now();
}

void FAINiagaraLogMonitor::WriteDebugInfo()
{
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	
	// Add metadata
	RootObject->SetStringField(TEXT("pluginName"), TEXT("AINiagara"));
	RootObject->SetStringField(TEXT("lastUpdate"), FDateTime::Now().ToString());
	RootObject->SetBoolField(TEXT("isMonitoring"), bIsMonitoring);
	RootObject->SetStringField(TEXT("logFilePath"), LastLogFilePath);
	RootObject->SetNumberField(TEXT("logFileSize"), LastLogFileSize);
	
	// Add Live Coding status
	RootObject->SetObjectField(TEXT("liveCoding"), LiveCodingStatus);
	
	// Add recent messages
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const TSharedPtr<FJsonObject>& Msg : RecentMessages)
	{
		MessagesArray.Add(MakeShareable(new FJsonValueObject(Msg)));
	}
	RootObject->SetArrayField(TEXT("recentMessages"), MessagesArray);
	RootObject->SetNumberField(TEXT("messageCount"), RecentMessages.Num());
	
	// Write to file
	FString OutputString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	
	if (FFileHelper::SaveStringToFile(OutputString, *DebugOutputPath))
	{
		UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Debug info written to %s"), *DebugOutputPath);
	}
	else
	{
		UE_LOG(LogAINiagaraDebug, Error, TEXT("AINiagaraLogMonitor: Failed to write debug info to %s"), *DebugOutputPath);
	}
}

bool FAINiagaraLogMonitor::Tick(float DeltaTime)
{
	// Stop ticking if monitoring is disabled
	if (!bIsMonitoring)
	{
		return false; // Stop ticking
	}

	ProcessLogFile();
	WriteDebugInfo();
	
	return true; // Continue ticking
}

void FAINiagaraLogMonitor::ProcessLogMessage(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category)
{
	// This would be called from a log hook if we implement one
	// For now, we process the log file directly
}

void FAINiagaraLogMonitor::CheckLiveCodingStatus()
{
	ProcessLogFile();
	WriteDebugInfo();
}

void FAINiagaraLogMonitor::ConsoleCommand_CheckLogs(const TArray<FString>& Args)
{
	UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Manual log check triggered"));
	FAINiagaraLogMonitor::Get().CheckLiveCodingStatus();
}

void FAINiagaraLogMonitor::ConsoleCommand_GetDebugPath(const TArray<FString>& Args)
{
	FString DebugPath = FAINiagaraLogMonitor::Get().GetDebugFilePath();
	UE_LOG(LogAINiagaraDebug, Log, TEXT("AINiagaraLogMonitor: Debug file path: %s"), *DebugPath);
	UE_LOG(LogTemp, Log, TEXT("AINiagara Debug Path: %s"), *DebugPath);
}

