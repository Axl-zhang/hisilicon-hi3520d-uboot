/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : jpeg_hdec_rwreg.c
Version		    : Initial Draft
Author		    : y00181162
Created		    : 2014/06/20
Description	    : write data to register and read data from register
                  CNcomment: ��д�Ĵ��� CNend\n
Function List 	:

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162		    Created file      	
******************************************************************************/

/*********************************add include here******************************/
#include "jpeg_hdec_rwreg.h"
#include "hi_drv_jpeg_reg.h"

/***************************** Macro Definition ******************************/


/******************** to see which include file we want to use***************/



/*************************** Structure Definition ****************************/



/********************** Global Variable declaration **************************/

/******************************* API forward declarations *******************/

/******************************* API realization *****************************/


/*****************************************************************************
* func			: JPEG_HDEC_WriteReg
* description	: writd data to register
				  CNcomment:  д�Ĵ���									CNend\n
* param[in] 	: pJpegRegVirAddr  CNcomment:  �Ĵ���ӳ�������������ַ CNend\n
* param[in] 	: s32PhyOff 	   CNcomment:  ƫ�Ƶ�ַ 				CNend\n
* param[in] 	: s32Val		   CNcomment:  Ҫд�ļĴ�����ֵ 		CNend\n
* retval		: NA
* others:		: NA
*****************************************************************************/
HI_VOID JPEG_HDEC_WriteReg(volatile HI_CHAR *pJpegRegVirAddr, HI_S32 s32PhyOff, HI_S32 s32Val)
{

		volatile HI_S32  *ps32Addr = NULL;

		/**
		** the phycial offset address can not larger than register length
		** CNcomment: ƫ�Ƶ�ַ���ܴ��ڼĴ������� CNend\n
		**/
		if(s32PhyOff < JPGD_REG_LENGTH)
		{
		    ps32Addr  = (volatile HI_S32*)(pJpegRegVirAddr + s32PhyOff); /*lint !e826 ignore by y00181162, because this is needed */  
            *ps32Addr = s32Val;
		}
	  
}

/*****************************************************************************
* func			: JPEG_HDEC_ReadReg
* description	: read data from register
				  CNcomment:  ���Ĵ�����ֵ									CNend\n
* param[in] 	: pJpegRegVirAddr	CNcomment:	�Ĵ���ӳ�������������ַ	CNend\n
* param[in] 	: s32PhyOff 		CNcomment:	ƫ�Ƶ�ַ					CNend\n
* retval		: HI_SUCCESS		CNcomment:	�ɹ�						CNend\n
* retval		: HI_FAILURE		CNcomment:	ʧ��						CNend\n
* others:		: NA
*****************************************************************************/
HI_S32 JPEG_HDEC_ReadReg(const volatile HI_CHAR *pJpegRegVirAddr, HI_S32 s32PhyOff)
{

		return s32PhyOff < JPGD_REG_LENGTH ?  *(volatile int *)(pJpegRegVirAddr + s32PhyOff) : HI_FAILURE; /*lint !e826 ignore by y00181162, because this is needed */  

}

/*****************************************************************************
* func			: JPEG_HDEC_CpyData2Reg
* description	: copy the mem data to register
				  CNcomment:  ��buf�е�ֵд���Ĵ�����					CNend\n
* param[in] 	: pJpegRegVirAddr  CNcomment:  �Ĵ���ӳ�������������ַ CNend\n
* param[in] 	: pInMem		   CNcomment:  Ҫ�����buffer��ַ		CNend\n
* param[in] 	: s32PhyOff 	   CNcomment:  ƫ�Ƶ�ַ 				CNend\n
* param[in] 	: u32Bytes		   CNcomment:  Ҫд���ֽ��� 			CNend\n
* retval		: NA
* others:		: NA
*****************************************************************************/
HI_VOID JPEG_HDEC_CpyData2Reg(volatile HI_CHAR *pJpegRegVirAddr,const HI_VOID *pInMem,HI_S32 s32PhyOff,HI_U32 u32Bytes)
{

      
		HI_U32 u32Cnt = 0;

		for(u32Cnt = 0; u32Cnt < u32Bytes; u32Cnt += 4)
		{
			*(volatile int *)(pJpegRegVirAddr + s32PhyOff + u32Cnt)=*(int *)((char*)pInMem + u32Cnt);/*lint !e826 ignore by y00181162, because this is needed */  
		}

}

/*****************************************************************************
* func			: JPEG_HDEC_CpyData2Buf
* description	: copy the register data to mem
				  CNcomment:  ���Ĵ����е�ֵд���û�buffer��				CNend\n
* param[in] 	: pJpegRegVirAddr	  CNcomment:  �Ĵ���ӳ�������������ַ	CNend\n
* param[in] 	: s32PhyOff 		  CNcomment:  ƫ�Ƶ�ַ			  CNend\n
* param[in] 	: u32Bytes			  CNcomment:  Ҫ������ֽ���	  CNend\n
* param[out]	: pOutMem			  CNcomment:  ���buffer��ַ	  CNend\n
* retval		: HI_SUCCESS		  CNcomment:  �ɹ�				  CNend\n
* retval		: HI_FAILURE		  CNcomment:  ʧ��				  CNend\n
* others:		: NA
*****************************************************************************/
HI_S32 JPEG_HDEC_CpyData2Buf(const volatile HI_CHAR *pJpegRegVirAddr,const HI_S32 s32PhyOff,const HI_U32 u32Bytes,HI_VOID *pOutMem)
{

		HI_U32 u32Cnt = 0;

		for(u32Cnt = 0; u32Cnt < u32Bytes; u32Cnt += 4)
		{ 
			*(volatile int *)((char*)pOutMem + u32Cnt)=*(int *)(pJpegRegVirAddr + s32PhyOff + u32Cnt);/*lint !e826 ignore by y00181162, because this is needed */  
		}
		return HI_SUCCESS;
}
