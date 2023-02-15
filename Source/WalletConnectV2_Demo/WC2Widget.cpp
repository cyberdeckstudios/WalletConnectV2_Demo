// Fill out your copyright notice in the Description page of Project Settings.


#include "WC2Widget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "WalletConnectV2_DemoCharacter.h"

void UWC2Widget::NativeConstruct()
{
	if (ConnectButton)
	{
		ConnectButton->OnClicked.AddDynamic(this, &UWC2Widget::CLK);
	}
}

void UWC2Widget::CLK()
{
	ConnectButton->SetVisibility(ESlateVisibility::Collapsed);
	CC = CC == nullptr ? Cast<AWalletConnectV2_DemoCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)) : CC;
	CC->SoitBeg();
}
