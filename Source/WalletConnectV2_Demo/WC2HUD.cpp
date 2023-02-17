// Fill out your copyright notice in the Description page of Project Settings.


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
