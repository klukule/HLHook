#pragma once

namespace HogwartsLegacy {
	public ref struct MFVector {
	public:
		float X;
		float Y;
		float Z;
	};

	public ref struct MFRotator {
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

		void InitializeOverwolf(System::Action<System::Boolean>^ callback);
		void GetPlayerPositionAndRotationOverwolf(System::Action<System::Object^, System::Object^>^ callback);
	};
}
