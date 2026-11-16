class OMD_DM_Rpc
{
	static const int OPEN = 77821011;
	static const int CHOICE = 77821012;
	static const int COUNT = 77821013;
	static const int RESULT = 77821014;
	static const int BOARD_REQ = 77821015;
	static const int BOARD_LIVE = 77821016;
	static const int BOARD_FINAL = 77821017;
	static const int PHASE = 77821018;
	static const int BOARD_MAX = 64;
};

class OMD_DM_Ui
{
	static const string LOGO_IMAGE = "set:omd_dm image:Lockup";
	static bool s_Esc;

	static void Reset()
	{
		s_Esc = false;
	}

	static string Clock(int seconds)
	{
		int minutes;
		int rest;
		string minText;
		string secText;

		if (seconds < 0)
		{
			seconds = 0;
		}

		minutes = seconds / 60;
		rest = seconds - (minutes * 60);
		minText = minutes.ToString();
		secText = rest.ToString();
		if (minutes < 10)
		{
			minText = "0" + minText;
		}

		if (rest < 10)
		{
			secText = "0" + secText;
		}

		return minText + ":" + secText;
	}

	static void Log(string action, string menuName, string reason)
	{
		if (!OMD_DM_BoardClient.LogOn())
		{
			return;
		}

		Print("[OMD_DM][UI] " + action + " menu=" + menuName + " reason=" + reason);
	}

	static void LogToggle(string keyName)
	{
		if (!OMD_DM_BoardClient.LogOn())
		{
			return;
		}

		Print("[OMD_DM][UI] toggle key=" + keyName);
	}
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
	static bool s_SwitchVote;
	static bool s_WantAuto;
	static bool s_Dismissed;
	static bool s_Refresh;
	static bool s_Close;
	static int s_Seconds;
	static int s_MyArena;
	static string s_MyLevel;
	static int s_PendingArena;
	static string s_PendingLevel;
	static bool s_Debug;
	static ref array<ref OMD_DM_VoteOption> s_Arenas;
	static ref array<ref OMD_DM_VoteOption> s_Levels;

	static void Reset()
	{
		s_Active = false;
		s_SwitchVote = false;
		s_WantAuto = false;
		s_Dismissed = false;
		s_Refresh = false;
		s_Close = false;
		s_Seconds = 0;
		s_MyArena = 0;
		s_MyLevel = "";
		s_PendingArena = 0;
		s_PendingLevel = "";
		s_Debug = false;
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
		bool debugOn;
		bool wasActive;
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

		debugOn = false;
		if (!ctx.Read(debugOn))
		{
			debugOn = false;
		}

		Ensure();
		wasActive = s_Active;
		s_Seconds = seconds;
		s_Arenas = arenas;
		s_Levels = levels;
		s_MyArena = myArena;
		s_MyLevel = myLevel;
		s_Debug = debugOn;
		s_Refresh = true;
		s_Active = true;
		s_Close = false;
		if (!OMD_DM_BoardClient.s_InMatch)
		{
			OMD_DM_BoardClient.s_PhaseKnown = true;
		}

		if (opening)
		{
			if (!wasActive)
			{
				s_PendingArena = 0;
				s_PendingLevel = "";
				s_Dismissed = false;
				s_WantAuto = true;
			}

			if (s_Debug)
			{
				Print("[OMD_DM][VOTE] rpc open sec=" + seconds.ToString());
			}
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
		s_SwitchVote = false;
		s_WantAuto = false;
		s_Dismissed = false;
		s_Refresh = false;
		s_Close = true;
		s_PendingArena = 0;
		s_PendingLevel = "";
		s_MyArena = 0;
		s_MyLevel = "";
		return true;
	}
};

class OMD_DM_BoardRow
{
	string name;
	int kills;
	int deaths;
	int assists;
	int points;
};

class OMD_DM_BoardClient
{
	static bool s_InMatch;
	static bool s_PhaseKnown;
	static bool s_HasFinal;
	static bool s_OpenFinal;
	static bool s_SwitchBoard;
	static bool s_CloseMenu;
	static bool s_Refresh;
	static bool s_Debug;
	static bool s_TimeKnown;
	static int s_SecondsLeft;
	static ref array<ref OMD_DM_BoardRow> s_Rows;

