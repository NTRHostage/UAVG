//NTRHostage

#pragma once

#include "CoreMinimal.h"
#include "UAVGText.h"
#include "UAVGComponent.h"
#include "IUAVGActorInterface.generated.h"

USTRUCT(BlueprintType)
struct FUAVGActorLineInfo
{
	GENERATED_BODY()
public:
	FUAVGActorLineInfo()
	{
	}
	FUAVGActorLineInfo(TArray<FUAVGText> InText) : DesiredText(InText)
	{
	}

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "UAVG|Info"))
	TArray<FUAVGText> DesiredText;
};

USTRUCT(BlueprintType)
struct FUAVGActorSelectionInfo
{
	GENERATED_BODY()
public:
	FUAVGActorSelectionInfo()
	{
	}

	FUAVGActorSelectionInfo(TArray<FText> InSelection) : SelectionTexts(InSelection)
	{
	}

	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "UAVG|Info"))
	TArray<FText> SelectionTexts;
};

UINTERFACE(Blueprintable)
class UUAVGActorInterface : public UInterface
{
	GENERATED_BODY()

};

class IUAVGActorInterface
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnRevivedFromSave(class UUAVGSaveGame* SaveObject);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnTextUpdated(int32 Index, const FText& NewText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnNewLine(const FUAVGActorLineInfo& Info);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnFaceSelection(const FUAVGActorSelectionInfo& Info);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnLineComplete();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnScriptComplete();

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void TriggerCustomEvent(const FString& EventName, const TArray<FString>& Arguments);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnEnvironmentDescriptorAdded(const FUAVGEnvironmentDescriptor& Added, const TArray<FUAVGEnvironmentDescriptor>& CurrentEnvironmentDescriptor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnEnvironmentDescriptorRemoved(const TArray<FUAVGEnvironmentDescriptor>& Removed, const TArray<FUAVGEnvironmentDescriptor>& CurrentEnvironmentDescriptor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnEnvironmentDescriptorChanged(const FUAVGEnvironmentDescriptor& Old, const FUAVGEnvironmentDescriptor& New, const TArray<FUAVGEnvironmentDescriptor>& CurrentEnvironmentDescriptor);
};
