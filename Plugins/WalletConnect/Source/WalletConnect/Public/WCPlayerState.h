// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "IWebSocket.h"
#include "WCPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECT_API AWCPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	void websocket(FString URL);
	void SendMessage(FString Message);
	void CloseSocket();
private:
	TSharedPtr<IWebSocket> Socket;
	class AWCController* WCController;
};
