/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : hi_jpeg_api.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : The user will use this api to realize some function
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/

#ifndef __HI_JPEG_API_H__
#define __HI_JPEG_API_H__


/*********************************add include here******************************/

#include "jpeglib.h"
#include "hi_type.h"

/*****************************************************************************/


#ifdef __cplusplus
#if __cplusplus
   extern "C" 
{
#endif
#endif /* __cplusplus */


     /***************************** Macro Definition ******************************/
	 /** \addtogroup	  JPEG */
	 /** @{ */	/** <!--[JPEG]*/

	 /** Maximum number of data components */
	 /** CNcomment:������ݷ����� CNend */
     #define MAX_PIXEL_COMPONENT_NUM            3
	 /** Maximum column number of hor coef */
	 /** CNcomment:���ˮƽ��ϵ�� CNend */
     #define MAX_HORCOEF_COL                     8
	 /** Maximum row number of hor coef */
	 /** CNcomment:���ˮƽ��ϵ�� CNend */
     #define MAX_HORCOEF_ROW                     4
	 /** Maximum column number of ver coef */
	 /** CNcomment:���ֱ��ϵ�� CNend */
	 #define MAX_VERCOEF_COL                     4
	 /** Maximum row number of ver coef */
	 /** CNcomment:���ֱ��ϵ�� CNend */
     #define MAX_VERCOEF_ROW                     4
	 /** Maximum column number of csc coef */
	 /** CNcomment:���csc��ϵ�� CNend */
	 #define MAX_CSCCOEF_COL                     3
	 /** Maximum row number of csc coef */
	 /** CNcomment:���csc��ϵ�� CNend */
     #define MAX_CSCCOEF_ROW                     3

     /** @} */  /** <!-- ==== Macro Definition end ==== */

	 /*************************** Enum Definition ****************************/
    /****************************************************************************/
	/*							   jpeg enum    					            */
	/****************************************************************************/
	
	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/

	/** enum of the input jpeg color space */
    /** CNcomment:�����jpegͼƬ��ʽ CNend */
	typedef enum hiHI_JPEG_FMT_E
	{
		JPEG_FMT_YUV400	    = 0,   /**< yuv400     */
		JPEG_FMT_YUV420	    = 1,   /**< yuv420     */
		JPEG_FMT_YUV422_21    = 2,   /**< yuv422_21  */
		JPEG_FMT_YUV422_12    = 3,   /**< yuv422_12  */
		JPEG_FMT_YUV444	    = 4,   /**< yuv444     */
		JPEG_FMT_YCCK   	    = 5,   /**< YCCK       */
		JPEG_FMT_CMYK   	    = 6,   /**< CMYK       */
		JPEG_FMT_BUTT	
	}HI_JPEG_FMT_E;

	/** enum of mem check type */
    /** CNcomment:�ڴ�ʣ���ж����� CNend */
	typedef enum hiHI_JPEG_MEMCHECK_E
	{
		JPEG_MEMCHECK_MEMSIZE    = 0,   /**< �ڴ��С   */
		JPEG_MEMCHECK_RESOLUTION = 1,   /**< �ֱ���     */
		JPEG_MEMCHECK_BUTT	
	}HI_JPEG_MEMCHECK_E;

    /** @} */  /** <!-- ==== enum Definition end ==== */
	
	/*************************** Structure Definition ****************************/
    /****************************************************************************/
	/*							   jpeg api structure    					    */
	/****************************************************************************/
	
	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/

	/** Structure of Rectangle */
    /** CNcomment:���� CNend */
	typedef struct hiJPEG_RECT_S
	{
		HI_S32 x, y;
		HI_S32 w, h;
	}HI_JPEG_RECT_S;

	/** Structure of the output yuvsp/rgb/yuv package message, only support 1/2/4/8/ scale */
	/** CNcomment:�������yuvsp/rgb/yuv pacekage �������Ϣ��ֻ֧��1/2/4/8���� CNend */
	typedef struct hiJPEG_SURFACE_S
	{
		 HI_CHAR* pOutPhy[MAX_PIXEL_COMPONENT_NUM];        /**< output physic address,0 is luminance or rgb buf, 1 is chroma buf    *//**<CNcomment:��������ַ��0�������y����rgb��ַ��1����Ϊɫ�ȵ�ַ CNend     */
		 HI_CHAR* pOutVir[MAX_PIXEL_COMPONENT_NUM];        /**< output virtual address,0 is luminance or rgb buf, 1 is chroma buf   *//**<CNcomment:��������ַ��0�������y����rgb��ַ��1����Ϊɫ�ȵ�ַ CNend     */
		 HI_U32   u32OutStride[MAX_PIXEL_COMPONENT_NUM];   /**< output stride                     *//**<CNcomment:����м�� CNend      */
         HI_BOOL  bUserPhyMem;                             /**< if use user physic memeory         *//**<CNcomment:�Ƿ�ʹ���û��������ڴ� CNend  */
	
	}HI_JPEG_SURFACE_S;
	
	/** Structure of the output message */
	/** CNcomment:������������Ϣ CNend */
	typedef struct hiJPEG_SURFACE_DESCRIPTION_S
	{
		 HI_JPEG_SURFACE_S	 stOutSurface; /**< output  surface   *//**<CNcomment:�����Ϣ CNend       */
		 HI_JPEG_RECT_S      stCropRect;   /**< output crop size  *//**<CNcomment:����ü����� CNend   */
		 HI_BOOL bCrop;                    /**<  if crop           *//**<CNcomment:�Ƿ�ü� CNend      */
	}HI_JPEG_SURFACE_DESCRIPTION_S;


	/** Structure of output argb sampler and filte message */
	/** CNcomment:�������argb�������˲������Ϣ CNend */
    typedef struct hiJPEG_DEC_COEF_S
    {
        HI_BOOL bEnHorMedian;                                  /**< if set hor median filter           *//**<CNcomment:�Ƿ�ˮƽ��ֵ�˲� CNend        */
		HI_BOOL bEnVerMedian;                                  /**< if set ver median filter           *//**<CNcomment:�Ƿ�ֱ��ֵ�˲� CNend        */
        HI_BOOL bSetHorSampleCoef;                             /**< if set hor sample coef            *//**<CNcomment:�Ƿ�����ˮƽ�������� CNend    */
		HI_BOOL bSetVerSampleCoef;                             /**< if set ver sample coef            *//**<CNcomment:�Ƿ����ô�ֱ�������� CNend    */
		HI_BOOL bSetCSCCoef;                                   /**< if set csc coef                    *//**<CNcomment:�Ƿ�����CSC���� CNend         */
		HI_S16  s16HorCoef[MAX_HORCOEF_ROW][MAX_HORCOEF_COL];  /**< set chrome hor sampler coefficient  *//**<CNcomment:ɫ���ϲ���ˮƽ�˲�ϵ�� CNend */
		HI_S16  s16VerCoef[MAX_VERCOEF_ROW][MAX_VERCOEF_COL];  /**< set chrome ver sampler coefficient  *//**<CNcomment:ɫ���ϲ�����ֱ�˲�ϵ�� CNend */
		HI_S16  s16CSCCoef[MAX_CSCCOEF_ROW][MAX_CSCCOEF_COL];  /**< set csc coefficient                 *//**<CNcomment:��ɫ�ռ�ת��ϵ�� CNend       */
    }HI_JPEG_DEC_COEF_S;


	/** Structure of the input and output jpeg width and height,input jpeg format */
	/** CNcomment:jpeg�ļ���ԭʼ��С�Լ����������С��ԭʼͼƬ��ʽ CNend */
	typedef struct hiJPEG_INFO_S
	{
	     HI_U32  u32Width[MAX_PIXEL_COMPONENT_NUM];     /**< 0 input and output rgb width and lu width,1 is ch width    *//**<CNcomment:0����ͽ������rgbͼƬ��Ȼ��߽���yuvsp���ȿ�ȣ�1Ϊɫ�ȿ�� CNend */
		 HI_U32  u32Height[MAX_PIXEL_COMPONENT_NUM];    /**< 0 input and output rgb height and lu width,1 is ch height  *//**<CNcomment:0����ͽ������rgbͼƬ�߶Ȼ��߽���yuvsp���ȸ߶ȣ�1Ϊɫ�ȸ߶� CNend */
		 HI_U32  u32OutStride[MAX_PIXEL_COMPONENT_NUM]; /**< 0 output rgb stride and lu stride,1 is ch stride           *//**<CNcomment:0�������rgb��yubsp�������м�࣬1Ϊɫ���м���� CNend             */
		 HI_U32  u32OutSize[MAX_PIXEL_COMPONENT_NUM];   /**< 0 output rgb size and lu size,1 is ch size		         *//**<CNcomment:0�������rgb��yubsp�Ĵ�С��1Ϊɫ�ȴ�С CNend            			  */
		 HI_JPEG_FMT_E enFmt;  /**< the input format  *//**<CNcomment:�����ͼƬ��ʽ CNend      */
		 HI_BOOL bOutInfo;     /**< false is get input information and the other get output information   *//**<CNcomment:FALSE��ȡ������Ϣ��TRUE��ȡ�����Ϣ CNend */
	}HI_JPEG_INFO_S;


	/** Structure of the leave memsize that jpeg decode can used */
	/** CNcomment:jpeg�������ʹ�õ��ڴ��С��Ϣ CNend */
	typedef struct hiJPEG_MEMSIZE_INFO_S
	{
	     HI_U32  u32MemSize;      /**< the memsize  *//**<CNcomment:ʣ���ڴ��С CNend */
		 HI_U32  u32Width;		   /**< limit width  *//**<CNcomment:���ƿ� CNend */
		 HI_U32  u32Height;		   /**< limit height *//**<CNcomment:���Ƹ� CNend */
		 HI_JPEG_MEMCHECK_E eCheckType; /**< limit type *//**<CNcomment:ͨ���������������� CNend */
	}HI_JPEG_MEMSIZE_INFO_S;
	
	/** @} */  /** <!-- ==== Structure Definition End ==== */
	
    /********************** Global Variable declaration **************************/
 
    /******************************* API declaration *****************************/


	/** \addtogroup	  JPEG */
	/** @{ */	/** <!--[JPEG]*/
	

	/** 
	\brief Sets dec output message. CNcomment:���ý����������Ϣ������ CNend
	\attention \n
	HI_JPEG_SetOutDesc should have called create jpeg decoder.set the output address \n
	and output stride,set whether crop, set crop rect \n
	CNcomment:�����ڴ���������֮����������֮ǰ���øýӿڣ���Ҫ���ý��������ַ����� \n
	          �м�࣬�����Ƿ�ü��Լ���Ӧ�Ĳü����� CNend\n

	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	*pstSurfaceDesc. CNcomment:�������������Ϣ CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetOutDesc
	*/
	HI_S32	HI_JPEG_SetOutDesc(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_SURFACE_DESCRIPTION_S *pstSurfaceDesc);

	/** 
	\brief Get Jpeg information. CNcomment:��ȡjpegͼƬ��Ϣ CNend
	\attention \n
	if you want to get input format and input width and input height,you should set bOutInfo false.\n
    others you can get the information as follows: output rgb widht/height/stride/size or output \n
    yuvsp lu width/height/stride/size and ch width/height/stride/size.\n
	you call this function should after read header and set the ouput parameter.\n
	CNcomment:��bOutInfo���ó�FALSE��ʱ�򣬿��Ի�ȡ��ͼƬ����Ŀ�Ⱥ͸߶��Լ����ظ�ʽ�������ó�TRUE�� \n
	          ʱ������Ի�ȡ��������Ϣ��Ҫ�ǽ���RGB���ȡ�����/�߶�/�м��/��С,Ҫ�ǽ������yuvsp��\n
	          ����Ի�ȡ�����Ⱥ�ɫ�ȵĿ��/�߶�/�м��/��С����Ϣ�� CNend\n

	\param[in]	cinfo. CNcomment:������� CNend
	\param[out]	pJpegInfo.  CNcomment:����jpeg�������Ϣ  CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetJpegInfo
	*/
	HI_S32  HI_JPEG_GetJpegInfo(j_decompress_ptr cinfo, HI_JPEG_INFO_S *pJpegInfo);


	/** 
	\brief set jpeg dec inflexion. CNcomment:��Ӳ������֧�ֵ�����£���������Ӳ��Ĺյ� CNend
	\attention \n
	HI_JPEG_SetInflexion should have called jpeg_create_decompress.if no call this \n
	function,use the default flexion \n
	CNcomment:�����ڴ����������֮����øú�����Ҫ��û�����ùյ㣬ʹ��Ĭ�ϵĹյ��С CNend\n

	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	u32flexionSize. CNcomment:Ҫ���õĽ���յ��С CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetInflexion
	*/
	HI_S32 HI_JPEG_SetInflexion(const struct jpeg_decompress_struct *cinfo, const HI_U32 u32flexionSize);
	

	/** 
	\brief get jpeg dec inflexion. CNcomment:��ȡ�����Ӳ������Ĺյ� CNend
	\attention \n
	HI_JPEG_GetInflexion should have called jpeg_create_decompress.\n
	CNcomment:�ڵ���HI_JPEG_GetInflexion֮ǰ�����Ѿ��������˽����� CNend\n

	\param[in]	cinfo. CNcomment:������� CNend
	\param[out]	pu32flexionSize. CNcomment:����յ��С CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetInflexion
	*/
	HI_S32 HI_JPEG_GetInflexion(const struct jpeg_decompress_struct *cinfo, HI_U32 *pu32flexionSize);
	

	/** 
	\brief set jpeg dec coef when output argb. CNcomment:�ڽ������ARGB��������������ϵ�� CNend
	\attention \n
	HI_JPEG_SetDecCoef should have called jpeg_create_decompress.set whether horizontal \n
	and vertical fliter,whether set horizontal and ver sample, whether set csc coefficient, \n
	and set there coefficient.if no call this function, use the default parameter. \n
	CNcomment:�����ڴ����������֮����øú�������Ҫ�����Ƿ�ֱ��ˮƽ�˲����Ƿ����ô�ֱ��ˮƽ \n
	          ����ϵ�����Ƿ�����CSSϵ�������������Ӧ��ϵ����Ҫ��û�е��øú�����ʹ��Ĭ��ֵ CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	*pstDecCoef. CNcomment:����ϵ�� CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetDecCoef
	*/
	HI_S32 HI_JPEG_SetDecCoef(const struct jpeg_decompress_struct *cinfo, const HI_JPEG_DEC_COEF_S *pstDecCoef);


	/** 
	\brief get jpeg dec coef when output argb. CNcomment:�ڽ������ARGB������»�ȡ���õ����ϵ�� CNend
	\attention \n
	HI_JPEG_GetDecCoef should have called HI_JPEG_SetDecCoef.\n
	CNcomment:�ڵ���HI_JPEG_GetDecCoef֮ǰ�����Ѿ�HI_JPEG_SetDecCoef CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[out]	pstOutDecCoef. CNcomment:�������ϵ�� CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetDecCoef
	*/
	HI_S32 HI_JPEG_GetDecCoef(const struct jpeg_decompress_struct *cinfo, HI_JPEG_DEC_COEF_S *pstOutDecCoef);


	/** 
	\brief set alpha value. CNcomment:����alpha��ֵ CNend
	\attention \n
	HI_JPEG_SetAlpha should have called jpeg_create_decompress.when decode output \n
	argb8888 and argb8888,we can call this function,if no call it,use the default value. \n
	CNcomment:�����ڴ����������֮����øú��������������ΪARGB8888��ABGR8888��ʱ����� \n
	���øú�����Ҫ��û�е��øú�������ʹ��Ĭ�ϵ�ֵ CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	s32Alpha. CNcomment:����alphaֵ CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetAlpha
	*/
	HI_S32 HI_JPEG_SetAlpha(const struct jpeg_decompress_struct *cinfo, const HI_U32 u32Alpha);


	/** 
	\brief set stream from flag of use phy mem  or virtual mem. CNcomment:���������������������ڴ���Ϣ CNend
	\attention \n
	if want to use this function,should call between create decompress and 
	jpeg_stdio_src or jpeg_mem_src.if not call this we should check\n
	CNcomment:���Ҫ���ã������ڴ������������������֮ǰ���ã����û�е��øýӿ�Ҳ�п������������ڴ� CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	pStreamPhyAddr. CNcomment:���������ַ CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetStreamPhyMem
	*/
	 HI_S32 HI_JPEG_SetStreamPhyMem(const struct jpeg_decompress_struct *cinfo, HI_CHAR* pStreamPhyAddr);

	/** 
	\brief set stream mem information. CNcomment:���������ڴ���Ϣ CNend
	\attention \n
	if want to use this function,should call before call
	jpeg_stdio_src or jpeg_mem_src.if not call this we should check\n
	CNcomment:���Ҫ���ã��������jpeg_mem_src֮ǰ CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	pPhyStremBuf.         CNcomment:����buffer��ʼ�����ַ CNend
	\param[in]	u32StremBufLen.       CNcomment:����buffer��С         CNend
	\param[in]	pVirSaveReturnBuf.    CNcomment:����buffer������ʼ��ַ CNend
	\param[in]	s32StrmReturnSize.    CNcomment:����������С           CNend
	\param[in]	bStreamBufNeedReturn. CNcomment:����buffer�Ƿ����     CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetBufInfo
	*/
	HI_S32 HI_JPEG_SetBufInfo(const struct jpeg_decompress_struct *cinfo,HI_CHAR* pPhyStremBuf,HI_U32 u32StremBufLen, HI_CHAR* pVirSaveReturnBuf,HI_S32 s32StrmReturnSize,HI_BOOL bStreamBufNeedReturn);

	/** 
	\brief set if dec output yuv420sp. CNcomment:�����Ƿ�ͳһ���yuv420sp��ʶ CNend
	\attention \n
	HI_JPEG_SetYCbCr420spFlag should have called jpeg_create_decompress.\n
	CNcomment:�ڵ���HI_JPEG_SetYCbCr420spFlag֮ǰ�����Ѿ��������˽����� CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	bOutYCbCr420sp. CNcomment:�Ƿ�ͳһ�������yuv420sp��ʽ CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetYCbCr420spFlag
	*/
	HI_S32 HI_JPEG_SetYCbCr420spFlag(const struct jpeg_decompress_struct *cinfo,const HI_BOOL bOutYCbCr420sp);


	/** 
	\brief set if output lu pixle sum value. CNcomment:�����Ƿ�ͳ������ֵ��ʶ CNend
	\attention \n
	HI_JPEG_SetLuPixSumFlag should have called jpeg_create_decompress.\n
	CNcomment:�ڵ���HI_JPEG_SetLuPixSumFlag֮ǰ�����Ѿ��������˽����� CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[in]	bLuPixSum. CNcomment:�����Ƿ�ͳ������ֵ��ʶ CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetLuPixSumFlag
	*/
	HI_S32 HI_JPEG_SetLuPixSumFlag(const struct jpeg_decompress_struct *cinfo,const HI_BOOL bLuPixSum);

	/** 
	\brief get lu pixle sum value. CNcomment:��ȡ����ֵ CNend
	\attention \n
	If you want to get the luminance value, you can call this function, \n
	but you should call it after jpeg_start_decompress and have call HI_JPEG_SetLuPixSumFlag.\n
	CNcomment:Ҫ����õ�����ֵ�����Ե��øú�������������jpeg_start_decompress֮����ö��ҽ��� \n
	          ֮ǰҪ����HI_JPEG_SetLuPixSumFlag CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[out]	u64LuPixSum. CNcomment:�������ֵ CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetLuPixSum
	*/
	HI_S32 HI_JPEG_GetLuPixSum(const struct jpeg_decompress_struct *cinfo,HI_U64 *u64LuPixSum);


	/** 
	\brief get jpeg dec time. CNcomment:��ȡjpeg����ʱ�� CNend
	\attention \n
	If you want to know how much the decode cost time ,you can call HI_JPEG_GetDecTime, \n
	but should have called it after jpeg_finish_decompress.\n
	CNcomment:Ҫ���뿴���뻨���˶���ʱ����Ե��øú������������ڽ������֮����� CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[out]	pu32DecTime. CNcomment:�����������ʱ�� CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_GetDecTime
	*/
	HI_S32 HI_JPEG_GetDecTime(const struct jpeg_decompress_struct *cinfo, HI_U32 *pu32DecTime);


	
	/** 
	\brief set the leave memory size. CNcomment:����ʣ���ڴ��С���Թ�����ʹ�� CNend
	\attention \n
	should have called it after  create jpeg decoder.\n
	CNcomment:�����ڴ���������֮����� CNend\n
	
	\param[in]	cinfo. CNcomment:������� CNend
	\param[int] sMemSizeInfo. CNcomment:�ڴ���Ϣ CNend
	
	\retval ::HI_SUCCESS 
	\retval ::HI_FAILURE
	
	\see \n
	::HI_JPEG_SetLeaveMemSize
	*/
	HI_S32 HI_JPEG_SetLeaveMemSize(const struct jpeg_decompress_struct *cinfo, HI_JPEG_MEMSIZE_INFO_S sMemSizeInfo);

	/** @} */  /** <!-- ==== API Declaration End ==== */

    /****************************************************************************/



#ifdef __cplusplus
    
#if __cplusplus
   
}
#endif
#endif /* __cplusplus */

#endif /* __HI_JPEG_API_H__*/
