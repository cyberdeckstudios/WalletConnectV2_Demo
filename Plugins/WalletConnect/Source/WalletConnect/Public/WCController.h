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
#include "GameFramework/PlayerController.h"
#include "Constructor.h"
#include "JsonRPC.h"
#include "Cryptography.h"
#include "Http.h"
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
	void signmessage();
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	void disconnect();
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
	TSharedRef<FJsonObject> txobj = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> txarr;
	bool Connected = false;
	bool QrOpen = false;
};
