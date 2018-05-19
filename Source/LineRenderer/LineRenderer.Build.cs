// Copyright 2016, Sigurdur Gunnarsson. All Rights Reserved. 

using UnrealBuildTool;

public class LineRenderer : ModuleRules
{
	public LineRenderer(TargetInfo Target)
	{
		
		PublicIncludePaths.AddRange(new string[] { "LineRenderer/Public" });

        PrivateIncludePaths.AddRange(new string[] { "LineRenderer/Private" });

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "RenderCore", "ShaderCore", "RHI", "RuntimeMeshComponent" });


        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
// 				"Slate",
// 				"SlateCore",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(new string[] {  });
    }
}
