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

#include "WCController.h"
#include "WCPlayerState.h"
#include "WCHUD.h"

void AWCController::BeginPlay()
{
	WCHUD = Cast<AWCHUD>(GetHUD());
	WCPlayerState = GetPlayerState<AWCPlayerState>();
}

void AWCController::InitilazeConnection()
{
	if (Url.IsEmpty())
	{
		Url = Constructor.GenerateUrl();
	}
	if (WCPlayerState != nullptr)
	{
		WCPlayerState->websocket(Url);
	}
}

UTexture2D* AWCController::CreateQrCode()
{
	WcStr = Constructor.GenerateWCString(Symkey, Topic);
	WCPlayerState->SendMessage(JsonRPC.irn_subscribe(Topic));
	QrOpen = true;
	UE_LOG(LogTemp, Warning, TEXT("WcStr: %s"), *WcStr);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "WcStr:" + WcStr);
	return Constructor.GenerateQrCode(WcStr);
}

void AWCController::wsc()
{
	if (Connected == true)
	{
		WCPlayerState->SendMessage(JsonRPC.irn_subscribe(Topic));
	}
	else if (QrOpen == true)
	{
		WCHUD->Refresh();
	}
}

void AWCController::wse(FString Error)
{
}

void AWCController::wsr(FString Message)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Message);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	Check(JsonObject);
}

void AWCController::wss(FString MessageString)
{
}

void AWCController::wsc(int32 StatusCode)
{
	if (StatusCode == 4010)
	{
		InitilazeConnection();
	}
}

void AWCController::Check(TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject->HasTypedField<EJson::String>("result"))
	{
		if (Connected == false)
		{
			Subid = JsonObject->GetStringField("result");
			Cryptography.GenerateX25519(XPublic, XPrivate);
			FString msg;
			Cryptography.Encrypt(JsonRPC.wc_sessionPropose(XPublic), Symkey, msg);
			WCPlayerState->SendMessage(JsonRPC.irn_publish_1100(Topic, msg));
		}
		else
		{
			Subid = JsonObject->GetStringField("result");
		}
	}
	else if (JsonObject->HasTypedField<EJson::String>("method"))
	{
		int64 idd = JsonObject->GetNumberField("id");
		WCPlayerState->SendMessage(JsonRPC.Acknowledge(idd));
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

void AWCController::DecryptMessage(FString message)
{
	FString Message;
	Cryptography.Decrypt(message, Symkey, Message);
	UE_LOG(LogTemp, Warning, TEXT("Message: %s"), *Message);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Message:" + Message);
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
				Symkey = "";
				Topic = "";
				PeerPublic = JsonObject->GetStringField("responderPublicKey");
				Cryptography.Exchangekeys(XPrivate, PeerPublic, Symkey);
				Connected = true;
				QrOpen = false;
				Cryptography.Digest(Symkey, Topic);
				WCPlayerState->SendMessage(JsonRPC.irn_subscribe(Topic));
			}
		}
		else if (JsonObject->HasTypedField<EJson::Array>("signatures"))
		{
			TArray<TSharedPtr<FJsonValue>> signatures;
			signatures = JsonObject->GetArrayField("signatures");
			if (signatures.Num() > 0)
			{
				TSharedPtr<FJsonObject> signatureobject = MakeShared<FJsonObject>();
				FString signature = "";
				int i = signatures.Num();
				if (txarr.Num() > 0)
				{
					for (uint8 x = 0; x < i; x++)
					{
						signatureobject = signatures[x]->AsObject();
						signature = signatureobject->GetStringField("signature");
						txobj = txarr[x]->AsObject().ToSharedRef();
						txobj->SetStringField("signature", signature);
						FString txstr;
						const auto Writr = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&txstr);
						FJsonSerializer::Serialize(txobj, Writr);
						FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
						Request->SetURL("https://devnet-api.multiversx.com/transactions");
						Request->OnProcessRequestComplete().BindUObject(this, &AWCController::OnResponseReceived);
						Request->SetHeader("content-type", "application/json");
						Request->SetVerb("POST");
						Request->SetContentAsString(txstr);
						Request->ProcessRequest();
					}
				}
			}
		}
		else if (JsonObject->HasTypedField<EJson::String>("signature"))
		{
			FString signature = JsonObject->GetStringField("signature");
			UE_LOG(LogTemp, Warning, TEXT("signature: %s"), *signature);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "signature:" + signature);
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
				WCPlayerState->SendMessage(JsonRPC.irn_publish_1103(Topic, Symkey));
				WCHUD->ConnectionDone(Account);
			}
		}
		else if (JsonObject->HasTypedField<EJson::String>("message"))
		{
			if (JsonObject->GetStringField("message") == "User discconnected")
			{
				WCPlayerState->SendMessage(JsonRPC.irn_publish_1113(Topic, Symkey));
				WCPlayerState->SendMessage(JsonRPC.irn_unsubscribe(Topic,Symkey));
				Connected = false;
				WcStr = "";
				Symkey = "";
				Topic = "";
				Subid = "";
				XPrivate = "";
				XPublic = "";
				PeerPublic = "";
				Account = "";
				WCHUD->disconnect();
			}
		}
	}
}

