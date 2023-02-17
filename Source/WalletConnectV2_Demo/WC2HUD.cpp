/*
 * Copyright (c) 2023 CYBERDECK Studios. (MIT License)
 * https://github.com/cyberdeck-studios/WalletConnectV2_Demo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */

#include "WC2HUD.h"
#include "WC2Widget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetTextLibrary.h"
#include "DataProcessHelper.h"


void AWC2HUD::BeginPlay()
{
	Super::BeginPlay();
	AddWC2Widget();
	DataProcessHelper = NewObject<UDataProcessHelper>();
	DataProcessHelper->InitilazeConnection();
}

void AWC2HUD::AddWC2Widget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && WC2WidgetClass)
	{
		WC2Widget = CreateWidget<UWC2Widget>(PlayerController, WC2WidgetClass);
		WC2Widget->AddToViewport(-1);
		PlayerController->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
	}
}

void AWC2HUD::DrawHUD()
{
	Super::DrawHUD();
}

void AWC2HUD::ConnectClicked()
{
	WC2Widget->ConnectButton->SetVisibility(ESlateVisibility::Collapsed);
	WC2Widget->QrImage->SetBrushFromTexture(DataProcessHelper->CreateQrCode());
	WC2Widget->QrPanel->SetVisibility(ESlateVisibility::Visible);
	AWC2HUD::ConnectionDone();
}

void AWC2HUD::ConnectionDone()
{
	address = DataProcessHelper->ReturnAccount();
	if (address.IsEmpty())
	{
		ConnectionDone();
	}

}
