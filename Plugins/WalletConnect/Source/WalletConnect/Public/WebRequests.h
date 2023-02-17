// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IWebSocket.h"
#include "DataProcessHelper.h"
#include "WebRequests.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECT_API UWebRequests : public UObject
{
	GENERATED_BODY()
public:
	void Initialize(UDataProcessHelper* DataProcessHelperRef);
	void websocket(FString URL);
	void SendMessage(FString Message);
	void CloseSocket();
private:
	TSharedPtr<IWebSocket> Socket;
	UDataProcessHelper *DataProcessHelper;
};
