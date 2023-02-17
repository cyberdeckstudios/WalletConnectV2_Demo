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

#include "Base58.h"
#include "Math/BigInt.h"

FString Base58::Encode(const FString Str)
{
	TArray<uint8> Bytes;
	Bytes.SetNum(Str.Len() / 2);
	int32 Total = HexToBytes(Str, Bytes.GetData());
	Bytes.SetNum(Total);

	const int BASE58 = 58;
	const FString b58chars = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
	FString Base58 = "";
	int512 IntData = 0;
	for (int i = 0; i < Bytes.Num(); i++)
	{
		IntData = IntData * 256 + Bytes[i];
	}

	while (IntData >= BASE58)
	{
		int512 ReminderBig = IntData % BASE58;
		int64 Reminder = ReminderBig.ToInt();
		Base58 += b58chars[Reminder];
		IntData /= BASE58;
	}
	int512 RB = IntData % BASE58;
	int64 R = RB.ToInt();
	Base58 += b58chars[R];
	for (int i = 0; i < Bytes.Num(); i++)
	{
		if (Bytes[i] == 0)
		{
			Base58 = "1" + Base58;
		}
		else
		{
			break;
		}
	}
	return Base58.Reverse();
}
