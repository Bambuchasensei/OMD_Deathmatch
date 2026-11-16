modded class PlayerBase
{
	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
	{
		super.OnRPC(sender, rpc_type, ctx);
		if (!GetGame())
		{
			return;
		}

		if (!GetGame().IsClient())
		{
			return;
		}

		if (rpc_type == OMD_DM_Rpc.BOARD_LIVE)
		{
			OMD_DM_BoardClient.Read(ctx, false);
			return;
		}

		if (rpc_type == OMD_DM_Rpc.BOARD_FINAL)
		{
			OMD_DM_BoardClient.Read(ctx, true);
		}
	}
};
