#ifndef SERVER
modded class MissionGameplay
{
	protected UAInput m_OMD_DM_VoteKey;
	protected ref OMD_DM_VoteMenu m_OMD_DM_VoteMenu;

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
	}

	override void OnMissionFinish()
	{
		CloseVoteMenu();
		OMD_DM_VoteClient.Reset();
		super.OnMissionFinish();
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		TickVote(timeslice);
	}

	protected void TickVote(float timeslice)
	{
		UIManager ui;
		bool wantOpen;

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
		if (OMD_DM_VoteClient.s_ForceOpen)
		{
			OMD_DM_VoteClient.s_ForceOpen = false;
			OMD_DM_VoteClient.s_UserClosed = false;
			wantOpen = true;
		}

		if (PressedVoteKey())
		{
			if (OMD_DM_VoteClient.s_Active)
			{
				if (!m_OMD_DM_VoteMenu)
				{
					OMD_DM_VoteClient.s_UserClosed = false;
					wantOpen = true;
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
};
#endif
