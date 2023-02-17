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

#include "WebRequests.h"
#include "WebSocketsModule.h"

void UWebRequests::Initialize(UDataProcessHelper* DataProcessHelperRef)
{
	DataProcessHelper = DataProcessHelperRef;
}

void UWebRequests::websocket(FString URL)
{
	const FString ServerURL = URL;

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	this->Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL);

	this->Socket->OnConnected().AddLambda([this]()-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Connected to websocket server."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Connected to websocket server.");
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
			DataProcessHelper->wsr(Message);

		});
	this->Socket->OnMessageSent().AddLambda([this](const FString& MessageString)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Sent message to websocket server: \"%s\"."), *MessageString);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Sent message to websocket server:" + MessageString);
		});

	this->Socket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Connection to websocket server has been closed with status code: \"%d\" and reason: \"%s\"."), StatusCode, *Reason);
		});

	this->Socket->Connect();
}

void UWebRequests::SendMessage(FString Message)
{
	this->Socket->Send(Message);
}

void UWebRequests::CloseSocket()
{
	this->Socket->Close();
}
