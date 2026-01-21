using UnrealBuildTool;

public class BooShaders : ModuleRules {
    public BooShaders(ReadOnlyTargetRules Target) : base(Target) 
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "RenderCore",
            "RHI"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Projects",
            "Renderer",
        });
    }
}
