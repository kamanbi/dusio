using UnrealBuildTool;
using System.Collections.Generic;

public class dusioEditorTarget : TargetRules
{
	public dusioEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("dusio");
	}
}
