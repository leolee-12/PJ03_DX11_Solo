#include "stdafx.h"
#include "..\Public\AnimationNameMap.h"

#include "RenderInstance.h"
#include "GameInstance.h"


#include <fstream>  
#include <iostream> 
#include <vector>   
#include <string>   
#include <sstream>

#include "Animation.h"


CAnimationNameMap::CAnimationNameMap()
{
}

void CAnimationNameMap::Initalize()
{
	m_MAP_AnimationPedia[SELECT_GOKU]["TEST"] = 0;

	m_MAP_AnimationPedia[PLAY_GOKU]["GKS000_body"] = 0;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS001_body"] = 1;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS002_body"] = 2;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS010_body"] = 3;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS011_body"] = 4;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS013_body"] = 5;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS021_body"] = 6;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS022_body"] = 7;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS026_body"] = 8;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS030_body"] = 9;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS031_body"] = 10;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS032_body"] = 11;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS033_body"] = 12;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS034_body"] = 13;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS035_body"] = 14;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS036_body"] = 15;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS037_body"] = 16;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS038_body"] = 17;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS040_body"] = 18;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS041_body"] = 19;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS042_body"] = 20;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS050_body"] = 21;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS051_body"] = 22;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS052_body"] = 23;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS060_body"] = 24;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS061_body"] = 25;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS062_body"] = 26;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS063_body"] = 27;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS064_body"] = 28;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS070_body"] = 29;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS072_body"] = 30;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS073_body"] = 31;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS074_body"] = 32;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS075_body"] = 33;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS076_body"] = 34;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS077_body"] = 35;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS078_body"] = 36;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS079_body"] = 37;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS080_body"] = 38;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS081_body"] = 39;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS090_body"] = 40;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS100_body"] = 41;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS101_body"] = 42;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS200_body"] = 43;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS201_body"] = 44;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS202_body"] = 45;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS203_body"] = 46;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS204_body"] = 47;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS205_body"] = 48;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS230_body"] = 49;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS231_body"] = 50;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS232_body"] = 51;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS260_body"] = 52;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS261_body"] = 53;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS262_body"] = 54;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS263_body"] = 55;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS263_body.001"] = 56;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS265_body"] = 57;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS301_body"] = 58;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS303_body"] = 59;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS304_body"] = 60;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS306cs_body"] = 61;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS400_body"] = 62;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS401_body"] = 63;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS402_body"] = 64;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS403_body"] = 65;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS404_body"] = 66;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS405_body"] = 67;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS406_body"] = 68;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS436_body"] = 69;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS436cs"] = 70;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS440_body"] = 71;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS441_body"] = 72;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS442_body"] = 73;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS443_body"] = 74;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS600cs"] = 75;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS601cs_body"] = 76;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS602cs_body"] = 77;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS610cs"] = 78;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS620cs_body"] = 79;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS621cs_body"] = 80;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS630_body"] = 81;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS631_body"] = 82;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS700_body"] = 83;
	m_MAP_AnimationPedia[PLAY_GOKU]["ready"] = 84;
	m_MAP_AnimationPedia[PLAY_GOKU]["stand"] = 85;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS_FRN801cs_01_body"] = 86;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS_FRN801cs_02_body"] = 87;
	m_MAP_AnimationPedia[PLAY_GOKU]["GKS_FRN805cs_body"] = 88;




	//m_MAP_AnimationPedia[PLAY_BOU]["BUN000_body"] = 0;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN001_body"] = 1;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN010_body"] = 2;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN011_body"] = 3;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN013_body"] = 4;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN021_body"] = 5;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN022_body"] = 6;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN026_body"] = 7;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN030_body"] = 8;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN031_body"] = 9;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN032_body"] = 10;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN033_body"] = 11;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN034_body"] = 12;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN035_body"] = 13;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN036_body"] = 14;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN037_body"] = 15;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN038_body"] = 16;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN040_body"] = 17;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN041_body"] = 18;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN042_body"] = 19;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN050_body"] = 20;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN051_body"] = 21;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN052_body"] = 22;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN060_body"] = 23;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN061_body"] = 24;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN062_body"] = 25;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN063_body"] = 26;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN070_body"] = 27;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN072_body"] = 28;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN073_body"] = 29;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN074_body"] = 30;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN075_body"] = 31;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN076_body"] = 32;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN077_body"] = 33;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN078_body"] = 34;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN079_body"] = 35;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN080_body"] = 36;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN081_body"] = 37;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN090_body"] = 38;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN100_body"] = 39;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN101_body"] = 40;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN200_body"] = 41;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN201_body"] = 42;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN202_body"] = 43;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN203_body"] = 44;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN204_body"] = 45;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN205_body"] = 46;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN230_body"] = 47;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN231_body"] = 48;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN232_body"] = 49;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN233_body"] = 50;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN260_body"] = 51;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN261_body"] = 52;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN262_body"] = 53;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN263_body"] = 54;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN265_body"] = 55;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN301_body"] = 56;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN303_body"] = 57;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN304_body"] = 58;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN306cs_body"] = 59;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN310_body"] = 60;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN400_body"] = 61;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN401_body"] = 62;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN402_body"] = 63;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN403_body"] = 64;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN430_body"] = 65;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN431_body"] = 66;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN431cs_body"] = 67;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN600cs_body"] = 68;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN601cs_body"] = 69;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN602cs_body"] = 70;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN610cs_body"] = 71;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN620cs_body"] = 72;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN621cs_body"] = 73;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN630_body"] = 74;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN631_body"] = 75;
	//m_MAP_AnimationPedia[PLAY_BOU]["BUN700_body"] = 76;
	//m_MAP_AnimationPedia[PLAY_BOU]["ready"] = 77;
	//m_MAP_AnimationPedia[PLAY_BOU]["stand"] = 78;

	m_MAP_AnimationPedia[PLAY_FRN]["FRN000_body"] = 0;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN001_body"] = 1;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN010_body"] = 2;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN011_body"] = 3;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN013_body"] = 4;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN021_body"] = 5;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN022_body"] = 6;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN026_body"] = 7;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN030_body"] = 8;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN031_body"] = 9;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN032_body"] = 10;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN033_body"] = 11;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN034_body"] = 12;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN035_body"] = 13;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN036_body"] = 14;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN037_body"] = 15;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN038_body"] = 16;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN040_body"] = 17;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN041_body"] = 18;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN042_body"] = 19;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN050_body"] = 20;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN051_body"] = 21;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN052_body"] = 22;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN060_body"] = 23;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN061_body"] = 24;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN062_body"] = 25;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN063_body"] = 26;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN070_body"] = 27;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN072_body"] = 28;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN073_body"] = 29;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN074_body"] = 30;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN075_body"] = 31;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN076_body"] = 32;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN077_body"] = 33;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN078_body"] = 34;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN079_body"] = 35;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN080_body"] = 36;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN081_body"] = 37;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN090_body"] = 38;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN100_body"] = 39;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN101_body"] = 40;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN200_body"] = 41;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN201_body"] = 42;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN202_body"] = 43;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN203_body"] = 44;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN204_body"] = 45;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN205_body"] = 46;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN230_body"] = 47;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN231_body"] = 48;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN232_body"] = 49;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN233_body"] = 50;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN260_body"] = 51;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN261_body"] = 52;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN262_body"] = 53;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN265_body"] = 54;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN266_body"] = 55;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN267_body"] = 56;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN301_body"] = 57;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN303_body"] = 58;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN304_body"] = 59;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN306cs_body"] = 60;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN310_body"] = 61;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN400_body"] = 62;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN402_body"] = 63;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN403_body"] = 64;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN430_body"] = 65;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN431_body"] = 66;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN431_fullpower"] = 67;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN432_body"] = 68;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN433_body"] = 69;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN433cs_body"] = 70;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN600cs_body"] = 71;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN601cs_body"] = 72;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN602cs_body"] = 73;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN610cs_body"] = 74;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN611cs_body"] = 75;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN620cs_body"] = 76;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN621cs_body"] = 77;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN630_body"] = 78;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN631_body"] = 79;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN700_body"] = 80;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_face600cs_1_01_body"] = 81;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_face600cs_1_02_body"] = 82;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_face600cs_1_03_body"] = 83;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_face610cs_1_01_body"] = 84;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_face611cs_1_01_body"] = 85;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_GKS801cs_01_body"] = 86;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_GKS805cs_body"] = 87;
	m_MAP_AnimationPedia[PLAY_FRN]["FRN_GKS819cs_body"] = 88;



	m_MAP_AnimationPedia[PLAY_HIT]["HTN000_body"] = 0;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN001_body"] = 1;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN010_body"] = 2;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN011_body"] = 3;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN013_body"] = 4;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN021_body"] = 5;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN022_body"] = 6;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN026_body"] = 7;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN030_body"] = 8;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN031_body"] = 9;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN032_body"] = 10;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN033_body"] = 11;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN034_body"] = 12;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN035_body"] = 13;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN036_body"] = 14;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN037_body"] = 15;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN038_body"] = 16;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN040_body"] = 17;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN041_body"] = 18;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN042_body"] = 19;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN050_body"] = 20;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN051_body"] = 21;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN052_body"] = 22;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN060_body"] = 23;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN061_body"] = 24;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN062_body"] = 25;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN063_body"] = 26;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN070_body"] = 27;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN072_body"] = 28;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN073_body"] = 29;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN074_body"] = 30;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN075_body"] = 31;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN076_body"] = 32;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN077_body"] = 33;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN078_body"] = 34;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN079_body"] = 35;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN080_body"] = 36;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN081_body"] = 37;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN090_body"] = 38;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN100_body"] = 39;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN101_body"] = 40;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN200_body"] = 41;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN201_body"] = 42;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN202_body"] = 43;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN203_body"] = 44;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN204_body"] = 45;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN205_body"] = 46;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN230_body"] = 47;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN231_body"] = 48;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN232_body"] = 49;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN260_body"] = 50;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN261_body"] = 51;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN262_body"] = 52;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN263_body"] = 53;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN265_body"] = 54;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN301_body"] = 55;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN303_body"] = 56;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN304_body"] = 57;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN306cs_body"] = 58;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN310_body"] = 59;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN400_body"] = 60;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN401_body"] = 61;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN402_body"] = 62;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN403_body"] = 63;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN405_body"] = 64;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN407_body"] = 65;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN408_body"] = 66;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN430_body"] = 67;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN431_body"] = 68;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN432_body"] = 69;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN432cs_body"] = 70;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN600cs_body"] = 71;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN601cs_body"] = 72;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN602cs_body"] = 73;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN610cs_body"] = 74;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN620cs_body"] = 75;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN621cs_body"] = 76;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN630_body"] = 77;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN631_body"] = 78;
	m_MAP_AnimationPedia[PLAY_HIT]["HTN700_body"] = 79;



	
																   
	m_MAP_AnimationPedia[PLAY_21]["ready"] = 0;
	m_MAP_AnimationPedia[PLAY_21]["stand"] = 1;
	m_MAP_AnimationPedia[PLAY_21]["TON000_body"] = 2;
	m_MAP_AnimationPedia[PLAY_21]["TON001_body"] = 3;
	m_MAP_AnimationPedia[PLAY_21]["TON010_body"] = 4;
	m_MAP_AnimationPedia[PLAY_21]["TON011_body"] = 5;
	m_MAP_AnimationPedia[PLAY_21]["TON013_body"] = 6;
	m_MAP_AnimationPedia[PLAY_21]["TON021_body"] = 7;
	m_MAP_AnimationPedia[PLAY_21]["TON022_body"] = 8;
	m_MAP_AnimationPedia[PLAY_21]["TON026_body"] = 9;
	m_MAP_AnimationPedia[PLAY_21]["TON030_body"] = 10;
	m_MAP_AnimationPedia[PLAY_21]["TON031_body"] = 11;
	m_MAP_AnimationPedia[PLAY_21]["TON032_body"] = 12;
	m_MAP_AnimationPedia[PLAY_21]["TON033_body"] = 13;
	m_MAP_AnimationPedia[PLAY_21]["TON034_body"] = 14;
	m_MAP_AnimationPedia[PLAY_21]["TON035_body"] = 15;
	m_MAP_AnimationPedia[PLAY_21]["TON036_body"] = 16;
	m_MAP_AnimationPedia[PLAY_21]["TON037_body"] = 17;
	m_MAP_AnimationPedia[PLAY_21]["TON038_body"] = 18;
	m_MAP_AnimationPedia[PLAY_21]["TON040_body"] = 19;
	m_MAP_AnimationPedia[PLAY_21]["TON041_body"] = 20;
	m_MAP_AnimationPedia[PLAY_21]["TON042_body"] = 21;
	m_MAP_AnimationPedia[PLAY_21]["TON050_body"] = 22;
	m_MAP_AnimationPedia[PLAY_21]["TON051_body"] = 23;
	m_MAP_AnimationPedia[PLAY_21]["TON052_body"] = 24;
	m_MAP_AnimationPedia[PLAY_21]["TON060_body"] = 25;
	m_MAP_AnimationPedia[PLAY_21]["TON061_body"] = 26;
	m_MAP_AnimationPedia[PLAY_21]["TON062_body"] = 27;
	m_MAP_AnimationPedia[PLAY_21]["TON063_body"] = 28;
	m_MAP_AnimationPedia[PLAY_21]["TON070_body"] = 29;
	m_MAP_AnimationPedia[PLAY_21]["TON072_body"] = 30;
	m_MAP_AnimationPedia[PLAY_21]["TON073_body"] = 31;
	m_MAP_AnimationPedia[PLAY_21]["TON074_body"] = 32;
	m_MAP_AnimationPedia[PLAY_21]["TON075_body"] = 33;
	m_MAP_AnimationPedia[PLAY_21]["TON076_body"] = 34;
	m_MAP_AnimationPedia[PLAY_21]["TON077_body"] = 35;
	m_MAP_AnimationPedia[PLAY_21]["TON078_body"] = 36;
	m_MAP_AnimationPedia[PLAY_21]["TON079_body"] = 37;
	m_MAP_AnimationPedia[PLAY_21]["TON080_body"] = 38;
	m_MAP_AnimationPedia[PLAY_21]["TON081_body"] = 39;
	m_MAP_AnimationPedia[PLAY_21]["TON090_body"] = 40;
	m_MAP_AnimationPedia[PLAY_21]["TON100_body"] = 41;
	m_MAP_AnimationPedia[PLAY_21]["TON101_body"] = 42;
	m_MAP_AnimationPedia[PLAY_21]["TON200_body"] = 43;
	m_MAP_AnimationPedia[PLAY_21]["TON201_body"] = 44;
	m_MAP_AnimationPedia[PLAY_21]["TON202_body"] = 45;
	m_MAP_AnimationPedia[PLAY_21]["TON203_body"] = 46;
	m_MAP_AnimationPedia[PLAY_21]["TON204_body"] = 47;
	m_MAP_AnimationPedia[PLAY_21]["TON205_body"] = 48;
	m_MAP_AnimationPedia[PLAY_21]["TON230_body"] = 49;
	m_MAP_AnimationPedia[PLAY_21]["TON231_body"] = 50;
	m_MAP_AnimationPedia[PLAY_21]["TON232_body"] = 51;
	m_MAP_AnimationPedia[PLAY_21]["TON233_body"] = 52;
	m_MAP_AnimationPedia[PLAY_21]["TON260_body"] = 53;
	m_MAP_AnimationPedia[PLAY_21]["TON261_body"] = 54;
	m_MAP_AnimationPedia[PLAY_21]["TON262_body"] = 55;
	m_MAP_AnimationPedia[PLAY_21]["TON301_body"] = 56;
	m_MAP_AnimationPedia[PLAY_21]["TON303_body"] = 57;
	m_MAP_AnimationPedia[PLAY_21]["TON304_body"] = 58;
	m_MAP_AnimationPedia[PLAY_21]["TON306cs_body"] = 59;
	m_MAP_AnimationPedia[PLAY_21]["TON310_body"] = 60;
	m_MAP_AnimationPedia[PLAY_21]["TON400_body"] = 61;
	m_MAP_AnimationPedia[PLAY_21]["TON401_body"] = 62;
	m_MAP_AnimationPedia[PLAY_21]["TON402_body"] = 63;
	m_MAP_AnimationPedia[PLAY_21]["TON403_body"] = 64;
	m_MAP_AnimationPedia[PLAY_21]["TON404_body"] = 65;
	m_MAP_AnimationPedia[PLAY_21]["TON405_body"] = 66;
	m_MAP_AnimationPedia[PLAY_21]["TON407_body"] = 67;
	m_MAP_AnimationPedia[PLAY_21]["TON409_body"] = 68;
	m_MAP_AnimationPedia[PLAY_21]["TON410_body"] = 69;
	m_MAP_AnimationPedia[PLAY_21]["TON411_body"] = 70;
	m_MAP_AnimationPedia[PLAY_21]["TON430_body"] = 71;
	m_MAP_AnimationPedia[PLAY_21]["TON431_body"] = 72;
	m_MAP_AnimationPedia[PLAY_21]["TON431cs_body"] = 73;
	m_MAP_AnimationPedia[PLAY_21]["TON432_body"] = 74;
	m_MAP_AnimationPedia[PLAY_21]["TON433_body"] = 75;
	m_MAP_AnimationPedia[PLAY_21]["TON433cs_body"] = 76;
	m_MAP_AnimationPedia[PLAY_21]["TON600cs_body"] = 77;
	m_MAP_AnimationPedia[PLAY_21]["TON601cs_body"] = 78;
	m_MAP_AnimationPedia[PLAY_21]["TON602cs_body"] = 79;
	m_MAP_AnimationPedia[PLAY_21]["TON610cs_body"] = 80;
	m_MAP_AnimationPedia[PLAY_21]["TON620cs_body"] = 81;
	m_MAP_AnimationPedia[PLAY_21]["TON621cs_body"] = 82;
	m_MAP_AnimationPedia[PLAY_21]["TON630_body"] = 83;
	m_MAP_AnimationPedia[PLAY_21]["TON631_body"] = 84;
	m_MAP_AnimationPedia[PLAY_21]["TON700_body"] = 85;




	m_MAP_AnimationPedia[OPENING_GOKU]["GKS600cs"] = 0;


	m_MAP_AnimationPedia[SKILL_GOKU]["GKS436cs"] = 0;
	m_MAP_AnimationPedia[SKILL_GOKU]["GKS440_body"] = 1;
	m_MAP_AnimationPedia[SKILL_GOKU]["GKS441_body"] = 2;
	m_MAP_AnimationPedia[SKILL_GOKU]["GKS442_body"] = 3;
	m_MAP_AnimationPedia[SKILL_GOKU]["GKS443_body"] = 4;


	m_MAP_AnimationPedia[SELECT_GOKU]["ready"] = 0;
	m_MAP_AnimationPedia[SELECT_GOKU]["stand"] = 1;

	m_MAP_AnimationPedia[SELECT_FRN]["ready"] = 0;
	m_MAP_AnimationPedia[SELECT_FRN]["stand"] = 1;

	m_MAP_AnimationPedia[SELECT_21]["ready"] = 0;
	m_MAP_AnimationPedia[SELECT_21]["stand"] = 1;

	m_MAP_AnimationPedia[SELECT_HIT]["ready"] = 0;
	m_MAP_AnimationPedia[SELECT_HIT]["stand"] = 1;




}

_int CAnimationNameMap::Get_AnimationIndex(_int Characterenum, string strAnimationName)
{
	//_int iDebug = m_MAP_AnimationPedia[Characterenum][strAnimationName];

	
	return m_MAP_AnimationPedia[Characterenum][strAnimationName];
}

