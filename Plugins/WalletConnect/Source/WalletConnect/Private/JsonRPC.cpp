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

#include "JsonRPC.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/Base64.h"

FString JsonRPC::Header()
{
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField("alg", "EdDSA");
	JsonObject->SetStringField("typ", "JWT");
	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);

	OutputString = FBase64::Encode(OutputString);
	OutputString = OutputString.Replace(TEXT("="), TEXT(""));
	return OutputString;
}

FString JsonRPC::Payload(FString ed25519PublicKey)
{
	FString sub;
	Cryptography.RandomKey(sub);
	int64 iat = FDateTime::UtcNow().ToUnixTimestamp();
	int64 exp = iat + 86400;
	FString iss = "ed01" + ed25519PublicKey;
	iss = Base58.Encode(iss);
	iss = "did:key:z" + iss;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField("iss", iss);
	JsonObject->SetStringField("sub", sub);
	JsonObject->SetStringField("aud", "wss://relay.walletconnect.com");
	JsonObject->SetNumberField("iat", iat);
	JsonObject->SetNumberField("exp", exp);
	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);

	OutputString = FBase64::Encode(OutputString);
	OutputString = OutputString.Replace(TEXT("="), TEXT(""));
	return OutputString;
}

FString JsonRPC::irn_subscribe(FString Topic)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetStringField("method", "irn_subscribe");

	TSharedRef<FJsonObject> ParamsObject = MakeShared<FJsonObject>();
	ParamsObject->SetStringField("topic", Topic);
	JsonObject->SetObjectField("params", ParamsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return OutputString;
}

FString JsonRPC::irn_unsubscribe(FString Topic, FString SubId)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetStringField("method", "irn_unsubscribe");

	TSharedRef<FJsonObject> ParamsObject = MakeShared<FJsonObject>();
	ParamsObject->SetStringField("topic", Topic);
	ParamsObject->SetStringField("id", SubId);
	JsonObject->SetObjectField("params", ParamsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return OutputString;
}

FString JsonRPC::wc_sessionPropose(FString X25519PublicKey)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "wc_sessionPropose");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();

	TSharedRef<FJsonObject> requiredNamespacesObject = MakeShared<FJsonObject>();

	TSharedRef<FJsonObject> elrondObject = MakeShared<FJsonObject>();

	TArray<TSharedPtr<FJsonValue>> methodsarray;
	methodsarray.Add(MakeShared<FJsonValueString>("erd_signTransaction"));
	methodsarray.Add(MakeShared<FJsonValueString>("erd_signTransactions"));
	methodsarray.Add(MakeShared<FJsonValueString>("erd_signMessage"));
	methodsarray.Add(MakeShared<FJsonValueString>("erd_signLoginToken"));
	methodsarray.Add(MakeShared<FJsonValueString>("erd_cancelAction"));

	elrondObject->SetArrayField("methods", methodsarray);

	TArray<TSharedPtr<FJsonValue>> chainsarray;
	chainsarray.Add(MakeShared<FJsonValueString>("elrond:D"));

	elrondObject->SetArrayField("chains", chainsarray);

	TArray<TSharedPtr<FJsonValue>> eventssarray;

	elrondObject->SetArrayField("events", eventssarray);

	requiredNamespacesObject->SetObjectField("elrond", elrondObject);

	paramsObject->SetObjectField("requiredNamespaces", requiredNamespacesObject);

	TArray<TSharedPtr<FJsonValue>> relaysArray;

	TSharedRef<FJsonObject> relaysObject = MakeShared<FJsonObject>();
	relaysObject->SetStringField("protocol", "irn");

	relaysArray.Add(MakeShared<FJsonValueObject>(relaysObject));

	paramsObject->SetArrayField("relays", relaysArray);

	TSharedRef<FJsonObject> proposerObject = MakeShared<FJsonObject>();
	proposerObject->SetStringField("publicKey", X25519PublicKey);

	TSharedRef<FJsonObject> metadataObject = MakeShared<FJsonObject>();
	metadataObject->SetStringField("description", "React App for CyberpunkCity");
	metadataObject->SetStringField("url", "https://cyberpunkcity.com");

	TArray<TSharedPtr<FJsonValue>> iconsArray;
	iconsArray.Add(MakeShared<FJsonValueString>("https://cyberpunkcity.com/wp-content/uploads/favicon.ico"));

	metadataObject->SetArrayField("icons", iconsArray);
	metadataObject->SetStringField("name", "Cyberpunk City Metaverse");

	proposerObject->SetObjectField("metadata", metadataObject);

	paramsObject->SetObjectField("proposer", proposerObject);

	jsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

