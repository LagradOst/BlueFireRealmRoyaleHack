//BLUEFIRE1337 REALM ROYALE SDK - UPDATED 2021-09-25
#pragma once

constexpr auto ENGINE = 0x2D02810; //process_base + ENGINE -> UEngine

constexpr auto LOCALPLAYER = 0x6D4; //UEngine -> localPlayer
constexpr auto CONTROLLER = 0x68; //localPlayer -> controller

constexpr auto ROTATION = 0x008C; // controller -> FRotator
constexpr auto WORLD_INFO = 0x0120; // controller -> worldInfo

constexpr auto ACKNOWLEDGEDPAWN = 0x0498; //controller -> acknowledgedPawn
constexpr auto REPLICATIONINFO = 0x0290; //controller -> replicationInfo
constexpr auto PLAYER_FOV_MULTIPLIER = 0x04CC; //controller -> float
constexpr auto CAMERA = 0x0478; //controller -> camera

constexpr auto REAL_LOCATION = 0x0494; // camera -> FVector
constexpr auto DEAFULT_FOV = 0x0290; //camera -> float
//FOV = DEAFULT_FOV * PLAYER_FOV_MULTIPLIER

constexpr auto PAWN_LIST = 0x05B4; // worldInfo -> pawnList
constexpr auto TIMESECONDS = 0x04EC; // worldInfo -> float

constexpr auto AMMO_COUNT = 0x04CC; // ATgDevice -> int
constexpr auto AMMO_MAX_COUNT = 0x4D4; // ATgDevice -> int

constexpr auto WEAPON = 0x04E4; // pawn -> ATgDevice
constexpr auto HEALTH = 0x3C4; // pawn -> int
constexpr auto MAX_HEALTH = 0x7AC; // pawn -> int

constexpr auto SHIELD = 0x998; // pawn -> float (convert to int)
constexpr auto LOCATION = 0x0080; // pawn -> FVector
constexpr auto VELOCITY = 0x0190; // pawn -> FVector
constexpr auto PLAYER_REPLICATION_INFO = 0x0440; // pawn -> PlayerReplicationInfo
constexpr auto NEXT_PAWN = 0x02AC; // pawn -> nextpawn
constexpr auto MESH = 0x048C; // pawn -> mesh

constexpr auto BOUNDS = 0x00A0; // mesh -> FBoxSphereBounds
constexpr auto LAST_RENDER_TIME = 0x0244; // mesh -> float

constexpr auto INSTANT_MOUNT = 0x2e2c; // localpawn -> write 0f

constexpr auto PLAYER_NAME = 0x0290; // replicationInfo -> FString (Tarray<wchar_t>)
constexpr auto IS_CHICKEN = 0x728; // pawn -> int

constexpr auto IS_CHICKEN_ID = 9204; 
constexpr auto IS_TURRET_ID = 4664;

constexpr auto CURRENT_PROJECTILES = 0x0610; // ATgDevice -> TArray<ATgProjectile>
constexpr auto PROJECTILE_SPEED = 0x0280; // ATgProjectile -> float

constexpr auto WORLD_GRAVITY_Z = 0x058C; // worldInfo -> float

constexpr auto ACCURACY= 0x944; // ATgDevice -> FAccuracySettings
constexpr auto RECOIL = 0x9E4; // ATgDevice -> FRecoilSettings

constexpr auto IS_BOT = 0x02BC; // APlayerReplicationInfo -> int >> 6 & 1