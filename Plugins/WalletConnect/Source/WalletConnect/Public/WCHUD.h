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
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Http.h"
#include "WCHUD.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECT_API AWCHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> WCWidgetClass;

	class UWCWidget* WCWidget;

	class AWCController* WCController;

	void ConnectClicked();
	void ConnectionDone(FString A);
	void BlackClicked();
	void Refresh();
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnResponseReceived2(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void OnResponseReceived3(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void getNonce(FString tx);

protected:
	virtual void BeginPlay() override;
	void AddWCWidget();
	FString Url = "";
	FString address = "";
	FString username = "";
	FString egold = "";
	FString cyber = "";
	FString Txtype = "";
};