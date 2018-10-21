//NTRHostage Coded

#include "AssetEditor_UAVGScript.h"

#include "EdGraph_UAVGScript.h"
#include "AssetGraphSchema_UAVGScript.h"
#include "UAVGScript.h"
#include "UAVGScriptGraphNodeRoot.h"

#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "SUAVGScriptGraphPalette.h"

#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "ModuleManager.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ScopedTransaction.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "Widgets/Docking/SDockTab.h"
#include "PropertyEditorModule.h"
#include "Framework/Application/SlateApplication.h"
#include "EditorStyleSet.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "AssetEditor_GenericGraph"
DEFINE_LOG_CATEGORY(LogUAVGScriptAssetEditor);

const FName UAVGScriptEditorAppName = FName(TEXT("UAVGScriptEditorApp"));
const FName FAssetEditor_UAVGScrpit::GraphCanvasTabId(TEXT("UAVGScriptEditor_GraphCanvas"));
const FName FAssetEditor_UAVGScrpit::PropertiesTabId(TEXT("UAVGScriptEditor_Properties"));
const FName FAssetEditor_UAVGScrpit::PaletteTabId(TEXT("UAVGScriptEditor_Palette"));

FAssetEditor_UAVGScrpit::FAssetEditor_UAVGScrpit()
	:EditingScript(nullptr)
{
	GEditor->RegisterForUndo(this);
}

FAssetEditor_UAVGScrpit::~FAssetEditor_UAVGScrpit()
{
	GEditor->UnregisterForUndo(this);
}

void FAssetEditor_UAVGScrpit::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_UAVGScriptEditor", "UAVG Script Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(GraphCanvasTabId, FOnSpawnTab::CreateSP(this, &FAssetEditor_UAVGScrpit::SpawnTab_GraphCanvas))
		.SetDisplayName(LOCTEXT("GraphCanvasTab", "Graph"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FAssetEditor_UAVGScrpit::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(PaletteTabId, FOnSpawnTab::CreateSP(this, &FAssetEditor_UAVGScrpit::SpawnTab_Palette))
		.SetDisplayName(LOCTEXT("PaletteTab", "Palette"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Tabs.Palette"));
}

void FAssetEditor_UAVGScrpit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(GraphCanvasTabId);
	InTabManager->UnregisterTabSpawner(PropertiesTabId);
	InTabManager->UnregisterTabSpawner(PaletteTabId);
}

void FAssetEditor_UAVGScrpit::SetDetailsViewTarget(class UObject* Target) const
{
	if (DetailsView.IsValid())
	{
		DetailsView->SetObject(Target, true /*What's this for?*/);
	}
}

void FAssetEditor_UAVGScrpit::OnSelectionChanged(const FGraphPanelSelectionSet& SelectionSet) const
{
	if (SelectionSet.Array().Num() <= 0)
	{
		SetDetailsViewTarget(CastChecked<UObject>(CastChecked<UEdGraph_UAVGScript>(EditingScript->MyEdGraph)->GetRootNode()));
	}
	else
	{
		SetDetailsViewTarget(SelectionSet.Array()[0]);
	}
}

void FAssetEditor_UAVGScrpit::InitUAVGScriptAssetEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UUAVGScript* InScrpit)
{
	EditingScript = InScrpit;

	EditingScript->SetFlags(RF_Transactional);

	TryCreateEditorGraph();

	FGraphEditorCommands::Register();
	BindEditorCommands();

	CreateInternalWidgets();
	
	TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_UAVGScriptEditor_Layout_v2")->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewStack()
			->SetSizeCoefficient(0.1f)
			->SetHideTabWell(true)
			->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
		)
		->Split
		(
			FTabManager::NewSplitter()
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.2f)
				->AddTab(PropertiesTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.8f)
				->AddTab(GraphCanvasTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.2f)
				->AddTab(PaletteTabId, ETabState::OpenedTab)
			)
		)
	);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, UAVGScriptEditorAppName, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, InScrpit, false);
}

void FAssetEditor_UAVGScrpit::PostUndo(bool bSuccess)
{
	if (bSuccess)
	{
		RefreshUAVGEditor();
	}
}

void FAssetEditor_UAVGScrpit::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(EditingScript);
}

void FAssetEditor_UAVGScrpit::TryCreateEditorGraph()
{
	if (EditingScript->MyEdGraph == nullptr)
	{
		EditingScript->MyEdGraph = CastChecked<UEdGraph_UAVGScript>(FBlueprintEditorUtils::CreateNewGraph(EditingScript, NAME_None, UEdGraph_UAVGScript::StaticClass(), UAssetGraphSchema_UAVGScript::StaticClass()));
		EditingScript->MyEdGraph->bAllowDeletion = false;
		const UEdGraphSchema* Schema = EditingScript->MyEdGraph->GetSchema();
		Schema->CreateDefaultNodesForGraph(*(EditingScript->MyEdGraph));
	}
}

void FAssetEditor_UAVGScrpit::BindEditorCommands()
{
	if (GraphEditorCommands.IsValid())
		return;
	GraphEditorCommands = MakeShareable(new FUICommandList);

	GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FAssetEditor_UAVGScrpit::OnCommandDeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FAssetEditor_UAVGScrpit::CanDeleteNodes));
}

void FAssetEditor_UAVGScrpit::OnCommandDeleteSelectedNodes() const
{
	if (!GraphEditor.IsValid() || EditingScript == nullptr)
		return;

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());
	GraphEditor->GetCurrentGraph()->Modify();
	TSet<UObject*> SelectedNodes = GraphEditor->GetSelectedNodes();
	GraphEditor->ClearSelectionSet();

	UUAVGScriptGraphNode* NodeObj = nullptr;
	for (UObject* Obj : SelectedNodes)
	{
		NodeObj = CastChecked<UUAVGScriptGraphNode>(Obj);
		if (NodeObj == nullptr || !NodeObj->CanUserDeleteNode())
			continue;

		NodeObj->Modify();

		const UEdGraphSchema* Schema = NodeObj->GetSchema();
		if (Schema != nullptr)
		{
			Schema->BreakNodeLinks(*NodeObj);
		}

		NodeObj->DestroyNode();
	}

	RefreshUAVGEditor();
}

