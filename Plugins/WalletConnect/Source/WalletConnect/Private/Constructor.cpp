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

#include "Constructor.h"
#include "QR/qrcodegen.hpp" // from https://github.com/nayuki/QR-Code-generator
#include "ImageUtils.h"

FString Constructor::GenerateWCString(FString& Symkey, FString& Topic)
{
	Cryptography.RandomKey(Symkey);
	Cryptography.Digest(Symkey , Topic);

	FString wcstr = "wc:" + Topic + "@2?relay-protocol=irn&symKey=" + Symkey;
	return wcstr;
}

FString Constructor::GenerateUrl()
{
	FString Public;
	FString Private;
	Cryptography.GenerateEd25519(Public, Private);
	FString header = JsonRPC.Header();
	FString payload = JsonRPC.Payload(Public);
	FString Message = header + "." + payload;
	FString Signature;
	Cryptography.Sign(Message, Private, Signature);
	FString Url = "wss://relay.walletconnect.com/?auth=" + Message + "." + Signature + "&projectId=dd1f7d13256af05e81dd66e669a66a26&ua=wc-2/Cyberdeck";
	return Url;
}

UTexture2D* Constructor::GenerateQrCode(FString string)
{
	qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(TCHAR_TO_UTF8(*string), qrcodegen::QrCode::Ecc::LOW);

	uint8 size = qr.getSize();
	TArray<FColor> pixels;
	pixels.SetNumZeroed(size * size);

	FColor black = FColor::White;
	FColor white = FColor::Black;

	for (uint8 x = 0; x < size; x++)
	{
		for (uint8 y = 0; y < size; y++)
		{
			FColor color = qr.getModule(x, y) ? white : black;
			pixels[x + y * size] = color;
		}
	}

	UTexture2D* texture = UTexture2D::CreateTransient(size, size, EPixelFormat::PF_B8G8R8A8, "QRCode");
	void* data = texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(data, pixels.GetData(), size * size * 4);
	texture->GetPlatformData()->Mips[0].BulkData.Unlock();
	texture->UpdateResource();

	texture->Filter = TextureFilter::TF_Nearest;

	return texture;
}
