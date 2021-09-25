// MADE BY BLUEFIRE1337

#pragma once
#include <cstdint>
#include "Offsets.h"
#include "GameClasses.h"
#include "Mhyprot/baseadress.h"

template<class T>
class TArray
{
	friend struct FString;
public:
	int Length() const
	{
		return m_nCount;
	}

	bool IsValid() const
	{
		if(m_nCount > m_nMax)
			return false;
		if(!m_Data)
			return false;
		return true;
	}

	uint64_t GetAddress() const
	{
		return m_Data;
	}

	T GetById(int i)
	{
		return read<T>(m_Data + i * 8);
	}


protected:
	uint64_t m_Data;
	uint32_t m_nCount;
	uint32_t m_nMax;
};

struct FString : public TArray<wchar_t>
{
	std::wstring ToWString() const
	{
		wchar_t* buffer = new wchar_t[m_nCount];
		read_array(m_Data, buffer, m_nCount);
		std::wstring ws(buffer);
		delete[] buffer;

		return ws;
	}

	std::string ToString() const
	{
		std::wstring ws = ToWString();
		std::string str(ws.begin(), ws.end());

		return str;
	}
};

class USkeletalMeshComponent {
public:
	inline FBoxSphereBounds GetBounds() {
		return read<FBoxSphereBounds>(data + BOUNDS);
	}

	inline float GetLastRenderTime() {
		return read<float>(data + LAST_RENDER_TIME);
	}

	inline bool IsVisible(float timeSeconds) {
		float lastRenderTime = GetLastRenderTime();
		return (lastRenderTime > timeSeconds - 0.05f);
	}

	uint64_t data;
};

class APlayerReplicationInfo {
public:
	FString GetName() {
		return read<FString>(data + PLAYER_NAME);
	}
	bool IsBot() {
		return read<int>(data + IS_BOT) >> 6 & 1;
	}

	uint64_t data;
};

class ATgProjectile {
public:
	inline float GetSpeed() {
		return read<float>(data + PROJECTILE_SPEED);
	}
	uint64_t data;
};

class ATgDevice {
public:
	// YOU CAN *SET* AMMO COUNT IN TUTORIAL, HOWEVER NOT IN A REAL MATCH
	inline int GetAmmoCount() {
		return read<int>(data + AMMO_COUNT);
	}

	inline int GetMaxAmmoCount() {
		return read<int>(data + AMMO_MAX_COUNT);
	}

	inline TArray<ATgProjectile> GetProjectiles() {
		return read<TArray<ATgProjectile>>(data + CURRENT_PROJECTILES);
	}

	inline FRecoilSettings GetRecoil(bool isScoped) {
		return read<FRecoilSettings>(data + RECOIL + (isScoped ? sizeof(FRecoilSettings) : 0));
	}

	inline bool SetRecoil(FRecoilSettings recoil, bool isScoped) {
		return write(data + RECOIL + (isScoped ? sizeof(FRecoilSettings) : 0), recoil);
	}

	inline void NoRecoil() {
		auto recoil = FRecoilSettings();
		recoil.bUsesRecoil = 1;
		recoil.fRecoilReductionPerSec = 0;
		recoil.fRecoilCenterDelay = 0;
		recoil.fRecoilSmoothRate = 0;
		SetRecoil(recoil, false);
		SetRecoil(recoil, true);
	}

	inline FAccuracySettings GetSpread(bool isScoped) {
		return read<FAccuracySettings>(data + ACCURACY + (isScoped ? sizeof(FAccuracySettings) : 0));
	}

	inline bool SetSpread(FAccuracySettings accuracy, bool isScoped) {
		return write(data + ACCURACY + (isScoped ? sizeof(FAccuracySettings) : 0), accuracy);
	}

	inline void NoSpread() {
		auto accuracy = GetSpread(false);
		accuracy.fAccuracyGainPerSec = 0;
		accuracy.fMaxAccuracy = 1;
		accuracy.fMinAccuracy = 1;
		SetSpread(accuracy, false);
		SetSpread(accuracy, true);
	}

