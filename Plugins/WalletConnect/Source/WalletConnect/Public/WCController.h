// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Constructor.h"
#include "JsonRPC.h"
#include "Cryptography.h"
#include "WCController.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECT_API AWCController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	void InitilazeConnection();
	UTexture2D* CreateQrCode();

	void wsc();
	void wse(FString Error);
	void wsr(FString Message);
	void wss(FString MessageString);
	void wsc(int32 StatusCode);

	void Check(TSharedPtr<FJsonObject> JsonObject);
	void DecryptMessage(FString message);
	void changeVars(bool qr);
	void transactionCyber(int nonce);
	void transaction(int nonce);
	void transactionmulti(int nonce);
private:
	Constructor Constructor;
	JsonRPC JsonRPC;
	Cryptography Cryptography;
	class AWCHUD* WCHUD;
	class AWCPlayerState* WCPlayerState;
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
	TSharedRef<FJsonObject> txobj;
	TArray<TSharedPtr<FJsonValue>> txarr;
	bool Connected = false;
	bool QrOpen = false;
};
