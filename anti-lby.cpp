#include "antiaim.h"
#include <chrono>
static bool wasMoving = true;
static bool preBreak = false;
static bool shouldBreak = false;
static bool brokeThisTick = false;
static bool fake_walk = false;
static int chocked = 0;
static bool gaymode = false;
static bool doubleflick = false;
static bool has_broken = false;
bool is_broken;


int GetFPS()
{
	static int fps = 0;
	static int count = 0;
	using namespace std::chrono;
	auto now = high_resolution_clock::now();
	static auto last = high_resolution_clock::now();
	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

void antiaim_helper::anti_lby(CUserCmd* cmd, bool& bSendPacket)
{
	if (Options::Menu.MiscTab.antilby.GetIndex() < 1)
		return;

	auto local_player = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	auto local_weapon = local_player->GetActiveWeaponHandle();

	if (!local_weapon)
		return;

	float b = rand() % 4;

	static float oldCurtime = Interfaces::Globals->curtime;

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (GameUtils::IsBomb(pWeapon) || GameUtils::IsGrenade(pWeapon))
		return;

	if (bSendPacket)
	{
		brokeThisTick = false;
		chocked = Interfaces::m_pClientState->chokedcommands;

		if (local_player->IsMoving() >= 0.1f && (local_player->GetFlags() & FL_ONGROUND))
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				wasMoving = false;
				oldCurtime = Interfaces::Globals->curtime;
				if (Interfaces::Globals->curtime - oldCurtime >= 1.1f)
				{
					shouldBreak = true;
					NextPredictedLBYUpdate = Interfaces::Globals->curtime;
				}
			}
			else
			{
				oldCurtime = Interfaces::Globals->curtime;
				wasMoving = true;
				has_broken = false;
			}
		}

		else
		{
			if (wasMoving &&Interfaces::Globals->curtime - oldCurtime > 0.22f)
			{
				wasMoving = false;				
				shouldBreak = true;
				NextPredictedLBYUpdate = Interfaces::Globals->curtime;
			}

			else if (Interfaces::Globals->curtime - oldCurtime > 1.1f)
			{
				shouldBreak = true;
				NextPredictedLBYUpdate = Interfaces::Globals->curtime;
			}

			else if (Interfaces::Globals->curtime - oldCurtime > 1.1f - TICKS_TO_TIME(chocked) - TICKS_TO_TIME(2))
			{
				preBreak = true;
			}

			else if (Interfaces::Globals->curtime - oldCurtime > 1.f - TICKS_TO_TIME(chocked + 12))
				doubleflick = true;

			
		}
	}
	else if (shouldBreak)
	{
		static int choked = 0;

		if (Options::Menu.MiscTab.pitch_up.GetState() && !(hackManager.pLocal()->GetFlags() & FL_DUCKING))
		{
			oldCurtime = Interfaces::Globals->curtime;
			cmd->viewangles.x = -70.f;
			shouldBreak = false;
		}

		switch (Options::Menu.MiscTab.antilby.GetIndex())
		{
		case 1:
		{
			brokeThisTick = true;
			oldCurtime = Interfaces::Globals->curtime;
			cmd->viewangles.y = cmd->viewangles.y + Options::Menu.MiscTab.BreakLBYDelta.GetValue();
			shouldBreak = false;
		}
		break;

		case 2:
		{
			brokeThisTick = true;
			oldCurtime = Interfaces::Globals->curtime;
			cmd->viewangles.y = cmd->viewangles.y + Options::Menu.MiscTab.BreakLBYDelta.GetValue();
			shouldBreak = false;
		}
		break;

		case 3:
		{
			float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.9 * max(choked, GlobalBREAK::prevChoked) + 100) : 129.0f;

			brokeThisTick = true;
			oldCurtime = Interfaces::Globals->curtime;
			cmd->viewangles.y += addAngle * 2;
			shouldBreak = false;
		}
		break;
		}
	}

	else if (preBreak && Options::Menu.MiscTab.antilby.GetIndex() == 2)
	{
		brokeThisTick = true;
		float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.3789 * max(chocked, GlobalBREAK::prevChoked) + 90) : 144.9f;
		cmd->viewangles.y = cmd->viewangles.y + Options::Menu.MiscTab.BreakLBYDelta2.GetValue();
		preBreak = false;
	}

	else if (preBreak && Options::Menu.MiscTab.antilby.GetIndex() > 2)
	{
		brokeThisTick = true;
		float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.3789 * max(chocked, GlobalBREAK::prevChoked) + 29) : 145.f;
		cmd->viewangles.y += addAngle;
		preBreak = false;

	}
}