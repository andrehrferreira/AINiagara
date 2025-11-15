// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "UObject/GCObject.h"

/**
 * This is the module definition for the editor mode. You can implement custom functionality
 * as your plugin module starts up and shuts down. See IModuleInterface for more extensibility options.
 */
class FAINiagaraModule : public IModuleInterface, public FGCObject
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/** FGCObject implementation */
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
#if (ENGINE_MAJOR_VERSION == 5)
	virtual FString GetReferencerName() const override;
#endif

protected:

	/**
	 * Run some initializations after the Engine has been initialized.
	 */
	void OnPostEngineInit();

private:

	/**
	 * Returns true if the chat window can be spawned.
	 */
	bool CanSpawnChatWindow();

	/**
	 * Spawns chat window and returns a ref of the DockTab to which the window
	 * has been pinned.
	 */
	TSharedRef<class SDockTab> SpawnChatWindow(const class FSpawnTabArgs& Args);

	/**
	 * Checks if the chat window is spawned.
	 */
	bool IsChatWindowSpawned();

	/**
	 * Invokes spawning chat window from the command.
	 */
	void InvokeChatWindowSpawn();

	/** DockTab reference with the chat window. */
	TWeakPtr<class SDockTab> ChatWindowTab;

	/** Handler for an OnPostEngineInit delegate. */
	FDelegateHandle OnPostEngineInitDelegateHandle;

	/** Tab ID for the chat window */
	static const FName ChatWindowTabId;
};
