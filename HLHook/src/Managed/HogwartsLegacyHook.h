#pragma once

namespace HogwartsLegacy {
	public ref class MFVector {
	public:
		float X;
		float Y;
		float Z;
	};

	public ref class MFRotator {
	public:
		float Pitch;
		float Yaw;
		float Roll;
	};

	public ref class Hook
	{
	public:
		bool Initialize();
		void GetPlayerPositionAndRotation([System::Runtime::InteropServices::Out] MFVector^% position, [System::Runtime::InteropServices::Out] MFRotator^% rotation);
		void GetPlayerPositionAndRotationOverwolf(System::Action<System::Object^, System::Object^>^ callback);
	};
}
