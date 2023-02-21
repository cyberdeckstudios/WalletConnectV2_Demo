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
#include "Blueprint/UserWidget.h"
#include "WCWidget.generated.h"

/**
 * 
 */
UCLASS()
class WALLETCONNECT_API UWCWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* QrPanel;

	UPROPERTY(meta = (BindWidget))
		class UCanvasPanel* StatsPanel;

	UPROPERTY(meta = (BindWidget))
		class UImage* QrImage;

	UPROPERTY(meta = (BindWidget))
		class UButton* Black;

	UPROPERTY(meta = (BindWidget))
		class UButton* ConnectButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* SendCyber;

	UPROPERTY(meta = (BindWidget))
		class UButton* SendTransaction;

	UPROPERTY(meta = (BindWidget))
		class UButton* SendTransactions;

	UPROPERTY(meta = (BindWidget))
		class UButton* SignMessage;

	UPROPERTY(meta = (BindWidget))
		class UButton* DisconnectButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* Quit;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Address;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* Username;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* CYBER;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* EGLD;

	virtual void NativeConstruct() override;

	UFUNCTION()
		void CLK();
	UFUNCTION()
		void CLK2();
	UFUNCTION()
		void CLK3();
	UFUNCTION()
		void CLK4();
	UFUNCTION()
		void CLK5();
	UFUNCTION()
		void CLK6();
	UFUNCTION()
		void CLK7();
	UFUNCTION()
		void CLK8();
	class AWCHUD* WCHUD;
};
