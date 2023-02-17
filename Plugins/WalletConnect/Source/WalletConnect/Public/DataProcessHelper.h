// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Constructor.h"
#include "JsonRPC.h"
#include "Cryptography.h"
#include "DataProcessHelper.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECT_API UDataProcessHelper : public UObject
{
	GENERATED_BODY()
private:
	Constructor Constructor;
	JsonRPC JsonRPC;
	Cryptography Cryptography;
	class UWebRequests* WebRequests;
	class AWC2HUD* WC2HUD;
public:
	UDataProcessHelper();
	void InitilazeConnection();
	UTexture2D* CreateQrCode();

	void wsc();
	void wse(FString Error);
	void wsr(FString Message);
	void wss(FString MessageString);
	void wsc(FString Reason);

	void Check(TSharedPtr<FJsonObject> JsonObject);
	void DecryptMessage(FString message);
	FString ReturnAccount();

protected:
	FString Url;
	FString WcStr;
	FString Symkey;
	FString Topic;
	FString Subid;
	FString XPrivate;
	FString XPublic;
	FString PeerPublic;
	FString Account;
};
