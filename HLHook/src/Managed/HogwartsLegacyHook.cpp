#include "HogwartsLegacyHook.h"
#include "../Scanner.h"
#include "../Memory.h"
#include "../UnrealEngine.h"
bool HogwartsLegacy::Hook::Initialize()
{
	Scanner* scanner = Scanner::GetInstance();
	if (!ErrorHandler(scanner->Initialize(), 0))
		return true;

	if (!ErrorHandler(scanner->FindUWorlds(), 0))
		return true;

	Log("Found UWorlds: ");
	Green;
	printf("0x%p\n\n", NativeMemory::GetInstance()->GetAddress("UWorlds"));
	Gray;

	return false;
}

void HogwartsLegacy::Hook::GetPlayerPositionAndRotation(MFVector^% position, MFRotator^% rotation)
{
	position = gcnew MFVector();
	rotation = gcnew MFRotator();

	auto UWorld = Read<char*>(NativeMemory::GetInstance()->GetAddress("UWorlds"));	// Get first UWorld from the array
	auto OwningGameInstance = Read<char*>(UWorld + 0x320);							// Get Owning Game Instance
	auto LocalPlayers = Read<char*>(OwningGameInstance + 0x38);						// Get local players array
	auto LocalPlayer = Read<char*>(LocalPlayers + 0x00);							// Get First local player
	auto PlayerController = Read<char*>(LocalPlayer + 0x30);						// Get associated player controller
	auto Pawn = Read<char*>(PlayerController + 0x278);								// Get associated pawn (character)
	auto RootComponent = Read<char*>(Pawn + 0x158);									// Get root component
	auto RelativeLocation = Read<FVector>(RootComponent + 0x134);					// Get location
	auto RelativeRotation = Read<FRotator>(RootComponent + 0x140);					// Get rotation

	// Copy native to managed
	position->X = RelativeLocation.X;
	position->Y = RelativeLocation.Y;
	position->Z = RelativeLocation.Z;

	rotation->Pitch = RelativeRotation.Pitch;
	rotation->Yaw = RelativeRotation.Yaw;
	rotation->Roll = RelativeRotation.Roll;
}

void HogwartsLegacy::Hook::GetPlayerPositionAndRotationOverwolf(System::Action<System::Object^, System::Object^>^ callback)
{
	MFVector^ pos;
	MFRotator^ rot;

	GetPlayerPositionAndRotation(pos, rot);

	callback(pos, rot);
}

