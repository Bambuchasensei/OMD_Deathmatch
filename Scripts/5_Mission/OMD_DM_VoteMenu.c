#ifndef SERVER
class OMD_DM_VoteRow
{
	Widget root;
	Widget back;
	TextWidget label;
	TextWidget count;
	int number;
	string id;
};

class OMD_DM_VoteMenu extends UIScriptedMenu
{
	protected static const int COLOR_IDLE = 0xF5282828;
	protected static const int COLOR_ON = 0xF5C8883A;
	protected static const string CHOICE_LAYOUT = "OMD_Deathmatch/gui/layouts/omd_dm_vote_choice.layout";

	protected Widget m_CloseButton;
	protected Widget m_VoteButton;
	protected Widget m_ModeButton;
	protected Widget m_ModeBack;
	protected Widget m_ArenaHost;
	protected Widget m_LevelHost;
	protected TextWidget m_Timer;
	protected TextWidget m_Status;
	protected ref array<ref OMD_DM_VoteRow> m_ArenaRows;
	protected ref array<ref OMD_DM_VoteRow> m_LevelRows;
	protected bool m_Excluded;
	protected float m_Second;

	void OMD_DM_VoteMenu()
	{
		m_ArenaRows = new array<ref OMD_DM_VoteRow>();
		m_LevelRows = new array<ref OMD_DM_VoteRow>();
		m_Excluded = false;
		m_Second = 0;
	}

	override Widget Init()
	{
		layoutRoot = g_Game.GetWorkspace().CreateWidgets("OMD_Deathmatch/gui/layouts/omd_dm_vote.layout");
		if (!layoutRoot)
		{
			return null;
		}

		m_CloseButton = layoutRoot.FindAnyWidget("CloseButton");
		m_VoteButton = layoutRoot.FindAnyWidget("VoteButton");
		m_ModeButton = layoutRoot.FindAnyWidget("ModeButton");
		m_ModeBack = layoutRoot.FindAnyWidget("ModeBack");
		m_ArenaHost = layoutRoot.FindAnyWidget("ArenaHost");
		m_LevelHost = layoutRoot.FindAnyWidget("LevelHost");
		m_Timer = TextWidget.Cast(layoutRoot.FindAnyWidget("TimerText"));
		m_Status = TextWidget.Cast(layoutRoot.FindAnyWidget("StatusText"));
		if (m_ModeBack)
		{
			m_ModeBack.SetColor(COLOR_ON);
		}

		if (m_ModeButton)
		{
			m_ModeButton.Enable(false);
		}

		Apply();
		return layoutRoot;
	}

	override void OnShow()
	{
		UIManager ui;

		super.OnShow();
		if (m_Excluded)
		{
			return;
		}

		if (!GetGame())
		{
			return;
		}

		if (!GetGame().GetMission())
		{
			return;
		}

		ui = GetGame().GetUIManager();
		if (!ui)
		{
			return;
		}

		ui.ShowUICursor(true);
		GetGame().GetMission().AddActiveInputExcludes({"menu"});
		m_Excluded = true;
	}

	override void OnHide()
	{
		DropExcludes();
		super.OnHide();
	}

	override bool IsHandlingPlayerDeathEvent()
	{
		return false;
	}

	override void OnPlayerDeath()
	{
	}

	void Shutdown()
	{
		DropExcludes();
		Close();
	}

	void Apply()
	{
		int pickArena;
		string pickLevel;

		m_Second = 0;
		BuildRows(m_ArenaHost, m_ArenaRows, OMD_DM_VoteClient.s_Arenas, true);
		BuildRows(m_LevelHost, m_LevelRows, OMD_DM_VoteClient.s_Levels, false);
		pickArena = OMD_DM_VoteClient.s_PendingArena;
		pickLevel = OMD_DM_VoteClient.s_PendingLevel;
		if (pickArena < 1)
		{
			pickArena = OMD_DM_VoteClient.s_MyArena;
		}

		if (pickLevel == "")
		{
			pickLevel = OMD_DM_VoteClient.s_MyLevel;
		}

		PaintRows(m_ArenaRows, true, pickArena, "");
		PaintRows(m_LevelRows, false, 0, pickLevel);
		SetTimer(OMD_DM_VoteClient.s_Seconds);
		SetStatus("");
	}

