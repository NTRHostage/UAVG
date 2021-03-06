//NTRHostage

#include "UAVGScriptGraph/Slate/SUAVGScriptGraphPin.h"

void SUAVGScriptGraphPin::Construct(const FArguments& InArgs, class UEdGraphPin* InPin)
{
	check(InPin);
	SetCursor(EMouseCursor::Default);
	bShowLabel = true;
	GraphPinObj = InPin;

	const UEdGraphSchema* Schema = InPin->GetSchema();
	check(Schema);

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SUAVGScriptGraphPin::GetPinBorder)
		.BorderBackgroundColor(this, &SUAVGScriptGraphPin::GetPinColour)
		.OnMouseButtonDown(this, &SUAVGScriptGraphPin::OnPinMouseDown)
		.Cursor(this, &SUAVGScriptGraphPin::GetPinCursor)
		.Padding(FMargin(10.f))
	);
}
