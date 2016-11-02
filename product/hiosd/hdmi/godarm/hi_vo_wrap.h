/******************************************************************************
  Copyright (C), 2001-2021, Hisilicon Tech. Co., Ltd.
******************************************************************************
File Name     : vo_wrap.h
Version       : Initial Draft
Author        : Hisilicon multimedia software group
Created       : 2010/02/12
Last Modified :
Description   :
Function List :
History       :
******************************************************************************/
#ifndef __VO_WRAP_H__
#define __VO_WRAP_H__


#ifdef __cplusplus
 #if __cplusplus
extern "C" {
 #endif
#endif /* __cplusplus */

#define IRQ_HANDLED 0

#define printk      printf
#define HI_ERR_DISP printf
#define HI_PRINT    printf

#ifndef MAX_DAC_NUM
#define MAX_DAC_NUM     6
#endif

/**������Ƶ��ʽö��*/
typedef enum hiHDMI_VIDEO_FMT_E
{
    HI_HDMI_VIDEO_FMT_1080P_60 = 0,     /**<1080p60����*/
    HI_HDMI_VIDEO_FMT_1080P_50,         /**<1080p50����*/
    HI_HDMI_VIDEO_FMT_1080P_30,         /**<1080p30����*/
    HI_HDMI_VIDEO_FMT_1080P_25,         /**<1080p25����*/
    HI_HDMI_VIDEO_FMT_1080P_24,         /**<1080p24����*/

    HI_HDMI_VIDEO_FMT_1080i_60,         /**<1080i60���� */
    HI_HDMI_VIDEO_FMT_1080i_50,         /**<1080i60���� */

    HI_HDMI_VIDEO_FMT_720P_60,          /**<720p60���� */
    HI_HDMI_VIDEO_FMT_720P_50,          /**<720p50���� */

    HI_HDMI_VIDEO_FMT_576P_50,          /**<576p50���� */
    HI_HDMI_VIDEO_FMT_480P_60,          /**<480p60���� */

    HI_HDMI_VIDEO_FMT_PAL,              /* B D G H I PAL */
    HI_HDMI_VIDEO_FMT_PAL_N,            /* (N)PAL        */
    HI_HDMI_VIDEO_FMT_PAL_Nc,           /* (Nc)PAL       */

    HI_HDMI_VIDEO_FMT_NTSC,             /* (M)NTSC       */
    HI_HDMI_VIDEO_FMT_NTSC_J,           /* NTSC-J        */
    HI_HDMI_VIDEO_FMT_NTSC_PAL_M,       /* (M)PAL        */

    HI_HDMI_VIDEO_FMT_SECAM_SIN,        /**< SECAM_SIN*/
    HI_HDMI_VIDEO_FMT_SECAM_COS,        /**< SECAM_COS*/

    HI_HDMI_VIDEO_FMT_861D_640X480_60,
    HI_HDMI_VIDEO_FMT_VESA_800X600_60,
    HI_HDMI_VIDEO_FMT_VESA_1024X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X720_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X800_60,
    HI_HDMI_VIDEO_FMT_VESA_1280X1024_60,
    HI_HDMI_VIDEO_FMT_VESA_1366X768_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60,
    HI_HDMI_VIDEO_FMT_VESA_1440X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X900_60_RB,
    HI_HDMI_VIDEO_FMT_VESA_1600X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1080_60,
    HI_HDMI_VIDEO_FMT_VESA_1920X1200_60,
    HI_HDMI_VIDEO_FMT_VESA_2048X1152_60,
    HI_HDMI_VIDEO_FMT_VESA_CUSTOMER_DEFINE,        // for lvds timing

    HI_HDMI_VIDEO_FMT_BUTT
}HI_HDMI_VIDEO_FMT_E;

/**�����߱�ת����ʽö��*/
typedef enum hiHDMI_ASPECT_CVRS_E
{
    HI_HDMI_ASPECT_CVRS_IGNORE = 0x0,   /**< ����*/
    HI_HDMI_ASPECT_CVRS_LETTERBOX,      /**< �Ӻڱ�*/
    HI_HDMI_ASPECT_CVRS_PANANDSCAN,     /**< �ü�*/
    HI_HDMI_ASPECT_CVRS_COMBINED,       /**< �ӺڱߺͲü����*/
    HI_HDMI_ASPECT_CVRS_BUTT
} HI_HDMI_ASPECT_CVRS_E;

typedef enum hiHDMI_DISP_MACROVISION_MODE_E
{
    HI_HDMI_DISP_MACROVISION_MODE_TYPE0,  /**<��������0 */
    HI_HDMI_DISP_MACROVISION_MODE_TYPE1,  /**<��������1 */
    HI_HDMI_DISP_MACROVISION_MODE_TYPE2,  /**<��������2 */
    HI_HDMI_DISP_MACROVISION_MODE_TYPE3,  /**<��������3 */
    HI_HDMI_DISP_MACROVISION_MODE_CUSTOM, /**<�û��Զ������� */
    HI_HDMI_DISP_MACROVISION_MODE_BUTT
} HI_HDMI_DISP_MACROVISION_MODE_E;

/* CGMS type select */
typedef enum hiHDMI_DISP_CGMS_TYPE_E
{
    HI_HDMI_DISP_CGMS_TYPE_A = 0x00,
    HI_HDMI_DISP_CGMS_TYPE_B,

    HI_HDMI_DISP_CGMS_TYPE_BUTT
}HI_HDMI_DISP_CGMS_TYPE_E;


/* definition of CGMS mode */
typedef enum hiHDMI_DISP_CGMS_MODE_E
{
   HI_HDMI_DISP_CGMS_MODE_COPY_FREELY    = 0,    /* copying is permitted without restriction */
   HI_HDMI_DISP_CGMS_MODE_COPY_NO_MORE   = 0x01, /* No more copies are allowed (one generation copy has been made) */
   HI_HDMI_DISP_CGMS_MODE_COPY_ONCE      = 0x02, /* One generation of copies may be made */
   HI_HDMI_DISP_CGMS_MODE_COPY_NEVER     = 0x03, /* No copying is permitted */
      
   HI_HDMI_DISP_CGMS_MODE_BUTT
}HI_HDMI_DISP_CGMS_MODE_E;


/** definition of CGMS configuration */
typedef struct hiHDMI_DISP_CGMS_CFG_S
{
    HI_BOOL                  bEnable;    /** HI_TRUE:CGMS is enabled; HI_FALSE:CGMS is disabled */
    HI_HDMI_DISP_CGMS_TYPE_E  enType;     /** type-A or type-B or None(BUTT) */
    HI_HDMI_DISP_CGMS_MODE_E  enMode;     /** CGMS mode. */
     
}HI_HDMI_DISP_CGMS_CFG_S;


typedef enum hiHDMI_DISP_DAC_MODE_E
{
    HI_HDMI_DISP_DAC_MODE_SILENCE = 0,    /**<�����*/
    HI_HDMI_DISP_DAC_MODE_CVBS,           /**<CVBS���*/
    HI_HDMI_DISP_DAC_MODE_Y,              /**<Y���ȷ������*/
    HI_HDMI_DISP_DAC_MODE_PB,             /**<��ɫɫ��������*/
    HI_HDMI_DISP_DAC_MODE_PR,             /**<��ɫɫ��������*/
    HI_HDMI_DISP_DAC_MODE_R,              /**<��ɫ�������*/
    HI_HDMI_DISP_DAC_MODE_G,              /**<��ɫ�������*/
    HI_HDMI_DISP_DAC_MODE_B,              /**<��ɫ�������*/
    HI_HDMI_DISP_DAC_MODE_SVIDEO_Y,       /**<SVIDEO_Y�������*/
    HI_HDMI_DISP_DAC_MODE_SVIDEO_C,       /**<SVIDEO_C�������*/
    HI_HDMI_DISP_DAC_MODE_HD_Y,           /**<����Y���ȷ������*/
    HI_HDMI_DISP_DAC_MODE_HD_PB,          /**<������ɫɫ��������*/
    HI_HDMI_DISP_DAC_MODE_HD_PR,          /**<�����ɫɫ��������*/
    HI_HDMI_DISP_DAC_MODE_HD_R,           /**<�����ɫ�������*/
    HI_HDMI_DISP_DAC_MODE_HD_G,           /**<������ɫ�������*/
    HI_HDMI_DISP_DAC_MODE_HD_B,           /**<������ɫ�������*/
    HI_HDMI_DISP_DAC_MODE_BUTT
}HI_HDMI_DISP_DAC_MODE_E;

/**����֧�ֵ���ʾ����ӿ� */
typedef struct  hiHDMI_DISP_INTERFACE_S
{
    HI_BOOL                      bScartEnable;                /**<�Ƿ����Scart�ź�*/
    HI_BOOL                      bBt1120Enable;               /**<�Ƿ����Bt1120�����ź�*/
    HI_BOOL                      bBt656Enable;                /**<�Ƿ����Bt656�����ź�*/
    HI_HDMI_DISP_DAC_MODE_E       enDacMode[MAX_DAC_NUM];      /**<����DAC������ź�*/
}HI_HDMI_DISP_INTERFACE_S ;

typedef enum hiHDMI_DISP_INTF_TYPE_E
{
    HI_HDMI_DISP_INTF_TYPE_TV = 0,    /**<����ӿ�ΪTV*/
    HI_HDMI_DISP_INTF_TYPE_LCD,       /**<����ӿ�ΪLCD*/
    HI_HDMI_DISP_INTF_TYPE_BUTT
}HI_HDMI_DISP_INTF_TYPE_E;

/**����LCD���������λ��*/
typedef enum hiHDMI_DISP_LCD_DATA_WIDTH_E
{
    HI_HDMI_DISP_LCD_DATA_WIDTH8 = 0,       /**<8λ*/
    HI_HDMI_DISP_LCD_DATA_WIDTH16,          /**<16λ*/
    HI_HDMI_DISP_LCD_DATA_WIDTH24,          /**<24λ*/
    HI_HDMI_DISP_LCD_DATA_WIDTH_BUTT
}HI_HDMI_DISP_LCD_DATA_WIDTH_E;


/**����LCD�����ݸ�ʽ*/
typedef enum hiHDMI_DISP_LCD_FORMAT_E
{
    HI_HDMI_DISP_LCD_FORMAT_YUV422 = 0,         /**<YUV422��λ��Ϊ16*/
    HI_HDMI_DISP_LCD_FORMAT_RGB565 = 0x8,       /**<RGB565��λ��Ϊ16*/
    HI_HDMI_DISP_LCD_FORMAT_RGB444 = 0xa,       /**<RGB444��λ��Ϊ16*/
    HI_HDMI_DISP_LCD_FORMAT_RGB666 = 0xb,       /**<RGB666��λ��Ϊ24*/
    HI_HDMI_DISP_LCD_FORMAT_RGB888 = 0xc,       /**<RGB888��λ��Ϊ24*/
    HI_HDMI_DISP_LCD_FORMAT_BUTT
}HI_HDMI_DISP_LCD_FORMAT_E;

/**����LCD�����ݸ�ʽ*/
typedef enum hiHI_HDMI_DISP_LCD_DATA_FMT
{
    HI_HDMI_DISP_LCD_DATA_FMT_YUV422 = 0,         /**<YUV422��λ��Ϊ16*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB565 = 0x8,       /**<RGB565��λ��Ϊ16*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB444 = 0xa,       /**<RGB444��λ��Ϊ16*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB666 = 0xb,       /**<RGB666��λ��Ϊ24*/
    HI_HDMI_DISP_LCD_DATA_FMT_RGB888 = 0xc,       /**<RGB888��λ��Ϊ24*/
    HI_HDMI_DISP_LCD_DATA_FMT_BUTT
}HI_HDMI_DISP_LCD_DATA_FMT_E;


/**����LCD��ʱ�����*/
typedef struct hiHDMI_DISP_LCD_PARA_S
{
    HI_U32                    VFB;                 /**<��ֱǰ����*/
    HI_U32                    VBB;                 /**<��ֱ������*/
    HI_U32                    VACT;                /**<��ֱ��Ч��*/
    HI_U32                    HFB;                 /**<ˮƽǰ����*/
    HI_U32                    HBB;                 /**<ˮƽ������*/
    HI_U32                    HACT;                /**<ˮƽ��Ч��*/
    HI_U32                    VPW;                 /**<��ֱ������*/
    HI_U32                    HPW;                 /**<ˮƽ������*/
    HI_BOOL                   IDV;                 /**<��Ч�����ź��Ƿ�ת*/
    HI_BOOL                   IHS;                 /**<ˮƽͬ�������ź��Ƿ�ת*/
    HI_BOOL                   IVS;                 /**<��ֱͬ�������ź��Ƿ�ת*/
    HI_BOOL                   ClockReversal;       /**<ʱ���Ƿ�ת*/
    HI_HDMI_DISP_LCD_DATA_WIDTH_E   DataWidth;      /**<����λ��*/
    HI_HDMI_DISP_LCD_FORMAT_E       ItfFormat;      /**<���ݸ�ʽ.*/
    HI_BOOL                   DitherEnable;        /**<�Ƿ�ʹ��Dither*/
    HI_U32                    ClkPara0;            /**<PLL  register SC_VPLL1FREQCTRL0  value */
    HI_U32                    ClkPara1;            /**<PLL  register SC_VPLL1FREQCTRL1 value*/
    HI_U32                    InRectWidth;
    HI_U32                    InRectHeight;
} HI_HDMI_DISP_LCD_PARA_S;



/**������ʾ��ɫ�Ľṹ�� */
typedef struct  hiHDMI_DISP_BG_COLOR_S
{
    HI_U8 u8Red ;            /**<��ɫ����*/
    HI_U8 u8Green ;          /**<��ɫ����*/
    HI_U8 u8Blue ;           /**<��ɫ����*/
} HI_HDMI_DISP_BG_COLOR_S;


#ifdef __cplusplus
 #if __cplusplus
}
 #endif
#endif /* __cplusplus */

#endif /* __VO_WRAP_H__ */
