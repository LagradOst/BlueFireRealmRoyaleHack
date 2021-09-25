// MADE BY BLUEFIRE1337 v1.0.0

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <random>
#include <stdint.h>
#include <list>

#include "Classes.h"
#include "Overlay.h"
#include <thread>
#include "Mhyprot/mhyprot.hpp"

bool tracersEnabled = true;
bool aimbotEnabled = true;
bool boxESPEnabled = true;
bool hpESPEnabled = true;
bool aimbotPrediction = true;
bool noRecoil = true; // cant change when in game
bool noSpread = true; // cant change when in game

bool smoothing = true;
bool lockWhenClose = true;
float smoothingValue = 0.04f; // from 0-1

float ScreenCenterX;
float ScreenCenterY;

bool Locked;
APawn LockedPawn;


#define NAMEOF(name) #name // USE FOR DEBUGGING
#define CHECKVAL(_name)		\
	/*printf("%s : 0x%llX\n", NAMEOF(_name), _name.data);*/ \
	if(!IsValid(_name.data)) {	\
		return false;			\
	}							\

#define PRINTVAL(_name) printf("%s : 0x%llX\n", NAMEOF(_name), _name);
#define PRINTVALD(_name) printf("%s : %d\n", NAMEOF(_name), _name);
#define PRINTVALF(_name) printf("%s : %f\n", NAMEOF(_name), _name);

//#include <thread>

HWND hGameWnd;
HWND hOverlayWnd;
RECT wndRect;

struct Vec2
{
public:
	float x;
	float y;
};

void ESPLoop();

uint64_t process_base = 0;
void print_exception(const std::exception& e, int level = 0)
{
	std::cerr << std::string(level, ' ') << "exception: " << e.what() << '\n';
	try {
		std::rethrow_if_nested(e);
	}
	catch(const std::exception& e) {
		print_exception(e, level + 1);
	}
	catch(...) {}
}

LONG WINAPI SimplestCrashHandler(EXCEPTION_POINTERS* ExceptionInfo)
{
	std::cout << "[!!] Crash at addr 0x" << ExceptionInfo->ExceptionRecord->ExceptionAddress << " by 0x" << std::hex << ExceptionInfo->ExceptionRecord->ExceptionCode << std::endl;

	return EXCEPTION_EXECUTE_HANDLER;
}

bool rendering = true;
int frame = 0;
FOverlay* g_overlay;

namespace math
{
	// Constants that we need for maths stuff
#define Const_URotation180        32768
#define Const_PI                  3.14159265358979323
#define Const_RadToUnrRot         10430.3783504704527
#define Const_UnrRotToRad         0.00009587379924285
#define Const_URotationToRadians  Const_PI / Const_URotation180 

	int ClampYaw(int angle) {
		static const auto max = Const_URotation180 * 2;

		while(angle > max)
		{
			angle -= max;
		}

		while(angle < 0) {
			angle += max;
		}
		return angle;
	}
	int ClampPitch(int angle) {
		if(angle > 16000) {
			angle = 16000;
		}
		if(angle < -16000) {
			angle = -16000;
		}
		return angle;
	}

	FRotator VectorToRotation(FVector vVector)
	{
		FRotator rRotation;

		rRotation.Yaw = atan2(vVector.Y, vVector.X) * Const_RadToUnrRot;
		rRotation.Pitch = atan2(vVector.Z, sqrt((vVector.X * vVector.X) + (vVector.Y * vVector.Y))) * Const_RadToUnrRot;
		rRotation.Roll = 0;

		return rRotation;
	}

	FVector RotationToVector(FRotator R)
	{
		float fYaw = R.Yaw * Const_URotationToRadians;
		float fPitch = R.Pitch * Const_URotationToRadians;
		float CosPitch = cos(fPitch);
		FVector Vec = FVector(cos(fYaw) * CosPitch,
			sin(fYaw) * CosPitch,
			sin(fPitch));
		return Vec;
	}

	float VectorMagnitude(FVector Vec) {
		return sqrt((Vec.X * Vec.X) + (Vec.Y * Vec.Y) + (Vec.Z * Vec.Z));
	}

