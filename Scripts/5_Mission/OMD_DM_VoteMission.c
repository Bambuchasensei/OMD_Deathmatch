#ifndef SERVER
modded class MissionGameplay
{
	protected UAInput m_OMD_DM_VoteKey;
	protected ref OMD_DM_VoteMenu m_OMD_DM_VoteMenu;
	protected UAInput m_OMD_DM_BoardKey;
	protected ref OMD_DM_BoardMenu m_OMD_DM_BoardMenu;
	protected ref OMD_DM_BoardHud m_OMD_DM_BoardHud;
	protected float m_OMD_DM_BoardWait;
	protected bool m_OMD_DM_BoardShown;
	protected bool m_OMD_DM_DeadLock;
	protected PlayerBase m_OMD_DM_SeenPlayer;

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
		CloseVote("auto");
		CloseBoardMenu("auto");
		HideHud("auto");
		if (m_OMD_DM_BoardHud)
		{
			m_OMD_DM_BoardHud.Destroy();
			m_OMD_DM_BoardHud = null;
		}

		m_OMD_DM_BoardShown = false;
		m_OMD_DM_DeadLock = false;
		m_OMD_DM_SeenPlayer = null;
		OMD_DM_VoteClient.Reset();
		OMD_DM_BoardClient.Reset();
		OMD_DM_Ui.Reset();
		super.OnMissionFinish();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		if (!AliveGate())
		{
			return;
		}

		if (CursorOpen())
		{
			ArmMenuKeys();
			if (EatEsc())
			{
				CloseCursor("esc");
			}
		}

		TickKeys();
		TickBoard(timeslice);
		TickVote(timeslice);
	}

	protected bool AliveGate()
	{
		PlayerBase player;
		bool alive;

		alive = false;
		player = null;
		if (GetGame())
		{
			if (Class.CastTo(player, GetGame().GetPlayer()))
			{
				if (player.IsAlive())
				{
					alive = true;
				}
			}
		}

		if (!alive)
		{
			if (!m_OMD_DM_DeadLock)
			{
				m_OMD_DM_DeadLock = true;
				ResetUi("death");
			}

			return false;
		}

		if (m_OMD_DM_DeadLock)
		{
			m_OMD_DM_DeadLock = false;
			ResetUi("death");
		}

		if (m_OMD_DM_SeenPlayer)
		{
			if (m_OMD_DM_SeenPlayer != player)
			{
				ResetUi("death");
			}
		}

		m_OMD_DM_SeenPlayer = player;
		return true;
	}

	protected void ResetUi(string reason)
	{
		OMD_DM_Ui.s_Esc = false;
		OMD_DM_VoteClient.s_SwitchVote = false;
		OMD_DM_VoteClient.s_WantAuto = false;
		OMD_DM_VoteClient.s_Dismissed = true;
		OMD_DM_VoteClient.s_Close = false;
		OMD_DM_BoardClient.s_CloseMenu = false;
		OMD_DM_BoardClient.s_SwitchBoard = false;
		CloseVote(reason);
		CloseBoardMenu(reason);
		HideHud(reason);
	}

	protected void TickKeys()
	{
		if (PressedVote())
		{
			OMD_DM_Ui.LogToggle("vote");
			if (!ForeignMenu())
			{
				ToggleVote();
			}
		}

		if (PressedBoard())
		{
			OMD_DM_Ui.LogToggle("board");
			if (!ForeignMenu())
			{
				ToggleBoard();
			}
		}
	}

	protected void ToggleVote()
	{
		if (VoteMenuIsOpen())
		{
			OMD_DM_VoteClient.s_Dismissed = true;
			OMD_DM_VoteClient.s_WantAuto = false;
			OMD_DM_VoteClient.s_SwitchVote = false;
			CloseVote("key");
			return;
		}

		if (!OMD_DM_VoteClient.s_Active)
		{
			return;
		}

		if (BoardMenuIsOpen())
		{
			CloseBoardMenu("key");
		}

		HideHud("key");
		OMD_DM_VoteClient.s_Dismissed = false;
		OMD_DM_VoteClient.s_WantAuto = false;
		OMD_DM_VoteClient.s_SwitchVote = true;
	}

	protected void ToggleBoard()
	{
		if (LobbyBoard())
		{
			if (BoardMenuIsOpen())
			{
				CloseBoardMenu("key");
				OMD_DM_BoardClient.s_SwitchBoard = false;
				return;
			}

			HideHud("key");
			if (VoteMenuIsOpen())
			{
				CloseVote("key");
			}

			OMD_DM_BoardClient.s_SwitchBoard = true;
			return;
		}

		if (BoardMenuIsOpen())
		{
			CloseBoardMenu("key");
		}

		if (VoteMenuIsOpen())
		{
			CloseVote("key");
		}

		if (m_OMD_DM_BoardShown)
		{
			HideHud("key");
			return;
		}

		m_OMD_DM_BoardWait = 0;
		ShowHud("key");
		SendBoardRequest();
	}

	protected bool LobbyBoard()
	{
		if (!OMD_DM_BoardClient.s_PhaseKnown)
		{
			return false;
		}

		if (OMD_DM_BoardClient.s_InMatch)
		{
			return false;
		}

		if (OMD_DM_BoardClient.s_HasFinal)
		{
			return true;
		}

		if (OMD_DM_VoteClient.s_Active)
		{
			return true;
		}

		return false;
	}

	protected void TickBoard(float timeslice)
	{
		SyncBoardRef();
		if (OMD_DM_BoardClient.s_InMatch)
		{
			OMD_DM_BoardClient.s_SwitchBoard = false;
			OMD_DM_BoardClient.s_OpenFinal = false;
		}

		if (OMD_DM_BoardClient.s_CloseMenu)
		{
			OMD_DM_BoardClient.s_CloseMenu = false;
			CloseBoardMenu("auto");
		}

		if (OMD_DM_BoardClient.s_OpenFinal)
		{
			if (!ForeignMenu())
			{
				if (VoteMenuIsOpen())
				{
					CloseVote("final");
				}
				else
				{
					OMD_DM_BoardClient.s_OpenFinal = false;
					OMD_DM_VoteClient.s_WantAuto = false;
					OMD_DM_VoteClient.s_Dismissed = true;
					HideHud("final");
					OpenBoard("final");
				}
			}
		}

		if (OMD_DM_BoardClient.s_SwitchBoard)
		{
			if (!VoteMenuIsOpen())
			{
				if (!ForeignMenu())
				{
					OMD_DM_BoardClient.s_SwitchBoard = false;
					HideHud("key");
					OpenBoard("key");
				}
			}
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

		if (m_OMD_DM_BoardShown)
		{
			m_OMD_DM_BoardWait = m_OMD_DM_BoardWait + timeslice;
			if (m_OMD_DM_BoardWait >= 2)
			{
				m_OMD_DM_BoardWait = 0;
				SendBoardRequest();
			}
		}

		if (m_OMD_DM_BoardMenu)
		{
			m_OMD_DM_BoardMenu.Tick();
		}
	}

	protected void TickVote(float timeslice)
	{
		SyncVoteRef();
		if (OMD_DM_VoteClient.s_Close)
		{
			OMD_DM_VoteClient.s_Close = false;
			CloseVote("auto");
		}

		if (OMD_DM_BoardClient.s_InMatch)
		{
			if (VoteMenuIsOpen())
			{
				CloseVote("auto");
			}

			OMD_DM_VoteClient.s_SwitchVote = false;
			OMD_DM_VoteClient.s_WantAuto = false;
		}

		if (OMD_DM_VoteClient.s_SwitchVote)
		{
			if (!BoardMenuIsOpen())
			{
				if (OMD_DM_VoteClient.s_Active)
				{
					if (!VoteMenuIsOpen())
					{
						OpenVote("key");
					}

					if (VoteMenuIsOpen())
					{
						OMD_DM_VoteClient.s_SwitchVote = false;
					}
				}
				else
				{
					OMD_DM_VoteClient.s_SwitchVote = false;
				}
			}
		}

		if (OMD_DM_VoteClient.s_WantAuto)
		{
			if (OMD_DM_VoteClient.s_Dismissed)
			{
				OMD_DM_VoteClient.s_WantAuto = false;
			}
			else
			{
				if (OMD_DM_BoardClient.s_HasFinal)
				{
					OMD_DM_VoteClient.s_WantAuto = false;
					OMD_DM_VoteClient.s_Dismissed = true;
				}
				else
				{
					if (OMD_DM_BoardClient.s_OpenFinal)
					{
						OMD_DM_VoteClient.s_WantAuto = false;
					}
					else
					{
						if (BoardMenuIsOpen())
						{
							OMD_DM_VoteClient.s_WantAuto = false;
							OMD_DM_VoteClient.s_Dismissed = true;
						}
						else
						{
							if (!ForeignMenu())
							{
								if (!VoteMenuIsOpen())
								{
									OpenVote("auto");
								}

								OMD_DM_VoteClient.s_WantAuto = false;
							}
						}
					}
				}
			}
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

	protected bool CursorOpen()
	{
		if (VoteMenuIsOpen())
		{
			return true;
		}

		if (BoardMenuIsOpen())
		{
			return true;
		}

		return false;
	}

	protected bool VoteMenuIsOpen()
	{
		if (!m_OMD_DM_VoteMenu)
		{
			return false;
		}

		if (!m_OMD_DM_VoteMenu.IsVisible())
		{
			return false;
		}

		return true;
	}

	protected bool BoardMenuIsOpen()
	{
		if (!m_OMD_DM_BoardMenu)
		{
			return false;
		}

		if (!m_OMD_DM_BoardMenu.IsVisible())
		{
			return false;
		}

		return true;
	}

	protected bool ForeignMenu()
	{
		UIManager ui;
		UIScriptedMenu menu;

		if (!GetGame())
		{
			return false;
		}

		ui = GetGame().GetUIManager();
		if (!ui)
		{
			return false;
		}

		menu = ui.GetMenu();
		if (!menu)
		{
			return false;
		}

		if (menu == m_OMD_DM_VoteMenu)
		{
			return false;
		}

		if (menu == m_OMD_DM_BoardMenu)
		{
			return false;
		}

		return true;
	}

	protected void ArmMenuKeys()
	{
		UAInputAPI inputApi;
		UAInput escKey;
		UAInput backKey;

		inputApi = GetUApi();
		if (!inputApi)
		{
			return;
		}

		escKey = inputApi.GetInputByID(UAUIMenu);
		backKey = inputApi.GetInputByID(UAUIBack);
		if (escKey)
		{
			escKey.Unlock();
		}

		if (backKey)
		{
			backKey.Unlock();
		}
	}

	protected bool EatEsc()
	{
		UAInputAPI inputApi;
		UAInput escKey;
		UAInput backKey;
		bool hit;

		hit = false;
		if (OMD_DM_Ui.s_Esc)
		{
			OMD_DM_Ui.s_Esc = false;
			hit = true;
		}

		inputApi = GetUApi();
		escKey = null;
		backKey = null;
		if (inputApi)
		{
			escKey = inputApi.GetInputByID(UAUIMenu);
			backKey = inputApi.GetInputByID(UAUIBack);
		}

		if (escKey)
		{
			if (escKey.LocalPress())
			{
				hit = true;
			}
		}

		if (backKey)
		{
			if (backKey.LocalPress())
			{
				hit = true;
			}
		}

		if (!hit)
		{
			return false;
		}

		if (escKey)
		{
			escKey.Supress();
		}

		if (backKey)
		{
			backKey.Supress();
		}

		return true;
	}

	protected bool PressedVote()
	{
		if (!m_OMD_DM_VoteKey)
		{
			return false;
		}

		return m_OMD_DM_VoteKey.LocalPress();
	}

	protected bool PressedBoard()
	{
		if (!m_OMD_DM_BoardKey)
		{
			return false;
		}

		return m_OMD_DM_BoardKey.LocalPress();
	}

	protected void SyncVoteRef()
	{
		if (!m_OMD_DM_VoteMenu)
		{
			return;
		}

		if (m_OMD_DM_VoteMenu.IsVisible())
		{
			return;
		}

		m_OMD_DM_VoteMenu = null;
	}

	protected void SyncBoardRef()
	{
		if (!m_OMD_DM_BoardMenu)
		{
			return;
		}

		if (m_OMD_DM_BoardMenu.IsVisible())
		{
			return;
		}

		m_OMD_DM_BoardMenu = null;
	}

	protected void OpenVote(string reason)
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
			if (m_OMD_DM_VoteMenu.IsVisible())
			{
				m_OMD_DM_VoteMenu.Apply();
				return;
			}

			m_OMD_DM_VoteMenu = null;
		}

		m_OMD_DM_VoteMenu = new OMD_DM_VoteMenu();
		ui.ShowScriptedMenu(m_OMD_DM_VoteMenu, null);
		OMD_DM_Ui.Log("open", "vote", reason);
	}

	protected void CloseVote(string reason)
	{
		if (!m_OMD_DM_VoteMenu)
		{
			return;
		}

		OMD_DM_Ui.Log("close", "vote", reason);
		m_OMD_DM_VoteMenu.Shutdown();
		m_OMD_DM_VoteMenu = null;
	}

	protected void OpenBoard(string reason)
	{
		UIManager ui;

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
			if (m_OMD_DM_BoardMenu.IsVisible())
			{
				m_OMD_DM_BoardMenu.Apply();
				return;
			}

			m_OMD_DM_BoardMenu = null;
		}

		m_OMD_DM_BoardMenu = new OMD_DM_BoardMenu();
		ui.ShowScriptedMenu(m_OMD_DM_BoardMenu, null);
		OMD_DM_Ui.Log("open", "board", reason);
	}

	protected void CloseBoardMenu(string reason)
	{
		if (!m_OMD_DM_BoardMenu)
		{
			return;
		}

		OMD_DM_Ui.Log("close", "board", reason);
		m_OMD_DM_BoardMenu.Shutdown();
		m_OMD_DM_BoardMenu = null;
	}

	protected void CloseCursor(string reason)
	{
		if (reason == "esc")
		{
			OMD_DM_VoteClient.s_Dismissed = true;
			OMD_DM_VoteClient.s_WantAuto = false;
			OMD_DM_VoteClient.s_SwitchVote = false;
			OMD_DM_BoardClient.s_SwitchBoard = false;
		}

		CloseVote(reason);
		CloseBoardMenu(reason);
	}

	protected void ShowHud(string reason)
	{
		if (!m_OMD_DM_BoardHud)
		{
			m_OMD_DM_BoardHud = new OMD_DM_BoardHud();
		}

		if (!m_OMD_DM_BoardHud)
		{
			return;
		}

		m_OMD_DM_BoardHud.ShowBoard(true);
		if (m_OMD_DM_BoardShown)
		{
			return;
		}

		m_OMD_DM_BoardShown = true;
		m_OMD_DM_BoardHud.Apply();
		OMD_DM_Ui.Log("open", "hud", reason);
	}

	protected void HideHud(string reason)
	{
		if (!m_OMD_DM_BoardShown)
		{
			if (m_OMD_DM_BoardHud)
			{
				m_OMD_DM_BoardHud.ShowBoard(false);
			}

			return;
		}

		m_OMD_DM_BoardShown = false;
		m_OMD_DM_BoardWait = 2;
		if (m_OMD_DM_BoardHud)
		{
			m_OMD_DM_BoardHud.ShowBoard(false);
		}

		OMD_DM_Ui.Log("close", "hud", reason);
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
};
#endif