	void Tick(float timeslice)
	{
		m_Second = m_Second + timeslice;
		if (m_Second < 1)
		{
			return;
		}

		m_Second = m_Second - 1;
		if (OMD_DM_VoteClient.s_Seconds > 0)
		{
			OMD_DM_VoteClient.s_Seconds = OMD_DM_VoteClient.s_Seconds - 1;
		}

		SetTimer(OMD_DM_VoteClient.s_Seconds);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int arenaIndex;
		int levelIndex;
		OMD_DM_VoteRow row;

		super.OnClick(w, x, y, button);
		if (Hits(m_CloseButton, w))
		{
			OMD_DM_VoteClient.s_UserClosed = true;
			Shutdown();
			return true;
		}

		if (Hits(m_VoteButton, w))
		{
			Submit();
			return true;
		}

		if (Hits(m_ModeButton, w))
		{
			return true;
		}

		arenaIndex = RowIndex(m_ArenaRows, w);
		if (arenaIndex >= 0)
		{
			row = m_ArenaRows.Get(arenaIndex);
			if (row)
			{
				OMD_DM_VoteClient.s_PendingArena = row.number;
				PaintRows(m_ArenaRows, true, row.number, "");
				SendChoice(row.number, "");
			}

			return true;
		}

		levelIndex = RowIndex(m_LevelRows, w);
		if (levelIndex >= 0)
		{
			row = m_LevelRows.Get(levelIndex);
			if (row)
			{
				OMD_DM_VoteClient.s_PendingLevel = row.id;
				PaintRows(m_LevelRows, false, 0, row.id);
				SendChoice(0, row.id);
			}

			return true;
		}

		return false;
	}

	protected void Submit()
	{
		int arenaNumber;
		string levelId;

		arenaNumber = OMD_DM_VoteClient.s_PendingArena;
		levelId = OMD_DM_VoteClient.s_PendingLevel;
		if (arenaNumber < 1)
		{
			if (levelId == "")
			{
				SetStatus("Выбери арену или уровень");
				return;
			}
		}

		SendChoice(arenaNumber, levelId);
	}

	protected void SendChoice(int arenaNumber, string levelId)
	{
		PlayerBase player;
		ScriptRPC rpc;
		bool living;

		if (!GetGame())
		{
			SetStatus("Нет игры");
			if (OMD_DM_VoteClient.s_Debug)
			{
				Print("[OMD_DM][VOTE] rpc choice no-game arena=" + arenaNumber.ToString() + " level=" + levelId);
			}

			return;
		}

		living = false;
		player = null;
		if (Class.CastTo(player, GetGame().GetPlayer()))
		{
			if (player.IsAlive())
			{
				living = true;
			}
		}

		rpc = new ScriptRPC();
		rpc.Write(arenaNumber);
		rpc.Write(levelId);
		if (living)
		{
			rpc.Send(player, OMD_DM_Rpc.CHOICE, true, null);
			SetStatus("Голос отправлен");
			if (OMD_DM_VoteClient.s_Debug)
			{
				Print("[OMD_DM][VOTE] rpc choice arena=" + arenaNumber.ToString() + " level=" + levelId);
			}

			return;
		}

		rpc.Send(null, OMD_DM_Rpc.CHOICE, true, null);
		SetStatus("Нет персонажа, голос отправлен");
		if (OMD_DM_VoteClient.s_Debug)
		{
			Print("[OMD_DM][VOTE] rpc choice no-player arena=" + arenaNumber.ToString() + " level=" + levelId);
		}
	}

	protected void BuildRows(Widget host, array<ref OMD_DM_VoteRow> rows, array<ref OMD_DM_VoteOption> options, bool useNumber)
	{
		int i;
		int count;
		float slot;
		float y;
		Widget created;
		OMD_DM_VoteRow row;
		OMD_DM_VoteOption option;

		ClearRows(rows);
		if (!host)
		{
			return;
		}

		if (!GetGame())
		{
			return;
		}

		if (!options)
		{
			return;
		}

		count = options.Count();
		if (count < 1)
		{
			return;
		}

		slot = 1.0;
		slot = slot / count;
		if (slot > 0.34)
		{
			slot = 0.34;
		}

		for (i = 0; i < count; i++)
		{
			option = options.Get(i);
			created = GetGame().GetWorkspace().CreateWidgets(CHOICE_LAYOUT, host);
			if (!created)
			{
				continue;
			}

			row = new OMD_DM_VoteRow();
			row.root = created;
			row.back = created.FindAnyWidget("ChoiceBack");
			row.label = TextWidget.Cast(created.FindAnyWidget("ChoiceLabel"));
			row.count = TextWidget.Cast(created.FindAnyWidget("ChoiceCount"));
			row.number = 0;
			row.id = "";
			if (option)
			{
				row.number = option.number;
				row.id = option.id;
				if (row.label)
				{
					if (useNumber)
					{
						row.label.SetText(option.number.ToString() + ". " + option.title);
					}
					else
					{
						row.label.SetText(option.title);
					}
				}

				if (row.count)
				{
					row.count.SetText(option.votes.ToString());
				}
			}

			y = i * slot;
			created.SetPos(0, y);
			created.SetSize(1, slot * 0.92);
			rows.Insert(row);
		}
	}

	protected void ClearRows(array<ref OMD_DM_VoteRow> rows)
	{
		int i;
		OMD_DM_VoteRow row;

		if (!rows)
		{
			return;
		}

		for (i = 0; i < rows.Count(); i++)
		{
			row = rows.Get(i);
			if (!row)
			{
				continue;
			}

			if (row.root)
			{
				row.root.Unlink();
			}
		}

		rows.Clear();
	}

	protected void PaintRows(array<ref OMD_DM_VoteRow> rows, bool byNumber, int number, string id)
	{
		int i;
		int color;
		bool selected;
		OMD_DM_VoteRow row;

		if (!rows)
		{
			return;
		}

		for (i = 0; i < rows.Count(); i++)
		{
			row = rows.Get(i);
			if (!row)
			{
				continue;
			}

			selected = false;
			if (byNumber)
			{
				if (row.number == number)
				{
					selected = true;
				}
			}
			else
			{
				if (row.id == id)
				{
					if (id != "")
					{
						selected = true;
					}
				}
			}

			color = COLOR_IDLE;
			if (selected)
			{
				color = COLOR_ON;
			}

			if (row.back)
			{
				row.back.SetColor(color);
			}
		}
	}

	protected int RowIndex(array<ref OMD_DM_VoteRow> rows, Widget w)
	{
		int i;
		int guard;
		Widget node;
		OMD_DM_VoteRow row;

		if (!rows)
		{
			return -1;
		}

		node = w;
		guard = 0;
		while (node)
		{
			guard = guard + 1;
			if (guard > 8)
			{
				return -1;
			}

			for (i = 0; i < rows.Count(); i++)
			{
				row = rows.Get(i);
				if (!row)
				{
					continue;
				}

				if (row.root == node)
				{
					return i;
				}
			}

			node = node.GetParent();
		}

		return -1;
	}

	protected bool Hits(Widget target, Widget w)
	{
		int guard;
		Widget node;

		if (!target)
		{
			return false;
		}

		node = w;
		guard = 0;
		while (node)
		{
			guard = guard + 1;
			if (guard > 8)
			{
				return false;
			}

			if (node == target)
			{
				return true;
			}

			node = node.GetParent();
		}

		return false;
	}

	protected void SetTimer(int seconds)
	{
		if (!m_Timer)
		{
			return;
		}

		m_Timer.SetText("Осталось " + seconds.ToString() + " сек");
	}

	protected void SetStatus(string text)
	{
		if (!m_Status)
		{
			return;
		}

		m_Status.SetText(text);
	}

	protected void DropExcludes()
	{
		Mission mission;

		if (!m_Excluded)
		{
			return;
		}

		m_Excluded = false;
		if (!GetGame())
		{
			return;
		}

		if (GetGame().GetUIManager())
		{
			GetGame().GetUIManager().ShowUICursor(false);
		}

		mission = GetGame().GetMission();
		if (!mission)
		{
			return;
		}

		mission.RemoveActiveInputExcludes({"menu"}, true);
	}
};
#endif
