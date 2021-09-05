#include "CreateAspectMap.h"
#include "TMath.h"


bool ACreateAspectMap::OnChange()
{
	return m_currentColorMode != m_coloredGradient;
}

void ACreateAspectMap::CreateMap()
{

	UTexture2D* aspectMap = CreateTexture(m_width, m_height);


	uint8* MipData = static_cast<uint8*>(aspectMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	// Create base mip.
	uint8* DestPtr = NULL;
	//const FLinearColor* SrcPtr = NULL;

	for (int y = 0; y < m_height; y++)
	{
		DestPtr = &MipData[(m_height - 1 - y) * m_width * sizeof(FColor)];
		//SrcPtr = const_cast<FLinearColor*>(&SrcData[(m_height - 1 - y) * m_width]);

		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1 = GetFirstDerivative(x, y);

			float aspect = static_cast<float>(Aspect(d1->X, d1->Y));

			auto color = Colorize(aspect, 0, true).ToFColor(false);

			//aspectMap->SetPixel(x, y, color);

			*DestPtr++ = color.B;
			*DestPtr++ = color.G;
			*DestPtr++ = color.R;
			*DestPtr++ = color.A;
		}

	}

	
	aspectMap->PlatformData->Mips[0].BulkData.Unlock();
	aspectMap->UpdateResource();
	//aspectMap->Apply();


	//SetTextureFromArray(m_material, Pixels);
	m_material = aspectMap;

}

float ACreateAspectMap::Aspect(float zx, float zy)
{
	float gyx = TMath::SafeDiv(zy, zx);
	float gxx = TMath::SafeDiv(zx, FMath::Abs(zx));

	float aspect = 180 - FMath::Atan(gyx) * TMath::Rad2Deg + 90 * gxx;
	aspect /= 360;

	return aspect;
}