#include "Visuals.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "Autowall.h"
void CVisuals::Init()
{
}
void CVisuals::Move(CUserCmd *pCmd, bool &bSendPacket) {}

bool screen_transformxx(const Vector& point, Vector& screen)
{
	const matrix3x4& w2sMatrix = Interfaces::Engine->WorldToScreenMatrix();
	screen.x = w2sMatrix[0][0] * point.x + w2sMatrix[0][1] * point.y + w2sMatrix[0][2] * point.z + w2sMatrix[0][3];
	screen.y = w2sMatrix[1][0] * point.x + w2sMatrix[1][1] * point.y + w2sMatrix[1][2] * point.z + w2sMatrix[1][3];
	screen.z = 0.0f;
	float w = w2sMatrix[3][0] * point.x + w2sMatrix[3][1] * point.y + w2sMatrix[3][2] * point.z + w2sMatrix[3][3];
	if (w < 0.001f) {
		screen.x *= 100000;
		screen.y *= 100000;
		return true;
	}
	float invw = 1.0f / w;
	screen.x *= invw;
	screen.y *= invw;
	return false;
}
bool world_to_screenxx(const Vector &origin, Vector &screen)
{
	if (!screen_transformxx(origin, screen)) {
		int iScreenWidth, iScreenHeight;
		Interfaces::Engine->GetScreenSize(iScreenWidth, iScreenHeight);
		screen.x = (iScreenWidth / 2.0f) + (screen.x * iScreenWidth) / 2;
		screen.y = (iScreenHeight / 2.0f) - (screen.y * iScreenHeight) / 2;
		return true;
	}
	return false;
}
void CVisuals::Draw()
{
	if (!Interfaces::Engine->IsConnected() || !Interfaces::Engine->IsInGame())
		return;
	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (Options::Menu.VisualsTab.OtherNoScope.GetState() && pLocal->IsAlive() && pLocal->IsScoped())
		NoScopeCrosshair();
	if (Options::Menu.VisualsTab.AutowallCrosshair.GetState())
	{
		AutowallCrosshair();
		Interfaces::Engine->ClientCmd_Unrestricted("crosshair 0");
	}
	else
	{
		Interfaces::Engine->ClientCmd_Unrestricted("crosshair 1");
	}
}
void CVisuals::NoScopeCrosshair()
{
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;
	IClientEntity* pLocal = hackManager.pLocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (GameUtils::IsSniper(pWeapon))
	{
		Render::Line(MidX - 2000, MidY, MidX + 2000, MidY, Color(0, 0, 0, 255));
		Render::Line(MidX, MidY - 2000, MidX, MidY + 2000, Color(0, 0, 0, 255));
	}
}
void CVisuals::DrawCrosshair()
{
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;
	Render::Line(MidX - 10, MidY, MidX + 10, MidY, Color(0, 255, 0, 255));
	Render::Line(MidX, MidY - 10, MidX, MidY + 10, Color(0, 255, 0, 255));
}
void CVisuals::DrawRecoilCrosshair()
{
	IClientEntity *pLocal = hackManager.pLocal();
	Vector ViewAngles;
	Interfaces::Engine->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;
	Vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;
	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = start + fowardVec, endScreen;
	if (Render::TransformScreen(end, endScreen) && pLocal->IsAlive())
	{
		Render::Line(endScreen.x - 10, endScreen.y, endScreen.x + 10, endScreen.y, Color(0, 255, 0, 255));
		Render::Line(endScreen.x, endScreen.y - 10, endScreen.x, endScreen.y + 10, Color(0, 255, 0, 255));
	}
}
void CVisuals::SpreadCrosshair()
{
	IClientEntity *pLocal = hackManager.pLocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (!hackManager.pLocal()->IsAlive())
		return;
//	if (!GameUtils::IsBallisticWeapon(pWeapon))
//		return;
	if (pWeapon == nullptr)
		return;
	int xs;
	int ys;
	Interfaces::Engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;
	float inaccuracy = pWeapon->GetInaccuracy() * 1000;
	char buffer4[64];
	sprintf_s(buffer4, "%.00f", inaccuracy);
	Render::DrawFilledCircle(Vector2D(xs, ys), Color(20, 20, 20, 124), inaccuracy, 60);
}
void CVisuals::AutowallCrosshair() //this was disgusting freaK
{
	IClientEntity *pLocal = hackManager.pLocal();
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;
	Vector ViewAngles;
	Interfaces::Engine->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;
	Vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;
	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = start + fowardVec, endScreen;
	int xs, ys;
	Interfaces::Engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;
	if (Render::TransformScreen(end, endScreen) && pLocal->IsAlive())
	{
		float damage = 0.f;
		Render::OutlinedRect(xs - 2, ys - 2, 5, 5, Color(27, 27, 27), Color(Options::Menu.ColorsTab.Menu.GetValue()));
	}
}
CAW_hitmaker* hitmarker_2 = new CAW_hitmaker();

void CAW_hitmaker::paint()
{
	if (!Options::Menu.VisualsTab.OtherHitmarker.GetState())
		return;

	float time = Interfaces::Globals->curtime;

	for (int i = 0; i < Xhitmarkers.size(); i++)
	{
		bool expired = time >= Xhitmarkers.at(i).impact.time + 2.f;
		if (expired)
			Xhitmarkers.at(i).alpha -= 1;
		if (expired && Xhitmarkers.at(i).alpha <= 0)
		{
			Xhitmarkers.erase(Xhitmarkers.begin() + i);
			continue;
		}

		Vector pos3D = Vector(Xhitmarkers.at(i).impact.x, Xhitmarkers.at(i).impact.y, Xhitmarkers.at(i).impact.z), pos2D;
		if (!world_to_screenxx(pos3D, pos2D))
			continue;
		if (pos3D == Vector(0, 0, 0))
			return;
		int lineSize = 16 / 2;
		Vector screen = pos2D;

		Interfaces::Surface->DrawSetColor(255, 255, 255, Xhitmarkers.at(i).alpha);
		Interfaces::Surface->DrawLine(pos2D.x - lineSize, pos2D.y - lineSize, pos2D.x - (lineSize / 4), pos2D.y - (lineSize / 4));
		Interfaces::Surface->DrawLine(pos2D.x - lineSize, pos2D.y + lineSize, pos2D.x - (lineSize / 4), pos2D.y + (lineSize / 4));
		Interfaces::Surface->DrawLine(pos2D.x + lineSize, pos2D.y + lineSize, pos2D.x + (lineSize / 4), pos2D.y + (lineSize / 4));
		Interfaces::Surface->DrawLine(pos2D.x + lineSize, pos2D.y - lineSize, pos2D.x + (lineSize / 4), pos2D.y - (lineSize / 4));
	}
}
