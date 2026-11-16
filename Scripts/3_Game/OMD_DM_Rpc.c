class OMD_DM_Rpc
{
	static const int OPEN = 77821011;
	static const int CHOICE = 77821012;
	static const int COUNT = 77821013;
	static const int RESULT = 77821014;
};

class OMD_DM_VoteOption
{
	int number;
	string id;
	string title;
	int votes;
};

class OMD_DM_VoteClient
{
	static bool s_Active;
	static bool s_ForceOpen;
	static bool s_Refresh;
	static bool s_Close;
	static bool s_UserClosed;
	static int s_Seconds;
	static int s_MyArena;
	static string s_MyLevel;
	static int s_PendingArena;
	static string s_PendingLevel;
	static ref array<ref OMD_DM_VoteOption> s_Arenas;
	static ref array<ref OMD_DM_VoteOption> s_Levels;

	static void Reset()
	{
		s_Active = false;
		s_ForceOpen = false;
		s_Refresh = false;
		s_Close = false;
		s_UserClosed = false;
		s_Seconds = 0;
		s_MyArena = 0;
		s_MyLevel = "";
		s_PendingArena = 0;
		s_PendingLevel = "";
		s_Arenas = null;
		s_Levels = null;
	}

	protected static void Ensure()
	{
		if (!s_Arenas)
		{
			s_Arenas = new array<ref OMD_DM_VoteOption>();
		}

		if (!s_Levels)
		{
			s_Levels = new array<ref OMD_DM_VoteOption>();
		}
	}

	static bool ReadState(ParamsReadContext ctx, bool opening)
	{
		int seconds;
		int arenaCount;
		int levelCount;
		int i;
		int number;
		int votes;
		int myArena;
		string id;
		string title;
		string myLevel;
		array<ref OMD_DM_VoteOption> arenas;
		array<ref OMD_DM_VoteOption> levels;
		OMD_DM_VoteOption option;

		if (!ctx.Read(seconds))
		{
			return false;
		}

		if (!ctx.Read(arenaCount))
		{
			return false;
		}

		if (arenaCount < 0)
		{
			return false;
		}

		if (arenaCount > 16)
		{
			return false;
		}

		arenas = new array<ref OMD_DM_VoteOption>();
		for (i = 0; i < arenaCount; i++)
		{
			if (!ctx.Read(number))
			{
				return false;
			}

			if (!ctx.Read(title))
			{
				return false;
			}

			if (!ctx.Read(votes))
			{
				return false;
			}

			option = new OMD_DM_VoteOption();
			option.number = number;
			option.id = "";
			option.title = title;
			option.votes = votes;
			arenas.Insert(option);
		}

		if (!ctx.Read(levelCount))
		{
			return false;
		}

		if (levelCount < 0)
		{
			return false;
		}

		if (levelCount > 16)
		{
			return false;
		}

		levels = new array<ref OMD_DM_VoteOption>();
		for (i = 0; i < levelCount; i++)
		{
			if (!ctx.Read(id))
			{
				return false;
			}

			if (!ctx.Read(title))
			{
				return false;
			}

			if (!ctx.Read(votes))
			{
				return false;
			}

			option = new OMD_DM_VoteOption();
			option.number = i + 1;
			option.id = id;
			option.title = title;
			option.votes = votes;
			levels.Insert(option);
		}

		if (!ctx.Read(myArena))
		{
			return false;
		}

		if (!ctx.Read(myLevel))
		{
			return false;
		}

		Ensure();
		s_Seconds = seconds;
		s_Arenas = arenas;
		s_Levels = levels;
		s_MyArena = myArena;
		s_MyLevel = myLevel;
		s_Refresh = true;
		s_Active = true;
		s_Close = false;
		if (opening)
		{
			s_ForceOpen = true;
			s_UserClosed = false;
			s_PendingArena = 0;
			s_PendingLevel = "";
		}

		return true;
	}

	static bool ReadResult(ParamsReadContext ctx)
	{
		string arenaName;
		string levelName;
		string arenaReason;
		string levelReason;

		if (!ctx.Read(arenaName))
		{
			return false;
		}

		if (!ctx.Read(levelName))
		{
			return false;
		}

		if (!ctx.Read(arenaReason))
		{
			return false;
		}

		if (!ctx.Read(levelReason))
		{
			return false;
		}

		s_Active = false;
		s_ForceOpen = false;
		s_Refresh = false;
		s_UserClosed = false;
		s_Close = true;
		s_PendingArena = 0;
		s_PendingLevel = "";
		s_MyArena = 0;
		s_MyLevel = "";
		return true;
	}
};
