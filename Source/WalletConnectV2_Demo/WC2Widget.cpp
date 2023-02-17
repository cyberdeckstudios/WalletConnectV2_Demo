// Fill out your copyright notice in the Description page of Project Settings.


#include "WC2Widget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "WC2HUD.h"

void UWC2Widget::NativeConstruct()
{
	if (ConnectButton)
	{
		ConnectButton->OnClicked.AddDynamic(this, &UWC2Widget::CLK);
	}
}

void UWC2Widget::CLK()
{
	WC2HUD = WC2HUD == nullptr ? Cast<AWC2HUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WC2HUD;
	WC2HUD->ConnectClicked();
}
