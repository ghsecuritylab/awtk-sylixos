/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: cpu_cfg_c6x.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2017 年 02 月 28 日
**
** 描        述: TI C6X DSP 类型与功能配置.
*********************************************************************************************************/

#ifndef __CPU_CFG_C6X_H
#define __CPU_CFG_C6X_H

/*********************************************************************************************************
  CPU 体系结构
*********************************************************************************************************/

#define LW_CFG_CPU_ARCH_C6X             1                               /*  CPU 架构                    */
#define LW_CFG_CPU_ARCH_FAMILY          "TI C6000(R)"                   /*  TI C6000 family             */

/*********************************************************************************************************
  CPU 字长与整型大小端定义
*********************************************************************************************************/

#if defined(_LITTLE_ENDIAN)                                             /*  cl6x, gcc compiler          */
#define LW_CFG_CPU_ENDIAN               0                               /*  0: 小端                     */
#else
#define LW_CFG_CPU_ENDIAN               1                               /*  1: 大端                     */
#endif

#define LW_CFG_CPU_WORD_LENGHT          32                              /*  CPU 字长                    */
#define LW_CFG_CPU_PHYS_ADDR_64BIT      0                               /*  物理地址 64bit 宽度         */

/*********************************************************************************************************
  C6X 配置
*********************************************************************************************************/

#define LW_CFG_C6X_CACHE_L2             1                               /*  是否允许管理 C6X 二级 CACHE */

/*********************************************************************************************************
  浮点运算单元 (TI 浮点 DSP 不需要像一般处理器一样切换特殊的浮点上下文, 浮点寄存器已经包含在普通上下文中)
*********************************************************************************************************/

#define LW_CFG_CPU_FPU_EN               0                               /*  CPU 是否拥有 FPU            */

/*********************************************************************************************************
  DSP 数字信号处理器
*********************************************************************************************************/

#define LW_CFG_CPU_DSP_EN               0                               /*  CPU 是否拥有 DSP            */

/*********************************************************************************************************
  ATOMIC
*********************************************************************************************************/

#define LW_CFG_CPU_ATOMIC_EN            0

#endif                                                                  /*  __CPU_CFG_C6X_H             */
/*********************************************************************************************************
  END
*********************************************************************************************************/
