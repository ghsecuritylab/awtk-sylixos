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
** 文   件   名: lwip_netif.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2017 年 12 月 14 日
**
** 描        述: lwip 网口特殊操作.
*********************************************************************************************************/

#ifndef __LWIP_NETIF_H
#define __LWIP_NETIF_H

/*********************************************************************************************************
  裁剪控制
*********************************************************************************************************/
#if LW_CFG_NET_EN > 0

#ifdef __SYLIXOS_KERNEL
#include "lwip/netif.h"

INT   netif_get_flags(struct netif *pnetif);
VOID  netif_callback_init(VOID);
#endif                                                                  /*  __SYLIXOS_KERNEL            */

#endif                                                                  /*  LW_CFG_NET_EN               */
#endif                                                                  /*  __LWIP_NETIF_H              */
/*********************************************************************************************************
  END
*********************************************************************************************************/
