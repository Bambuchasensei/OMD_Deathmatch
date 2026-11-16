#ifndef SERVER
modded class MissionGameplay
{
	protected UAInput m_OMD_DM_VoteKey;
	protected ref OMD_DM_VoteMenu m_OMD_DM_VoteMenu;
	protected UAInput m_OMD_DM_BoardKey;
	protected ref OMD_DM_BoardMenu m_OMD_DM_BoardMenu;
	protected ref OMD_DM_BoardHud m_OMD_DM_BoardHud;
	protected float m_OMD_DM_BoardWait;
	protected bool m_OMD_DM_BoardHeld;
	protected bool m_OMD_DM_BoardShown;

	override void OnInit()
	{
		UAInputAPI inputApi;

		super.OnInit();
		inputApi = GetUApi();
		if (!inputApi)
		{
			return;
		}

		m_OMD_DM_VoteKey = inputApi.GetInputByName("UAOMDDMVote");
		m_OMD_DM_BoardKey = inputApi.GetInputByName("UAOMDDMScoreboard");
		m_OMD_DM_BoardWait = 2;
	}

	override void OnMissionFinish()
	{
		m_OMD_DM_BoardHeld = false;
		m_OMD_DM_BoardShown = false;
		CloseBoard();
		CloseVoteMenu();
		OMD_DM_VoteClient.Reset();
		super.OnMissionFinish();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		TickBoard(timeslice);
		TickVote(timeslice);
	}

	protected void TickVote(float timeslice)
	{
		UIManager ui;
		bool wantOpen;
		bool forced;

		if (!GetGame())
		{
			return;
		}

		ui = GetGame().GetUIManager();
		if (!ui)
		{
			return;
		}

		if (m_OMD_DM_VoteMenu)
		{
			if (ui.GetMenu() != m_OMD_DM_VoteMenu)
			{
				m_OMD_DM_VoteMenu = null;
			}
		}

		if (OMD_DM_VoteClient.s_Close)
		{
			OMD_DM_VoteClient.s_Close = false;
			CloseVoteMenu();
		}

		wantOpen = false;
		forced = false;
		if (OMD_DM_VoteClient.s_ForceOpen)
		{
			OMD_DM_VoteClient.s_ForceOpen = false;
			OMD_DM_VoteClient.s_UserClosed = false;
			wantOpen = true;
			forced = true;
		}

		if (PressedVoteKey())
		{
			if (OMD_DM_VoteClient.s_Active)
			{
				if (!m_OMD_DM_VoteMenu)
				{
					OMD_DM_VoteClient.s_UserClosed = false;
					wantOpen = true;
					forced = false;
				}
			}
		}

		if (!m_OMD_DM_VoteMenu)
		{
			if (OMD_DM_VoteClient.s_Active)
			{
				if (!OMD_DM_VoteClient.s_UserClosed)
				{
					if (!ui.GetMenu())
					{
						wantOpen = true;
						forced = false;
					}
				}
			}
		}

		if (wantOpen)
		{
			if (forced)
			{
				if (BoardCoversVote())
				{
					wantOpen = false;
				}
			}
		}

		if (wantOpen)
		{
			OpenVoteMenu();
		}

		if (!m_OMD_DM_VoteMenu)
		{
			OMD_DM_VoteClient.s_Refresh = false;
			return;
		}

		if (OMD_DM_VoteClient.s_Refresh)
		{
			OMD_DM_VoteClient.s_Refresh = false;
			m_OMD_DM_VoteMenu.Apply();
		}

		m_OMD_DM_VoteMenu.Tick(timeslice);
	}

	protected bool BoardCoversVote()
	{
		UIManager ui;

		if (!m_OMD_DM_BoardMenu)
		{
			return false;
		}

		if (!GetGame())
		{
			return false;
		}

		ui = GetGame().GetUIManager();
		if (!ui)
		{
			return false;
		}

		if (ui.GetMenu() != m_OMD_DM_BoardMenu)
		{
			return false;
		}

		return true;
	}

	protected bool PressedVoteKey()
	{
		if (!m_OMD_DM_VoteKey)
		{
			return false;
		}

		return m_OMD_DM_VoteKey.LocalPress();
	}

	protected void OpenVoteMenu()
	{
		UIManager ui;

		if (!OMD_DM_VoteClient.s_Active)
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

		if (m_OMD_DM_VoteMenu)
		{
			m_OMD_DM_VoteMenu.Apply();
			return;
		}

		m_OMD_DM_VoteMenu = new OMD_DM_VoteMenu();
		ui.ShowScriptedMenu(m_OMD_DM_VoteMenu, null);
		if (ui.GetMenu() != m_OMD_DM_VoteMenu)
		{
			m_OMD_DM_VoteMenu = null;
		}
	}

	protected void CloseVoteMenu()
	{
		if (!m_OMD_DM_VoteMenu)
		{
			return;
		}

		m_OMD_DM_VoteMenu.Shutdown();
		m_OMD_DM_VoteMenu = null;
	}

	protected void TickBoard(float timeslice)
	{
		UIManager ui;
		bool pressed;
		bool released;

		if (!GetGame())
		{
			return;
		}

		ui = GetGame().GetUIManager();
		if (!ui)
		{
			return;
		}

		if (m_OMD_DM_BoardMenu)
		{
			if (ui.GetMenu() != m_OMD_DM_BoardMenu)
			{
				m_OMD_DM_BoardMenu = null;
			}
		}

		if (OMD_DM_BoardClient.s_CloseMenu)
		{
			OMD_DM_BoardClient.s_CloseMenu = false;
			CloseBoardMenu();
		}

		if (OMD_DM_BoardClient.s_OpenFinal)
		{
			OMD_DM_BoardClient.s_OpenFinal = false;
			OpenBoard();
		}

		pressed = PressedBoard();
		released = ReleasedBoard();
		if (pressed)
		{
			m_OMD_DM_BoardHeld = true;
			if (OMD_DM_BoardClient.LogOn())
			{
				Print("[OMD_DM][BOARD] key down");
			}
		}

		if (ui.GetMenu())
		{
			m_OMD_DM_BoardHeld = false;
		}

		if (pressed)
		{
			if (m_OMD_DM_BoardHeld)
			{
				SendBoardRequest();
				m_OMD_DM_BoardWait = 0;
			}
		}

		if (released)
		{
			m_OMD_DM_BoardHeld = false;
			if (OMD_DM_BoardClient.LogOn())
			{
				Print("[OMD_DM][BOARD] key up");
			}

			if (OMD_DM_BoardClient.s_HasFinal)
			{
				if (!OMD_DM_BoardClient.s_InMatch)
				{
					if (!ui.GetMenu())
					{
						OpenBoard();
					}
				}
			}
		}

		if (ui.GetMenu())
		{
			m_OMD_DM_BoardHeld = false;
		}

		if (m_OMD_DM_BoardHeld)
		{
			m_OMD_DM_BoardWait = m_OMD_DM_BoardWait + timeslice;
			if (m_OMD_DM_BoardWait >= 2)
			{
				m_OMD_DM_BoardWait = 0;
				SendBoardRequest();
			}

			ShowBoardHud();
		}
		else
		{
			m_OMD_DM_BoardWait = 2;
			HideBoardHud();
		}

		if (OMD_DM_BoardClient.s_Refresh)
		{
			OMD_DM_BoardClient.s_Refresh = false;
			if (m_OMD_DM_BoardHud)
			{
				m_OMD_DM_BoardHud.Apply();
			}

			if (m_OMD_DM_BoardMenu)
			{
				m_OMD_DM_BoardMenu.Apply();
			}
		}

		if (m_OMD_DM_BoardMenu)
		{
			m_OMD_DM_BoardMenu.Tick();
		}
	}

	protected bool PressedBoard()
	{
		if (!m_OMD_DM_BoardKey)
		{
			return false;
		}

		return m_OMD_DM_BoardKey.LocalPress();
	}

	protected bool ReleasedBoard()
	{
		if (!m_OMD_DM_BoardKey)
		{
			return false;
		}

		return m_OMD_DM_BoardKey.LocalRelease();
	}

	protected void SendBoardRequest()
	{
		PlayerBase player;
		ScriptRPC rpc;

		if (!GetGame())
		{
			return;
		}

		if (!Class.CastTo(player, GetGame().GetPlayer()))
		{
			return;
		}

		rpc = new ScriptRPC();
		rpc.Send(player, OMD_DM_Rpc.BOARD_REQ, true, null);
		if (OMD_DM_BoardClient.LogOn())
		{
			Print("[OMD_DM][BOARD] req sent");
		}
	}

	protected void ShowBoardHud()
	{
		if (!m_OMD_DM_BoardHud)
		{
			m_OMD_DM_BoardHud = new OMD_DM_BoardHud();
		}

		m_OMD_DM_BoardHud.ShowBoard(true);
		if (!m_OMD_DM_BoardShown)
		{
			m_OMD_DM_BoardShown = true;
			m_OMD_DM_BoardHud.Apply();
		}
	}

	protected void HideBoardHud()
	{
		m_OMD_DM_BoardShown = false;
		if (!m_OMD_DM_BoardHud)
		{
			return;
		}

		m_OMD_DM_BoardHud.ShowBoard(false);
	}

	protected void OpenBoard()
	{
		UIManager ui;

		HideBoardHud();
		if (!GetGame())
		{
			return;
		}

		ui = GetGame().GetUIManager();
		if (!ui)
		{
			return;
		}

		if (m_OMD_DM_BoardMenu)
		{
			m_OMD_DM_BoardMenu.Apply();
			return;
		}

		m_OMD_DM_BoardMenu = new OMD_DM_BoardMenu();
		ui.ShowScriptedMenu(m_OMD_DM_BoardMenu, null);
		if (ui.GetMenu() != m_OMD_DM_BoardMenu)
		{
			m_OMD_DM_BoardMenu = null;
		}
	}

	protected void CloseBoardMenu()
	{
		if (!m_OMD_DM_BoardMenu)
		{
			return;
		}

		m_OMD_DM_BoardMenu.Shutdown();
		m_OMD_DM_BoardMenu = null;
	}

	protected void CloseBoard()
	{
		CloseBoardMenu();
		m_OMD_DM_BoardShown = false;
		if (m_OMD_DM_BoardHud)
		{
			m_OMD_DM_BoardHud.Destroy();
			m_OMD_DM_BoardHud = null;
		}

		OMD_DM_BoardClient.Reset();
	}
};
#endif