bool FAssetEditor_UAVGScrpit::CanDeleteNodes() const
{
	if (!GraphEditor.IsValid() || EditingScript == nullptr)
		return false;

	const TSet<UObject*>& SelectedNodes = GraphEditor->GetSelectedNodes();
	
	UUAVGScriptGraphNode* NodeObj = nullptr;
	for (UObject* Obj : SelectedNodes)
	{
		NodeObj = Cast<UUAVGScriptGraphNode>(Obj);
		
		if (NodeObj != nullptr && NodeObj->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

FName FAssetEditor_UAVGScrpit::GetToolkitFName() const
{
	return FName("FUAVGScriptEditor");
}

FText FAssetEditor_UAVGScrpit::GetToolkitName() const
{
	const bool bDirtyState = EditingScript->GetOutermost()->IsDirty();

	FFormatNamedArguments Args;
	Args.Add(TEXT("UAVGScriptName"), FText::FromString(EditingScript->GetName()));
	Args.Add(TEXT("DirtyState"), bDirtyState ? FText::FromString(TEXT("*")) : FText::GetEmpty());
	return FText::Format(LOCTEXT("UAVGScriptToolkitName", "{UAVGScriptName}{DirtyState}"), Args);
}

FText FAssetEditor_UAVGScrpit::GetBaseToolkitName() const
{
	return LOCTEXT("UAVGScriptEditorAppLabel", "UAVG Script Editor");
}

FLinearColor FAssetEditor_UAVGScrpit::GetWorldCentricTabColorScale() const
{
	return FLinearColor::White;
}

FString FAssetEditor_UAVGScrpit::GetWorldCentricTabPrefix() const
{
	return TEXT("UAVGScriptEditor");
}

void FAssetEditor_UAVGScrpit::SaveAsset_Execute()
{
	if (EditingScript && EditingScript->MyEdGraph)
	{
		RebuildRuntimeScript();
	}
	FAssetEditorToolkit::SaveAsset_Execute();
}

void FAssetEditor_UAVGScrpit::CreateInternalWidgets()
{
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	const FDetailsViewArgs DetailsViewArgs(false, false, true, FDetailsViewArgs::ObjectsUseNameArea, false);
	DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(CastChecked<UEdGraph_UAVGScript>(EditingScript->MyEdGraph)->GetRootNode());

	Palette = SNew(SUAVGScriptGraphPalette);

	GraphEditor = CreateGraphEditorWidget();
}

TSharedRef<SGraphEditor> FAssetEditor_UAVGScrpit::CreateGraphEditorWidget()
{
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText_UAVGScriptGraph", "UAVG SCRIPT");
	AppearanceInfo.InstructionText = LOCTEXT("AppearanceInstructionText_UAVGScriptGraph", "Right Click to Add Nodes");

	SGraphEditor::FGraphEditorEvents InEvents;
	InEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(this, &FAssetEditor_UAVGScrpit::OnSelectionChanged);
	//InEvents.OnCreateActionMenu = SGraphEditor::FOnCreateActionMenu::CreateSP(this, &FAssetEditor_UAVGScrpit::OnCreateGraphActionMenu);

	return SNew(SGraphEditor)
		.AdditionalCommands(GraphEditorCommands)
		.IsEditable(true)
		.Appearance(AppearanceInfo)
		.GraphToEdit(EditingScript->MyEdGraph)
		.GraphEvents(InEvents)
		.ShowGraphStateOverlay(false);
}

void FAssetEditor_UAVGScrpit::RebuildRuntimeScript()
{
	if (EditingScript && EditingScript->MyEdGraph && EditingScript->MyEdGraph->IsA(UEdGraph_UAVGScript::StaticClass()))//TODO Error Message
	{
		UEdGraph_UAVGScript* ScriptGraph = CastChecked<UEdGraph_UAVGScript>(EditingScript->MyEdGraph);
		ScriptGraph->RebulidRuntimeScript();
	}
}

void FAssetEditor_UAVGScrpit::RefreshUAVGEditor() const
{
	if (GraphEditor.IsValid())
	{
		GraphEditor->ClearSelectionSet();
		GraphEditor->NotifyGraphChanged();
		SetDetailsViewTarget(CastChecked<UEdGraph_UAVGScript>(EditingScript->MyEdGraph)->GetRootNode());
		FSlateApplication::Get().DismissAllMenus();
	}
}

TSharedRef<SDockTab> FAssetEditor_UAVGScrpit::SpawnTab_GraphCanvas(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == GraphCanvasTabId);

	return SNew(SDockTab)
		.Label(LOCTEXT("UAVGScriptEditorGraphCanvasTitle", "Graph"))
		[
			GraphEditor.ToSharedRef()
		];
}
TSharedRef<SDockTab> FAssetEditor_UAVGScrpit::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Details"))
		.Label(LOCTEXT("UAVGScriptEditorPropertiesTitle", "Details"))
		[
			DetailsView.ToSharedRef()
		];
}
TSharedRef<SDockTab> FAssetEditor_UAVGScrpit::SpawnTab_Palette(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PaletteTabId);

	return SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("Kismet.Tabs.Palette"))
		.Label(LOCTEXT("UAVGScriptEditorGraphPaletteTitle", "Palette"))
		[
			Palette.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE