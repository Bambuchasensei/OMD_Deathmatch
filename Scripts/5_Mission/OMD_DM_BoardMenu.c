#ifndef SERVER
class OMD_DM_BoardMenu extends UIScriptedMenu
{
	protected static const string MENU_LAYOUT = "OMD_Deathmatch/gui/layouts/omd_dm_board_menu.layout";
	protected static const int COLOR_ON = 0xF2336138;
	protected static const int COLOR_OFF = 0xF22A2A2A;

	protected Widget m_CloseButton;
	protected Widget m_VoteButton;
	protected Widget m_VoteBack;
	protected ref OMD_DM_BoardLines m_Top;
	protected ref OMD_DM_BoardLines m_List;
	protected bool m_Excluded;
	protected bool m_Focus;

	void OMD_DM_BoardMenu()
	{
		m_Excluded = false;
		m_Focus = false;
	}

	override Widget Init()
	{
		Widget topHost;
		Widget listHost;

		if (!GetGame())
		{
			return null;
		}

		if (!GetGame().GetWorkspace())
		{
			return null;
		}

		layoutRoot = GetGame().GetWorkspace().CreateWidgets(MENU_LAYOUT);
		if (!layoutRoot)
		{
			return null;
		}

		m_CloseButton = layoutRoot.FindAnyWidget("CloseButton");
		m_VoteButton = layoutRoot.FindAnyWidget("VoteButton");
		m_VoteBack = layoutRoot.FindAnyWidget("VoteBack");
		topHost = layoutRoot.FindAnyWidget("TopHost");
		listHost = layoutRoot.FindAnyWidget("ListHost");
		m_Top = new OMD_DM_BoardLines(topHost);
		m_List = new OMD_DM_BoardLines(listHost);
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

		GetGame().GetMission().AddActiveInputExcludes({"menu"});
		m_Excluded = true;
		KeepCursor();
	}

	override void OnHide()
	{
		DropExcludes();
		super.OnHide();
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);
		KeepCursor();
	}

	override bool IsHandlingPlayerDeathEvent()
	{
		return false;
	}

	override void OnPlayerDeath()
	{
	}

	override bool OnKeyDown(Widget w, int x, int y, int key)
	{
		if (key == KeyCode.KC_ESCAPE)
		{
			OMD_DM_Ui.s_Esc = true;
			return true;
		}

		if (key == KeyCode.KC_TAB)
		{
			return true;
		}

		return super.OnKeyDown(w, x, y, key);
	}

	void Shutdown()
	{
		DropExcludes();
		Close();
	}

	void Apply()
	{
		if (m_Top)
		{
			m_Top.ShowRows(0.33);
		}

		if (m_List)
		{
			m_List.ShowRows(0.0);
		}

		RefreshVote();
	}

	void Tick()
	{
		RefreshVote();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);
		if (Hits(m_CloseButton, w))
		{
			OMD_DM_Ui.Log("close", "board", "x");
			Shutdown();
			return true;
		}

		if (Hits(m_VoteButton, w))
		{
			OpenVote();
			return true;
		}

		return false;
	}

	protected void OpenVote()
	{
		if (!OMD_DM_VoteClient.s_Active)
		{
			return;
		}

		OMD_DM_VoteClient.s_Dismissed = false;
		OMD_DM_VoteClient.s_WantAuto = false;
		OMD_DM_VoteClient.s_SwitchVote = true;
		OMD_DM_Ui.Log("close", "board", "key");
		Shutdown();
	}

	protected void RefreshVote()
	{
		if (!m_VoteButton)
		{
			return;
		}

		if (OMD_DM_VoteClient.s_Active)
		{
			m_VoteButton.Enable(true);
			if (m_VoteBack)
			{
				m_VoteBack.SetColor(COLOR_ON);
			}

			return;
		}

		m_VoteButton.Enable(false);
		if (m_VoteBack)
		{
			m_VoteBack.SetColor(COLOR_OFF);
		}
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

		if (m_Focus)
		{
			if (GetGame().GetInput())
			{
				GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_MOUSE);
				GetGame().GetInput().ChangeGameFocus(-1, INPUT_DEVICE_KEYBOARD);
			}

			m_Focus = false;
		}

		mission = GetGame().GetMission();
		if (!mission)
		{
			return;
		}

		mission.RemoveActiveInputExcludes({"menu"}, true);
	}

	protected void KeepCursor()
	{
		UIManager ui;

		if (!m_Excluded)
		{
			return;
		}

		if (!GetGame())
		{
			return;
		}

		ui = GetGame().GetUIManager();
		if (!ui)
		{
			return;
		}

		if (ui.IsCursorVisible())
		{
			return;
		}

		if (!m_Focus)
		{
			if (GetGame().GetInput())
			{
				GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_MOUSE);
				GetGame().GetInput().ChangeGameFocus(1, INPUT_DEVICE_KEYBOARD);
				m_Focus = true;
			}
		}

		ui.ShowUICursor(true);
	}
};
#endif
