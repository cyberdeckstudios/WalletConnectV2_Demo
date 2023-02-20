// Fill out your copyright notice in the Description page of Project Settings.


#include "WCPlayerState.h"
#include "WCController.h"
#include "WebSocketsModule.h"

void AWCPlayerState::websocket(FString URL)
{
	const FString ServerURL = URL;

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	this->Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL);

	WCController =Cast<AWCController>(GetOwner());

	this->Socket->OnConnected().AddLambda([this]()-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Connected to websocket server."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Connected to websocket server.");
			WCController->wsc();
		});

	this->Socket->OnConnectionError().AddLambda([this](const FString& Error)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to connect to websocket server with error: \"%s\"."), *Error);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Failed to connect to websocket server with error:" + Error);
		});

	this->Socket->OnMessage().AddLambda([this](const FString& Message)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Received message from websocket server: \"%s\"."), *Message);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Received message from websocket server:" + Message);
			WCController->wsr(Message);

		});
	this->Socket->OnMessageSent().AddLambda([this](const FString& MessageString)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Sent message to websocket server: \"%s\"."), *MessageString);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Sent message to websocket server:" + MessageString);

		});

	this->Socket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Connection to websocket server has been closed with status code: \"%d\" and reason: \"%s\"."), StatusCode, *Reason);
			WCController->wsc(StatusCode);
		});

	this->Socket->Connect();
}

void AWCPlayerState::SendMessage(FString Message)
{
	this->Socket->Send(Message);
}

void AWCPlayerState::CloseSocket()
{
	this->Socket->Close();
}