	void Normalize(FVector& v)
	{
		float size = VectorMagnitude(v);

		if(!size)
		{
			v.X = v.Y = v.Z = 1;
		}
		else
		{
			v.X /= size;
			v.Y /= size;
			v.Z /= size;
		}
	}


	float VectorMagnitude(Vec2 Vec) {
		return sqrt((Vec.x * Vec.x) + (Vec.y * Vec.y));
	}
	void Normalize(Vec2& v)
	{
		float size = VectorMagnitude(v);

		if(!size)
		{
			v.x = v.y = 1;
		}
		else
		{
			v.x /= size;
			v.y /= size;
		}
	}


	void GetAxes(FRotator R, FVector& X, FVector& Y, FVector& Z)
	{
		X = RotationToVector(R);
		Normalize(X);
		R.Yaw += 16384;
		FRotator R2 = R;
		R2.Pitch = 0.f;
		Y = RotationToVector(R2);
		Normalize(Y);
		Y.Z = 0.f;
		R.Yaw -= 16384;
		R.Pitch += 16384;
		Z = RotationToVector(R);
		Normalize(Z);
	}

	FVector VectorSubtract(FVector s1, FVector s2)
	{
		FVector temp{};
		temp.X = s1.X - s2.X;
		temp.Y = s1.Y - s2.Y;
		temp.Z = s1.Z - s2.Z;

		return temp;
	}

	FVector VectorAdd(FVector s1, FVector s2)
	{
		FVector temp;
		temp.X = s1.X + s2.X;
		temp.Y = s1.Y + s2.Y;
		temp.Z = s1.Z + s2.Z;

		return temp;
	}

	FVector VectorScale(FVector s1, float scale)
	{
		FVector temp;
		temp.X = s1.X * scale;
		temp.Y = s1.Y * scale;
		temp.Z = s1.Z * scale;

		return temp;
	}

	float VectorDotProduct(const FVector& A, const FVector& B)
	{
		float tempx = A.X * B.X;
		float tempy = A.Y * B.Y;
		float tempz = A.Z * B.Z;

		return (tempx + tempy + tempz);
	}

	void AimAtVector(FVector TargetVec, FVector PlayerLocation, FRotator& AimRot)
	{
		FVector AimVec;
		AimVec.X = TargetVec.X - PlayerLocation.X;
		AimVec.Y = TargetVec.Y - PlayerLocation.Y;
		AimVec.Z = TargetVec.Z - PlayerLocation.Z;

		FRotator AimAtRot = VectorToRotation(AimVec);
		AimRot = AimAtRot;
	}

	FVector GetAngleTo(FVector TargetVec, FVector OriginVec)
	{
		FVector Diff;
		Diff.X = TargetVec.X - OriginVec.X;
		Diff.Y = TargetVec.Y - OriginVec.Y;
		Diff.Z = TargetVec.Z - OriginVec.Z;

		return Diff;
	}

	float GetDistance(FVector to, FVector from) {
		float deltaX = to.X - from.X;
		float deltaY = to.Y - from.Y;
		float deltaZ = to.Z - from.Z;

		return (float)sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);
	}

	float GetCrosshairDistance(float Xx, float Yy, float xX, float yY)
	{
		return sqrt((yY - Yy) * (yY - Yy) + (xX - Xx) * (xX - Xx));
	}
}


// W2S (WorldToScreen) is used to map the 3D game's world coordinates to 2D screen space coordinates.
bool W2S(FVector target, Vec2& dst, FRotator myRot, FVector myLoc, float fov)
{
	FVector AxisX, AxisY, AxisZ, Delta, Transformed;
	math::GetAxes(myRot, AxisX, AxisY, AxisZ);

	Delta = math::VectorSubtract(target, myLoc);
	Transformed.X = math::VectorDotProduct(Delta, AxisY);
	Transformed.Y = math::VectorDotProduct(Delta, AxisZ);
	Transformed.Z = math::VectorDotProduct(Delta, AxisX);

	if(Transformed.Z < 1.00f)
		Transformed.Z = 1.00f;

	float GFOV = fov;
	
	dst.x = ScreenCenterX + Transformed.X * (ScreenCenterX / tan(GFOV * Const_PI / 360.0f)) / Transformed.Z;
	dst.y = ScreenCenterY + -Transformed.Y * (ScreenCenterX / tan(GFOV * Const_PI / 360.0f)) / Transformed.Z;
	//return true; // I WANT TO SEE PPL BEHIND ME

	if(dst.x >= 0.0f && dst.x <= FOverlay::ScreenWidth)
	{
		if(dst.y >= 0.0f && dst.y <= FOverlay::ScreenHeight)
		{
			return true;
		}
	}
	return false;
}


