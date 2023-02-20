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

#include "WCWidget.h"
#include "Components/Button.h"
#include "WCHUD.h"

void UWCWidget::NativeConstruct()
{
	if (ConnectButton)
	{
		ConnectButton->OnClicked.AddDynamic(this, &UWCWidget::CLK);
	}
	if (Black)
	{
		Black->OnClicked.AddDynamic(this, &UWCWidget::CLK2);
	}
	if (SendCyber)
	{
		SendCyber->OnClicked.AddDynamic(this, &UWCWidget::CLK3);
	}
	if (SendTransaction)
	{
		SendTransaction->OnClicked.AddDynamic(this, &UWCWidget::CLK4);
	}
	if (SendTransactions)
	{
		SendTransactions->OnClicked.AddDynamic(this, &UWCWidget::CLK5);
	}
	if (SignMessage)
	{
		SignMessage->OnClicked.AddDynamic(this, &UWCWidget::CLK6);
	}
}

void UWCWidget::CLK()
{
	WCHUD = WCHUD == nullptr ? Cast<AWCHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WCHUD;
	WCHUD->ConnectClicked();
}

void UWCWidget::CLK2()
{
	WCHUD = WCHUD == nullptr ? Cast<AWCHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WCHUD;
	WCHUD->BlackClicked();
}

void UWCWidget::CLK3()
{
	WCHUD = WCHUD == nullptr ? Cast<AWCHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WCHUD;
	WCHUD->getNonce("cyber");
}

void UWCWidget::CLK4()
{
	WCHUD = WCHUD == nullptr ? Cast<AWCHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WCHUD;
	WCHUD->getNonce("single");
}

void UWCWidget::CLK5()
{
	WCHUD = WCHUD == nullptr ? Cast<AWCHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WCHUD;
	WCHUD->getNonce("multi");
}

void UWCWidget::CLK6()
{
	WCHUD = WCHUD == nullptr ? Cast<AWCHUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WCHUD;
	WCHUD->getNonce("message");
}

