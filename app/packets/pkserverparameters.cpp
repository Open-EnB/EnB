// Earth & Beyond client (reconstruction): app\packets\pkserverparameters.cpp. Notes at the end of the file.
#include "pkserverparameters.h"
#include <stdio.h>

ServerParametersPacketClass::ServerParametersPacketClass(float zband_min, float zband_max, const Vector2 &min, const Vector2 &max, float fog_near, float fog_far, int debris_mode, bool light_backdrop, bool fog_backdrop, bool swap_backdrop, float backdrop_fog_near, float backdrop_fog_far, float max_tilt, bool auto_level, float impulse_rate, float decay_velocity, float decay_spin, short backdrop_base_asset, unsigned int sector_num) :
	ZBandMin(zband_min),
	ZBandMax(zband_max),
	Min(min),
	Max(max),
	FogNear(fog_near),
	FogFar(fog_far),
	DebrisMode(debris_mode),
	LightBackdrop(light_backdrop),
	FogBackdrop(fog_backdrop),
	SwapBackdrop(swap_backdrop),
	BackdropFogNear(backdrop_fog_near),
	BackdropFogFar(backdrop_fog_far),
	MaxTilt(max_tilt),
	AutoLevel(auto_level),
	ImpulseRate(impulse_rate),
	DecayVelocity(decay_velocity),
	DecaySpin(decay_spin),
	BackdropBaseAsset(backdrop_base_asset),
	SectorNum(sector_num)
{
}

ServerParametersPacketClass::ServerParametersPacketClass(RawPacketClass *raw)
{
	raw->Get(&ZBandMin, sizeof(ZBandMin));
	raw->Get(&ZBandMax, sizeof(ZBandMax));
	raw->Get(&Min, sizeof(Min));
	raw->Get(&Max, sizeof(Max));
	raw->Get(&FogNear, sizeof(FogNear));
	raw->Get(&FogFar, sizeof(FogFar));
	raw->Get(&DebrisMode, sizeof(DebrisMode));
	raw->Get(&LightBackdrop, sizeof(LightBackdrop));
	raw->Get(&FogBackdrop, sizeof(FogBackdrop));
	raw->Get(&SwapBackdrop, sizeof(SwapBackdrop));
	raw->Get(&BackdropFogNear, sizeof(BackdropFogNear));
	raw->Get(&BackdropFogFar, sizeof(BackdropFogFar));
	raw->Get(&MaxTilt, sizeof(MaxTilt));
	raw->Get(&AutoLevel, sizeof(AutoLevel));
	raw->Get(&ImpulseRate, sizeof(ImpulseRate));
	raw->Get(&DecayVelocity, sizeof(DecayVelocity));
	raw->Get(&DecaySpin, sizeof(DecaySpin));
	raw->Get(&BackdropBaseAsset, sizeof(BackdropBaseAsset));
	raw->Get(&SectorNum, sizeof(SectorNum));
}

RawPacketClass *ServerParametersPacketClass::Build()
{
	RawPacketClass *raw = new RawPacketClass(66, Get_Data_Size());
	raw->Add(&ZBandMin, sizeof(ZBandMin));
	raw->Add(&ZBandMax, sizeof(ZBandMax));
	raw->Add(&Min, sizeof(Min));
	raw->Add(&Max, sizeof(Max));
	raw->Add(&FogNear, sizeof(FogNear));
	raw->Add(&FogFar, sizeof(FogFar));
	raw->Add(&DebrisMode, sizeof(DebrisMode));
	raw->Add(&LightBackdrop, sizeof(LightBackdrop));
	raw->Add(&FogBackdrop, sizeof(FogBackdrop));
	raw->Add(&SwapBackdrop, sizeof(SwapBackdrop));
	raw->Add(&BackdropFogNear, sizeof(BackdropFogNear));
	raw->Add(&BackdropFogFar, sizeof(BackdropFogFar));
	raw->Add(&MaxTilt, sizeof(MaxTilt));
	raw->Add(&AutoLevel, sizeof(AutoLevel));
	raw->Add(&ImpulseRate, sizeof(ImpulseRate));
	raw->Add(&DecayVelocity, sizeof(DecayVelocity));
	raw->Add(&DecaySpin, sizeof(DecaySpin));
	raw->Add(&BackdropBaseAsset, sizeof(BackdropBaseAsset));
	raw->Add(&SectorNum, sizeof(SectorNum));
	return(raw);
}

int ServerParametersPacketClass::Get_Data_Size()
{
	return(sizeof(ZBandMin) + sizeof(ZBandMax) + sizeof(Min) + sizeof(Max) + sizeof(FogNear) + sizeof(FogFar) + sizeof(DebrisMode) + sizeof(LightBackdrop) + sizeof(FogBackdrop) + sizeof(SwapBackdrop) + sizeof(BackdropFogNear) + sizeof(BackdropFogFar) + sizeof(MaxTilt) + sizeof(AutoLevel) + sizeof(ImpulseRate) + sizeof(DecayVelocity) + sizeof(DecaySpin) + sizeof(BackdropBaseAsset) + sizeof(SectorNum));
}

int ServerParametersPacketClass::Print(char *buffer, int)
{
	return(sprintf(buffer, "%s { ZBandMin=%f ZBandMax=%f XMin=%f YMin = %f XMax = %f YMax = %f FogNear=%f FogFar=%f DebrisMode=%d LightBackdrop=%s FogBackdrop=%s SwapBackdrop=%s BackdropFogNear=%f BackdropFogFar=%f MaxTilt=%f AutoLevel=%s ImpulseRate=%f DecayVelocity=%f DecaySpin=%f BackdropBaseAsset=%hd SectorNum=%u}", "Server_Parameters",
		ZBandMin, ZBandMax, Min.X, Min.Y, Max.X, Max.Y, FogNear, FogFar, DebrisMode,
		LightBackdrop ? "true" : "false", FogBackdrop ? "true" : "false", SwapBackdrop ? "true" : "false",
		BackdropFogNear, BackdropFogFar, MaxTilt, AutoLevel ? "true" : "false", ImpulseRate, DecayVelocity, DecaySpin,
		BackdropBaseAsset, SectorNum));
}


/*
**	Reconstruction notes (pkserverparameters.cpp)
**
**	* Client 0x8A5AB0-0x8A60F0 (0x8A5A80 is the previous object's guarded static, 0xC416FC): SERVER_PARAMETERS
**	  66, written by hand (builder P4, 2026-10-26). Its implicit destructor is this object's (0x8A5BF0).
**	* The file name is provisional (no assert names it).
*/