bool IsValid(uint64_t adress) {
	if(adress == 0 || adress == 0xCCCCCCCCCCCCCCCC) {
		return false;
	}
	return true;
}

UEngine CurrentUEngine;
ULocalPlayer CurrentLocalPlayer;
APlayerController CurrentController;
APawn CurrentAcknowledgedPawn;
ACamera CurrentCamera;
AWorldInfo CurrentWorldInfo;
APawn CurrentPawnList;
ATgDevice CurrentWeapon;
int CurrentHealth;
float CurrentFOV;
int CurrentLoopFrame = 0;

float gravity = 0;

void NoRecoil() {
	while(true) {
		Sleep(100);
		if(!IsValid(CurrentWeapon.data)) continue;
		if(noRecoil)
			CurrentWeapon.NoRecoil();
		if(noSpread)
			CurrentWeapon.NoSpread();
	}
}

bool MainAddress() {
	CurrentLoopFrame++;

	CurrentUEngine = GetUEngine(process_base);
	CHECKVAL(CurrentUEngine);

	CurrentLocalPlayer = CurrentUEngine.GetLocalPlayer();
	CHECKVAL(CurrentLocalPlayer);

	CurrentController = CurrentLocalPlayer.GetController();
	CHECKVAL(CurrentController);

	CurrentAcknowledgedPawn = CurrentController.GetAcknowledgedPawn();
	CHECKVAL(CurrentAcknowledgedPawn);

	CurrentCamera = CurrentController.GetCamera();
	CHECKVAL(CurrentCamera);

	CurrentWorldInfo = CurrentController.GetWorldInfo();
	CHECKVAL(CurrentWorldInfo);

	CurrentPawnList = CurrentWorldInfo.GetPawnList();
	CHECKVAL(CurrentPawnList);

	CurrentWeapon = CurrentAcknowledgedPawn.GetWeapon();
	CHECKVAL(CurrentWeapon);

	CurrentHealth = CurrentAcknowledgedPawn.GetHealth();

	if(gravity == 0) {
		gravity = CurrentWorldInfo.GetGravity();
	}

	CurrentFOV = CurrentCamera.GetDeafultFov() * CurrentController.GetFovMultiplier();
	return true;
}

void HackTick() {
	if(MainAddress()) { // 0-1ms
		ESPLoop();
	}
	else {
		Sleep(100);
	}
}

float speed = 16000.f;

