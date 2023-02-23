#pragma once

#include "Utils.h"
#include "Memory.h"

#pragma warning(disable:4996)

template<class T>
class TArray
{
public:
	T* Data;
	int Count;
	int Max;

	friend class FString;

	bool IsValidIndex(const int i) const
	{
		return i < Count;
	}
};


class FString : public TArray<wchar_t>
{
	std::string ToString() const
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, Data, Count, nullptr, 0, nullptr, nullptr);
		std::string str(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, Data, Count, &str[0], size, nullptr, nullptr);
		return str;
	}
};

struct FVector {
	float X;
	float Y;
	float Z;
};

struct FRotator {
	float Pitch;
	float Yaw;
	float Roll;
};