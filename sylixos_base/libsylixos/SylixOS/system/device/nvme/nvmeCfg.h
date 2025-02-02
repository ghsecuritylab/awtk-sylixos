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
** 文   件   名: nvmeCfg.h
**
** 创   建   人: Qin.Fei (秦飞)
**
** 文件创建日期: 2017 年 7 月 17 日
**
** 描        述: NVMe 驱动配置.
*********************************************************************************************************/

#ifndef __NVME_CFG_H
#define __NVME_CFG_H

/*********************************************************************************************************
  裁剪宏
*********************************************************************************************************/
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_NVME_EN > 0)
/*********************************************************************************************************
  驱动版本 (0x01000101 为 v1.0.1)
*********************************************************************************************************/
#define NVME_CTRL_DRV_VER_NUM               0x01000101                  /* 驱动版本数值                 */
/*********************************************************************************************************
  驱动参数
*********************************************************************************************************/
#define NVME_CTRL_NAME_MAX                  (32 + 1)                    /* 类型名称最大值               */
#define NVME_DRV_NAME_MAX                   (32 + 1)                    /* 驱动名称最大值               */
#define NVME_DEV_NAME_MAX                   (32 + 1)                    /* 设备名称最大值               */
#define NVME_CTRL_IRQ_NAME_MAX              (32 + 1)                    /* 中断名称最大值               */
#define NVME_NAME                           "nvme"
#define NVME_MEDIA_NAME                     "/media/nvme"
#define NVME_CMD_DEPTH_MAX                  16                          /*  每个队列的命令深度          */
#define NVME_SQ_SIZE(cmd_depth)             (cmd_depth * sizeof(NVME_COMMAND_CB))
#define NVME_CQ_SIZE(cmd_depth)             (cmd_depth * sizeof(NVME_COMPLETION_CB))
/*********************************************************************************************************
  TRIM 操作
*********************************************************************************************************/
#define NVME_TRIM_EN                        1                           /* 是否使能 TRIM 操作           */
/*********************************************************************************************************
  FLUSH 操作
*********************************************************************************************************/
#define NVME_CACHE_EN                       1                           /* 是否使能 CACHE 回写操作      */
/*********************************************************************************************************
  命令超时参数
*********************************************************************************************************/
#define NVME_ADMIN_TIMEOUT                  (10 * LW_TICK_HZ)           /* Admin 命令超时时间           */
#define NVME_IO_TIMEOUT                     ( 5 * LW_TICK_HZ)           /* I/O 命令超时时间             */
/*********************************************************************************************************
  容量参数
*********************************************************************************************************/
#define NVME_KB                             LW_CFG_KB_SIZE
#define NVME_MB                             (NVME_KB * NVME_KB)
/*********************************************************************************************************
  磁盘 CACHE 参数
*********************************************************************************************************/
#define NVME_CACHE_PL                       4                           /* 并发线程数量                 */
#define NVME_CACHE_BURST_RD                 64                          /* 读猝发参数                   */
#define NVME_CACHE_BURST_WR                 64                          /* 写猝发参数                   */
#define NVME_DRIVE_DISKCACHE_MSG_COUNT      16                          /* 磁盘缓冲消息数量             */
#define NVME_DRIVE_DISKCACHE_PARALLEL_EN    LW_TRUE                     /* 磁盘缓冲是否使能并行操作     */
#define NVME_CACHE_SIZE                     (LW_CFG_NVME_CACHE_SIZE_MB * NVME_MB)
                                                                        /* 磁盘缓存大小                 */
/*********************************************************************************************************
  控制器参数
*********************************************************************************************************/
#define NVME_CTRL_MAX                       6                           /* 控制器数量最大值             */
#define NVME_DRIVE_MAX                      32                          /* 驱动器数量最大值             */
/*********************************************************************************************************
  超时时间参数
*********************************************************************************************************/
#define NVME_ID_DELAYED_RECOVERY            0                           /* 暂时不使用延迟回收, 必须为 0 */
#define NVME_SEM_TIMEOUT_DEF                (5 * LW_TICK_HZ)            /* 同步信号超时时间             */

#endif                                                                  /*  (LW_CFG_DEVICE_EN > 0) &&   */
                                                                        /*  (LW_CFG_NVME_EN > 0)        */
#endif                                                                  /*  __NVME_CFG_H                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
