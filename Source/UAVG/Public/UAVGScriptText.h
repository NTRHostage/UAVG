//NTRHostage

#pragma once

#include "CoreMinimal.h"
#include "UAVGScriptText.generated.h"

UCLASS(Blueprintable, BlueprintType)
class UAVG_API UUAVGScriptText : public UObject
{
	GENERATED_BODY()
public:
	UUAVGScriptText();

	TArray<FString> GetLocalizedScriptLines();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FText> ScriptLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<class UStringTable*> StringTableToSearch;

public:
	/** The filename of the script created from. This may not always exist on disk, as we may have previously loaded and cached the font data inside this asset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString SourceFilename;

#ifdef WITH_EDITOR
public:
	void EditorSetupScriptText(const FString& ScriptString, const FString& FilePath);
#endif
};
