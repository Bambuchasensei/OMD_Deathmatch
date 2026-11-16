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

		if (rpc_type == OMD_DM_Rpc.OPEN)
		{
			OMD_DM_VoteClient.ReadState(ctx, true);
			return;
		}

		if (rpc_type == OMD_DM_Rpc.COUNT)
		{
			OMD_DM_VoteClient.ReadState(ctx, false);
			return;
		}

		if (rpc_type == OMD_DM_Rpc.RESULT)
		{
			OMD_DM_VoteClient.ReadResult(ctx);
		}
	}
};
