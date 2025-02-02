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
** 文   件   名: lwip_ifctl.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2017 年 12 月 08 日
**
** 描        述: ioctl 网络接口支持.
*********************************************************************************************************/

#ifndef __LWIP_IFCTL_H
#define __LWIP_IFCTL_H

/*********************************************************************************************************
  裁剪控制
*********************************************************************************************************/
#if LW_CFG_NET_EN > 0

extern INT  __ifIoctlInet(INT  iCmd, PVOID  pvArg);
extern INT  __ifIoctlPacket(INT  iCmd, PVOID  pvArg);

#if LW_CFG_NET_WIRELESS_EN > 0
extern INT  __ifIoctlWireless(INT  iCmd, PVOID  pvArg);
#endif                                                                  /*  LW_CFG_NET_WIRELESS_EN > 0  */

#endif                                                                  /*  LW_CFG_NET_EN               */
#endif                                                                  /*  __LWIP_IFCTL_H              */
/*********************************************************************************************************
  END
*********************************************************************************************************/
