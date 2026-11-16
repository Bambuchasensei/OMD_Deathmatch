#ifndef SERVER
class OMD_DM_Logo
{
	static void Apply(Widget root)
	{
		ImageWidget logo;
		TextWidget fallback;
		bool loaded;

		if (!root)
		{
			return;
		}

		logo = ImageWidget.Cast(root.FindAnyWidget("LogoImage"));
		fallback = TextWidget.Cast(root.FindAnyWidget("LogoText"));
		loaded = false;
		if (logo)
		{
			loaded = logo.LoadImageFile(0, OMD_DM_Ui.LOGO_IMAGE);
			logo.Show(loaded);
		}

		if (fallback)
		{
			if (loaded)
			{
				fallback.Show(false);
			}
			else
			{
				fallback.Show(true);
			}
		}
	}
};

class OMD_DM_BoardLine
{
	Widget root;
	Widget back;
	Widget bar;
	TextWidget place;
	TextWidget name;
	TextWidget kills;
	TextWidget deaths;
	TextWidget assists;
	TextWidget points;
};

class OMD_DM_BoardLines
{
	protected static const string ROW_LAYOUT = "OMD_Deathmatch/gui/layouts/omd_dm_board_row.layout";
	protected static const int COLOR_TEXT = 0xFFE8E8E4;
	protected static const int COLOR_NEG = 0xFFD9A441;
	protected static const int COLOR_OWN = 0xFF242724;
	protected static const int COLOR_BAR = 0xFFC9403A;
	protected static const int COLOR_CLEAR = 0x00000000;

	protected Widget m_Host;
	protected ref array<ref OMD_DM_BoardLine> m_Lines;
	protected string m_LocalName;
	protected bool m_Compact;

	void OMD_DM_BoardLines(Widget host)
	{
		m_Host = host;
		m_Lines = new array<ref OMD_DM_BoardLine>();
	}

	void Clear()
	{
		int i;
		OMD_DM_BoardLine line;

		if (!m_Lines)
		{
			return;
		}

		for (i = 0; i < m_Lines.Count(); i++)
		{
			line = m_Lines.Get(i);
			if (!line)
			{
				continue;
			}

			if (line.root)
			{
				line.root.Unlink();
			}
		}

		m_Lines.Clear();
	}

	void ShowRows(int maxRows, bool compact)
	{
		int i;
		int count;
		int shown;
		float slot;
		float y;
		float hostH;
		Widget created;
		OMD_DM_BoardRow data;
		OMD_DM_BoardLine line;

		Clear();
		m_Compact = compact;
		CacheLocal();
		if (!m_Host)
		{
			return;
		}

		if (!GetGame())
		{
			return;
		}

		if (!GetGame().GetWorkspace())
		{
			return;
		}

		if (!OMD_DM_BoardClient.s_Rows)
		{
			return;
		}

		count = OMD_DM_BoardClient.s_Rows.Count();
		shown = count;
		if (maxRows > 0)
		{
			if (count > maxRows)
			{
				shown = maxRows;
			}
		}

		if (shown < 1)
		{
			return;
		}

		if (compact)
		{
			slot = 0.1;
		}
		else
		{
			hostH = shown;
			hostH = hostH * 0.09;
			m_Host.SetSize(1, hostH);
			slot = 1.0 / shown;
		}

		for (i = 0; i < shown; i++)
		{
			data = OMD_DM_BoardClient.s_Rows.Get(i);
			created = GetGame().GetWorkspace().CreateWidgets(ROW_LAYOUT, m_Host);
			if (!created)
			{
				continue;
			}

			line = new OMD_DM_BoardLine();
			line.root = created;
			line.back = created.FindAnyWidget("RowBack");
			line.bar = created.FindAnyWidget("OwnBar");
			line.place = TextWidget.Cast(created.FindAnyWidget("PlaceText"));
			line.name = TextWidget.Cast(created.FindAnyWidget("NameText"));
			line.kills = TextWidget.Cast(created.FindAnyWidget("KillText"));
			line.deaths = TextWidget.Cast(created.FindAnyWidget("DeathText"));
			line.assists = TextWidget.Cast(created.FindAnyWidget("AssistText"));
			line.points = TextWidget.Cast(created.FindAnyWidget("PointText"));
			ApplyColumns(line);
			FillLine(line, data, i);
			y = i * slot;
			created.SetPos(0, y);
			created.SetSize(1, slot * 0.88);
			m_Lines.Insert(line);
		}
	}

	protected void FillLine(OMD_DM_BoardLine line, OMD_DM_BoardRow data, int index)
	{
		int placeNum;
		string playerName;
		int kills;
		int deaths;
		int assists;
		int points;

		if (!line)
		{
			return;
		}

		placeNum = index + 1;
		playerName = "";
		kills = 0;
		deaths = 0;
		assists = 0;
		points = 0;
		if (data)
		{
			playerName = data.name;
			kills = data.kills;
			deaths = data.deaths;
			assists = data.assists;
			points = data.points;
		}

		if (line.place)
		{
			line.place.SetText(placeNum.ToString());
		}

		if (line.name)
		{
			line.name.SetText(playerName);
		}

		if (line.kills)
		{
			line.kills.SetText(kills.ToString());
		}

		if (line.deaths)
		{
			line.deaths.SetText(deaths.ToString());
		}

		if (line.assists)
		{
			line.assists.SetText(assists.ToString());
		}

		if (line.points)
		{
			line.points.SetText(points.ToString());
		}

		PaintText(line.place, COLOR_TEXT);
		PaintText(line.name, COLOR_TEXT);
		PaintText(line.kills, COLOR_TEXT);
		PaintText(line.deaths, COLOR_TEXT);
		PaintText(line.assists, COLOR_TEXT);
		if (points < 0)
		{
			PaintText(line.points, COLOR_NEG);
		}
		else
		{
			PaintText(line.points, COLOR_TEXT);
		}

		if (line.back)
		{
			if (IsOwn(playerName))
			{
				line.back.SetColor(COLOR_OWN);
			}
			else
			{
				line.back.SetColor(COLOR_CLEAR);
			}
		}

		if (line.bar)
		{
			if (IsOwn(playerName))
			{
				line.bar.SetColor(COLOR_BAR);
			}
			else
			{
				line.bar.SetColor(COLOR_CLEAR);
			}
		}
	}

