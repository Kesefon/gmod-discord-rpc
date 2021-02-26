-- This requires a special module to be installed before it works correctly
-- Sorry to disappoint you
print("TTT2 DiscordRPC")
if file.Find("lua/bin/gmcl_gdiscord_*.dll", "GAME")[1] == nil then return end
require("gdiscord")

-- Configuration
local map_restrict = false -- Should a display default image be displayed if the map is not in a given list?
local map_list = {
    gm_flatgrass = true,
    gm_construct = true
}
local public_roles = {
    detective = true,
    spectator = true,
    sheriff = true,
    deputy = true
}
local image_fallback = "default"
local discord_id = "729381129903276033"
local refresh_time = 20

local client

function DiscordUpdate()
    local rpc_data = {}
    rpc_data["state"] = "TTT2"

    -- Determine the max number of players
    rpc_data["partySize"] = player.GetCount()
    rpc_data["partyMax"] = game.MaxPlayers()

    -- Handle map stuff
    -- See the config
    rpc_data["largeImageKey"] = game.GetMap()
    rpc_data["largeImageText"] = game.GetMap()
    if map_restrict and not map_list[map] then
        rpc_data["largeImageKey"] = image_fallback
    end

    local role_name = client:GetSubRoleData().name

    if GAMEMODE.round_state == ROUND_ACTIVE then
        if public_roles[role_name] or client:OnceFound() then
            rpc_data["smallImageKey"] = role_name
            rpc_data["smallImageText"] = role_name
        end

        if HasteMode() then
            rpc_data["endTimestamp"] = math.floor(os.time() + GetGlobalFloat("ttt_haste_end", 0) - CurTime())
        else
            rpc_data["endTimestamp"] = math.floor(os.time() + GetGlobalFloat("ttt_round_end", 0) - CurTime())
        end

    elseif GAMEMODE.round_state == ROUND_PREP then
        rpc_data["state"] = "Preparing"
        rpc_data["endTimestamp"] = math.floor(os.time() + GetGlobalFloat("ttt_round_end", 0) - CurTime())

    elseif GAMEMODE.round_state == ROUND_POST then
        rpc_data["state"] = "Round Over"

        rpc_data["smallImageKey"] = role_name
        rpc_data["smallImageText"] = role_name
    end

    DiscordUpdateRPC(rpc_data)
end

hook.Add("InitPostEntity", "UpdateDiscordStatus", function()
    client = LocalPlayer()
    DiscordRPCInitialize(discord_id)
    DiscordUpdate()

    timer.Create("DiscordRPCTimer", refresh_time, 0, DiscordUpdate)
end)