	static void Reset()
	{
		s_InMatch = false;
		s_PhaseKnown = false;
		s_HasFinal = false;
		s_OpenFinal = false;
		s_SwitchBoard = false;
		s_CloseMenu = false;
		s_Refresh = false;
		s_Debug = false;
		s_TimeKnown = false;
		s_SecondsLeft = 0;
		s_Rows = null;
	}

	static bool ReadPhase(ParamsReadContext ctx)
	{
		bool inMatch;
		bool debugOn;
		bool wasMatch;

		if (!ctx.Read(inMatch))
		{
			return false;
		}

		debugOn = false;
		if (!ctx.Read(debugOn))
		{
			debugOn = false;
		}

		wasMatch = s_InMatch;
		s_PhaseKnown = true;
		s_InMatch = inMatch;
		s_Debug = debugOn;
		if (inMatch)
		{
			if (!wasMatch)
			{
				s_OpenFinal = false;
				s_CloseMenu = true;
			}
		}

		if (LogOn())
		{
			if (inMatch)
			{
				Print("[OMD_DM][UI] phase inMatch=true");
			}
			else
			{
				Print("[OMD_DM][UI] phase inMatch=false");
			}
		}

		return true;
	}

	static bool LogOn()
	{
		if (s_Debug)
		{
			return true;
		}

		if (OMD_DM_VoteClient.s_Debug)
		{
			return true;
		}

		return false;
	}

	static bool Read(ParamsReadContext ctx, bool finalSnap)
	{
		int count;
		int i;
		int kills;
		int deaths;
		int assists;
		int points;
		string name;
		bool debugOn;
		int timeLeft;
		array<ref OMD_DM_BoardRow> rows;
		OMD_DM_BoardRow row;

		if (!ctx.Read(count))
		{
			return false;
		}

		if (count < 0)
		{
			return false;
		}

		if (count > OMD_DM_Rpc.BOARD_MAX)
		{
			return false;
		}

		rows = new array<ref OMD_DM_BoardRow>();
		for (i = 0; i < count; i++)
		{
			if (!ctx.Read(name))
			{
				return false;
			}

			if (!ctx.Read(kills))
			{
				return false;
			}

			if (!ctx.Read(deaths))
			{
				return false;
			}

			if (!ctx.Read(assists))
			{
				return false;
			}

			if (!ctx.Read(points))
			{
				return false;
			}

			row = new OMD_DM_BoardRow();
			row.name = name;
			row.kills = kills;
			row.deaths = deaths;
			row.assists = assists;
			row.points = points;
			rows.Insert(row);
		}

		debugOn = false;
		if (!ctx.Read(debugOn))
		{
			debugOn = false;
		}

		timeLeft = 0;
		s_TimeKnown = false;
		if (ctx.Read(timeLeft))
		{
			s_SecondsLeft = timeLeft;
			s_TimeKnown = true;
		}

		s_Rows = rows;
		s_Refresh = true;
		s_Debug = debugOn;
		if (LogOn())
		{
			if (finalSnap)
			{
				Print("[OMD_DM][BOARD] final recv rows=" + count.ToString());
			}
			else
			{
				if (s_TimeKnown)
				{
					Print("[OMD_DM][BOARD] live recv rows=" + count.ToString() + " time=" + timeLeft.ToString());
				}
				else
				{
					Print("[OMD_DM][BOARD] live recv rows=" + count.ToString());
				}
			}
		}

		if (finalSnap)
		{
			s_InMatch = false;
			s_PhaseKnown = true;
			s_HasFinal = true;
			s_OpenFinal = true;
			s_CloseMenu = false;
			return true;
		}

		s_InMatch = true;
		s_PhaseKnown = true;
		s_OpenFinal = false;
		s_CloseMenu = true;
		return true;
	}
};
