// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class Boomr : ModuleRules
{
	public Boomr(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"Boomr",
				"Boomr/Core",
				"Boomr/Core/Pawn",
				"Boomr/Core/Weapons",
				"Boomr/Ui",
			}
		);	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
		});
		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"Slate", 
			"SlateCore",
			"BooShaders",
		});
	}
}
