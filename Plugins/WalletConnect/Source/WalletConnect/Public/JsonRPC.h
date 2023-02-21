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
#include "Base58.h"
#include "Cryptography.h"
/**
 * 
 */
class JsonRPC
{
public:
	Base58 Base58;
	Cryptography Cryptography;

	FString Header();
	FString Payload(FString ed25519PublicKey);
	FString irn_subscribe(FString Topic);
	FString irn_unsubscribe(FString Topic, FString SubId);
	FString wc_sessionPropose(FString X25519PublicKey);
	FString irn_publish_1100(FString Topic, FString EncMsg);
	FString Acknowledge(int64 id);
	FString irn_publish_1103(FString Topic, FString SharedKey);
	FString irn_publish_1113(FString Topic, FString SharedKey);
	FString irn_publish_1108(FString Topic, FString EncMsg);
	FString irn_publish_1112(FString Topic, FString SharedKey);
	FString wc_sessionDelete(int64 id);
	TSharedRef<FJsonObject> TxObject(int Nonce, FString Value, FString Receiver, FString Sender, FString data);
	FString wc_sessionRequest(TArray<TSharedPtr<FJsonValue>> transactionsarray);
	FString erd_signMessage(FString Adress);
};