FString JsonRPC::irn_publish_1100(FString Topic, FString EncMsg)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", Topic);
	paramsObject->SetStringField("message", EncMsg);
	paramsObject->SetNumberField("ttl", 300);
	paramsObject->SetBoolField("prompt", true);
	paramsObject->SetNumberField("tag", 1100);

	jsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

FString JsonRPC::Acknowledge(int64 id)
{
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetBoolField("result", true);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return OutputString;
}

FString JsonRPC::irn_publish_1103(FString Topic, FString SharedKey)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", Topic);

	FString Message;

	Cryptography.Encrypt(Acknowledge(id), SharedKey, Message);

	paramsObject->SetStringField("message", Message);
	paramsObject->SetNumberField("ttl", 300);
	paramsObject->SetBoolField("prompt", false);
	paramsObject->SetNumberField("tag", 1103);

	JsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return OutputString;

}
FString JsonRPC::irn_publish_1113(FString Topic, FString SharedKey)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", Topic);

	FString Message;

	Cryptography.Encrypt(Acknowledge(id), SharedKey, Message);

	paramsObject->SetStringField("message", Message);
	paramsObject->SetNumberField("ttl", 300);
	paramsObject->SetBoolField("prompt", false);
	paramsObject->SetNumberField("tag", 1113);

	JsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return OutputString;

}

FString JsonRPC::irn_publish_1108(FString Topic, FString EncMsg)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", Topic);
	paramsObject->SetStringField("message", EncMsg);
	paramsObject->SetNumberField("ttl", 300);
	paramsObject->SetBoolField("prompt", true);
	paramsObject->SetNumberField("tag", 1108);

	jsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

FString JsonRPC::irn_publish_1112(FString Topic, FString SharedKey)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", Topic);

	FString Message;

	Cryptography.Encrypt(wc_sessionDelete(id), SharedKey, Message);

	paramsObject->SetStringField("message", Message);
	paramsObject->SetNumberField("ttl", 300);
	paramsObject->SetBoolField("prompt", false);
	paramsObject->SetNumberField("tag", 1112);

	JsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return OutputString;

}

FString JsonRPC::wc_sessionDelete(int64 id)
{
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "wc_sessionDelete");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();

	paramsObject->SetStringField("message", "User discconnected");
	paramsObject->SetNumberField("code", 6000);

	jsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

TSharedRef<FJsonObject> JsonRPC::TxObject(int Nonce, FString Value, FString Receiver, FString Sender,FString data)
{
	TSharedRef<FJsonObject> transactionsObject = MakeShared<FJsonObject>();
	transactionsObject->SetNumberField("nonce", Nonce);
	transactionsObject->SetStringField("value", Value);
	transactionsObject->SetStringField("receiver", Receiver);
	transactionsObject->SetStringField("sender", Sender);
	transactionsObject->SetNumberField("gasPrice", 1000000000);
	transactionsObject->SetNumberField("gasLimit", 500000);
	transactionsObject->SetStringField("data", data);
	transactionsObject->SetStringField("chainID", "D");
	transactionsObject->SetNumberField("version", 1);

	return transactionsObject;
}

FString JsonRPC::wc_sessionRequest(TArray<TSharedPtr<FJsonValue>> transactionsarray)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "wc_sessionRequest");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();

	TSharedRef<FJsonObject> requestObject = MakeShared<FJsonObject>();
	requestObject->SetStringField("method", "erd_signTransactions");

	TSharedRef<FJsonObject> paramsObject2 = MakeShared<FJsonObject>();

	paramsObject2->SetArrayField("transactions", transactionsarray);

	requestObject->SetObjectField("params", paramsObject2);

	paramsObject->SetObjectField("request", requestObject);
	paramsObject->SetStringField("chainId", "elrond:D");

	jsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

FString JsonRPC::erd_signMessage(FString Adress)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "wc_sessionRequest");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();

	TSharedRef<FJsonObject> requestObject = MakeShared<FJsonObject>();
	requestObject->SetStringField("method", "erd_signMessage");

	TSharedRef<FJsonObject> paramsObject2 = MakeShared<FJsonObject>();

	paramsObject2->SetStringField("address", Adress);
	paramsObject2->SetStringField("message", "Sign this message");

	requestObject->SetObjectField("params", paramsObject2);

	paramsObject->SetObjectField("request", requestObject);
	paramsObject->SetStringField("chainId", "elrond:D");

	jsonObject->SetObjectField("params", paramsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}
