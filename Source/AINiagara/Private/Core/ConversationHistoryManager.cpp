// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/ConversationHistoryManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Misc/DateTime.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "NiagaraSystem.h"

UConversationHistoryManager::UConversationHistoryManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bAutoPersistenceEnabled(true)
{
}

UConversationHistoryManager* UConversationHistoryManager::Get()
{
	static UConversationHistoryManager* Instance = nullptr;
	
	if (!Instance)
	{
		Instance = NewObject<UConversationHistoryManager>();
		Instance->AddToRoot(); // Prevent garbage collection
		Instance->RegisterAssetEventHooks(); // Register hooks on creation
	}
	
	return Instance;
}

TArray<FConversationMessage> UConversationHistoryManager::GetHistory(const FString& AssetPath) const
{
	const TArray<FConversationMessage>* History = ConversationHistories.Find(AssetPath);
	if (History)
	{
		return *History;
	}
	
	return TArray<FConversationMessage>();
}

void UConversationHistoryManager::AddMessage(const FString& AssetPath, const FString& Role, const FString& Content)
{
	TArray<FConversationMessage>& History = ConversationHistories.FindOrAdd(AssetPath);
	
	FConversationMessage Message;
	Message.Role = Role;
	Message.Content = Content;
	Message.Timestamp = FDateTime::Now();
	
	History.Add(Message);
	
	// Auto-save after adding message (if auto-persistence is enabled)
	if (bAutoPersistenceEnabled)
	{
		SaveHistory(AssetPath);
	}
}

void UConversationHistoryManager::ClearHistory(const FString& AssetPath)
{
	ConversationHistories.Remove(AssetPath);
	
	// Delete history file
	FString HistoryFilePath = GetHistoryFilePath(AssetPath);
	if (FPaths::FileExists(HistoryFilePath))
	{
		IFileManager::Get().Delete(*HistoryFilePath);
	}
}

void UConversationHistoryManager::ClearAllHistory()
{
	ConversationHistories.Empty();
	
	// Delete all history files
	FString HistoryDir = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("History");
	if (FPaths::DirectoryExists(HistoryDir))
	{
		IFileManager::Get().DeleteDirectory(*HistoryDir, false, true);
	}
}

bool UConversationHistoryManager::LoadHistory(const FString& AssetPath)
{
	FString HistoryFilePath = GetHistoryFilePath(AssetPath);
	
	if (!FPaths::FileExists(HistoryFilePath))
	{
		return false;
	}
	
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *HistoryFilePath))
	{
		return false;
	}
	
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		return false;
	}
	
	const TArray<TSharedPtr<FJsonValue>>* MessagesArray;
	if (!JsonObject->TryGetArrayField(TEXT("messages"), MessagesArray))
	{
		return false;
	}
	
	TArray<FConversationMessage> History;
	for (const TSharedPtr<FJsonValue>& MessageValue : *MessagesArray)
	{
		TSharedPtr<FJsonObject> MessageObject = MessageValue->AsObject();
		if (!MessageObject.IsValid())
		{
			continue;
		}
		
		FConversationMessage Message;
		MessageObject->TryGetStringField(TEXT("role"), Message.Role);
		MessageObject->TryGetStringField(TEXT("content"), Message.Content);
		
		FString TimestampString;
		if (MessageObject->TryGetStringField(TEXT("timestamp"), TimestampString))
		{
			FDateTime::ParseIso8601(*TimestampString, Message.Timestamp);
		}
		else
		{
			Message.Timestamp = FDateTime::Now();
		}
		
		History.Add(Message);
	}
	
	ConversationHistories.Add(AssetPath, History);
	return true;
}

bool UConversationHistoryManager::SaveHistory(const FString& AssetPath)
{
	const TArray<FConversationMessage>* History = ConversationHistories.Find(AssetPath);
	if (!History)
	{
		return false;
	}
	
	FString HistoryFilePath = GetHistoryFilePath(AssetPath);
	
	// Ensure directory exists
	FString HistoryDir = FPaths::GetPath(HistoryFilePath);
	IFileManager::Get().MakeDirectory(*HistoryDir, true);
	
	// Build JSON object
	TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
	
	TArray<TSharedPtr<FJsonValue>> MessagesArray;
	for (const FConversationMessage& Message : *History)
	{
		TSharedPtr<FJsonObject> MessageObject = MakeShareable(new FJsonObject);
		MessageObject->SetStringField(TEXT("role"), Message.Role);
		MessageObject->SetStringField(TEXT("content"), Message.Content);
		MessageObject->SetStringField(TEXT("timestamp"), Message.Timestamp.ToIso8601());
		
		MessagesArray.Add(MakeShareable(new FJsonValueObject(MessageObject)));
	}
	
	RootObject->SetArrayField(TEXT("messages"), MessagesArray);
	RootObject->SetStringField(TEXT("assetPath"), AssetPath);
	
	// Serialize to string
	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);
	
	// Write to file
	return FFileHelper::SaveStringToFile(JsonString, *HistoryFilePath);
}

