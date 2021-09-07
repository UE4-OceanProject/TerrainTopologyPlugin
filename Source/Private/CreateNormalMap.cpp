#include "CreateNormalMap.h"

ACreateNormalMap::ACreateNormalMap(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	m_coloredGradient = false;
}

void ACreateNormalMap::CreateMap()
{

	//TArray<FLinearColor> Pixels;
	//Pixels.SetNum(m_width * m_height);
	UTexture2D* normalMap = CreateTexture(m_width, m_height);


	uint8* MipData = static_cast<uint8*>(normalMap->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE));

	// Create base mip.
	uint8* DestPtr = NULL;

	for (int y = 0; y < m_height; y++)
	{
		DestPtr = &MipData[(m_height - 1 - y) * m_width * sizeof(FColor)];
		//SrcPtr = const_cast<FLinearColor*>(&SrcData[(m_height - 1 - y) * m_width]);
		for (int x = 0; x < m_width; x++)
		{
			FVector2D* d1 = GetFirstDerivative(x, y);

			//Not to sure of the orientation.
			//Might need to flip x or y

			auto n = new FVector();
			n->X = d1->X * 0.5f + 0.5f;
			n->Y = -d1->Y * 0.5f + 0.5f;
			n->Z = 1.0f;

			n->Normalize();
			n = new FVector(n->X, n->Y, n->Z);

			//Pixels[x + y * m_width] = FLinearColor(n->X, n->Y , n->Z , 1 );
			FColor UEColor = FLinearColor(n->X, n->Y, n->Z, 1).ToFColor(false);
			*DestPtr++ = UEColor.B ;
			*DestPtr++ = UEColor.G ;
			*DestPtr++ = UEColor.R ;
			*DestPtr++ = UEColor.A;
			delete n;
		}

	}

	//SetTextureFromArray(m_material, Pixels);

	
	normalMap->PlatformData->Mips[0].BulkData.Unlock();
	normalMap->UpdateResource();
	//aspectMap->Apply();


	//SetTextureFromArray(m_material, Pixels);
	m_material = normalMap;

}