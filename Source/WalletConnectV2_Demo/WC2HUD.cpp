// Fill out your copyright notice in the Description page of Project Settings.


#include "WC2HUD.h"
#include "WC2Widget.h"
#include "Components/CanvasPanel.h"
#include "qrcodegen.hpp" // from https://github.com/nayuki/QR-Code-generator
#include "ImageUtils.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetTextLibrary.h"


void AWC2HUD::BeginPlay()
{
	Super::BeginPlay();
	AddWC2Widget();
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
		InputMode.SetWidgetToFocus(WC2Widget->TakeWidget());
		PlayerController->SetInputMode(InputMode);

	}
}

void AWC2HUD::DrawHUD()
{
	Super::DrawHUD();
}

void AWC2HUD::SetQr(FString string)
{
	WC2Widget->ConnectButton->SetVisibility(ESlateVisibility::Collapsed);
	WC2Widget->QrImage->SetBrushFromTexture(GenerateQrCode(string));
	WC2Widget->QrPanel->SetVisibility(ESlateVisibility::Visible);
}

void AWC2HUD::dissQr(FString Add)
{
	address = Add;
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AWC2HUD::OnResponseReceived);
	Request->SetURL("https://api.multiversx.com/accounts/" + address);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void AWC2HUD::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FString message = Response->GetContentAsString();
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(message);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	egold = JsonObject->GetStringField("balance");
	username = JsonObject->GetStringField("username");
	username = username.LeftChop(7);
	FHttpRequestRef Request2 = FHttpModule::Get().CreateRequest();
	Request2->OnProcessRequestComplete().BindUObject(this, &AWC2HUD::OnResponseReceived2);
	Request2->SetURL("https://api.multiversx.com/accounts/" + address + "/tokens/CYBER-489c1c?extract=balance");
	Request2->SetVerb("GET");
	Request2->ProcessRequest();

}

void AWC2HUD::OnResponseReceived2(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	cyber = Response->GetContentAsString();
	WC2Widget->QrImage->SetBrushFromTexture(nullptr);
	WC2Widget->QrPanel->SetVisibility(ESlateVisibility::Hidden);
	WC2Widget->Black->SetVisibility(ESlateVisibility::Hidden);
	APlayerController* PlayerController = GetOwningPlayerController();
	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
	WC2Widget->StatsPanel->SetVisibility(ESlateVisibility::Visible);
	cyber = cyber.LeftChop(10);
	float cy = FCString::Atof(*cyber);
	cy /= 1e8;
	WC2Widget->CYBER->SetText(UKismetTextLibrary::Conv_FloatToText(cy, HalfToEven));
	egold = egold.LeftChop(10);
	float eg = FCString::Atof(*egold);
	eg /= 1e8;
	WC2Widget->EGLD->SetText(UKismetTextLibrary::Conv_FloatToText(eg, HalfToEven));
	WC2Widget->Username->SetText(FText::FromString(username));
	FString A = address.LeftChop(58);
	FString B = address.RightChop(58);
	WC2Widget->Address->SetText(FText::FromString(A + "..." + B));
}

UTexture2D* AWC2HUD::GenerateQrCode(FString string)
{
	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(TCHAR_TO_UTF8(*string), qrcodegen::QrCode::Ecc::LOW);

	uint8 size = qr.getSize();
	TArray<FColor> pixels;
	pixels.SetNumZeroed(size * size);

	FColor black = FColor::Black;
	FColor white = FColor::White;

	for (uint8 x = 0; x < size; x++)
	{
		for (uint8 y = 0; y < size; y++)
		{
			FColor color = qr.getModule(x, y) ? white : black;
			pixels[x + y * size] = color;
		}
	}

	UTexture2D* texture = UTexture2D::CreateTransient(size, size, EPixelFormat::PF_B8G8R8A8, "QRCode");
	void* data = texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(data, pixels.GetData(), size * size * 4);
	texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	texture->UpdateResource();

	texture->Filter = TextureFilter::TF_Nearest;

	return texture;
}