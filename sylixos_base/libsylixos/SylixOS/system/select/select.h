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
** 文   件   名: select.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2007 年 11 月 07 日
**
** 描        述:  IO 系统 select 子系统综合头文件.

** BUG: 
2013.05.07  将 FD_SETSIZE 修改为 2048. 
            由于 FD_ZERO() 巧妙的使用了 sizeof(*(p) 所以可以做到与以前 FD_SETSIZE 较小时编译的应用兼容.
*********************************************************************************************************/

#ifndef __SELECT_H
#define __SELECT_H

/*********************************************************************************************************
  一组文件掩码类型
*********************************************************************************************************/

typedef ULONG               fd_mask;                                    /*  单位掩码                    */

/*********************************************************************************************************
  CONFIG MACRO
  
  FD_SETSIZE 将包括所有可能的文件描述符, LW_CFG_MAX_FILES 必须小于 FD_SETSIZE
*********************************************************************************************************/

#ifndef FD_SETSIZE                                                      
#define FD_SETSIZE          2048                                        /*  最多支持的文件数量          */
#endif                                                                  /*  FD_SETSIZE                  */

#ifndef NBBY
#define NBBY                8                                           /*  每个字节有 8 位             */
#endif                                                                  /*  NBBY                        */

#define NFDBITS             (sizeof(fd_mask) * NBBY)                    /*  每一个单位掩码的位数        */

/*********************************************************************************************************
  x 类型包含多少倍的 y 
*********************************************************************************************************/

#define __HOWMANY(x, y)     ((((x) + ((y) - 1))) / (y))

/*********************************************************************************************************
  文件集的类型
*********************************************************************************************************/

typedef struct fd_set {
    fd_mask                 fds_bits[__HOWMANY(FD_SETSIZE, NFDBITS)];   /*  建立掩码表                  */
} fd_set;

/*********************************************************************************************************
  USER OP MACRO
*********************************************************************************************************/

#define FD_SET(n, p)        ((p)->fds_bits[(n) / NFDBITS] |= (ULONG)( (1ul << ((n) % NFDBITS))))
#define FD_CLR(n, p)        ((p)->fds_bits[(n) / NFDBITS] &= (ULONG)(~(1ul << ((n) % NFDBITS))))
#define FD_ISSET(n, p)      ((p)->fds_bits[(n) / NFDBITS] &  (ULONG)( (1ul << ((n) % NFDBITS))))
#define FD_ZERO(p)          lib_bzero((PVOID)(p), sizeof(*(p)))         /*  编译时兼容不同的 FD_SETSIZE */

/*********************************************************************************************************
  API 函数
*********************************************************************************************************/
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_SELECT_EN > 0)

LW_API INT     select(INT               iWidth, 
                      fd_set           *pfdsetRead,
                      fd_set           *pfdsetWrite,
                      fd_set           *pfdsetExcept,
                      struct timeval   *ptmvalTO);                      /*  BSD 标准 select()           */
					  
LW_API INT     pselect(INT                     iWidth, 
                       fd_set                 *pfdsetRead,
                       fd_set                 *pfdsetWrite,
                       fd_set                 *pfdsetExcept,
                       const struct timespec  *ptmspecTO,
                       const sigset_t         *sigsetMask);             /*  BSD 标准 pselect()          */

LW_API INT     waitread(INT  iFd, struct timeval   *ptmvalTO);          /*  等待单个文件可读            */

LW_API INT     waitwrite(INT  iFd, struct timeval   *ptmvalTO);         /*  等待单个文件可写            */

LW_API INT     waitexcept(INT  iFd, struct timeval   *ptmvalTO);        /*  等待单个文件可异常          */

#endif                                                                  /*  (LW_CFG_DEVICE_EN > 0) &&   */
                                                                        /*  (LW_CFG_SELECT_EN > 0)      */
                                                                        
/*********************************************************************************************************
  select struct
*********************************************************************************************************/

#ifdef __SYLIXOS_KERNEL
#include "selectType.h"
#endif                                                                  /*  __SYLIXOS_KERNEL            */

#endif                                                                  /*  __SELECT_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
