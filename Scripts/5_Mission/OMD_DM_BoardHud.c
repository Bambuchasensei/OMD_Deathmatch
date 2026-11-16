#ifndef SERVER
class OMD_DM_BoardLine
{
	Widget root;
	Widget back;
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
	protected static const int COLOR_1 = 0xFFFFE08A;
	protected static const int COLOR_2 = 0xFFE6E6E6;
	protected static const int COLOR_3 = 0xFFE0A060;
	protected static const int COLOR_N = 0xFFF2F2F2;
	protected static const int COLOR_BACK_1 = 0x663A3018;
	protected static const int COLOR_BACK_2 = 0x442E2E2E;
	protected static const int COLOR_BACK_3 = 0x66382A18;
	protected static const int COLOR_BACK_N = 0x00000000;

	protected Widget m_Host;
	protected ref array<ref OMD_DM_BoardLine> m_Lines;

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

	void ShowRows(float fixedSlot)
	{
		int i;
		int count;
		int shown;
		float slot;
		float y;
		float denom;
		Widget created;
		OMD_DM_BoardRow data;
		OMD_DM_BoardLine line;

		Clear();
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
		if (fixedSlot > 0)
		{
			if (count > 3)
			{
				shown = 3;
			}
		}

		if (shown < 1)
		{
			return;
		}

		if (fixedSlot > 0)
		{
			slot = fixedSlot;
		}
		else
		{
			denom = shown;
			m_Host.SetSize(1, denom * 0.085);
			slot = 1.0 / denom;
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
			line.place = TextWidget.Cast(created.FindAnyWidget("PlaceText"));
			line.name = TextWidget.Cast(created.FindAnyWidget("NameText"));
			line.kills = TextWidget.Cast(created.FindAnyWidget("KillText"));
			line.deaths = TextWidget.Cast(created.FindAnyWidget("DeathText"));
			line.assists = TextWidget.Cast(created.FindAnyWidget("AssistText"));
			line.points = TextWidget.Cast(created.FindAnyWidget("PointText"));
			FillLine(line, data, i);
			y = i * slot;
			created.SetPos(0, y);
			created.SetSize(1, slot * 0.92);
			m_Lines.Insert(line);
		}
	}

	protected void FillLine(OMD_DM_BoardLine line, OMD_DM_BoardRow data, int index)
	{
		int placeNum;
		int color;
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

		color = RankColor(index);
		PaintText(line.place, color);
		PaintText(line.name, color);
		PaintText(line.kills, color);
		PaintText(line.deaths, color);
		PaintText(line.assists, color);
		PaintText(line.points, color);
		if (line.back)
		{
			line.back.SetColor(RankBack(index));
		}
	}

	protected void PaintText(TextWidget widget, int color)
	{
		if (!widget)
		{
			return;
		}

		widget.SetColor(color);
	}

	protected int RankColor(int index)
	{
		if (index == 0)
		{
			return COLOR_1;
		}

		if (index == 1)
		{
			return COLOR_2;
		}

		if (index == 2)
		{
			return COLOR_3;
		}

		return COLOR_N;
	}

	protected int RankBack(int index)
	{
		if (index == 0)
		{
			return COLOR_BACK_1;
		}

		if (index == 1)
		{
			return COLOR_BACK_2;
		}

		if (index == 2)
		{
			return COLOR_BACK_3;
		}

		return COLOR_BACK_N;
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
		if (!m_Lines)
		{
			return;
		}

		m_Lines.ShowRows(0.0);
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
