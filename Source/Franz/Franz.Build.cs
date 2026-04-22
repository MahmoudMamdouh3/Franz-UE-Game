// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Franz : ModuleRules
{
	public Franz(ReadOnlyTargetRules Target) : base(Target)
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
			"Franz",
			"Franz/Variant_Platforming",
			"Franz/Variant_Platforming/Animation",
			"Franz/Variant_Combat",
			"Franz/Variant_Combat/AI",
			"Franz/Variant_Combat/Animation",
			"Franz/Variant_Combat/Gameplay",
			"Franz/Variant_Combat/Interfaces",
			"Franz/Variant_Combat/UI",
			"Franz/Variant_SideScrolling",
			"Franz/Variant_SideScrolling/AI",
			"Franz/Variant_SideScrolling/Gameplay",
			"Franz/Variant_SideScrolling/Interfaces",
			"Franz/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
