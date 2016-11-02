/******************************************************************************

  Copyright (C), 2001-2011, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : aio_ext.h
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2009/5/5
  Description   :
  History       :
  1.Date        : 2009/5/5
    Author      : p00123320
    Modification: Created file
******************************************************************************/


#ifndef __AIAO_EXT_H__
#define __AIAO_EXT_H__

typedef struct hiAIO_DRV_DEV_CTX_S
{
    struct
    {
        HI_U32          u32BufSize;         /*ѭ��buf�Ĵ�С*/
        HI_U32          u32PhyAddr;         /*ѭ��buf������ʼ��ַ*/
        HI_U8*          pu8VirAddr;         /*ѭ��buf������ʼ��ַ*/

        HI_U32          u32RptrOffSet;         /*ѭ��buf��ָ�룬��¼��ָ���������ʼ��ַ��ƫ����*/
        HI_U32          u32WptrOffSet;         /*ѭ��bufдָ�룬��¼дָ���������ʼ��ַ��ƫ����*/

    } stCirBuf;

    AIO_ATTR_S          stAioAttr;
    HI_BOOL             bEnable;

    HI_U64              u64LastPts;         /*��֡ʱ���*/
    HI_U32              u32FrmTime;         /*֡ʱ����*/
    HI_U32              u32MaxFrmTime;      /*֮֡�����ʱ����*/
    HI_U32              u32IsrTime;         /*�жϴ���ʱ��*/
    HI_U32              u32MaxIsrTime;      /*�жϴ�����ʷ�ʱ��*/
    HI_U32              u32AioFifoLen;      /*AIO FIFO ���,һ��DMA�������Ƶ���ݣ�u32AioFifoLen���ֽڣ���СΪ8�����Ϊ32*/
    HI_U32              u32FifoLenBase;     /*AIO FIFO ��ȵ�2���ݼ�������8��2��������(����ͨ������ϲ��㷨����λ����)*/
    HI_U32              u32FifoShift;       /*AIO FIFO��������λ(��׼PCM��Ҫ��λ1λ)*/
    HI_U32              u32TransLen;        /*�ܵ�dma���䳤��,byteΪ��λ(���������*u32AioFifoLen) */

    HI_S32              as32ChnIndex[AIO_MAX_CHN_NUM];

    HI_U32              u32IntCnt;
    HI_U32              u32fifoIntCnt;          /*�ж���Ŀ��������*/
    HI_U32              u32buffIntCnt;          /*�ж���Ŀ��������*/
    AUDIO_TRACK_MODE_E  enTrackMode;
    HI_BOOL             bMute;
    AUDIO_FADE_S        stFade;
    HI_S32              s32Volume;
	HI_BOOL             bMicInl;
	HI_BOOL             bMicInr;
}AIO_DRV_DEV_CTX_S;

#endif
