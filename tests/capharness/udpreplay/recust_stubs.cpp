// udpreplay: recustomization_cost.cpp's wire object (client 0x8492F0, 0x849920-0x8499A0), which
// RECUSTOMIZE_AVATAR_START 131 holds; the file is not rebuilt. Same bytes on the wire as the client's.
#include "..\..\..\app\packets\pkrecustomize.h"

RecustomizationCostClass::RecustomizationCostClass()
{
	for (int i = 0; i < 14; i++) Data.Unknown00[i] = 0;
}

RecustomizationCostClass::RecustomizationCostClass(const RecustomizationCostDataStruct &data) : Data(data)
{
}

bool RecustomizationCostClass::Write(LinearBufferClass *buffer)
{
	for (int i = 0; i < 14; i++) buffer->Write(&Data.Unknown00[i], 4);
	return(true);
}

bool RecustomizationCostClass::Read(LinearBufferClass *buffer)
{
	for (int i = 0; i < 14; i++) buffer->Read(&Data.Unknown00[i], 4);
	return(true);
}

int RecustomizationCostClass::Get_Size()
{
	return(0x38);
}
