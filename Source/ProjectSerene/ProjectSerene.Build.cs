// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectSerene : ModuleRules
{
	public ProjectSerene(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate",
			"SlateCore",
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ProjectSerene",
			"ProjectSerene/GAS",
			"ProjectSerene/Inventory",
			"ProjectSerene/Player",
			"ProjectSerene/Variant_Horror",
			"ProjectSerene/Variant_Horror/Pickups",
			"ProjectSerene/Variant_Horror/UI",
			"ProjectSerene/Variant_Shooter",
			"ProjectSerene/Variant_Shooter/AI",
			"ProjectSerene/Variant_Shooter/UI",
			"ProjectSerene/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