void callAimbot() {
	if(!IsValid(LockedPawn.data)) return;
	int Hp = LockedPawn.GetHealth();

	if(Hp > 1)
	{
		FRotator AimRotation = FRotator{ 0, 0, 0 };

		bool isPawnVisible = LockedPawn.GetMesh().IsVisible(CurrentWorldInfo.GetTimeSeconds());

		if(isPawnVisible)
		{
			auto isChickenOffset = LockedPawn.IsChicken() ? -50 : 0;
			FVector TargetLocation = LockedPawn.GetLocation();
			FVector RealLocation = CurrentCamera.GetRealLocation();

			if(aimbotPrediction) {
				auto wep = CurrentAcknowledgedPawn.GetWeapon();
				auto currentProjectiles = wep.GetProjectiles();

				if(currentProjectiles.Length() > 0)
				{
					float tempSpeed = currentProjectiles.GetById(0).GetSpeed();
					if(tempSpeed > 4000) {
						speed = tempSpeed;
					}
				}
				bool isGravity = wep.GetMaxAmmoCount() <= 1;

				FVector TargetVelocity = LockedPawn.GetVelocity() / 2.0f;
				float TravelTime = math::GetDistance(CurrentAcknowledgedPawn.GetLocation(), TargetLocation) / speed;
				if(TravelTime < 0.15f) {
					TravelTime = 0.f;
				}
				TargetLocation = {
					(TargetLocation.X + TargetVelocity.X * TravelTime),
					(TargetLocation.Y + TargetVelocity.Y * TravelTime),
					 TargetLocation.Z + (isGravity ? (TravelTime * -gravity / 5.f) : 0)
				};
				math::AimAtVector(TargetLocation + FVector(0, 0, 50 + isChickenOffset), RealLocation, AimRotation);

				if(smoothing) { // idk kinda spagetti to me, but it works
					FRotator currentRotation = CurrentController.GetRotation();
					currentRotation.Roll = 0;

					auto diff = currentRotation - AimRotation;

					auto realDiff = diff;
					auto a = math::ClampYaw(currentRotation.Yaw);
					auto b = math::ClampYaw(AimRotation.Yaw);
					const auto Full360 = Const_URotation180 * 2;

					auto dist1 = -(a - b + Full360) % Full360;
					auto dist2 = (b - a + Full360) % Full360;

					auto dist = dist1;
					if(abs(dist2) < abs(dist1)) {
						dist = dist2;
					}

					auto smoothAmount = smoothingValue;

					if(lockWhenClose && abs(dist) + abs(diff.Pitch) < Const_URotation180 / 100) {
						smoothAmount = 1;
					}

					diff.Yaw = (int)(dist * smoothAmount);
					diff.Pitch = (int)(diff.Pitch * smoothAmount);
					AimRotation = currentRotation + diff;
				}

				CurrentController.SetRotation(AimRotation);
			}
			else {
				math::AimAtVector(TargetLocation + FVector(0, 0, 50 + isChickenOffset), RealLocation, AimRotation);
				CurrentController.SetRotation(AimRotation);
			}
		}
	}
	else
	{
		Locked = false;
		LockedPawn = APawn{};
		return;
	}
}

