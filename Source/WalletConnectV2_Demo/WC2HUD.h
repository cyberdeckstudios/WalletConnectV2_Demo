// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Http.h"
#include "WC2HUD.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECTV2_DEMO_API AWC2HUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> WC2WidgetClass;

	class UWC2Widget* WC2Widget;
	void SetQr(FString string);
	void dissQr(FString Add);
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnResponseReceived2(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	FString address = "";
	FString username = "";
	FString egold = "";
	FString cyber = "";

protected:
	virtual void BeginPlay() override;
	void AddWC2Widget();

	UTexture2D* GenerateQrCode(FString string);	
};
