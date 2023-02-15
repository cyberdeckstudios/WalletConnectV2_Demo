// Copyright Epic Games, Inc. All Rights Reserved.

#include "WalletConnectV2_DemoCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Misc/SecureHash.h"
#include "Misc/Crc.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/FileHelper.h"
#include "Misc/Base64.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Containers/StringConv.h"
#include "Containers/Array.h"
#include "Containers/UnrealString.h"
#include "Math/BigInt.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "WC2HUD.h"
#include "GameFramework/PlayerController.h"
#include "Http.h"

#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/sha.h"
#include "openssl/md5.h"
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/err.h"
#include "openssl/aes.h"
#include "openssl/rand.h"
#include <openssl/kdf.h>
THIRD_PARTY_INCLUDES_END
#undef UI

//////////////////////////////////////////////////////////////////////////
// AWalletConnectV2_DemoCharacter

AWalletConnectV2_DemoCharacter::AWalletConnectV2_DemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AWalletConnectV2_DemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AWalletConnectV2_DemoCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AWalletConnectV2_DemoCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AWalletConnectV2_DemoCharacter::Look);

	}

}

void AWalletConnectV2_DemoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AWalletConnectV2_DemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWalletConnectV2_DemoCharacter::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FString NonceAsStr = Response->GetContentAsString();
	nance = FCString::Atoi(*NonceAsStr);
	FString a = T_t_e(nance);
	FString b = Enc(a, ortak);
	FString c = Thx(b);
	this->Socket->Send(c);
}

void AWalletConnectV2_DemoCharacter::doit()
{
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AWalletConnectV2_DemoCharacter::OnResponseReceived);
	Request->SetURL("https://api.multiversx.com/accounts/" + Account + "?extract=nonce");
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void AWalletConnectV2_DemoCharacter::SoitBeg()
{
	bool succ = GenerateEd25519(edPublic, edPrivate, edPemPublic, edPemPrivate);
	FString didkey = "ed01" + edPublic;
	didkey = Base58(didkey);
	didkey = "did:key:z" + didkey;
	FString header = Header();
	FString payload = Payload(didkey);
	FString Message = header + "." + payload;
	FString Signature = Sign(Message, edPemPrivate);
	Url = "wss://relay.walletconnect.com/?auth=" + Message + "." + Signature + "&projectId=dd1f7d13256af05e81dd66e669a66a26&ua=wc-2/python/desktop/Cyberdeck";
	bool suxx = GenerateX25519(xPublic, xPrivate, xPemPublic, xPemPrivate);
	WC_STR = WCSTR(simet, konu);
	UE_LOG(LogTemp, Warning, TEXT("WC_STR: %s"), *WC_STR);
	websocket(Url);
}

