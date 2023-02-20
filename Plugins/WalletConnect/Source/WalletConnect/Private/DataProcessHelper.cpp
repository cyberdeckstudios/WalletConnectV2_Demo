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

#include "DataProcessHelper.h"
#include "WebRequests.h"
#include "WCHUD.h"


UDataProcessHelper::UDataProcessHelper()
{
	Url="";
}

void UDataProcessHelper::Initialize(AWCHUD* WCHUDRef)
{
	WCHUD = WCHUDRef;
	WebRequests = NewObject<UWebRequests>(this, TEXT("WebRequests"), RF_Transient);
	WebRequests->Initialize(this);
}

void UDataProcessHelper::InitilazeConnection()
{
	if (Url.IsEmpty())
	{
		Url = Constructor.GenerateUrl();
	}
	Url = "wss://" + Url;
	WebRequests->websocket(Url);
	WCHUD->ConnectionDone(Url);
}

UTexture2D* UDataProcessHelper::CreateQrCode()
{
	WcStr = "";
	Symkey = "";
	Topic = "";
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "a");
	WcStr = Constructor.GenerateWCString(Symkey, Topic);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "b");
	WebRequests->SendMessage(JsonRPC.irn_subscribe(Topic));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "c");
	return Constructor.GenerateQrCode(WcStr);
}

void UDataProcessHelper::wsc()
{
	if (Connected == true)
	{
		WebRequests->SendMessage(JsonRPC.irn_subscribe(Topic));
	}
	else if (QrOpen ==true)
	{
		WCHUD->Refresh();
	}
}

void UDataProcessHelper::wse(FString Error)
{
}

void UDataProcessHelper::wsr(FString Message)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Message);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	Check(JsonObject);
}

void UDataProcessHelper::wss(FString MessageString)
{
}

void UDataProcessHelper::wsc(int32 StatusCode)
{
	if (StatusCode == 4010)
	{
		InitilazeConnection();
	}
}

void UDataProcessHelper::Check(TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject->HasTypedField<EJson::String>("result"))
	{
		if (Connected == false)
		{
			Subid = JsonObject->GetStringField("result");
			Cryptography.GenerateX25519(XPublic, XPrivate);
			WebRequests->SendMessage(JsonRPC.irn_publish_1100(Topic, JsonRPC.wc_sessionPropose(XPublic)));
		}
		else
		{
			Subid = JsonObject->GetStringField("result");
		}
	}
	else if (JsonObject->HasTypedField<EJson::String>("method"))
	{
		int64 idd = JsonObject->GetNumberField("id");
		WebRequests->SendMessage(JsonRPC.Acknowledge(idd));
		if (JsonObject->HasTypedField<EJson::Object>("params"))
		{
			JsonObject = JsonObject->GetObjectField("params");
			if (JsonObject->HasTypedField<EJson::Object>("data"))
			{
				JsonObject = JsonObject->GetObjectField("data");
				if (JsonObject->HasTypedField<EJson::String>("message"))
				{
					FString message = JsonObject->GetStringField("message");
					DecryptMessage(message);
				}
			}
		}
	}
}

void UDataProcessHelper::DecryptMessage(FString message)
{
	FString Message;
	Cryptography.Decrypt(message, Symkey, Message);
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Message);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	if (JsonObject->HasTypedField<EJson::Object>("result"))
	{
		JsonObject = JsonObject->GetObjectField("result");
		if (JsonObject->HasTypedField<EJson::String>("responderPublicKey"))
		{
			if (PeerPublic.IsEmpty())
			{
				PeerPublic = JsonObject->GetStringField("responderPublicKey");
				Cryptography.Exchangekeys(XPrivate, PeerPublic, Symkey);
				Cryptography.Digest(Symkey, Topic);
				WebRequests->SendMessage(JsonRPC.irn_subscribe(Topic));
			}
		}
	}
	else if (JsonObject->HasTypedField<EJson::Object>("params"))
	{
		JsonObject = JsonObject->GetObjectField("params");
		if (JsonObject->HasTypedField<EJson::Object>("namespaces"))
		{
			JsonObject = JsonObject->GetObjectField("namespaces");
			if (JsonObject->HasTypedField<EJson::Object>("elrond"))
			{
				JsonObject = JsonObject->GetObjectField("elrond");
				TArray<TSharedPtr<FJsonValue>> accounts;
				accounts = JsonObject->GetArrayField("accounts");
				Account = accounts[0]->AsString().RightChop(9);
				int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
				FString msg = JsonRPC.Acknowledge(id);
				Cryptography.Encrypt(msg, Symkey, msg);
				WebRequests->SendMessage(JsonRPC.irn_publish_1108(id,Topic,msg));
			}
		}
	}
}

void UDataProcessHelper::printVars()
{
	UE_LOG(LogTemp, Warning, TEXT("Url:"), *Url);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Url:" + Url);
	UE_LOG(LogTemp, Warning, TEXT("WcStr:"), *WcStr);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "WcStr:" + WcStr);
	UE_LOG(LogTemp, Warning, TEXT("Symkey:"), *Symkey);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Symkey:" + Symkey);
	UE_LOG(LogTemp, Warning, TEXT("Topic:"), *Topic);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Topic:" + Topic);
}