void UConversationHistoryManager::SaveAllHistory()
{
	for (const auto& Pair : ConversationHistories)
	{
		SaveHistory(Pair.Key);
	}
}

int32 UConversationHistoryManager::GetHistoryCount(const FString& AssetPath) const
{
	const TArray<FConversationMessage>* History = ConversationHistories.Find(AssetPath);
	return History ? History->Num() : 0;
}

bool UConversationHistoryManager::HasHistory(const FString& AssetPath) const
{
	return ConversationHistories.Contains(AssetPath) || FPaths::FileExists(GetHistoryFilePath(AssetPath));
}

FString UConversationHistoryManager::GetHistoryFilePath(const FString& AssetPath) const
{
	FString Filename = AssetPathToFilename(AssetPath);
	FString HistoryDir = FPaths::ProjectSavedDir() / TEXT("AINiagara") / TEXT("History");
	return HistoryDir / Filename + TEXT(".json");
}

FString UConversationHistoryManager::AssetPathToFilename(const FString& AssetPath) const
{
	// Convert asset path to safe filename
	// Example: "/Game/VFX/FireSystem" -> "Game_VFX_FireSystem"
	FString SafeFilename = AssetPath;
	
	// Remove leading slash
	if (SafeFilename.StartsWith(TEXT("/")))
	{
		SafeFilename.RemoveAt(0);
	}
	
	// Replace slashes and invalid characters with underscores
	SafeFilename.ReplaceCharInline(TEXT('/'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT('\\'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT(':'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT('*'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT('?'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT('"'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT('<'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT('>'), TEXT('_'));
	SafeFilename.ReplaceCharInline(TEXT('|'), TEXT('_'));
	
	return SafeFilename;
}

void UConversationHistoryManager::RegisterAssetEventHooks()
{
	if (!bAutoPersistenceEnabled)
	{
		return;
	}

	// Register for asset saved events
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
	
	// Hook into package saved event
	UPackage::PackageSavedWithContextEvent.AddUObject(this, &UConversationHistoryManager::OnPackageSaved);
	
	UE_LOG(LogTemp, Log, TEXT("AINiagara: Registered asset event hooks for history persistence"));
}

void UConversationHistoryManager::UnregisterAssetEventHooks()
{
	// Unregister package saved event
	UPackage::PackageSavedWithContextEvent.RemoveAll(this);
	
	UE_LOG(LogTemp, Log, TEXT("AINiagara: Unregistered asset event hooks"));
}

void UConversationHistoryManager::SetAutoPersistence(bool bEnable)
{
	if (bAutoPersistenceEnabled == bEnable)
	{
		return;
	}

	bAutoPersistenceEnabled = bEnable;

	if (bEnable)
	{
		RegisterAssetEventHooks();
	}
	else
	{
		UnregisterAssetEventHooks();
	}
}

void UConversationHistoryManager::OnPackageSaved(const FString& PackageFilename, UPackage* Package, FObjectPostSaveContext ObjectSaveContext)
{
	if (!Package || !bAutoPersistenceEnabled)
	{
		return;
	}

	// Check if this package contains a Niagara system
	TArray<UObject*> Objects;
	GetObjectsWithOuter(Package, Objects, false);

	for (UObject* Object : Objects)
	{
		if (UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(Object))
		{
			// Get asset path
			FString AssetPath = NiagaraSystem->GetPathName();
			
			// Save history if it exists
			if (HasHistory(AssetPath))
			{
				if (SaveHistory(AssetPath))
				{
					UE_LOG(LogTemp, Log, TEXT("AINiagara: Auto-saved conversation history for %s"), *AssetPath);
				}
			}
		}
	}
}

void UConversationHistoryManager::OnAssetSaved(const FString& PackageName, UObject* Asset)
{
	if (!Asset || !bAutoPersistenceEnabled)
	{
		return;
	}

	// Check if this is a Niagara system
	if (UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(Asset))
	{
		FString AssetPath = Asset->GetPathName();
		
		// Save history if it exists
		if (HasHistory(AssetPath))
		{
			if (SaveHistory(AssetPath))
			{
				UE_LOG(LogTemp, Log, TEXT("AINiagara: Auto-saved conversation history for %s"), *AssetPath);
			}
		}
	}
}

void UConversationHistoryManager::OnAssetOpened(UObject* Asset)
{
	if (!Asset || !bAutoPersistenceEnabled)
	{
		return;
	}

	// Check if this is a Niagara system
	if (UNiagaraSystem* NiagaraSystem = Cast<UNiagaraSystem>(Asset))
	{
		FString AssetPath = Asset->GetPathName();
		
		// Load history if it exists on disk but not in memory
		if (!ConversationHistories.Contains(AssetPath) && FPaths::FileExists(GetHistoryFilePath(AssetPath)))
		{
			if (LoadHistory(AssetPath))
			{
				UE_LOG(LogTemp, Log, TEXT("AINiagara: Auto-loaded conversation history for %s"), *AssetPath);
			}
		}
	}
}