bool AWalletConnectV2_DemoCharacter::GenerateEd25519(FString& Public, FString& Private, FString& PemPublic, FString& PemPrivate)
{
	EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr);
	if (!ctx)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create EVP_PKEY_CTX"));
		return false;
	}
	if (EVP_PKEY_keygen_init(ctx) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize key generation"));
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	EVP_PKEY* pkey = nullptr;
	if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate key pair"));
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	std::size_t private_key_len, public_key_len;
	if (EVP_PKEY_get_raw_private_key(pkey, nullptr, &private_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get length of private key"));
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, nullptr, &public_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get length of public key"));
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	unsigned char* private_key = new unsigned char[private_key_len];
	unsigned char* public_key = new unsigned char[public_key_len];

	if (EVP_PKEY_get_raw_private_key(pkey, private_key, &private_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get private key"));
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public_key, &public_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get public key"));
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	Public = BytesToHex(public_key, public_key_len).ToLower();
	Private = BytesToHex(private_key, private_key_len).ToLower();
	delete[] private_key;
	delete[] public_key;

	long privKeySize;
	long pubKeySize;
	char* privKeyData;
	char* pubKeyData;
	BIO* BIOPrivate = nullptr;
	BIO* BIOPublic = nullptr;
	BIOPublic = BIO_new(BIO_s_mem());
	if (1 != PEM_write_bio_PUBKEY(BIOPublic, pkey))
	{
		BIO_free_all(BIOPublic);
		BIO_free_all(BIOPrivate);
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
	}
	BIOPrivate = BIO_new(BIO_s_mem());
	if (1 != PEM_write_bio_PrivateKey(BIOPrivate, pkey, 0, 0, 0, 0, 0))
	{
		BIO_free_all(BIOPublic);
		BIO_free_all(BIOPrivate);
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
	}
	privKeySize = BIO_get_mem_data(BIOPrivate, &privKeyData);
	pubKeySize = BIO_get_mem_data(BIOPublic, &pubKeyData);
	PemPublic = FString(std::string(pubKeyData, pubKeySize).c_str());
	PemPrivate = FString(std::string(privKeyData, privKeySize).c_str());

	BIO_free_all(BIOPublic);
	BIO_free_all(BIOPrivate);
	EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
	return true;
}

bool AWalletConnectV2_DemoCharacter::GenerateX25519(FString& Public, FString& Private, FString& PemPublic, FString& PemPrivate)
{
	EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_X25519, nullptr);
	if (!ctx)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create EVP_PKEY_CTX"));
		return false;
	}
	if (EVP_PKEY_keygen_init(ctx) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to initialize key generation"));
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	EVP_PKEY* pkey = nullptr;
	if (EVP_PKEY_keygen(ctx, &pkey) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate key pair"));
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	std::size_t private_key_len, public_key_len;
	if (EVP_PKEY_get_raw_private_key(pkey, nullptr, &private_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get length of private key"));
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, nullptr, &public_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get length of public key"));
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	unsigned char* private_key = new unsigned char[private_key_len];
	unsigned char* public_key = new unsigned char[public_key_len];

	if (EVP_PKEY_get_raw_private_key(pkey, private_key, &private_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get private key"));
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}
	if (EVP_PKEY_get_raw_public_key(pkey, public_key, &public_key_len) <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get public key"));
		delete[] private_key;
		delete[] public_key;
		EVP_PKEY_free(pkey);
		EVP_PKEY_CTX_free(ctx);
		return false;
	}

	Public = BytesToHex(public_key, public_key_len).ToLower();
	Private = BytesToHex(private_key, private_key_len).ToLower();
	delete[] private_key;
	delete[] public_key;

	long privKeySize;
	long pubKeySize;
	char* privKeyData;
	char* pubKeyData;
	BIO* BIOPrivate = nullptr;
	BIO* BIOPublic = nullptr;
	BIOPublic = BIO_new(BIO_s_mem());
	if (1 != PEM_write_bio_PUBKEY(BIOPublic, pkey))
	{
		BIO_free_all(BIOPublic);
		BIO_free_all(BIOPrivate);
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
	}
	BIOPrivate = BIO_new(BIO_s_mem());
	if (1 != PEM_write_bio_PrivateKey(BIOPrivate, pkey, 0, 0, 0, 0, 0))
	{
		BIO_free_all(BIOPublic);
		BIO_free_all(BIOPrivate);
		EVP_PKEY_CTX_free(ctx);
		EVP_PKEY_free(pkey);
	}
	privKeySize = BIO_get_mem_data(BIOPrivate, &privKeyData);
	pubKeySize = BIO_get_mem_data(BIOPublic, &pubKeyData);
	PemPublic = FString(std::string(pubKeyData, pubKeySize).c_str());
	PemPrivate = FString(std::string(privKeyData, privKeySize).c_str());

	BIO_free_all(BIOPublic);
	BIO_free_all(BIOPrivate);
	EVP_PKEY_free(pkey);
	EVP_PKEY_CTX_free(ctx);
	return true;
}

void AWalletConnectV2_DemoCharacter::websocket(FString URL)
{
	const FString ServerURL = URL;

	if (!FModuleManager::Get().IsModuleLoaded("WebSockets"))
	{
		FModuleManager::Get().LoadModule("WebSockets");
	}

	this->Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL);

	this->Socket->OnConnected().AddLambda([this]()-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Connected to websocket server."));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Connected to websocket server.");
	this->Socket->Send(Sub(konu));
		});

	this->Socket->OnConnectionError().AddLambda([this](const FString& Error)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Failed to connect to websocket server with error: \"%s\"."), *Error);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Failed to connect to websocket server with error:" + Error);
		});

	this->Socket->OnMessage().AddLambda([this](const FString& Message)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Received message from websocket server: \"%s\"."), *Message);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Received message from websocket server:" + Message);
	checkJSON(Message);

		});
	this->Socket->OnMessageSent().AddLambda([this](const FString& MessageString)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Sent message to websocket server: \"%s\"."), *MessageString);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Sent message to websocket server:" + MessageString);
		});

	this->Socket->OnClosed().AddLambda([this](int32 StatusCode, const FString& Reason, bool bWasClean)-> void
		{
			UE_LOG(LogTemp, Log, TEXT("Connection to websocket server has been closed with status code: \"%d\" and reason: \"%s\"."), StatusCode, *Reason);
		});

	this->Socket->Connect();
}