void ESPLoop() {
	APawn CurrentPawn = CurrentPawnList;
	int players = 0;
	bool isAimbotActive = aimbotEnabled && GetAsyncKeyState(VK_RBUTTON);

	if(!isAimbotActive || !IsValid(LockedPawn.data) || !LockedPawn.GetMesh().IsVisible(CurrentWorldInfo.GetTimeSeconds())) {
		LockedPawn = APawn{};
		Locked = false;
	}

	float ClosestDistance = 999999.0f;

	while(IsValid(CurrentPawn.data))
	{
		APawn nextPawn = CurrentPawn.GetNextPawn(); // +NextPawn
		if(!IsValid(CurrentPawn.data))
		{
			CurrentPawn = nextPawn;
			continue;
		}

		int Hp = CurrentPawn.GetHealth();
		players++;
		//TeamIndex == teamIndex ||
		if(
			CurrentPawn.data == CurrentAcknowledgedPawn.data ||
			Hp < 1 || Hp > 5000)
		{
			CurrentPawn = nextPawn;
			continue;
		}

		auto mesh = CurrentPawn.GetMesh();
		if(!IsValid(mesh.data)) {
			CurrentPawn = nextPawn;
			continue;
		}

		if(CurrentPawn.IsTurret()) {
			CurrentPawn = nextPawn;
			continue;
		}

		FBoxSphereBounds PlayerHitbox = mesh.GetBounds();

		FVector min, max, posFeet;
		Vec2 smin, smax, pos;

		min = math::VectorSubtract(PlayerHitbox.Origin, PlayerHitbox.BoxExtent);
		max = math::VectorAdd(PlayerHitbox.Origin, PlayerHitbox.BoxExtent);
		posFeet = CurrentPawn.GetLocation();

		posFeet.Z += 50; // Y POS, -30 = body

		FVector posHead = posFeet;

		FRotator Rotation = CurrentController.GetRotation();
		FVector RealLocation = CurrentCamera.GetRealLocation();

		bool isPawnVisible = mesh.IsVisible(CurrentWorldInfo.GetTimeSeconds());
		auto posw2s = W2S(posFeet, pos, Rotation, RealLocation, CurrentFOV);

		// TRACERS
		auto tracerDistance = math::VectorMagnitude(math::VectorSubtract(RealLocation, posFeet)) / 50.f;
		int red = (int)max(0, 250 - tracerDistance);
		Vec2 normalizedHead;
		normalizedHead.x = pos.x - ScreenCenterX;
		normalizedHead.y = pos.y - ScreenCenterY;
		math::Normalize(normalizedHead);
		const float offsetCircle = 10.0f;

		auto tracerColor = D2D1::ColorF(isPawnVisible ? 0.f : red, min(250, tracerDistance) / 255.f, isPawnVisible ? red : 0);// D3DCOLOR_ARGB(255, isPawnVisible ? 0 : red, (int)min(250, tracerDistance), isPawnVisible ? red : 0);
		if(tracersEnabled) {
			g_overlay->draw_line(ScreenCenterX + normalizedHead.x * offsetCircle, ScreenCenterY + normalizedHead.y * offsetCircle, pos.x, pos.y, tracerColor);
		}

		try {
			if(W2S(min, smin, Rotation, RealLocation, CurrentFOV) &&
				W2S(max, smax, Rotation, RealLocation, CurrentFOV) &&
				posw2s)
			{

				float flWidth = fabs((smax.y - smin.y) / 4);
				//auto color = isPawnVisible ? D3DCOLOR_ARGB(255, 250, 0, 0) : D3DCOLOR_ARGB(255, 0, 0, 255);

				//BOX
				if(boxESPEnabled) {
					g_overlay->draw_line(pos.x - flWidth, smin.y, pos.x + flWidth, smin.y, tracerColor); // bottom
					g_overlay->draw_line(pos.x - flWidth, smax.y, pos.x + flWidth, smax.y, tracerColor); // up
					g_overlay->draw_line(pos.x - flWidth, smin.y, pos.x - flWidth, smax.y, tracerColor); // left
					g_overlay->draw_line(pos.x + flWidth, smin.y, pos.x + flWidth, smax.y, tracerColor); // right

				}

				//HP
				if(hpESPEnabled) {
					auto shield = CurrentPawn.GetSheild();
					g_overlay->draw_text(pos.x + flWidth + 10, smax.y + 10, D2D1::ColorF(1.f, 1.f, 0), "HP:%d", (Hp));
					g_overlay->draw_text(pos.x + flWidth + 10, smax.y + 20, D2D1::ColorF(1.f, 1.f, 0), "SHIELD:%d", (shield));

					auto replInfo = CurrentPawn.GetPlayerReplicationInfo();
					auto name = replInfo.GetName().ToWString();
					g_overlay->draw_text(pos.x + flWidth + 10, smax.y + 30, D2D1::ColorF(1.f, 1.f, 0), "%S", name.c_str());
					bool isBot = replInfo.IsBot();
					if(isBot) {
						g_overlay->draw_text(pos.x + flWidth + 10, smax.y + 40, D2D1::ColorF(1.f, 0.f, 0), "Bot");
					}

					auto hpProcentage = 1 - (float)Hp / CurrentPawn.GetMaxHealth();
					auto shieldProcentage = 1 - (float)shield / CurrentPawn.GetMaxShield();

					g_overlay->draw_box(pos.x, smin.y + 5, flWidth * 2 + 1, 6, D2D1::ColorF(0, 0, 0));
					g_overlay->draw_box(pos.x - (hpProcentage)*flWidth, smin.y + 5, flWidth * 2 * (1 - hpProcentage), 5, D2D1::ColorF(0.39, 1.00, 0.44));


					g_overlay->draw_box(pos.x, smin.y + 12, flWidth * 2 + 1, 6, D2D1::ColorF(0, 0, 0));
					g_overlay->draw_box(pos.x - (shieldProcentage)*flWidth, smin.y + 12, flWidth * 2 * (1 - shieldProcentage), 5, D2D1::ColorF(0.39, 0.4, 1.00));
				}

				if(isAimbotActive && isPawnVisible && !Locked) {
					Vec2 headPos;
					if(W2S(posHead, headPos, Rotation, RealLocation, CurrentFOV))
					{
						//TODO SETTING
						float aimfov = 30.0f;//(Settings.AimbotFOV + 1) * ((PlayerController->LODDistanceFactor * 4)); //TODO FIX THIS
						float cx = headPos.x;
						float cy = headPos.y;
						float radiusx_ = aimfov * (ScreenCenterX / CurrentFOV);
						float radiusy_ = aimfov * (ScreenCenterY / CurrentFOV);
						float crosshairDistance = math::GetCrosshairDistance(cx, cy, ScreenCenterX, ScreenCenterY);
						if(tracerDistance < 30.f || cx >= ScreenCenterX - radiusx_ && cx <= ScreenCenterX + radiusx_ && cy >= ScreenCenterY - radiusy_ && cy <= ScreenCenterY + radiusy_)
						{
							if(crosshairDistance < ClosestDistance)
							{
								ClosestDistance = crosshairDistance;
								LockedPawn = CurrentPawn;
							}
						}
						CurrentPawn = nextPawn;
						continue;
					}
				}
			}
			CurrentPawn = nextPawn;
		}
		catch(const std::exception&) {
			CurrentPawn = nextPawn;
			continue;
		}
	}

	if(IsValid(LockedPawn.data))
	{
		Locked = true;
		callAimbot();
	}

	auto running = "Running";
	auto loopFrame = (frame % 400);
	if(loopFrame < 100) {
		running = "Running |";
	}
	else if(loopFrame < 200) {
		running = "Running /";
	}
	else if(loopFrame < 300) {
		running = "Running -";
	}
	else if(loopFrame < 400) {
		running = "Running \\";
	}

	g_overlay->draw_text(5, 5, D2D1::ColorF(0.18f, 0.24f, 0.64f), running);
}

