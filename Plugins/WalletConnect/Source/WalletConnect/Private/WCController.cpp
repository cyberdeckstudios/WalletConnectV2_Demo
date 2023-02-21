// Fill out your copyright notice in the Description page of Project Settings.


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
			UE_LOG(LogTemp, Warning, TEXT("Subid: %s"), *Subid);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Subid:" + Subid);
			Cryptography.GenerateX25519(XPublic, XPrivate);
			UE_LOG(LogTemp, Warning, TEXT("XPublic: %s"), *XPublic);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "XPublic:" + XPublic);
			UE_LOG(LogTemp, Warning, TEXT("XPrivate: %s"), *XPrivate);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "XPrivate:" + XPrivate);
			FString msg;
			UE_LOG(LogTemp, Warning, TEXT("Symkey1: %s"), *Symkey);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Symkey1:" + Symkey);
			Cryptography.Encrypt(JsonRPC.wc_sessionPropose(XPublic), Symkey, msg);
			WCPlayerState->SendMessage(JsonRPC.irn_publish_1100(Topic, msg));
		}
		else
		{
			Subid = JsonObject->GetStringField("result");
			UE_LOG(LogTemp, Warning, TEXT("Subid: %s"), *Subid);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Subid:" + Subid);
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
				UE_LOG(LogTemp, Warning, TEXT("PeerPublic: %s"), *PeerPublic);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "PeerPublic:" + PeerPublic);
				Cryptography.Exchangekeys(XPrivate, PeerPublic, Symkey);
				UE_LOG(LogTemp, Warning, TEXT("Symkey2: %s"), *Symkey);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Symkey2:" + Symkey);
				Connected = true;
				Cryptography.Digest(Symkey, Topic);
				WCPlayerState->SendMessage(JsonRPC.irn_subscribe(Topic));
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
				WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(id, Topic, msg));
				WCHUD->ConnectionDone(Account);
			}
		}
	}
	else if (JsonObject->HasTypedField<EJson::Object>("result"))
	{
		JsonObject = JsonObject->GetObjectField("result");
		TArray<TSharedPtr<FJsonValue>> signatures;
		signatures = JsonObject->GetArrayField("signatures");
		if (signatures.Num() > 1)
		{
			TSharedPtr<FJsonObject> signatureobject = signatures[0]->AsObject();
			FString signature0 = signatureobject->GetStringField("signature");
			signatureobject = signatures[1]->AsObject();
			FString signature1 = signatureobject->GetStringField("signature");
			signatureobject = signatures[2]->AsObject();
			FString signature2 = signatureobject->GetStringField("signature");
			TArray<TSharedPtr<FJsonValue>> TXArr;
			TSharedPtr<FJsonObject> txobject = txarr[0]->AsObject();
			txobject->SetStringField("signature", signature0);
			TXArr.Add(MakeShared<FJsonValueObject>(txobject));
			txobject = txarr[1]->AsObject();
			txobject->SetStringField("signature", signature1);
			TXArr.Add(MakeShared<FJsonValueObject>(txobject));
			txobject = txarr[2]->AsObject();
			txobject->SetStringField("signature", signature2);
			TXArr.Add(MakeShared<FJsonValueObject>(txobject));
			FString txstr;
			const auto Writr = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&txstr);
			FJsonSerializer::Serialize(TXArr, Writr);

			FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
			Request->SetURL("https://devnet-api.multiversx.com/transactions");
			Request->SetHeader("content-type", "application/json");
			Request->SetVerb("POST");
			Request->SetContentAsString(txstr);
			Request->ProcessRequest();
		}
		else
		{
			TSharedPtr<FJsonObject> signatureobject = signatures[0]->AsObject();
			FString signature = signatureobject->GetStringField("signature");
			txobj->SetStringField("signature", signature);
			FString txstr;
			const auto Writr = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&txstr);
			FJsonSerializer::Serialize(txobj, Writr);

			FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
			Request->SetURL("https://api.multiversx.com/transactions");
			Request->SetHeader("content-type", "application/json");
			Request->SetVerb("POST");
			Request->SetContentAsString(txstr);
			Request->ProcessRequest();
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
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(id,Topic,msg));
}

void AWCController::transaction(int nonce)
{
	txarr.Empty();
	txobj = JsonRPC.TxObject(nonce, "1000000000000000", Account, Account, "dGVzdGRhdGE=");
	txarr.Add(MakeShared<FJsonValueObject>(txobj));
	FString msg = JsonRPC.wc_sessionRequest(txarr);
	Cryptography.Encrypt(msg, Symkey, msg);
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(id, Topic, msg));
}

void AWCController::transactionmulti(int nonce)
{
	txarr.Empty();

	txarr.Add(MakeShared<FJsonValueObject>(JsonRPC.TxObject(nonce, "1000000000000000", Account, Account, "dGVzdGRhdGE=")));
	txarr.Add(MakeShared<FJsonValueObject>(JsonRPC.TxObject(nonce+1, "2000000000000000", Account, Account, "")));
	txarr.Add(MakeShared<FJsonValueObject>(JsonRPC.TxObject(nonce+2, "3000000000000000", Account, Account, "dGhpcmQ=")));
	FString msg = JsonRPC.wc_sessionRequest(txarr);
	Cryptography.Encrypt(msg, Symkey, msg);
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	WCPlayerState->SendMessage(JsonRPC.irn_publish_1108(id, Topic, msg));
}