FString AWalletConnectV2_DemoCharacter::Sub(FString Topic)
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

FString AWalletConnectV2_DemoCharacter::UnSub()
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetStringField("method", "irn_unsubscribe");

	TSharedRef<FJsonObject> ParamsObject = MakeShared<FJsonObject>();
	ParamsObject->SetStringField("topic", konu);
	ParamsObject->SetStringField("id", Subid);
	JsonObject->SetObjectField("params", ParamsObject);

	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);
	return OutputString;
}

FString AWalletConnectV2_DemoCharacter::WCSTR(FString& Symkey, FString& Topic)
{
	unsigned char buffer[32];
	RAND_bytes(buffer, 32);
	Symkey = BytesToHex(buffer, 32).ToLower();
	Topic = Digest(buffer, 32).ToLower();
	FString wcstr = "wc:" + Topic + "@2?relay-protocol=irn&symKey=" + Symkey;
	return wcstr;
}

FString AWalletConnectV2_DemoCharacter::Digest(const uint8* Data, uint32 Size)
{
	EVP_MD_CTX* ctx = EVP_MD_CTX_create();
	const EVP_MD* evp = EVP_sha256();
	int ret = EVP_DigestInit(ctx, evp);
	ret = EVP_DigestUpdate(ctx, Data, Size);
	unsigned int length = 0;
	TArray<uint8> HashBytes;
	HashBytes.SetNum(EVP_MAX_MD_SIZE);
	ret = EVP_DigestFinal(ctx, HashBytes.GetData(), &length);
	EVP_MD_CTX_destroy(ctx);
	return ret == 1 ? BytesToHex(HashBytes.GetData(), length) : "";
}

