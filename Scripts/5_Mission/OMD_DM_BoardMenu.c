#ifndef SERVER
class OMD_DM_BoardMenu extends UIScriptedMenu
{
	protected static const string MENU_LAYOUT = "OMD_Deathmatch/gui/layouts/omd_dm_board_menu.layout";
	protected static const int COLOR_BTN = 0xFF8E1B1B;
	protected static const int COLOR_BTN_HOVER = 0xFFA32222;
	protected static const int COLOR_BTN_LINE = 0xFFC9403A;
	protected static const int COLOR_OFF = 0xFF1A1C1A;
	protected static const int COLOR_OFF_LINE = 0xFF34372F;
	protected static const int COLOR_FOOT = 0xFF1A1C1A;
	protected static const int COLOR_FOOT_HOVER = 0xFF242724;
	protected static const int COLOR_MUTED = 0xFF9A9C96;
	protected static const int COLOR_NEG = 0xFFD9A441;

	protected Widget m_CloseButton;
	protected Widget m_CloseFoot;
	protected Widget m_CloseFootBack;
	protected Widget m_VoteButton;
	protected Widget m_VoteBack;
	protected Widget m_VoteBorder;
	protected Widget m_P1Card;
	protected Widget m_P2Card;
	protected Widget m_P3Card;
	protected TextWidget m_P1Name;
	protected TextWidget m_P2Name;
	protected TextWidget m_P3Name;
	protected TextWidget m_P1Score;
	protected TextWidget m_P2Score;
	protected TextWidget m_P3Score;
	protected ref OMD_DM_BoardLines m_List;
	protected bool m_Excluded;
	protected bool m_Focus;
	protected bool m_VoteHot;

	void OMD_DM_BoardMenu()
	{
		m_Excluded = false;
		m_Focus = false;
		m_VoteHot = false;
	}

	override Widget Init()
	{
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
		m_CloseFoot = layoutRoot.FindAnyWidget("CloseFoot");
		m_CloseFootBack = layoutRoot.FindAnyWidget("CloseFootBack");
		m_VoteButton = layoutRoot.FindAnyWidget("VoteButton");
		m_VoteBack = layoutRoot.FindAnyWidget("VoteBack");
		m_VoteBorder = layoutRoot.FindAnyWidget("VoteBorder");
		m_P1Card = layoutRoot.FindAnyWidget("P1Card");
		m_P2Card = layoutRoot.FindAnyWidget("P2Card");
		m_P3Card = layoutRoot.FindAnyWidget("P3Card");
		m_P1Name = TextWidget.Cast(layoutRoot.FindAnyWidget("P1Name"));
		m_P2Name = TextWidget.Cast(layoutRoot.FindAnyWidget("P2Name"));
		m_P3Name = TextWidget.Cast(layoutRoot.FindAnyWidget("P3Name"));
		m_P1Score = TextWidget.Cast(layoutRoot.FindAnyWidget("P1Score"));
		m_P2Score = TextWidget.Cast(layoutRoot.FindAnyWidget("P2Score"));
		m_P3Score = TextWidget.Cast(layoutRoot.FindAnyWidget("P3Score"));
		listHost = layoutRoot.FindAnyWidget("ListHost");
		m_List = new OMD_DM_BoardLines(listHost);
		OMD_DM_Logo.Apply(layoutRoot);
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
		FillPodium();
		if (m_List)
		{
			m_List.ShowRows(0, false);
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

		if (Hits(m_CloseFoot, w))
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
			PaintVote(m_VoteHot);
			return;
		}

		m_VoteHot = false;
		m_VoteButton.Enable(false);
		if (m_VoteBack)
		{
			m_VoteBack.SetColor(COLOR_OFF);
		}

		if (m_VoteBorder)
		{
			m_VoteBorder.SetColor(COLOR_OFF_LINE);
		}
	}

	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (Hits(m_VoteButton, w))
		{
			if (OMD_DM_VoteClient.s_Active)
			{
				m_VoteHot = true;
				PaintVote(true);
			}
		}

		if (Hits(m_CloseFoot, w))
		{
			if (m_CloseFootBack)
			{
				m_CloseFootBack.SetColor(COLOR_FOOT_HOVER);
			}
		}

		return false;
	}

	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (Hits(m_VoteButton, w))
		{
			m_VoteHot = false;
			RefreshVote();
		}

		if (Hits(m_CloseFoot, w))
		{
			if (m_CloseFootBack)
			{
				m_CloseFootBack.SetColor(COLOR_FOOT);
			}
		}

		return false;
	}

	protected void PaintVote(bool hot)
	{
		if (m_VoteBorder)
		{
			m_VoteBorder.SetColor(COLOR_BTN_LINE);
		}

		if (!m_VoteBack)
		{
			return;
		}

		if (hot)
		{
			m_VoteBack.SetColor(COLOR_BTN_HOVER);
			return;
		}

		m_VoteBack.SetColor(COLOR_BTN);
	}

	protected void FillPodium()
	{
		FillSlot(m_P1Card, m_P1Name, m_P1Score, 0);
		FillSlot(m_P2Card, m_P2Name, m_P2Score, 1);
		FillSlot(m_P3Card, m_P3Name, m_P3Score, 2);
	}

	protected void FillSlot(Widget card, TextWidget nameBox, TextWidget scoreBox, int index)
	{
		OMD_DM_BoardRow data;

		data = null;
		if (OMD_DM_BoardClient.s_Rows)
		{
			if (index >= 0)
			{
				if (index < OMD_DM_BoardClient.s_Rows.Count())
				{
					data = OMD_DM_BoardClient.s_Rows.Get(index);
				}
			}
		}

		if (!card)
		{
			return;
		}

		if (!data)
		{
			card.Show(false);
			return;
		}

		card.Show(true);
		if (nameBox)
		{
			nameBox.SetText(data.name);
		}

		if (!scoreBox)
		{
			return;
		}

		scoreBox.SetText(data.points.ToString());
		if (data.points < 0)
		{
			scoreBox.SetColor(COLOR_NEG);
			return;
		}

		scoreBox.SetColor(COLOR_MUTED);
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
