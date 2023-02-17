// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Cryptography.h"
#include "JsonRPC.h"

/**
 * 
 */
class Constructor
{
public:
	Cryptography Cryptography;
	JsonRPC JsonRPC;

	FString GenerateWCString(FString& Symkey, FString& Topic);
	FString GenerateUrl();
	UTexture2D* GenerateQrCode(FString string);
};