void AWalletConnectV2_DemoCharacter::checkJSON(FString message)
{
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(message);
	TSharedPtr<FJsonObject> JsonObject;
	FJsonSerializer::Deserialize(JsonReader, JsonObject);
	if (JsonObject->HasTypedField<EJson::Boolean>("result"))
	{
		UE_LOG(LogTemp, Warning, TEXT("good"));
	}
	else if (JsonObject->HasTypedField<EJson::String>("result"))
	{

		if (bFirst == false)
		{
			Subid = JsonObject->GetStringField("result");
			FString megg = M_t_E(xPublic);
			FString enmegg = Enc(megg, simet);
			FString J_t_s = RPC_JSON(konu, enmegg);
			this->Socket->Send(J_t_s);
			WC2HUD = WC2HUD == nullptr ? Cast<AWC2HUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WC2HUD;
			WC2HUD->SetQr(WC_STR);
			bFirst = true;
		}
		else
		{
			newSubid = JsonObject->GetStringField("result");
		}
	}
	else if (JsonObject->HasTypedField<EJson::String>("method"))
	{
		int64 idd = JsonObject->GetNumberField("id");
		this->Socket->Send(ACK_JSON(idd));
		if (JsonObject->HasTypedField<EJson::Object>("params"))
		{
			JsonObject = JsonObject->GetObjectField("params");
			if (JsonObject->HasTypedField<EJson::Object>("data"))
			{
				JsonObject = JsonObject->GetObjectField("data");
				if (JsonObject->HasTypedField<EJson::String>("message"))
				{
					FString message = JsonObject->GetStringField("message");
					if (sk == false)
					{
						sk = true;
						FString DeCC = Decr(message, simet);
						UE_LOG(LogTemp, Warning, TEXT("decrypted: %s"), *DeCC);
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Decrypted:" + DeCC);
						TSharedRef<TJsonReader<>> Jsonreader = TJsonReaderFactory<>::Create(DeCC);
						TSharedPtr<FJsonObject> Jsonobject;
						FJsonSerializer::Deserialize(Jsonreader, Jsonobject);
						Jsonobject = Jsonobject->GetObjectField("result");
						FString peerpkey = Jsonobject->GetStringField("responderPublicKey");
						Exchangekeys(xPrivate, peerpkey);
						this->Socket->Send(Sub(newkonu));
					}
					else
					{
						FString DeCC2 = Decr(message, ortak);
						UE_LOG(LogTemp, Warning, TEXT("decrypted: %s"), *DeCC2);
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Decrypted:" + DeCC2);
						TSharedRef<TJsonReader<>> Jsonreader2 = TJsonReaderFactory<>::Create(DeCC2);
						TSharedPtr<FJsonObject> Jsonobject2;
						FJsonSerializer::Deserialize(Jsonreader2, Jsonobject2);
						if (Jsonobject2->HasTypedField<EJson::Object>("params"))
						{
							Jsonobject2 = Jsonobject2->GetObjectField("params");
							if (Jsonobject2->HasTypedField<EJson::Object>("namespaces"))
							{
								Jsonobject2 = Jsonobject2->GetObjectField("namespaces");
								if (Jsonobject2->HasTypedField<EJson::Object>("elrond"))
								{
									Jsonobject2 = Jsonobject2->GetObjectField("elrond");
									TArray<TSharedPtr<FJsonValue>> accountsA;
									accountsA = Jsonobject2->GetArrayField("accounts");
									Account = accountsA[0]->AsString();
									Account = Account.RightChop(9);
									GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Black, Account);
									WC2HUD = WC2HUD == nullptr ? Cast<AWC2HUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WC2HUD;
									WC2HUD->dissQr(Account);
									this->Socket->Send(ACK_JSON2());
								}
							}
						}
						else if (Jsonobject2->HasTypedField<EJson::Object>("result"))
						{
							Jsonobject2 = Jsonobject2->GetObjectField("result");
							TArray<TSharedPtr<FJsonValue>> signatures;
							signatures = Jsonobject2->GetArrayField("signatures");
							TSharedPtr<FJsonObject> signatureobject = signatures[0]->AsObject();
							FString signature = signatureobject->GetStringField("signature");
							GEngine->AddOnScreenDebugMessage(-1, 50.f, FColor::Black, signature);
							TSharedRef<FJsonObject> transactionsarrayObject = MakeShared<FJsonObject>();
							transactionsarrayObject->SetNumberField("nonce", nance);
							transactionsarrayObject->SetStringField("value", "0");
							transactionsarrayObject->SetStringField("receiver", "erd1a40kkjl2u73yhk7vlv60dvdzswxpr39u2yhdzcleq3q4mgs78j0sw4trf6");
							transactionsarrayObject->SetStringField("sender", Account);
							transactionsarrayObject->SetNumberField("gasPrice", 1000000000);
							transactionsarrayObject->SetNumberField("gasLimit", 500000);
							transactionsarrayObject->SetStringField("data", "RVNEVFRyYW5zZmVyQDQzNTk0MjQ1NTIyZDM0MzgzOTYzMzE2M0AwZGUwYjZiM2E3NjQwMDAwQDc0NjU3Mzc0");
							transactionsarrayObject->SetStringField("chainID", "1");
							transactionsarrayObject->SetNumberField("version", 1);
							transactionsarrayObject->SetStringField("signature", signature);
							FString txstr;
							const auto Writr = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&txstr);
							FJsonSerializer::Serialize(transactionsarrayObject, Writr);

							FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
							Request->SetURL("https://api.multiversx.com/transactions");
							Request->SetHeader("content-type", "application/json");
							Request->SetVerb("POST");
							Request->SetContentAsString(txstr);
							Request->ProcessRequest();
						}
					}
				}
			}
		}
	}

}

