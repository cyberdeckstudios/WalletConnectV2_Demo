// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class WalletConnectV2_Demo : ModuleRules
{
	public WalletConnectV2_Demo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
