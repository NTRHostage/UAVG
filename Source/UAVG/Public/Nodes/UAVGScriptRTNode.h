//NTRHostage

#pragma once

#include "CoreMinimal.h"
#include "UAVGText.h"
#include "UAVGScriptRTNode.generated.h"

UENUM(BlueprintType)
enum class EUAVGRuntimeNodeType : uint8
{
	URNT_NULL,
	URNT_Root,
	URNT_Say,
	URNT_CustomEvent,
	URNT_EnvironmentDescriptor,
	URNT_RunSubScript,
	URNT_Selection,
	URNT_SetWhiteboardVar,
	URNT_MAX,
};

struct FUAVGScriptRuntimeNodeArriveResponse
{
public:
	FUAVGScriptRuntimeNodeArriveResponse()
	{
		NodeType = EUAVGRuntimeNodeType::URNT_NULL;
	}
	FUAVGScriptRuntimeNodeArriveResponse(EUAVGRuntimeNodeType Type)
	{
		NodeType = Type;
	}
	EUAVGRuntimeNodeType NodeType = EUAVGRuntimeNodeType::URNT_NULL;

	/// For Say Node
	
	TArray<FUAVGText> DesiredTexts;

	/// For Event Node
	
	FString EventName;
	TArray<FString> EventArguments;

	/// For EnvAdd Node
	
	FString EnvironmentToAdd;
	TArray<FString> AdditonalEnvironmentArguments;

	/// For EnvRm Node
	
	TArray<FString> EnvironmentsToRemove;

	/// For SubScript Node
	
	class UUAVGScript* SubScriptToRun;

	/// For Selection Node
	
	TArray<FText> SelectionTexts;

	/// For Set Whiteboard Variable Node

	FString Key;
	FString Value;
};

UCLASS(Abstract, BlueprintType)
class UAVG_API UUAVGScriptRuntimeNode : public UObject
{
	GENERATED_BODY()
public:
	UUAVGScriptRuntimeNode();

	virtual FUAVGScriptRuntimeNodeArriveResponse OnArrive(class UUAVGComponent* InComponent)
	{
		return FUAVGScriptRuntimeNodeArriveResponse();
	}

	virtual void OnLeave(class UUAVGComponent* InComponent){}

	virtual void OnCurrentNodeReset(class UUAVGComponent* InComponent){}
	
	virtual UUAVGScriptRuntimeNode* GetNextNode(class UUAVGComponent* InComponent = nullptr);

	virtual void WarpUAVGSaveGame(class UUAVGComponent* InComponent, class UUAVGSaveGame* InSave){}
	virtual bool UnWarpUAVGSaveGame(class UUAVGComponent* InComponent, class UUAVGSaveGame* InSave) { return true; }

#ifdef WITH_EDITOR
	/*Called When Editor Saving Graph*/
	virtual bool SetChild(TArray<UUAVGScriptRuntimeNode*> InChildNodes);
	virtual bool SetParent(TArray<UUAVGScriptRuntimeNode*> InParentNodes);
	bool CheckNodeCanBePaentOrChild(UUAVGScriptRuntimeNode* InNode) const;
	/*End Called When Editor Saving Graph*/
#endif // WITH_EDITOR
public:
	UPROPERTY()
	TArray<UUAVGScriptRuntimeNode*> MyParentNodes;
	UPROPERTY()
	TArray<UUAVGScriptRuntimeNode*> MyChildNodes;
};