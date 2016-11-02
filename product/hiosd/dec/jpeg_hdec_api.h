/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : jpeg_hdec_api.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : this include file is cover all api of jpeg dec
                  CNcomment: �ڲ��Ľ���ʵ�ֶ�����������ļ��� CNend\n
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/
#ifndef __JPEG_HDEC_API_H__
#define __JPEG_HDEC_API_H__


/*********************************add include here******************************/
#include  "jpeglib.h"

#include  "hi_type.h"
#include  "hi_jpeg_config.h"
#include  "hi_jpeg_hal_api.h"
#include  "jpeg_hdec_rwreg.h"
#include  "hi_jpeg_api.h"

#ifdef CONFIG_JPEG_FPGA_TEST_SAVE_SCEN_ENABLE
#include "hi_drv_jpeg_reg.h"
#endif

/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" 
{
#endif
#endif /* __cplusplus */


    /***************************** Macro Definition ******************************/
    /** \addtogroup 	 JPEG HDEC MACRO */
    /** @{ */  /** <!-- ��JPEG HDEC MACRO�� */

	 /** jpeg device */
	 /** CNcomment:jpeg �豸 */
	#define JPG_DEV 			              "/dev/jpeg"
	#define MMZ_DEV                          "/dev/mmz_userdev"
    #define CSC_DEV                          "/dev/hi_tde"
	
	 /** @} */	/*! <!-- Macro Definition end */


	 /*************************** Enum Definition ****************************/
    /****************************************************************************/
	/*							   jpeg enum    					            */
	/****************************************************************************/
	
	/** \addtogroup      JPEG HDEC ENUM */
    /** @{ */  /** <!-- ��JPEG HDEC ENUM�� */

	/** enum of the dec scale */
    /** CNcomment:�������ű��� */
	typedef enum tagJPEG_HDEC_SCALE_E
	{
	    JPEG_SCALEDOWN_1 = 0,      /**< scanle down 1 *//**<CNcomment:������   */
	    JPEG_SCALEDOWN_2 = 1,      /**< scanle down 2 *//**<CNcomment:��С2��  */
	    JPEG_SCALEDOWN_4 = 2,      /**< scanle down 4 *//**<CNcomment:��С4��  */
	    JPEG_SCALEDOWN_8 = 3,      /**< scanle down 8 *//**<CNcomment:��С8��  */
	    JPEG_SCALEDOWN_BUTT
	}JPEG_HDEC_SCALE_E;

	
    /** @} */  /*! <!-- enum Definition end */

	/*************************** Structure Definition ****************************/
    /****************************************************************************/
	/*							   jpeg api structure    					    */
	/****************************************************************************/
	
	/** \addtogroup      JPEG HDEC STRUCTURE */
    /** @{ */  /** <!-- ��JPEG HDEC STRUCTURE�� */


     /** Structure of stream message��this memory from user alloc */
	 /** CNcomment:����buffer��Ϣ���ڴ���Դ���û������
	               Ҫ�������ڴ������ģ��е��¼����ŵ�
	               �ŵ�һ:����Ҫcpy������ʡ�ǳ����ʱ��
	               �ŵ��:����Ҫ�ٷ�������buffer������ʡ�ڴ�
	               �ŵ���:ֱ��ȫ���͸�Ӳ�����룬����Ҫ��������������ʡ�˽����жϵ�ʱ�� */
	
     /** deal with the data in hard decode */
	 /** CNcomment:����Ӳ������֮�����������
	               ������ֱ�Ӷ�������buffer�У�������Ҫ���п��� */
	 typedef struct tagJPEG_HDEC_DATA_S
	 {
	     HI_CHAR* pDataPhyBuf;          /**< use mem data,save these        *//**<CNcomment:������Դ���ڴ棬���buffer����������Щ����         */
		 HI_CHAR* pDataVirBuf;          /**< use mem data,save these        *//**<CNcomment:������Դ���ڴ棬���buffer����������Щ����         */
         HI_CHAR* pSaveStreamPhyBuf;    /**< save stream physics buffer     *//**<CNcomment:Ӳ������Ҫ��ŵ����������ڴ�                       */
		 HI_CHAR* pSaveStreamVirBuf;    /**< save stream virtual buffer     *//**<CNcomment:Ӳ������Ҫ��ŵ����������ڴ�                       */
		 /** add para that deal with the stream buffer return **/
		 HI_CHAR* pStartBufPhy;		  /**< the buffer start phy address  *//**<CNcomment: ����buffer����ʼ�����ַ                          */
		 HI_CHAR* pDataVirBufReturn;    /**< the stream buffer return      *//**<CNcomment: �������Ƶ���ʼ�����ַ                            */
		 HI_S32   s32BufLen;             /**< the buffer len                *//**<CNcomment: ����buffer��С                                    */
		 HI_S32   s32StreamReturnLen;   /**< the stream buffer return len  *//**<CNcomment: ���������Ƶ�������С                              */
		 HI_BOOL  bNeedStreamReturn;    /**< if has stream return          *//**<CNcomment: �Ƿ�����������                                     */
		 HI_BOOL  bUserPhyMem;           /**< use user physics memory        *//**<CNcomment:�û����������ڴ�                                   */
		 HI_BOOL  bUseFileData;          /**< use user file stream           *//**<CNcomment:�û��ļ�����                                		 */
		 HI_BOOL  bUseInsideData;        /**< use inside deal withe the data *//**<CNcomment:ʹ���ڲ�����������ʽ                               */
		 HI_BOOL  bReadToDataBuf;        /**< if read data to stream buffer  *//**<CNcomment:�Ƿ�Ҫֱ�Ӷ���������buffer�У����������ļ���˵�� */
		 HI_U32   u32ReadDataSize;       /**< read data size one time        *//**<CNcomment:ÿ�ζ�ȡ��������С,���û������ڴ�                  */
		 HI_U64   u64DataSize;           /**< save user stream size           *//**<CNcomment:�û��ͽ��е���֡������С                           */
		 HI_U32   u32ConsumeDataSize;   /**< the consume stream size before hard decode  *//**<CNcomment:Ӳ������֮ǰ�����˶������������������ڴ�������˵�� */
	 }JPEG_HDEC_DATA_S;


	/** when hard decode failure, callback to soft decode need message */
    /** CNcomment:Ӳ�����벻�ɹ����˵����������Ҫ����Ϣ */
	typedef struct tagJPEG_HDEC_TO_SDEC_S
	{
		 HI_CHAR* pLeaveBuf;     /**< remain stream buffer        *//**<CNcomment:ʣ������           */
		 HI_U32   u32FilePos;    /**< the current file position   *//**<CNcomment:��ǰ�������ļ�λ�� */
		 HI_U32   u32StreamPos;  /**< the current stream position *//**<CNcomment:��ǰ������λ��     */
		 HI_U32   u32LeaveByte;  /**< remain stream size          *//**<CNcomment:ʣ���������       */
	}JPEG_HDEC_TO_SDEC_S;


	/** the jpeg sof information��the hard decode is only use dec para */
    /** CNcomment:jpeg��С�����Ϣ��Ӳ������ֻ�н���ֱ��ʵĸ��� */
	typedef struct tagJPEG_HDEC_SOFINFO_S
	{
	     HI_BOOL bCalcSize;        /**< the jpeg size has  calculated *//**<CNcomment:jpeg��С�Ѿ��������ˣ�����Ҫ���¼�����  */
		 HI_U32  u32YWidth;        /**< the lu width         *//**<CNcomment:���ȿ���        */
		 HI_U32  u32YHeight;       /**< the lu height        *//**<CNcomment:���ȸ߶�          */
		 HI_U32  u32YSize;         /**< the lu size          *//**<CNcomment:���ȴ�С          */
		 HI_U32  u32CWidth;        /**< the ch width         *//**<CNcomment:ɫ�ȿ��          */
		 HI_U32  u32CHeight;       /**< the ch height        *//**<CNcomment:ɫ�ȸ߶�          */
		 HI_U32  u32CSize;         /**< the ch size          *//**<CNcomment:ɫ�ȴ�С          */
		 HI_U32  u32YStride;       /**< the lu stride        *//**<CNcomment:�����м��        */
		 HI_U32  u32CbCrStride;    /**< the ch stride        *//**<CNcomment:ɫ���м��        */
		 HI_U32  u32DisplayW;      /**< the display width    *//**<CNcomment:��ʾ���          */
		 HI_U32  u32DisplayH;      /**< the display height   *//**<CNcomment:��ʾ�߶�          */
		 HI_U32  u32DisplayStride;/**< the display stride   *//**<CNcomment:��ʾ�м��        */
		 HI_U32  u32DecW;          /**< the dec width        *//**<CNcomment:������          */
		 HI_U32  u32DecH;          /**< the dec height       *//**<CNcomment:����߶�          */
		 HI_U32  u32DecStride;    /**< the dec stride       *//**<CNcomment:���� �м��        */
		 HI_U32  u32InWandH;       /**< set to register size *//**<CNcomment:���Ӳ���ķֱ���  */
		 #ifdef CONFIG_JPEG_HARDDEC2ARGB
		 HI_U32  u32McuWidth;      /**< the mcu width        */
		 HI_U32  u32MINSize;       /**< the min size         */
		 HI_U32  u32MIN1Size;      /**< the min1 size        */
		 HI_U32  u32RGBSizeReg;    /**< set to register size *//**<CNcomment:���Ӳ���Ĵ�����ֵ  */
		 #endif	
	}JPEG_HDEC_SOFINFO_S;

	/** the jpeg hard decode output middle buffer and csc output middle buffer */
	/** CNcomment:Ӳ����������м�buffer����ɫ�ռ�ת������м�buffer */
	typedef struct tagJPEG_MIDDLE_SURFACE_S
	{
		 HI_CHAR* pMiddlePhy[MAX_PIXEL_COMPONENT_NUM]; /**< jpeg hard dec output physics buffer *//**<CNcomment:jpeg Ӳ��������������ַ  */
		 HI_CHAR* pMiddleVir[MAX_PIXEL_COMPONENT_NUM]; /**< jpeg hard dec output vitual  buffer *//**<CNcomment:jpeg Ӳ��������������ַ  */
		 HI_CHAR* pOutPhy;                             /**<  csc output physics buffer          *//**<CNcomment:��ɫ�ռ�ת����������ַ   */
		 HI_CHAR* pOutVir;                             /**<  csc output virtual buffer          *//**<CNcomment:��ɫ�ռ�ת����������ַ   */
	}JPEG_MIDDLE_SURFACE_S;
	
	/** the private structure of hard decode */
	/** CNcomment:Ӳ���������ӵ�˽�нṹ�� */
	typedef struct tagJPEG_HDEC_HANDLE_S
	{

		  volatile HI_CHAR *pJpegRegVirAddr;  /**< mmap the jpeg virtual address           *//**<CNcomment:jpegӳ�������������ַ         */
          HI_BOOL  bHdecEnd;                  /**< hard decode success                     *//**<CNcomment:Ӳ������ɹ�                   */
		  HI_BOOL  bCSCEnd;                   /**< color space convert success             *//**<CNcomment:��ɫ�ռ�ת���ɹ�               */
		  HI_BOOL  bOutYCbCrSP;               /**< if output yuvsp                         *//**<CNcomment:�Ƿ�������yuv semi_planer   */
          HI_BOOL  bFirstDec;                 /**< if use external stream, call dec agine  *//**<CNcomment: ���ʹ���ⲿ������Ӳ������ʧ�ܵڶ��ε��� */
          HI_U32   u32StrideAlign;           /**< how many bytes the output stride align   *//**<CNcomment: ���stride�����ֽڶ���       */
		  #ifdef CONFIG_JPEG_HARDDEC2ARGB
		  HI_BOOL  bDecARGB;                  /**< the jpeg support dec to argb            *//**<CNcomment:jpegӲ��֧�ֽ������ARGB       */
          HI_CHAR* pMinPhyBuf;                /**< dec to argb need row buffer             *//**<CNcomment:�������ARGB��Ҫ����buffer     */
		  #endif
		  #ifdef CONFIG_JPEG_FPGA_TEST_SAVE_SCEN_ENABLE
          HI_BOOL bSaveScen;                  /**< if you want to save scen, set it        *//**<CNcomment:����뱣������ֳ������������� */
		  HI_S32  s32RegData[JPGD_REG_LENGTH];
		  FILE*   pScenFile;
		  HI_CHAR* pScenFileName;
		  #endif
		  #ifdef CONFIG_JPEG_OUTPUT_YUV420SP
		  HI_BOOL  bOutYUV420SP;              /**< all dec output yuv420sp                *//**<CNcomment:ͳһ���yuv420sp��ʽ        */ 
		  #endif
		  #ifdef CONFIG_JPEG_OUTPUT_LUPIXSUM
		  HI_BOOL  bLuPixSum;                 /**< count the lu pixle sum value            *//**<CNcomment:ͳ�����Ⱥ���Ϣ              */
          HI_U64   u64LuPixValue;             /**< the lu pixle value                      *//**<CNcomment:����ֵ��                    */
		  #endif
		  #ifdef CONFIG_JPEG_GETDECTIME
		  HI_U32   u32DecTime;                /**< calc the decode time                     *//**<CNcomment:�������ʱ��                */
		  HI_U32   u32CurTime;                /**< calc the current time                    *//**<CNcomment:��ȡ��ǰ��ʱ��              */
		  #endif
		  HI_S32   s32ClientData;             /**< the client pointer of private structure  *//**<CNcomment:˽�нṹ��ָ��              */
		  HI_S32   s32JpegDev;				   /**< the jpeg device						     *//**<CNcomment:jpeg�豸					 */
		  HI_S32   s32CscDev;                 /**< the csc device                           *//**<CNcomment:csc�豸                     */
		  HI_U32   u32ScalRation;             /**< the dec scale                            *//**<CNcomment:���ű���                    */
		  HI_U32   u32Inflexion;              /**< the soft and hard dec flexiont           *//**<CNcomment:����Ӳ������Ĺյ��С    */
          HI_U32   u32Alpha;                  /**< set alpha value of out ARGB8888 ABGR8888  *//**<CNcomment:����alpha��ֵ              */
		  HI_BOOL  bReleaseRes;               /**< the resouce has release                  *//**<CNcomment:��Դ�Ѿ��ͷŵ���            */
		  HI_BOOL  bInHardDec;                /**< hard dec support and dev open success,now into hard decode */
		  #ifdef CONFIG_JPEG_USE_PRIVATE_MMZ
		  HI_S32   s32MMZDev; 				   /**< the mmz device 						     *//**<CNcomment:MMZ�豸          */
  		  HI_VOID *pSaveStreamMemHandle;
		  HI_VOID *pMiddleMemHandle;
		  HI_VOID *pMinMemHandle;
		  HI_VOID *pOutMemHandle;
          #endif
		  HI_U32 u32ResByteConsum;           /**< consume byte after suspend              *//**<CNcomment:�������ĵ�������  */
		  HI_U32 u32CurrentOffset;           /**< the stream consume current offset       *//**<CNcomment:�����ĵ�ǰλ��    */
		  HI_U32 u32NeedMemSize;			   /**< the need memsize                       *//**<CNcomment:��Ҫϵͳ�ڴ��С  */
		  HI_U32 u32LeaveMemSize;			   /**< the leave memsize                      *//**<CNcomment:ʣ���ڴ��С      */
		  HI_U8 u8Fac[MAX_PIXEL_COMPONENT_NUM][2]; /**< save the sample factor            *//**<CNcomment:����������Ӵ�С   */
		  #ifdef CONFIG_JPEG_PROC_ENABLE
		  HI_JPEG_DEC_STATE_E eDecState;       /**< the decode state                        *//**<CNcomment:����״̬          */
          #endif
		  JPEG_HDEC_SOFINFO_S            stJpegSofInfo;    /**< the jpeg sof information               *//**<CNcomment:jpegͼƬ��С��Ϣ           */
          JPEG_HDEC_DATA_S               stHDecDataBuf;    /**< the hard decode data buffer message    *//**<CNcomment:Ӳ��������Ҫ��������Ϣ     */
		  HI_JPEG_FMT_E	                 enImageFmt;	    /**< jpeg picture type                      *//**<CNcomment:jpegͼƬ��ʽ               */
		  JPEG_HDEC_TO_SDEC_S            stJpegHtoSInfo;   /**< hard dec failure callback to soft dec  *//**<CNcomment:Ӳ�ⲻ�ɹ����˵����Ĺ��� */
          HI_JPEG_DEC_COEF_S             stDecCoef;        /**< hard dec out argb set coef             *//**<CNcomment:�������ARGBʱ�����õĲ��� */
		  HI_JPEG_SURFACE_DESCRIPTION_S  stOutDesc;        /**< the user output message description    *//**<CNcomment:�û����õ������Ϣ����     */
          JPEG_MIDDLE_SURFACE_S          stMiddleSurface;
		  	
	}JPEG_HDEC_HANDLE_S,*JPEG_HDEC_HANDLE_S_PTR;

	

	/** @} */  /*! <!-- Structure Definition end */

	
    /********************** Global Variable declaration **************************/
 
    /******************************* API declaration *****************************/

	/** \addtogroup      JPEG HDEC API */
    /** @{ */  /** <!-- ��JPEG HDEC API�� */
	

	/*****************************************************************************
	* func			: JPEG_HDEC_Init
	* description	: init the private structure para
					  CNcomment: ��ʼ��˽�нṹ�����	CNend\n
	* param[in] 	: cinfo 	  CNcomment: �������	CNend\n
	* retval		: HI_SUCCESS  CNcomment: �ɹ�  CNend\n
	* retval		: HI_FAILURE  CNcomment: ʧ��  CNend\n
	* others:		: NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_Init(j_common_ptr cinfo);
	
	
	/*****************************************************************************
	* func			: JPEG_HDEC_Destroy
	* description	: destroy the hard decompress
					  CNcomment: ����Ӳ��������         CNend\n
	* param[in] 	: cinfo 	  CNcomment: �������	CNend\n
	* retval		: HI_SUCCESS  CNcomment: �ɹ�       CNend\n
	* retval		: HI_FAILURE  CNcomment: ʧ��       CNend\n
	* others:		: NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_Destroy(const struct jpeg_common_struct *cinfo);

	/*****************************************************************************
	* func			: JPEG_HDEC_Abort
	* description	: when want use the decompress again,call this
					  CNcomment:  ��������ʹ�ý����������øýӿ� CNend\n
	* param[in] 	: cinfo 	  CNcomment: �������	           CNend\n
	* retval		: HI_SUCCESS  CNcomment: �ɹ�  CNend\n
	* retval		: HI_FAILURE  CNcomment: ʧ��  CNend\n
	* others:		: NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_Abort(const struct jpeg_common_struct *cinfo);


	/*****************************************************************************
	* func			: JPEG_HDEC_OpenDev
	* description	: open some device that decode need
					  CNcomment: �򿪽�����Ҫ������豸 	   CNend\n
	* param[in] 	: cinfo 		CNcomment: �������    CNend\n
	* retval		: HI_SUCCESS	CNcomment: �ɹ� 	   CNend\n
	* retval		: HI_FAILURE	CNcomment: ʧ�� 	   CNend\n
	* others:		: NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_OpenDev(const struct jpeg_decompress_struct *cinfo);

	/*****************************************************************************
	* func			: JPEG_HDEC_CloseDev
	* description	: closxe some device that decode need
					  CNcomment: �رս���򿪵�����豸 	   CNend\n
	* param[in] 	: cinfo 		CNcomment: �������    CNend\n
	* retval		: HI_SUCCESS	CNcomment: �ɹ� 	   CNend\n
	* retval		: HI_FAILURE	CNcomment: ʧ�� 	   CNend\n
	* others:		: NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_CloseDev(const struct jpeg_common_struct *cinfo);
		

	/*****************************************************************************
	* func			: JPEG_HDEC_IfSupport
	* description	: check whether the hard decode support
	                  CNcomment: �ж��Ƿ�֧��Ӳ������ CNend\n
	* param[in]     : cinfo       CNcomment: ������� CNend\n
	* retval	    : HI_SUCCESS  CNcomment: �ɹ�     CNend\n
	* retval	    : HI_FAILURE  CNcomment: ʧ��     CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_IfSupport(j_decompress_ptr cinfo);
	

	/*****************************************************************************
	* func			: JPEG_HDEC_CheckStreamMemType
	* description	: check the stream buffer type, if user no call the function
	                  of set stream buffer type,call this function
	                  CNcomment: ��ѯ����buffer���ͣ��������������ڴ棬��������
	                  �ڴ棬Ҫ���û�û�е�����������buffer���ͣ����øýӿ�
	* param[in]     : cinfo       CNcomment: �������  CNend\n
	* param[in]     : pVirBuf     CNcomment: �����ڴ�  CNend\n
	* retval	    : HI_SUCCESS  CNcomment: �ɹ�      CNend\n
	* retval	    : HI_FAILURE  CNcomment: ʧ��      CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_CheckStreamMemType(const struct jpeg_decompress_struct *cinfo,HI_UCHAR* pVirBuf);
	

	/*****************************************************************************
	* func			: JPEG_HDEC_DuplicateStreamInfo
	* description	: save the stream information before into hard decode
	                  CNcomment: �ڽ���Ӳ������֮ǰ����������Ϣ����������λ��
	                  ʣ��������Լ�ʣ����������
	* param[in]     : cinfo       CNcomment: �������  CNend\n
	* retval	    : HI_SUCCESS  CNcomment: �ɹ�      CNend\n
	* retval	    : HI_FAILURE  CNcomment: ʧ��      CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32	JPEG_HDEC_DuplicateStreamInfo(const struct jpeg_decompress_struct *cinfo);
	
	/*****************************************************************************
	* func			: JPEG_HDEC_ResumeStreamInfo
	* description	: resume the stream information when hard decode failure,and
	                  then into soft decode
	                  CNcomment: ��Ӳ������ʧ�ܵ�ʱ��ָ�ԭ�ȱ����������Ϣ��Ȼ��
	                  ���������������
	* param[in]     : cinfo       CNcomment: �������  CNend\n
	* retval	    : HI_SUCCESS  CNcomment: �ɹ�      CNend\n
	* retval	    : HI_FAILURE  CNcomment: ʧ��      CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32  JPEG_HDEC_ResumeStreamInfo(j_decompress_ptr cinfo);
                                          

	/*****************************************************************************
	* func			: JPEG_HDEC_Start
	* description	: start jpeg hard decode
	                  CNcomment: ��ʼӲ������
	* param[in]     : cinfo       CNcomment: �������  CNend\n
	* retval	    : HI_SUCCESS  CNcomment: �ɹ�      CNend\n
	* retval	    : HI_FAILURE  CNcomment: ʧ��      CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_Start(j_decompress_ptr cinfo);

	/*****************************************************************************
	* func			: JPEG_HDEC_HardCSC
	* description	: use hard csc
	                  CNcomment: ʹ��Ӳ��������ɫ�ռ�ת�� CNend\n
	* param[in]     : cinfo      CNcomment: ������� CNend\n
	* retval	    : HI_SUCCESS CNcomment: �ɹ�     CNend\n
	* retval	    : HI_FAILURE CNcomment: ʧ��     CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_HardCSC(j_decompress_ptr cinfo);


    #ifdef CONFIG_JPEG_SOFTCSC_ENABLE
	/*****************************************************************************
	* func			: JPEG_HDEC_SoftCSC
	* description	: use soft csc, is use to debug the hard csc
	                  CNcomment: ʹ�����������ɫ�ռ�ת��,������λӲ��ת�������� CNend\n
	* param[in] 	: cinfo      CNcomment: �������  CNend\n
	* retval		: HI_SUCCESS CNcomment: �ɹ�      CNend\n
	* retval		: HI_FAILURE CNcomment: ʧ��      CNend\n
	* others:		: NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_SoftCSC(j_decompress_ptr cinfo);
    #endif


	/*****************************************************************************
	* func			: JPEG_HDEC_OutUserBuf
	* description	: output the scanlines buffer
	                  CNcomment:  ������û���buffer�� CNend\n
	* param[in]     : cinfo      CNcomment:  �������  CNend\n
	* param[out]    : max_lines  CNcomment:  ��������  CNend\n
	* param[out]    : scanlines  CNcomment:  ��buffer  CNend\n
	* retval	    : HI_SUCCESS CNcomment:  �ɹ�      CNend\n
	* retval	    : HI_FAILURE CNcomment:  ʧ��      CNend\n
	* others:	    : NA
	*****************************************************************************/
	HI_S32 JPEG_HDEC_OutUserBuf(j_decompress_ptr cinfo,JDIMENSION max_lines, HI_CHAR *scanlines);
 
	/** @} */  /*! <!-- API declaration end */
	
    /****************************************************************************/



#ifdef __cplusplus
    
#if __cplusplus
   
}
#endif
#endif /* __cplusplus */

#endif /* __JPEG_HDEC_API_H__*/