FString AWalletConnectV2_DemoCharacter::M_t_E(FString xkey)
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
	chainsarray.Add(MakeShared<FJsonValueString>("elrond:1"));
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
	proposerObject->SetStringField("publicKey", xkey);

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

FString AWalletConnectV2_DemoCharacter::Enc(const FString& smsg, FString skey)
{
	std::string message = std::string(TCHAR_TO_UTF8(*smsg));
	int outlen;
	int ciphertext_len = message.length() + EVP_CIPHER_block_size(EVP_chacha20_poly1305());
	unsigned char* ciphertext = new unsigned char[ciphertext_len];

	// Nonce and key buffers
	unsigned char nonce[12];
	unsigned char key[32];
	unsigned char tag[16];

	// Generate random nonce and key
	RAND_bytes(nonce, 12);
	HexToBytes(skey, key);
	FString Nonce = BytesToHex(nonce, 12);
	// Encrypt
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, key, nonce);
	EVP_EncryptUpdate(ctx, ciphertext, &outlen, (unsigned char*)message.c_str(), message.length());
	int tmp = 0;
	EVP_EncryptFinal_ex(ctx, ciphertext + outlen, &tmp);
	EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag);
	EVP_CIPHER_CTX_free(ctx);
	outlen += tmp;
	unsigned char* result = new unsigned char[16 + 13 + outlen];
	result[0] = 0;
	memcpy(result + 1, nonce, 12);
	memcpy(result + 1 + 12, ciphertext, outlen);
	memcpy(result + 1 + 12 + outlen, tag, 16);
	FString RESS = FBase64::Encode(result, 1 + 12 + outlen + 16);
	return RESS;

}

FString AWalletConnectV2_DemoCharacter::Decr(const FString& smsg, FString skey)
{

	int ciphertext_len = FBase64::GetDecodedDataSize(smsg);
	unsigned char* ciphertext = new unsigned char[ciphertext_len];
	unsigned char* result = new unsigned char[ciphertext_len];
	FBase64::Decode(*smsg, smsg.Len(), ciphertext);
	unsigned char nonce[12];
	unsigned char key[32];
	unsigned char tag[16];
	memcpy(nonce, ciphertext + 1, 12);
	memcpy(result, ciphertext + 1 + 12, ciphertext_len - 1 - 12 - 16);
	memcpy(tag, ciphertext + ciphertext_len - 16, 16);

	HexToBytes(skey, key);

	int outlen;
	int plaintext_len = ciphertext_len - 1 - 12 - 16;
	unsigned char* plaintext = new unsigned char[plaintext_len];
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex(ctx, EVP_chacha20_poly1305(), nullptr, key, nonce);
	EVP_DecryptUpdate(ctx, plaintext, &outlen, result, plaintext_len);
	int tmp = 0;
	EVP_DecryptFinal_ex(ctx, plaintext + outlen, &tmp);

	outlen += tmp;
	EVP_CIPHER_CTX_free(ctx);
	FString decrypted_hex = BytesToHexLower(plaintext, plaintext_len);
	TArray<uint8> Bytes;
	Bytes.SetNum(decrypted_hex.Len() / 2);
	int32 Total = HexToBytes(decrypted_hex, Bytes.GetData());
	Bytes.SetNum(Total);

	if (Bytes.Last() != 0)
	{
		TArray<uint8> Copy(Bytes);
		Copy.Add(0);
		return FString(UTF8_TO_TCHAR(Copy.GetData()));
	}
	return FString(UTF8_TO_TCHAR(Bytes.GetData()));

}

FString AWalletConnectV2_DemoCharacter::RPC_JSON(FString topic, FString encmsg)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", topic);
	paramsObject->SetStringField("message", encmsg);
	paramsObject->SetNumberField("ttl", 300);
	paramsObject->SetBoolField("prompt", true);
	paramsObject->SetNumberField("tag", 1100);

	jsonObject->SetObjectField("params", paramsObject);
	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

