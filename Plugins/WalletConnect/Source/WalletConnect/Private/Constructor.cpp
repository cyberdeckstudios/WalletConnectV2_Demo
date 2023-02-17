// Fill out your copyright notice in the Description page of Project Settings.


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

	FColor black = FColor::Black;
	FColor white = FColor::White;

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
