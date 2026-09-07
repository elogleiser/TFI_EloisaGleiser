// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TFI_EloisaGleiser : ModuleRules
{
	public TFI_EloisaGleiser(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"TFI_EloisaGleiser",
			"TFI_EloisaGleiser/Variant_Platforming",
			"TFI_EloisaGleiser/Variant_Platforming/Animation",
			"TFI_EloisaGleiser/Variant_Combat",
			"TFI_EloisaGleiser/Variant_Combat/AI",
			"TFI_EloisaGleiser/Variant_Combat/Animation",
			"TFI_EloisaGleiser/Variant_Combat/Gameplay",
			"TFI_EloisaGleiser/Variant_Combat/Interfaces",
			"TFI_EloisaGleiser/Variant_Combat/UI",
			"TFI_EloisaGleiser/Variant_SideScrolling",
			"TFI_EloisaGleiser/Variant_SideScrolling/AI",
			"TFI_EloisaGleiser/Variant_SideScrolling/Gameplay",
			"TFI_EloisaGleiser/Variant_SideScrolling/Interfaces",
			"TFI_EloisaGleiser/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