	uint64_t data;
};

class APawn {
public:
	inline int GetId() {
		return read<int>(data + IS_CHICKEN);
	}

	inline bool IsChicken() {
		return GetId() == IS_CHICKEN_ID;
	}

	inline bool IsTurret() {
		return GetId() == IS_TURRET_ID;
	}

	inline ATgDevice GetWeapon() {
		return read<ATgDevice>(data + WEAPON);
	}

	inline int GetHealth() {
		return read<int>(data + HEALTH);
	}
	inline int GetMaxHealth() {
		return (int)read<float>(data + MAX_HEALTH);
	}

	inline int GetSheild() {
		return (int)read<float>(data + SHIELD);
	}

	inline int GetMaxShield() {
		return 1500;
	}

	inline FVector GetLocation() {
		return read<FVector>(data + LOCATION);
	}

	inline FRotator GetRotation() {
		return read<FRotator>(data + ROTATION);
	}

	inline FVector GetVelocity() {
		return read<FVector>(data + VELOCITY);
	}

	inline APlayerReplicationInfo GetPlayerReplicationInfo() {
		return read<APlayerReplicationInfo>(data + PLAYER_REPLICATION_INFO);
	}

	inline APawn GetNextPawn() {
		return read<APawn>(data + NEXT_PAWN);
	}

	inline USkeletalMeshComponent GetMesh() {
		return read<USkeletalMeshComponent>(data + MESH);
	}

	inline void SetInstantMount() {
		write(data + INSTANT_MOUNT, 0.f);
	}

	// ONLY USE FOR TESTING, SET LOCATION DOES NOT WORK IN A REAL MATCH
	inline void WarpToTutorialWeapons() {
		write(data + LOCATION, FVector(-44042.683594, 22772.787109, -1315.050049));
	}

	inline void WarpToShootingRange() {
		write(data + LOCATION, FVector(-41854.433594, 11586.427734, -1204.877808) + FVector(0, 1000, 0));
	}

	uint64_t data;
};

class ACamera {
public:
	inline float GetDeafultFov() {
		return read<float>(data + DEAFULT_FOV);
	}

	inline FVector GetRealLocation() {
		return read<FVector>(data + REAL_LOCATION);
	}

	uint64_t data;
};

class AWorldInfo {
public:
	inline float GetTimeSeconds() {
		return read<float>(data + TIMESECONDS);
	}

	inline float GetGravity() {
		return read<float>(data + WORLD_GRAVITY_Z);
	}

	inline APawn GetPawnList() {
		return read<APawn>(data + PAWN_LIST);
	}

	uint64_t data;
};

class APlayerController {
public:
	inline FRotator GetRotation() {
		return read<FRotator>(data + ROTATION);
	}

	inline void SetRotation(FRotator rotation) {
		write(data + ROTATION, rotation);
	}

	inline float GetFovMultiplier() {
		return read<float>(data + PLAYER_FOV_MULTIPLIER);
	}

	inline APawn GetAcknowledgedPawn() {
		return read<APawn>(data + ACKNOWLEDGEDPAWN);
	}

	inline ACamera GetCamera() {
		return read<ACamera>(data + CAMERA);
	}

	inline AWorldInfo GetWorldInfo() {
		return read<AWorldInfo>(data + WORLD_INFO);
	}

	uint64_t data;
};

class ULocalPlayer {
public:
	inline APlayerController GetController() {
		return read<APlayerController>(data + CONTROLLER);
	}

	uint64_t data;
};

class UEngine {
public:
	inline ULocalPlayer GetLocalPlayer() {
		return read<ULocalPlayer>(read<uint64_t>(data + LOCALPLAYER));
	}

	uint64_t data;
};


UEngine GetUEngine(uint64_t base) {
	return read<UEngine>(base + ENGINE);
}

struct FName
{
	uint32_t Index;
	uint32_t Number;

	inline bool operator==(const FName& other) const
	{
		return Index == other.Index;
	};
};