	protected void PaintText(TextWidget widget, int color)
	{
		if (!widget)
		{
			return;
		}

		widget.SetColor(color);
		if (m_Compact)
		{
			widget.SetTextExactSize(13);
		}
		else
		{
			widget.SetTextExactSize(14);
		}
	}

	protected bool IsOwn(string playerName)
	{
		if (m_LocalName == "")
		{
			return false;
		}

		if (playerName == m_LocalName)
		{
			return true;
		}

		return false;
	}

	protected void CacheLocal()
	{
		PlayerBase player;
		PlayerIdentity identity;

		m_LocalName = "";
		if (!GetGame())
		{
			return;
		}

		if (!Class.CastTo(player, GetGame().GetPlayer()))
		{
			return;
		}

		identity = player.GetIdentity();
		if (!identity)
		{
			return;
		}

		m_LocalName = identity.GetName();
	}

	protected void ApplyColumns(OMD_DM_BoardLine line)
	{
		if (!line)
		{
			return;
		}

		if (m_Compact)
		{
			Move(line.bar, 0.000, 0.006, 0.20, 0.60);
			Move(line.place, 0.030, 0.058, 0, 1);
			Move(line.name, 0.108, 0.420, 0, 1);
			Move(line.kills, 0.548, 0.084, 0, 1);
			Move(line.deaths, 0.652, 0.084, 0, 1);
			Move(line.assists, 0.756, 0.084, 0, 1);
			Move(line.points, 0.860, 0.130, 0, 1);
			return;
		}

		Move(line.bar, 0.000, 0.006, 0.20, 0.60);
		Move(line.place, 0.018, 0.040, 0, 1);
		Move(line.name, 0.070, 0.430, 0, 1);
		Move(line.kills, 0.520, 0.110, 0, 1);
		Move(line.deaths, 0.640, 0.100, 0, 1);
		Move(line.assists, 0.750, 0.100, 0, 1);
		Move(line.points, 0.860, 0.130, 0, 1);
	}

	protected void Move(Widget widget, float x, float wdt, float y, float hgt)
	{
		if (!widget)
		{
			return;
		}

		widget.SetPos(x, y);
		widget.SetSize(wdt, hgt);
	}
};

class OMD_DM_BoardHud
{
	protected static const string HUD_LAYOUT = "OMD_Deathmatch/gui/layouts/omd_dm_board_hud.layout";

	protected Widget m_Root;
	protected ref OMD_DM_BoardLines m_Lines;

	void OMD_DM_BoardHud()
	{
		Widget host;

		m_Root = null;
		m_Lines = null;
		if (!GetGame())
		{
			return;
		}

		if (!GetGame().GetWorkspace())
		{
			return;
		}

		m_Root = GetGame().GetWorkspace().CreateWidgets(HUD_LAYOUT);
		if (!m_Root)
		{
			return;
		}

		m_Root.Show(false);
		host = m_Root.FindAnyWidget("ListHost");
		m_Lines = new OMD_DM_BoardLines(host);
	}

	void ShowBoard(bool show)
	{
		if (!m_Root)
		{
			return;
		}

		m_Root.Show(show);
	}

	void Apply()
	{
		TextWidget hint;
		TextWidget clock;
		bool empty;

		empty = true;
		if (OMD_DM_BoardClient.s_Rows)
		{
			if (OMD_DM_BoardClient.s_Rows.Count() > 0)
			{
				empty = false;
			}
		}

		if (m_Root)
		{
			hint = TextWidget.Cast(m_Root.FindAnyWidget("HintText"));
			clock = TextWidget.Cast(m_Root.FindAnyWidget("MatchTime"));
			if (hint)
			{
				if (empty)
				{
					hint.SetText("ЗАГРУЗКА");
				}
				else
				{
					hint.SetText("NUM - ЗАКРЫТЬ");
				}
			}

			if (clock)
			{
				if (OMD_DM_BoardClient.s_TimeKnown)
				{
					clock.SetText(OMD_DM_Ui.Clock(OMD_DM_BoardClient.s_SecondsLeft));
				}
				else
				{
					clock.SetText("");
				}
			}
		}

		if (!m_Lines)
		{
			return;
		}

		if (empty)
		{
			m_Lines.Clear();
			return;
		}

		m_Lines.ShowRows(10, true);
	}

	void Destroy()
	{
		if (m_Lines)
		{
			m_Lines.Clear();
		}

		if (m_Root)
		{
			m_Root.Unlink();
		}

		m_Root = null;
		m_Lines = null;
	}
};
#endif
