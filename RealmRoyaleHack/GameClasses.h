#pragma once

struct FVector
{
public:
	float X;
	float Y;
	float Z;
	FVector() {

	}

	inline FVector(float x, float y, float z) {
		X = x;
		Y = y;
		Z = z;
	}

	inline FVector operator+(FVector a) {
		return { a.X + X,a.Y + Y, a.Z + Z };
	}
	inline FVector operator-(FVector a) {
		return { a.X - X,a.Y - Y, a.Z - Z };
	}
	inline FVector operator*(int a) {
		return { a * X,a * Y, a * Z };
	}
	inline FVector operator/(int a) {
		return { X / a,Y / a,  Z / a };
	}
	inline FVector operator/(float a) {
		return { X / a,Y / a,  Z / a };
	}
	inline float Size() {
		return sqrt(X * X + Y * Y + Z * Z);
	}
	inline FVector Normalize() {
		auto temp = FVector(X, Y, Z);
		auto size = temp.Size();
		if(size <= 0.f) {
			size = 0.1f;
		}
		return  temp / size;
	}

	inline void PrintVal() {
		printf("( %f,%f,%f )\n",X,Y,Z);
	}
};


struct FRotator
{
	int Pitch;
	int Yaw;
	int Roll;

	inline FRotator operator+(FRotator a) {
		return { a.Pitch + Pitch,a.Yaw + Yaw, a.Roll + Roll };
	}

	inline FRotator operator-(FRotator a) {
		return { a.Pitch - Pitch,a.Yaw - Yaw, a.Roll - Roll };
	}

	inline FRotator operator*(FRotator a) {
		return { a.Pitch * Pitch,a.Yaw * Yaw, a.Roll * Roll };
	}

	inline FRotator operator*(float a) {
		return { (int)round(a * Pitch),(int)round(a * Yaw), (int)round(a * Roll) };
	}
};

// ScriptStruct Core.Object.BoxSphereBounds
// 0x001C
struct FBoxSphereBounds
{
	struct FVector                                     Origin;                                                   // 0x0000(0x000C) (Edit)
	struct FVector                                     BoxExtent;                                                // 0x000C(0x000C) (Edit)
	float                                              SphereRadius;                                             // 0x0018(0x0004) (Edit)
};

struct FAccuracySettings
{
	int                                                bUsesAdvancedAccuracy;                                    // 0x0000(0x0004)
	float                                              fMaxAccuracy;                                             // 0x0004(0x0004)
	float                                              fMinAccuracy;                                             // 0x0008(0x0004)
	float                                              fAccuracyLossPerShot;                                     // 0x000C(0x0004)
	float                                              fAccuracyGainPerSec;                                      // 0x0010(0x0004)
	float                                              fAccuracyGainDelay;                                       // 0x0014(0x0004)
	int                                                nNumFreeShots;                                            // 0x0018(0x0004)
};

struct FRecoilSettings {
	int                                                bUsesRecoil;                                              // 0x0000(0x0004)
	float                                              fRecoilReductionPerSec;                                   // 0x0004(0x0004)
	float                                              fRecoilCenterDelay;                                       // 0x0008(0x0004)
	float                                              fRecoilSmoothRate;                                        // 0x000C(0x0004) 
};