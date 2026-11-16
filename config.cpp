class CfgPatches
{
	class OMD_Deathmatch
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts"
		};
	};
};

class CfgMods
{
	class OMD_Deathmatch
	{
		dir = "OMD_Deathmatch";
		picture = "";
		action = "";
		hideName = 0;
		hidePicture = 0;
		name = "OMD Deathmatch";
		credits = "OMD";
		author = "OMD";
		authorID = "0";
		version = "0.1.0";
		type = "mod";
		dependencies[] = { "Game", "World", "Mission" };
		inputs = "OMD_Deathmatch/inputs.xml";
		class defs
		{
			class imageSets
			{
				files[] = { "OMD_Deathmatch/gui/imagesets/omd_dm.imageset" };
			};
			class gameScriptModule
			{
				value = "";
				files[] = { "OMD_Deathmatch/Scripts/3_Game" };
			};
			class worldScriptModule
			{
				value = "";
				files[] = { "OMD_Deathmatch/Scripts/4_World" };
			};
			class missionScriptModule
			{
				value = "";
				files[] = { "OMD_Deathmatch/Scripts/5_Mission" };
			};
		};
	};
};
