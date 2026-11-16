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

	void OMD_DM_BoardMenu()
	{
		m_Excluded = false;
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

		ui.ShowUICursor(true);
		GetGame().GetMission().AddActiveInputExcludes({"menu"});
		m_Excluded = true;
	}

	override void OnHide()
	{
		DropExcludes();
		super.OnHide();
	}

	override void OnPlayerDeath()
	{
		DropExcludes();
		super.OnPlayerDeath();
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

		OMD_DM_VoteClient.s_UserClosed = false;
		OMD_DM_VoteClient.s_ForceOpen = true;
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