void AWCController::changeVars(bool qr)
{
	QrOpen = qr;
}

void AWCController::transactionCyber(int nonce)
{
	txarr.Empty();
	txobj = JsonRPC.TxObject(nonce, "0", Account, Account, "RVNEVFRyYW5zZmVyQDQzNTk0MjQ1NTIyZDM0MzgzOTYzMzE2M0AwZGUwYjZiM2E3NjQwMDAwQDc0NjU3Mzc0");
	txarr.Add(MakeShared<FJsonValueObject>(txobj));
	FString msg = JsonRPC.wc_sessionRequest(txarr);
	Cryptography.Encrypt(msg, Symkey, msg);
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(Topic,msg));
}

void AWCController::transaction(int nonce)
{
	txarr.Empty();
	txobj = JsonRPC.TxObject(nonce, "1000000000000000", Account, Account, "dGVzdGRhdGE=");
	txarr.Add(MakeShared<FJsonValueObject>(txobj));
	FString msg = JsonRPC.wc_sessionRequest(txarr);
	Cryptography.Encrypt(msg, Symkey, msg);
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(Topic, msg));
}

void AWCController::transactionmulti(int nonce)
{
	txarr.Empty();

	txarr.Add(MakeShared<FJsonValueObject>(JsonRPC.TxObject(nonce, "1000000000000000", Account, Account, "dGVzdGRhdGE=")));
	txarr.Add(MakeShared<FJsonValueObject>(JsonRPC.TxObject(nonce+1, "2000000000000000", Account, Account, "")));
	txarr.Add(MakeShared<FJsonValueObject>(JsonRPC.TxObject(nonce+2, "3000000000000000", Account, Account, "dGhpcmQ=")));
	FString msg = JsonRPC.wc_sessionRequest(txarr);
	Cryptography.Encrypt(msg, Symkey, msg);
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(Topic, msg));
}

void AWCController::signmessage()
{
	txarr.Empty();
	FString msg = JsonRPC.erd_signMessage(Account);
	Cryptography.Encrypt(msg, Symkey, msg);
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(Topic, msg));
}

void AWCController::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FString res = Response->GetContentAsString();
	UE_LOG(LogTemp, Warning, TEXT("Response: %s"), *res);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Response:" + res);
}

void AWCController::disconnect()
{
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1112(Topic, Symkey));
	WCPlayerState->SendMessage(JsonRPC.irn_unsubscribe(Topic, Symkey));
	Connected = false;
	WcStr ="";
	Symkey = "";
	Topic = "";
	Subid = "";
	XPrivate = "";
	XPublic = "";
	PeerPublic = "";
	Account = "";
	WCHUD->disconnect();
}

