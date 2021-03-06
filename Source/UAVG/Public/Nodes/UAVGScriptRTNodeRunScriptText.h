//NTRHostage

#pragma once

#include "CoreMinimal.h"
#include "UAVGScriptRTNode.h"
#include "UAVGScriptRTNodeRunScriptText.generated.h"

UCLASS(Blueprintable)
class UAVG_API UUAVGScriptRuntimeNodeRunScriptText : public UUAVGScriptRuntimeNode
{
	GENERATED_BODY()
public:
	UUAVGScriptRuntimeNodeRunScriptText();

	FUAVGScriptRuntimeNodeArriveResponse OnArrive(class UUAVGComponent* InComponent);

	UPROPERTY(BlueprintReadOnly)
	class UUAVGScriptText* ScriptTextAsset;

	virtual void OnCurrentNodeReset(UUAVGComponent* InComponent) override;
	
	virtual UUAVGScriptRuntimeNode* GetNextNode(class UUAVGComponent* InComponent = nullptr) override;

	void WarpUAVGSaveGame(class UUAVGComponent* InComponent, class UUAVGSaveGame* InSave) override;
	bool UnWarpUAVGSaveGame(class UUAVGComponent* InComponent, class UUAVGSaveGame* InSave) override;
protected:
	UPROPERTY(BlueprintReadOnly)
	TMap<class UUAVGComponent*, class UUAVGTextScriptInterpreter*> Interpreters;
};