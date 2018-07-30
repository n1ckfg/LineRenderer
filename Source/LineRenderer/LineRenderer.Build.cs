using UnrealBuildTool;

public class LineRenderer : ModuleRules
{
	public LineRenderer(ReadOnlyTargetRules Target) : base(Target)
	{
		
		PublicIncludePaths.AddRange(new string[] { "LineRenderer/Public" });

        PrivateIncludePaths.AddRange(new string[] { "LineRenderer/Private" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "ProceduralMeshComponent" });

        PrivateDependencyModuleNames.AddRange(new string[] { "CoreUObject", "Engine", "Slate", "SlateCore" });

        DynamicallyLoadedModuleNames.AddRange(new string[] { });
    }
}
