/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : jpeg_hdec_rwreg.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : the write and read about the register realize in this file
                  CNcomment: ��д�Ĵ��������ڸ��ļ���ʵ�� CNend\n
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/

#ifndef __JPEG_HDEC_RWREG_H__
#define __JPEG_HDEC_RWREG_H__


/*********************************add include here******************************/
#include  "hi_jpeg_config.h"
#include  "hi_type.h"


/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" 
{
#endif
#endif /* __cplusplus */


    /***************************** Macro Definition ******************************/
    /** \addtogroup 	 JPEG RWREG MACRO */
    /** @{ */  /** <!-- ��JPEG RWREG MACRO�� */


	 /** @} */	/*! <!-- Macro Definition end */


	 /*************************** Enum Definition ****************************/

	/** \addtogroup      JPEG RWREG ENUM */
    /** @{ */  /** <!-- ��JPEG RWREG ENUM�� */


	
    /** @} */  /*! <!-- enum Definition end */

	/*************************** Structure Definition ****************************/

	/** \addtogroup      JPEG RWREG STRUCTURE */
    /** @{ */  /** <!-- ��JPEG RWREG STRUCTURE�� */

	/** @} */  /*! <!-- Structure Definition end */

	
    /********************** Global Variable declaration **************************/
 
    /******************************* API declaration *****************************/

	/** \addtogroup      JPEG RWREG API */
    /** @{ */  /** <!-- ��JPEG RWREG API�� */
	
	 
	/*****************************************************************************
	* func			: JPEG_HDEC_WriteReg
	* description	: writd data to register
	                  CNcomment:  д�Ĵ���	                                CNend\n
	* param[in]     : pJpegRegVirAddr  CNcomment:  �Ĵ���ӳ�������������ַ	CNend\n
	* param[in]     : s32PhyOff        CNcomment:  ƫ�Ƶ�ַ             	CNend\n
	* param[in]     : s32Val           CNcomment:  Ҫд�ļĴ�����ֵ     	CNend\n
	* retval	    : NA
	* others:	    : NA
	*****************************************************************************/
	HI_VOID JPEG_HDEC_WriteReg(volatile HI_CHAR *pJpegRegVirAddr, const HI_S32 s32PhyOff, const HI_S32 s32Val);

	/*****************************************************************************
	* func			: JPEG_HDEC_ReadReg
	* description	: read data from register
	                  CNcomment:  ���Ĵ�����ֵ	                                CNend\n
	* param[out]    : pJpegRegVirAddr   CNcomment:  �Ĵ���ӳ�������������ַ	CNend\n
	* param[in]     : s32PhyOff         CNcomment:  ƫ�Ƶ�ַ	                CNend\n
	* retval	    : HI_SUCCESS        CNcomment:  �ɹ�	                    CNend\n
	* retval	    : HI_FAILURE        CNcomment:  ʧ��	                    CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_ReadReg(const volatile HI_CHAR *pJpegRegVirAddr, const HI_S32 s32PhyOff);

	/*****************************************************************************
	* func			: JPEG_HDEC_CpyData2Reg
	* description	: copy the mem data to register
	                  CNcomment:  ��buf�е�ֵд���Ĵ�����	                CNend\n
	* param[in]     : pJpegRegVirAddr  CNcomment:  �Ĵ���ӳ�������������ַ	CNend\n
	* param[in]     : pInMem           CNcomment:  Ҫ�����buffer��ַ	    CNend\n
	* param[in]     : s32PhyOff        CNcomment:  ƫ�Ƶ�ַ	                CNend\n
	* param[in]     : u32Bytes         CNcomment:  Ҫд���ֽ���          	CNend\n
	* retval	    : NA
	* others:	    : NA
	*****************************************************************************/
	HI_VOID JPEG_HDEC_CpyData2Reg(volatile HI_CHAR *pJpegRegVirAddr,const HI_VOID *pInMem,const HI_S32 s32PhyOff,const HI_U32 u32Bytes);


	/*****************************************************************************
	* func			: JPEG_HDEC_CpyData2Buf
	* description	: copy the register data to mem
	                  CNcomment:  ���Ĵ����е�ֵд���û�buffer��	            CNend\n
	* param[in]     : pJpegRegVirAddr     CNcomment:  �Ĵ���ӳ�������������ַ	CNend\n
	* param[in]     : s32PhyOff           CNcomment:  ƫ�Ƶ�ַ	          CNend\n
	* param[in]     : u32Bytes            CNcomment:  Ҫ������ֽ���	  CNend\n
	* param[out]    : pOutMem             CNcomment:  ���buffer��ַ	  CNend\n
	* retval	    : HI_SUCCESS          CNcomment:  �ɹ�	              CNend\n
	* retval	    : HI_FAILURE          CNcomment:  ʧ��	              CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_CpyData2Buf(const volatile HI_CHAR *pJpegRegVirAddr,const HI_S32 s32PhyOff,const HI_U32 u32Bytes,HI_VOID *pOutMem);


	/** @} */  /*! <!-- API declaration end */
	
    /****************************************************************************/



#ifdef __cplusplus
    
#if __cplusplus
   
}
#endif
#endif /* __cplusplus */

#endif /* __JPEG_HDEC_RWREG_H__*/
