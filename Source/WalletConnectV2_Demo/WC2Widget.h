// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WC2Widget.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECTV2_DEMO_API UWC2Widget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* QrPanel;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* StatsPanel;

	UPROPERTY(meta = (BindWidget))
		class UImage* QrImage;

	UPROPERTY(meta = (BindWidget))
		class UImage* Black;

	UPROPERTY(meta = (BindWidget))
		class UButton* ConnectButton;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Address;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Username;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CYBER;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* EGLD;

	virtual void NativeConstruct() override;

	UFUNCTION()
		void CLK();

	class AWC2HUD *WC2HUD;

};
