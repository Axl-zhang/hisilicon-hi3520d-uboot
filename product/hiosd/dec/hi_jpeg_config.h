/******************************************************************************
*
* Copyright (C) 2014 Hisilicon Technologies Co., Ltd.  All rights reserved. 
*
* This program is confidential and proprietary to Hisilicon  Technologies Co., Ltd. (Hisilicon), 
* and may not be copied, reproduced, modified, disclosed to others, published or used, in
* whole or in part, without the express prior written permission of Hisilicon.
*
******************************************************************************
File Name	    : hi_jpeg_config.h
Version		    : Initial Draft
Author		    : 
Created		    : 2014/06/20
Description	    : this file is through set macro to select different funciton,
                  and select different platform
Function List 	: 

			  		  
History       	:
Date				Author        		Modification
2014/06/20		    y00181162  		    Created file      	
******************************************************************************/

 

#ifndef __HI_JPEG_CONFIG_H__
#define __HI_JPEG_CONFIG_H__


/*****************************************************************************/


#ifdef __cplusplus
      #if __cplusplus
   
extern "C" 
{

      #endif
#endif /* __cplusplus */

    /***************************** Macro Definition ******************************/

	/** \addtogroup 	 JPEG CFG */
    /** @{ */  /** <!-- ��JPEG CFG�� */


    /****************************************************************************
     **�꿪�ض�����Makefile�ж���
     ****************************************************************************/
	/** this macro is add google function and use in android version,define in Makefile */
	/** CNcomment:��android��google�汾����jpeg-6b�У���Makefile�п��Ƹú꿪�� CNend */
	#if 0
	#define CONFIG_JPEG_ADD_GOOGLEFUNCTION
	#endif

//�Ƿ�Ӳ����
#define HARD_DEC
/****************************************************************************
** function marco definition no depend chip platform
** CNcomment:��оƬƽ̨�޹صĺ궨��
****************************************************************************/
     #ifdef CONFIG_HI_SDK_JPEG_VERSION
	     #ifdef CONFIG_GFX_ANDROID_SDK
			/** use gfx mmz to alloc mem */
			/** CNcomment:ʹ���Լ���װ��MMZ�����ڴ� */
			#define CONFIG_JPEG_USE_PRIVATE_MMZ
		    /** use android debug message */
			/** CNcomment:Android�汾�ĵ�����Ϣ */
			#define CONFIG_JPEG_ANDROID_DEBUG_ENABLE
		 #elif defined(CONFIG_GFX_BVT_SDK)
		 	/** CNcomment:ʹ���Լ���װ��MMZ�����ڴ� */
			#define CONFIG_JPEG_USE_PRIVATE_MMZ
		 #else
			/** the some function realize before the main function */
			/** CNcomment:��Щ������main����֮ǰʵ�֣�Ҳ���ǳ�������֮�����һ�� */
			#define CONFIG_JPEG_REALIZEFUNC_BEFORMAINFUNC
	     #endif
	 #else
	 	/** the some function realize before the main function */
		/** CNcomment:��Щ������main����֮ǰʵ�֣�Ҳ���ǳ�������֮�����һ�� */
	    #define CONFIG_JPEG_REALIZEFUNC_BEFORMAINFUNC
		/** ���ؿ��ʱ���Ҫʵ�֣��������������ſ�������ʹ�� **/
	    #ifdef CONFIG_JPEG_USE_PRIVATE_MMZ
			#undef CONFIG_JPEG_USE_PRIVATE_MMZ
	    #endif
	 #endif
	 

	 /** fpga and eda and advca not use this file **/
	 #if defined(CONFIG_HI_SDK_JPEG_VERSION) && !defined(CONFIG_GFX_ADVCA_RELEASE)
	 /** if need proc message */
	 /** CNcomment:�Ƿ���Ҫproc��Ϣ */
     #define CONFIG_JPEG_PROC_ENABLE
	 #endif

	/** if need use soft csc to debug whether the hard csc has problem */
	/** CNcomment:ʹ�����ת������λӲ��ת���Ƿ������� */
    #define CONFIG_JPEG_SOFTCSC_ENABLE

	/** use hard idct method */
	/** CNcomment:ʹ��Ӳ����IDCT�㷨 */
    #define CONFIG_JPEG_USEHWIDCT

	/** support motion jpeg decode */
	/** CNcomment:֧��motion jpeg ���� */
	#define CONFIG_JPEG_MPG_DEC_ENABLE

	/** use the calc default value dqt and dht value */
	/** CNcomment:ʹ�ü���õ�������͹��������ֵ��ֱ�����üĴ��� */
	#ifdef CONFIG_JPEG_MPG_DEC_ENABLE
		//#define CONFIG_JPEG_USE_CALC_DEFAULT_VALUE
	#endif

/****************************************************************************
 ** function marco definition depend chip platform
 ** CNcomment:��оƬƽ̨��صĺ궨��
 ****************************************************************************/
	/** if support crop */
	/** CNcomment:�Ƿ�֧�ֲü����� */
	#define CONFIG_JPEG_OUTPUT_CROP
	/** if support suspend */
	/** CNcomment:�Ƿ�֧�ִ������� */
	#define CONFIG_JPEG_SUSPEND
	/** get jpeg dec time */
	/** CNcomment:��ȡjpeg�����ʱ�� */
	#define CONFIG_JPEG_GETDECTIME
	/** all jpeg dec output yuv420sp */
	/** CNcomment:ͳһ�������yuv420sp,Ӳ�����������֧��yuv400 -> yuv420
				��Ҫ����Ҫ֧�����0x80 */
	#define CONFIG_JPEG_OUTPUT_YUV420SP
	/** dec jpeg file output lu pixel value sum */
	/** CNcomment:ͳ������ֵ */
	#define CONFIG_JPEG_OUTPUT_LUPIXSUM
	/** if support jpeg hard dec to argb8888 */
	/** CNcomment:�Ƿ�֧��jpegӲ���������argb8888 */
	#define CONFIG_JPEG_HARDDEC2ARGB
	/** CNcomment:��������DDR�޷������4K�ڴ�����⣬����JPGD�߼�
	 ** Ҫ�Կ�4K�ڴ���д����������һ�ζ�С����
	 ** 0x0 - 0x1000 - 0x2000 - 0x3000 �ڴ���ʼ��ַ+��СҪ�������ڵ����� */
	/** use sdk CRG write */
	/** CNcomment:ʹ��SDK��CRG���� */	
	#define CONFIG_JPEG_USE_SDK_CRG_ENABLE

	/** HSCP201405300013 HSCP201405290010 DTS2014061006717**/
	/** CNcomment:hifone֮���޸ĵİ��ղ������������� **/
	#define CONFIG_JPEG_SET_SAMPLEFACTOR


	/****************************************************************************
     ** ���¸��ݲ�ͬоƬƽ̨����֧�ֵĹ��ܺ꿪�ش�
     ****************************************************************************/
	#ifdef CONFIG_GFX_BVT_SDK
		#undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_SUSPEND
		#undef CONFIG_JPEG_GETDECTIME
		#undef CONFIG_JPEG_HARDDEC2ARGB
		#undef CONFIG_JPEG_USE_SDK_CRG_ENABLE
		#undef CONFIG_JPEG_PROC_ENABLE
		#define CONFIG_JPEG_CSC_DISABLE
		/** �ü���yuvsp����Ĵ��� **/
		#define CONFIG_JPEG_CODE_CUT_RGB
	#endif

	#if   defined(HI_CHIP_TYPE_hi3531) || defined(HI_CHIP_TYPE_hi3532)   \
       || defined(HI_CHIP_TYPE_hi3521) || defined(HI_CHIP_TYPE_hi3520A)  \
       || defined(HI_CHIP_TYPE_hi3520D)
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
	#endif
	
	#if    defined(CHIP_TYPE_hi3716cv200)   \
        || defined(CHIP_TYPE_hi3716mv400)   \
        || defined(CHIP_TYPE_hi3718cv100)   \
        || defined(CHIP_TYPE_hi3718mv100)   \
        || defined(CHIP_TYPE_hi3719cv100)   \
        || defined(CHIP_TYPE_hi3719mv100)   \
        || defined(CHIP_TYPE_hi3719mv100_a)
        #undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
		#undef CONFIG_JPEG_HARDDEC2ARGB         /**CV200 EC֮���Ѿ��޸�**/
		/** the save stream buffer whether 4 byte align */
	    /** CNcomment:�洢����buffer�Ƿ���Ҫ4�ֽڶ���,���bug��3535/CV200(EC)֮���оƬ�����**/
	    #define CONFIG_JPEG_STREAMBUF_4ALIGN
	#endif

	#if defined(CHIP_TYPE_hi3712)       \
        || defined(CHIP_TYPE_hi3716c)    \
        || defined(CHIP_TYPE_hi3716m)    \
        || defined(CHIP_TYPE_hi3716h)
        #undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
		#undef CONFIG_JPEG_HARDDEC2ARGB
		#undef CONFIG_JPEG_USE_SDK_CRG_ENABLE
	#endif

	#ifdef CHIP_TYPE_hi3716mv310
        #undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_OUTPUT_YUV420SP
		#undef CONFIG_JPEG_OUTPUT_LUPIXSUM
		#undef CONFIG_JPEG_HARDDEC2ARGB
	#endif

	#ifndef CHIP_TYPE_hifone
		#undef CONFIG_JPEG_SET_SAMPLEFACTOR
	#else
		#undef CONFIG_JPEG_OUTPUT_CROP
		#undef CONFIG_JPEG_HARDDEC2ARGB
	#endif

	#ifndef CHIP_TYPE_hi3716mv310
		/** ֻ֧��mv310�Ŀ�������������ƽ̨����ts����˲�� **/
		#ifdef HI_MCE_SUPPORT
			#undef HI_MCE_SUPPORT
		#endif
	#endif
	/****************************************************************************
     ** function marco definition use to fpga test and deal with bug by eda
	 ** CNcomment:����Ӳ���߼����Լ���λbugʹ�õĺ꿪��
     ****************************************************************************/
	/** output the need output os memory size */
	/** CNcomment:���OS��Ҫ���ڴ��С */
	//#define CONFIG_JPEG_OUTPUT_NEED_OS_MEMSIZE
	
	/** test press */
	/** CNcomment:���Է�ѹ */
	//#define CONFIG_JPEG_TEST_CHIP_PRESS

	/**
	 ** Ĭ��֧�ֿ�4K��FPGA����Ҫ���Ե���֧�ֿ�4K�����(�����쳣)
	 ** ����Ŀ�4Kֻ����ȡ��ַ��ʱ����п�4K���������CBB��֧��
	 ** ��4K
	 **/
	//#define CONFIG_JPEG_4KDDR_DISABLE
    //#define CONFIG_JPEG_4KDDR_DISABLE_SHOWMSG /** ��ʾ��Ϣ **/
	
	/** save the scen information,use it to eda simulation */
	/** CNcomment:���ֳ�ʹ�ܣ����������õģ��ֳ����߼���EDA���� */
	//#define CONFIG_JPEG_FPGA_TEST_SAVE_SCEN_ENABLE
	/** save the data to bmp picture */
	/** CNcomment:����bmpͼƬ */
    //#define CONFIG_JPEG_TEST_SAVE_BMP_PIC
    /** save yuv semi-planer data */
	/** CNcomment:����yuv semi-planer���� */
    //#define CONFIG_JPEG_TEST_SAVE_YUVSP_DATA
	/** the chip hard decode random reset test */
	/** CNcomment:�����λ��Ƭ���� */
	//#define CONFIG_JPEG_TEST_CHIP_RANDOM_RESET
	/** run decode reset test */
	/** CNcomment:������������������λ */
	//#define CONFIG_JPEG_TEST_ALL_DEC_RANDOM_RESET
	/** the ck_gt_en is open */
	/** CNcomment:ʱ���ſش� */
	//#define CONFIG_JPEG_TEST_CHIP_OPEN_CK
	/** test hard decode capa */
	/** CNcomment:����Ӳ���������� */
	//#define CONFIG_JPEG_TEST_HARD_DEC_CAPA
	/** save the scen information,use it to eda simulation */
	/** CNcomment:���Դ������ӵļ����ӿ� */
	//#define CONFIG_JPEG_FPGA_TEST_SUSPEND_ENABLE
	/** test the continue stream ddr change */
	/** CNcomment:������������ַ�����仯����� */
	//#define CONFIG_JPEG_FPGA_TEST_CONTINUE_STREAM_DDR_CHANGE
	/** test set the different outstanding value */
	/** CNcomment:�������ò�ͬ��outstandingֵ,�Զ��ȽϺ꿪�ش� */
	//#define CONFIG_JPEG_FPGA_TEST_SET_DIFFERENT_OUTSTANDING_VALUE


	#ifdef CONFIG_HI_FPGA_JPEG_VERSION
		#define CONFIG_JPEG_FPGA_TEST_ENABLE
		#define CONFIG_JPEG_CSC_DISABLE
		#undef CONFIG_JPEG_USE_SDK_CRG_ENABLE
	#endif

	
    #ifdef CONFIG_GFX_ADVCA_RELEASE
	    /** this is not has warning message */
	    /** CNcomment:ֻ�д��ϲ����Ų����и澯��Ϣ */
        #define JPEG_TRACE( fmt,args...)
	#else
	    #ifdef __KERNEL__
		#define JPEG_TRACE               GFX_Printk
		#else
	 	#define JPEG_TRACE               GFX_Printf
		#endif
	#endif

    /** @} */  /*! <!-- Macro Definition end */

	
    /*************************** Structure Definition ****************************/
	
    /***************************  The enum of Jpeg image format  ******************/


    /********************** Global Variable declaration **************************/
    

    /******************************* API declaration *****************************/
	
	
    #ifdef __cplusplus

        #if __cplusplus



}
      
        #endif
        
   #endif /* __cplusplus */

#endif /* __HI_JPEG_CONFIG_H__*/
