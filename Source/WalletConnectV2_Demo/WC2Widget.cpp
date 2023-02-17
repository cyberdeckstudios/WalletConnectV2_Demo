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

#include "WC2Widget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "WC2HUD.h"

void UWC2Widget::NativeConstruct()
{
	if (ConnectButton)
	{
		ConnectButton->OnClicked.AddDynamic(this, &UWC2Widget::CLK);
	}
}

void UWC2Widget::CLK()
{
	WC2HUD = WC2HUD == nullptr ? Cast<AWC2HUD>(GetWorld()->GetFirstPlayerController()->GetHUD()) : WC2HUD;
	WC2HUD->ConnectClicked();
}
