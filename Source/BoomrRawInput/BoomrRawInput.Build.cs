using UnrealBuildTool;

public class BoomrRawInput : ModuleRules
{
	public BoomrRawInput(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				"BoomrRawInput"
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"ApplicationCore",
				"Slate",
				"SlateCore",
			}
		);
	}
}
