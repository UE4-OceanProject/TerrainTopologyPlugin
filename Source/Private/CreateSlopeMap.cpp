#include "CreateSlopeMap.h"
#include "TMath.h"

ACreateSlopeMap::ACreateSlopeMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = false;
}

bool ACreateSlopeMap::OnChange()
{
	return m_currentColorMode != m_coloredGradient;
}

void ACreateSlopeMap::CreateMap()
{

	UTexture2D* slopeMap = CreateTexture(m_width, m_height);


	uint8* MipData = static_cast<uint8*>(slopeMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	// Create base mip.
	uint8* DestPtr = NULL;

	for (int y = 0; y < m_height; y++)
	{
		DestPtr = &MipData[(m_height - 1 - y) * m_width * sizeof(FColor)];
		//SrcPtr = const_cast<FLinearColor*>(&SrcData[(m_height - 1 - y) * m_width]);

		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1 = GetFirstDerivative(x, y);

			float slope = Slope(d1->X, d1->Y);

			auto color = Colorize(slope, 0.4f, true);

			FColor UEColor = color.ToFColor(false);

			//slopeMap->SetPixel(x, y, color);
			*DestPtr++ = UEColor.B;
			*DestPtr++ = UEColor.G;
			*DestPtr++ = UEColor.R;
			*DestPtr++ = UEColor.A;
		}

	}

	slopeMap->PlatformData->Mips[0].BulkData.Unlock();
	slopeMap->UpdateResource();
	//slopeMap->Apply();
	m_material = slopeMap;

}

float ACreateSlopeMap::Slope(float zx, float zy)
{
	float p = zx * zx + zy * zy;
	float g = TMath::SafeSqrt(p);

	return FMath::Atan(g) * TMath::Rad2Deg / 90.0f;
}