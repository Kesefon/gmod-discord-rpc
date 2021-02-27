#define GMMODULE
#include "GarrysMod/Lua/Interface.h"
#include "discord_rpc.h"
#include <stdio.h>
#include <string.h>
#include <utility>

DiscordUser cbConUser;
std::pair<int, char> cbDisconnected;
std::pair<int, char> cbError;
char cbJoinSecret;
char cbSpectateSecret;
DiscordUser cbJoinRequest;
bool cbConUserTriggered, cbDisconnectedTriggered, cbErrorTriggered,
    cbJoinTriggered, cbSpectateTriggered, cbJoinRequestTriggered;

static void HandleDiscordReady(const DiscordUser *connectedUser) {
    cbConUser = *connectedUser;
    cbConUserTriggered = true;
}

static void HandleDiscordDisconnected(int errcode, const char *message) {
    cbDisconnected.first = errcode;
    cbDisconnected.second = *message;
    cbDisconnectedTriggered = true;
}

static void HandleDiscordError(int errcode, const char *message) {
    cbError.first = errcode;
    cbError.second = *message;
    cbConUserTriggered = true;
}

static void HandleDiscordJoin(const char *secret) {
    cbJoinSecret = *secret;
    cbJoinTriggered = true;
}

static void HandleDiscordSpectate(const char *secret) {
    cbSpectateSecret = *secret;
    cbSpectateTriggered = true;
}

static void HandleDiscordJoinRequest(const DiscordUser *request) {
    cbJoinRequest = *request;
    cbJoinRequestTriggered = true;
}

LUA_FUNCTION(StartDiscordStatus) {

    const char *appid = LUA->GetString(1);

    // Discord RPC
    DiscordEventHandlers handlers{};

    handlers.ready = HandleDiscordReady;
    handlers.disconnected = HandleDiscordDisconnected;
    handlers.errored = HandleDiscordError;
    handlers.joinGame = HandleDiscordJoin;
    handlers.spectateGame = HandleDiscordSpectate;
    handlers.joinRequest = HandleDiscordJoinRequest;

    Discord_Initialize(appid, &handlers, 1, 0);

    return 0;
}

LUA_FUNCTION(RunDiscordCallbacks) {
    Discord_RunCallbacks();
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->GetField(-1, "hook");
    if (cbConUserTriggered) {
        LUA->GetField(-1, "Run");
        LUA->PushString("DiscordReady");
        LUA->PushString(cbConUser.userId);
        LUA->PushString(cbConUser.username);
        LUA->PushString(cbConUser.discriminator);
        LUA->PushString(cbConUser.avatar);
        LUA->Call(5, 0);
        cbConUserTriggered = false;
    }

    if (cbDisconnectedTriggered) {
        LUA->GetField(-1, "Run");
        LUA->PushString("DiscordDisconnected");
        LUA->PushNumber(cbDisconnected.first);
        LUA->PushString(&cbDisconnected.second);
        LUA->Call(3, 0);
        cbDisconnectedTriggered = false;
    }

    if (cbErrorTriggered) {
        LUA->GetField(-1, "Run");
        LUA->PushString("DiscordError");
        LUA->PushNumber(cbError.first);
        LUA->PushString(&cbError.second);
        LUA->Call(3, 0);
        cbErrorTriggered = false;
    }

    if (cbJoinTriggered) {
        LUA->GetField(-1, "Run");
        LUA->PushString("DiscordJoin");
        LUA->PushString(&cbJoinSecret);
        LUA->Call(2, 0);
        cbJoinTriggered = false;
    }

    if (cbSpectateTriggered) {
        LUA->GetField(-1, "Run");
        LUA->PushString("DiscordSpectate");
        LUA->PushString(&cbSpectateSecret);
        LUA->Call(2, 0);
        cbSpectateTriggered = false;
    }

    if (cbJoinRequestTriggered) {
        LUA->GetField(-1, "Run");
        LUA->PushString("DiscordJoinRequest");
        LUA->PushString(cbJoinRequest.userId);
        LUA->PushString(cbJoinRequest.username);
        LUA->PushString(cbJoinRequest.discriminator);
        LUA->PushString(cbJoinRequest.avatar);
        LUA->Call(5, 0);
        cbJoinRequestTriggered = false;
    }

    LUA->Pop(2);

    return 0;
}

LUA_FUNCTION(DiscordRespond) {
    const char *userid = LUA->GetString(1);
    const int reply = LUA->GetNumber(2);
    Discord_Respond(userid, reply);
    return 0;
}

LUA_FUNCTION(UpdateDiscordStatus) {
    DiscordRichPresence discordP;
    memset(&discordP, 0, sizeof(discordP));
    LUA->GetField(1, "state");
    discordP.state = LUA->GetString();
    LUA->GetField(1, "details");
    discordP.details = LUA->GetString();
    LUA->GetField(1, "startTimestamp");
    discordP.startTimestamp = LUA->GetNumber();
    LUA->GetField(1, "endTimestamp");
    discordP.endTimestamp = LUA->GetNumber();
    LUA->GetField(1, "largeImageKey");
    discordP.largeImageKey = LUA->GetString();
    LUA->GetField(1, "largeImageText");
    discordP.largeImageText = LUA->GetString();
    LUA->GetField(1, "smallImageKey");
    discordP.smallImageKey = LUA->GetString();
    LUA->GetField(1, "smallImageText");
    discordP.smallImageText = LUA->GetString();
    LUA->GetField(1, "partyId");
    discordP.partyId = LUA->GetString();
    LUA->GetField(1, "partySize");
    discordP.partySize = LUA->GetNumber();
    LUA->GetField(1, "partyMax");
    discordP.partyMax = LUA->GetNumber();
    LUA->GetField(1, "partyPrivacy");
    discordP.partyPrivacy = LUA->GetNumber();
    LUA->GetField(1, "matchSecret");
    discordP.matchSecret = LUA->GetString();
    LUA->GetField(1, "joinSecret");
    discordP.joinSecret = LUA->GetString();
    LUA->GetField(1, "spectateSecret");
    discordP.spectateSecret = LUA->GetString();
    LUA->GetField(1, "instance");
    discordP.instance = LUA->GetNumber();

    Discord_UpdatePresence(&discordP);
    return 0;
}

GMOD_MODULE_OPEN() {
    // Create the functions
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushCFunction(StartDiscordStatus);
    LUA->SetField(-2, "DiscordRPCInitialize");
    LUA->Pop();

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushCFunction(RunDiscordCallbacks);
    LUA->SetField(-2, "DiscordRPCRunCallbacks");
    LUA->Pop();

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushCFunction(DiscordRespond);
    LUA->SetField(-2, "DiscordRPCRespond");
    LUA->Pop();

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
    LUA->PushCFunction(UpdateDiscordStatus);
    LUA->SetField(-2, "DiscordUpdateRPC");
    LUA->Pop();

    return 0;
}

GMOD_MODULE_CLOSE() {
    Discord_Shutdown();
    return 0;
}