FString AWalletConnectV2_DemoCharacter::ACK_JSON(int64 id)
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

FString AWalletConnectV2_DemoCharacter::ACK_JSON2()
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetNumberField("id", id);
	JsonObject->SetStringField("jsonrpc", "2.0");
	JsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", newkonu);
	FString Message = Enc(ACK_JSON(id), ortak);
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

FString AWalletConnectV2_DemoCharacter::Exchangekeys(FString myPRKey, FString peerPubKey)
{
	//std::string private_key_pem = std::string(TCHAR_TO_UTF8(*myPRKey));
	//BIO* bio = BIO_new_mem_buf((void*)private_key_pem.c_str(), private_key_pem.length());
	//EVP_PKEY* private_key = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
	//BIO_free(bio);
	unsigned char* my_private_key = new unsigned char[32];
	HexToBytes(myPRKey, my_private_key);
	EVP_PKEY* private_key = EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, nullptr, my_private_key, 32);

	unsigned char* peer_public_key = new unsigned char[32];
	HexToBytes(peerPubKey, peer_public_key);
	EVP_PKEY* peerkey = EVP_PKEY_new_raw_public_key(EVP_PKEY_X25519, nullptr, peer_public_key, 32);

	delete[] my_private_key;
	delete[] peer_public_key;

	EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new(private_key, NULL);
	EVP_PKEY_derive_init(pctx);
	EVP_PKEY_derive_set_peer(pctx, peerkey);
	EVP_PKEY_free(private_key);
	EVP_PKEY_free(peerkey);
	size_t shared_key_len;

	EVP_PKEY_derive(pctx, NULL, &shared_key_len);
	UE_LOG(LogTemp, Warning, TEXT("%d"), shared_key_len);
	unsigned char* shared_key = new unsigned char[shared_key_len];
	EVP_PKEY_derive(pctx, shared_key, &shared_key_len);
	EVP_PKEY_CTX_free(pctx);

	EVP_PKEY_CTX* kdf_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
	EVP_PKEY_derive_init(kdf_ctx);
	EVP_PKEY_CTX_set_hkdf_md(kdf_ctx, EVP_sha256());
	EVP_PKEY_CTX_set1_hkdf_salt(kdf_ctx, NULL, 0);
	EVP_PKEY_CTX_set1_hkdf_key(kdf_ctx, shared_key, shared_key_len);
	unsigned char derived_key[32];
	size_t derived_key_len = 32;
	EVP_PKEY_derive(kdf_ctx, derived_key, &derived_key_len);
	EVP_PKEY_CTX_free(kdf_ctx);

	ortak = BytesToHexLower(derived_key, derived_key_len);
	newkonu = Digest(derived_key, derived_key_len).ToLower();
	return "true";
}

FString AWalletConnectV2_DemoCharacter::Thx(FString encmsg)
{
	int64 id = (FDateTime::UtcNow().GetTicks() - FDateTime(1970, 1, 1).GetTicks()) / 10;
	TSharedRef<FJsonObject> jsonObject = MakeShared<FJsonObject>();
	jsonObject->SetNumberField("id", id);
	jsonObject->SetStringField("jsonrpc", "2.0");
	jsonObject->SetStringField("method", "irn_publish");

	TSharedRef<FJsonObject> paramsObject = MakeShared<FJsonObject>();
	paramsObject->SetStringField("topic", newkonu);
	paramsObject->SetStringField("message", encmsg);
	paramsObject->SetNumberField("ttl", 300);
	paramsObject->SetBoolField("prompt", true);
	paramsObject->SetNumberField("tag", 1108);

	jsonObject->SetObjectField("params", paramsObject);
	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

FString AWalletConnectV2_DemoCharacter::T_t_e(int nonce)
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
	TArray<TSharedPtr<FJsonValue>> transactionsarray;
	TSharedRef<FJsonObject> transactionsarrayObject = MakeShared<FJsonObject>();
	transactionsarrayObject->SetNumberField("nonce", nonce);
	transactionsarrayObject->SetStringField("value", "0");
	transactionsarrayObject->SetStringField("receiver", "erd1a40kkjl2u73yhk7vlv60dvdzswxpr39u2yhdzcleq3q4mgs78j0sw4trf6");
	transactionsarrayObject->SetStringField("sender", Account);
	transactionsarrayObject->SetNumberField("gasPrice", 1000000000);
	transactionsarrayObject->SetNumberField("gasLimit", 500000);
	transactionsarrayObject->SetStringField("data", "RVNEVFRyYW5zZmVyQDQzNTk0MjQ1NTIyZDM0MzgzOTYzMzE2M0AwZGUwYjZiM2E3NjQwMDAwQDc0NjU3Mzc0");
	transactionsarrayObject->SetStringField("chainID", "1");
	transactionsarrayObject->SetNumberField("version", 1);
	transactionsarray.Add(MakeShared<FJsonValueObject>(transactionsarrayObject));
	paramsObject2->SetArrayField("transactions", transactionsarray);
	requestObject->SetObjectField("params", paramsObject2);
	paramsObject->SetObjectField("request", requestObject);
	paramsObject->SetStringField("chainId", "elrond:1");
	jsonObject->SetObjectField("params", paramsObject);
	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(jsonObject, Writer);
	return OutputString;
}

