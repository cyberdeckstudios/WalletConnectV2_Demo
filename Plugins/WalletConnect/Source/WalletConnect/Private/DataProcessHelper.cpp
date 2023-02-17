// Fill out your copyright notice in the Description page of Project Settings.


#include "DataProcessHelper.h"
#include "WebRequests.h"

UDataProcessHelper::UDataProcessHelper()
{
	WebRequests = NewObject<UWebRequests>(this, TEXT("WebRequests"), RF_Transient);
	WebRequests->Initialize(this);
}

void UDataProcessHelper::InitilazeConnection()
{
	if (Url.IsEmpty())
	{
		Url = Constructor.GenerateUrl();
	}
	WebRequests->websocket(Url);
}

UTexture2D* UDataProcessHelper::CreateQrCode()
{
	WcStr = Constructor.GenerateWCString(Symkey, Topic);
	WebRequests->SendMessage(JsonRPC.irn_subscribe(Topic));
	return Constructor.GenerateQrCode(WcStr);
}

void UDataProcessHelper::wsc()
{
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

void UDataProcessHelper::wsc(FString Reason)
{
}

void UDataProcessHelper::Check(TSharedPtr<FJsonObject> JsonObject)
{
	if (JsonObject->HasTypedField<EJson::String>("result"))
	{
		if (Subid.IsEmpty())
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

FString UDataProcessHelper::ReturnAccount()
{
	return Account;
}
