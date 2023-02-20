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

#include "WCHUD.h"
#include "WalletConnect/WCWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetTextLibrary.h"
#include "WCController.h"

void AWCHUD::BeginPlay()
{
	Super::BeginPlay();
	AddWCWidget();
	WCController = Cast<AWCController>(GetOwner());
	WCController->InitilazeConnection();
}

void AWCHUD::AddWCWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && WCWidgetClass)
	{
		WCWidget = CreateWidget<UWCWidget>(PlayerController, WCWidgetClass);
		WCWidget->AddToViewport(-1);
		PlayerController->SetShowMouseCursor(true);
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
	}
}

void AWCHUD::DrawHUD()
{
	Super::DrawHUD();
}

void AWCHUD::ConnectClicked()
{
	WCWidget->ConnectButton->SetVisibility(ESlateVisibility::Collapsed);
	WCWidget->QrImage->SetBrushFromTexture(WCController->CreateQrCode());
	WCWidget->QrPanel->SetVisibility(ESlateVisibility::Visible);
	WCWidget->Black->SetVisibility(ESlateVisibility::Visible);
}

void AWCHUD::ConnectionDone(FString A)
{
	address = A;

	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AWCHUD::OnResponseReceived);
	Request->SetURL("https://api.multiversx.com/accounts/" + address);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void AWCHUD::BlackClicked()
{
	WCWidget->ConnectButton->SetVisibility(ESlateVisibility::Visible);
	WCWidget->QrPanel->SetVisibility(ESlateVisibility::Collapsed);
	WCWidget->Black->SetVisibility(ESlateVisibility::Hidden);
	WCWidget->QrImage->SetBrush(FSlateBrush());
	WCController->changeVars(false);
}

void AWCHUD::Refresh()
{
	WCWidget->QrImage->SetBrushFromTexture(nullptr);
	WCWidget->QrImage->SetBrushFromTexture(WCController->CreateQrCode());
}

void AWCHUD::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FString message = Response->GetContentAsString();
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(message);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	egold = JsonObject->GetStringField("balance");
	username = JsonObject->GetStringField("username");
	username = username.LeftChop(7);
	Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AWCHUD::OnResponseReceived2);
	Request->SetURL("https://api.multiversx.com/accounts/" + address + "/tokens/CYBER-489c1c?extract=balance");
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void AWCHUD::OnResponseReceived2(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	cyber = Response->GetContentAsString();
	WCWidget->QrImage->SetBrushFromTexture(nullptr);
	WCWidget->QrPanel->SetVisibility(ESlateVisibility::Collapsed);
	WCWidget->Black->SetVisibility(ESlateVisibility::Collapsed);
	WCWidget->DisconnectButton->SetVisibility(ESlateVisibility::Visible);
	WCWidget->StatsPanel->SetVisibility(ESlateVisibility::Visible);
	cyber = cyber.LeftChop(10);
	float cy = FCString::Atof(*cyber);
	cy /= 1e8;
	WCWidget->CYBER->SetText(UKismetTextLibrary::Conv_FloatToText(cy, HalfToEven));
	egold = egold.LeftChop(10);
	float eg = FCString::Atof(*egold);
	eg /= 1e8;
	WCWidget->EGLD->SetText(UKismetTextLibrary::Conv_FloatToText(eg, HalfToEven));
	WCWidget->Username->SetText(FText::FromString(username));
	FString A = address.LeftChop(58);
	FString B = address.RightChop(58);
	WCWidget->Address->SetText(FText::FromString(A + "..." + B));
}

void AWCHUD::OnResponseReceived3(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FString NonceAsStr = Response->GetContentAsString();
	int nonce = FCString::Atoi(*NonceAsStr);
	if (Txtype == "cyber")
	{
		WCController->transactionCyber(nonce);
	}
	if (Txtype == "single")
	{
		WCController->transaction(nonce);
	}
	if (Txtype == "multi")
	{
		WCController->transactionmulti(nonce);
	}
}

void AWCHUD::getNonce(FString tx)
{
	Txtype = tx;
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	FHttpResponsePtr Response;
	Request->OnProcessRequestComplete().BindUObject(this, &AWCHUD::OnResponseReceived3);
	Request->SetURL("https://api.multiversx.com/accounts/" + address + "?extract=nonce");
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

