// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "IWebSocket.h"
#include "Http.h"
#define UI UI_ST
THIRD_PARTY_INCLUDES_START
#include "openssl/evp.h"
THIRD_PARTY_INCLUDES_END
#undef UI
#include "WalletConnectV2_DemoCharacter.generated.h"


UCLASS(config=Game)
class AWalletConnectV2_DemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

public:
	AWalletConnectV2_DemoCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	FString Base58(const FString& Str);
	FString Header();
	FString Payload(FString& iss);
	FString Sign(FString& data, FString& privatekey);
	static bool GenerateEd25519(FString& Public, FString& Private, FString& PemPublic, FString& PemPrivate);
	static bool GenerateX25519(FString& Public, FString& Private, FString& PemPublic, FString& PemPrivate);
	void websocket(FString URL);
	FString Sub(FString Topic);
	FString UnSub();
	FString WCSTR(FString& Symkey, FString& Topic);
	FString Digest(const uint8* Data, uint32 Size);
	void checkJSON(FString message);
	FString M_t_E(FString xkey);
	FString Enc(const FString& smsg, FString skey);
	FString Decr(const FString& smsg, FString skey);
	FString RPC_JSON(FString topic, FString encmsg);
	FString ACK_JSON(int64 id);
	FString ACK_JSON2();
	FString Exchangekeys(FString myPRKey, FString peerPubKey);
	FString Thx(FString encmsg);
	FString T_t_e(int nonce);

private:
	TSharedPtr<IWebSocket> Socket;
	class AWC2HUD* WC2HUD;
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	void doit();
	void SoitBeg();
	UPROPERTY(BlueprintReadOnly)
	FString Url;
	FString simet = "";
	FString konu = "";
	FString Subid = "";
	FString WC_STR = "";
	FString edPublic = "";
	FString edPrivate = "";
	FString edPemPublic = "";
	FString edPemPrivate = "";
	FString xPublic = "";
	FString xPrivate = "";
	FString xPemPublic = "";
	FString xPemPrivate = "";
	FString newSubid = "";
	FString ortak = "";
	FString newkonu = "";
	FString Account = "";
	int nance;
	bool bFirst = false;
	bool sk = false;
};