FString AWalletConnectV2_DemoCharacter::Base58(const FString& Str)
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

FString AWalletConnectV2_DemoCharacter::Header()
{
	TSharedRef<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	JsonObject->SetStringField("alg", "EdDSA");
	JsonObject->SetStringField("typ", "JWT");
	FString OutputString;
	const auto Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonObject, Writer);

	OutputString = FBase64::Encode(OutputString);

	return OutputString;
}

FString AWalletConnectV2_DemoCharacter::Payload(FString& iss)
{
	unsigned char buffer[32];
	RAND_bytes(buffer, 32);
	FString sub = BytesToHex(buffer, 32);
	int64 iat = FDateTime::UtcNow().ToUnixTimestamp();
	int64 exp = iat + 86400;
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

FString AWalletConnectV2_DemoCharacter::Sign(FString& data, FString& privatekey)
{

	const std::string message = std::string(TCHAR_TO_UTF8(*data));

	const unsigned char* Data = (unsigned char*)message.c_str();
	size_t Size = message.length();

	unsigned char* signature = nullptr;
	size_t signature_len = 0;
	std::string StrKey = std::string(TCHAR_TO_UTF8(*privatekey));
	BIO* Bio = BIO_new_mem_buf((void*)StrKey.c_str(), -1);
	EVP_PKEY* PKey = PEM_read_bio_PrivateKey(Bio, nullptr, nullptr, nullptr);
	BIO_free(Bio);

	EVP_MD_CTX* SignCtx = EVP_MD_CTX_new();

	if (!EVP_DigestSignInit(SignCtx, nullptr, nullptr, nullptr, PKey))
	{
		return "false";
	}

	if (!EVP_DigestSign(SignCtx, nullptr, &signature_len, Data, Size))
	{
		unsigned long errorCode = ERR_get_error();
		char errorString[120];
		ERR_error_string(errorCode, errorString);
		FString err = FString(std::string(errorString, 120).c_str());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *err);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "error: " + err);
		return "false";
	}

	signature = new unsigned char[signature_len];

	if (!EVP_DigestSign(SignCtx, signature, &signature_len, Data, Size))
	{
		unsigned long errorCode = ERR_get_error();
		char errorString[120];
		ERR_error_string(errorCode, errorString);
		FString err = FString(std::string(errorString, 120).c_str());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *err);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "error: " + err);
		return "false";
	}

	FString Signature = FBase64::Encode(signature, signature_len);
	Signature = Signature.Replace(TEXT("\n"), TEXT(""));
	Signature = Signature.Replace(TEXT("="), TEXT(""));
	Signature = Signature.Replace(TEXT("+"), TEXT("-"));
	Signature = Signature.Replace(TEXT("/"), TEXT("_"));
	EVP_PKEY_free(PKey);
	EVP_MD_CTX_free(SignCtx);
	return Signature;
}