static void render(FOverlay* overlay)
{
	while(rendering)
	{
		overlay->begin_scene();

		overlay->clear_scene();
		frame++;
		HackTick();

		overlay->end_scene();
	}
}

void exiting() {
	std::cout << "Exiting";

	rendering = false;
	g_overlay->begin_scene();

	g_overlay->clear_scene();

	g_overlay->draw_text_white(200, 200, "END");

	g_overlay->end_scene();
}

static void _init(FOverlay* overlay)
{
	// Initialize the window
	if(!overlay->window_init()) {
		printf("[!] Error init overlay window\n");
		return;
	}

	// D2D Failed to initialize?
	if(!overlay->init_d2d())
		return;

	// render loop
	std::thread r(render, overlay);
	std::thread nor(NoRecoil);

	r.join(); // threading

	overlay->d2d_shutdown();

	return;
}

int main()
{
	SetConsoleTitle("BLUEFIRE1337's Realm Royale Cheat");
	SetUnhandledExceptionFilter(SimplestCrashHandler);
	//initTrace();

	system("sc stop mhyprot2"); // RELOAD DRIVER JUST IN CASE
	system("CLS"); // CLEAR

	auto process_name = "Realm.exe";
	auto process_id = GetProcessId(process_name);
	if(!process_id)
	{
		printf("[!] process \"%s\ was not found\n", process_name);
		return -1;
	}

	printf("[+] %s (%d)\n", process_name, process_id);

	//
	// initialize its service, etc
	//
	if(!mhyprot::init())
	{
		printf("[!] failed to initialize vulnerable driver\n");
		return -1;
	}

	if(!mhyprot::driver_impl::driver_init(
		false, // print debug
		false // print seedmap
	))
	{
		printf("[!] failed to initialize driver properly\n");
		mhyprot::unload();
		return -1;
	}
	process_base = GetProcessBase(process_id);
	if(!process_base) {
		printf("[!] failed to get baseadress\n");
		mhyprot::unload();
		return -1;
	}

	//printf("[+] Game Base is 0x%llX\n", process_base);
	IMAGE_DOS_HEADER dos_header = read<IMAGE_DOS_HEADER>(process_base);
	printf("[+] Game header Magic is 0x%llX\n", dos_header.e_magic);
	if(dos_header.e_magic != 0x5A4D) {
		printf("[!] Game header Magic should be 0x5A4D\n");
	}

	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	HDC monitor = GetDC(hDesktop);

	int current = GetDeviceCaps(monitor, VERTRES);
	int total = GetDeviceCaps(monitor, DESKTOPVERTRES);

	FOverlay::ScreenWidth = (desktop.right - desktop.left) * total / current;
	FOverlay::ScreenHeight = (desktop.bottom - desktop.top) * total / current;

	ScreenCenterX = FOverlay::ScreenWidth / 2.f;
	ScreenCenterY = FOverlay::ScreenHeight / 2.f;

	MainAddress();

	g_overlay = { 0 };
	_init(g_overlay);
}

