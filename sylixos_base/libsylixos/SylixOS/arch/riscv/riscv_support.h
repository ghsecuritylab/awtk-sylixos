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
** 文   件   名: riscv_support.h
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2018 年 03 月 20 日
**
** 描        述: RISC-V 体系构架支持.
*********************************************************************************************************/

#ifndef __ARCH_RISCV_SUPPORT_H
#define __ARCH_RISCV_SUPPORT_H

#define __LW_SCHEDULER_BUG_TRACE_EN                                     /*  测试多核调度器              */
#define __LW_KERNLOCK_BUG_TRACE_EN                                      /*  测试内核锁                  */

#if LW_CFG_SPINLOCK_RESTRICT_EN > 0
#define __LW_SPINLOCK_BUG_TRACE_EN                                      /*  测试自旋锁                  */
#endif

/*********************************************************************************************************
  汇编相关头文件
*********************************************************************************************************/

#include "arch/assembler.h"

/*********************************************************************************************************
  存储器定义 (CPU 栈区操作)
*********************************************************************************************************/

#define CPU_STK_GROWTH              1                                   /*  1：入栈从高地址向低地址     */
                                                                        /*  0：入栈从低地址向高地址     */
/*********************************************************************************************************
  arch 已经提供的接口如下:
*********************************************************************************************************/

#define __ARCH_KERNEL_PARAM         archKernelParam

VOID    archKernelParam(CPCHAR  pcParam);

/*********************************************************************************************************
  RISC-V 处理器断言
*********************************************************************************************************/

VOID    archAssert(INT  iCond, CPCHAR  pcFunc, CPCHAR  pcFile, INT  iLine);

/*********************************************************************************************************
  RISC-V 处理器线程上下文相关接口
*********************************************************************************************************/

PLW_STACK       archTaskCtxCreate(ARCH_REG_CTX          *pregctx,
                                  PTHREAD_START_ROUTINE  pfuncTask,
                                  PVOID                  pvArg,
                                  PLW_STACK              pstkTop, 
                                  ULONG                  ulOpt);
VOID            archTaskCtxSetFp(PLW_STACK               pstkDest,
                                 ARCH_REG_CTX           *pregctxDest,
                                 const ARCH_REG_CTX     *pregctxSrc);
ARCH_REG_CTX   *archTaskRegsGet(ARCH_REG_CTX  *pregctx, ARCH_REG_T  *pregSp);
VOID            archTaskRegsSet(ARCH_REG_CTX  *pregctxDest, const ARCH_REG_CTX  *pregctxSrc);

#if LW_CFG_DEVICE_EN > 0
VOID        archTaskCtxShow(INT  iFd, const ARCH_REG_CTX  *pregctx);
#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
VOID        archTaskCtxPrint(PVOID  pvBuffer, size_t  stSize, const ARCH_REG_CTX  *pregctx);

VOID        archTaskCtxStart(PLW_CLASS_CPU  pcpuSw);
VOID        archTaskCtxSwitch(PLW_CLASS_CPU  pcpuSw);

VOID        archTaskCtxCopy(ARCH_REG_CTX  *pregctxDest, const ARCH_REG_CTX  *pregctxSrc);

#if LW_CFG_COROUTINE_EN > 0
VOID        archCrtCtxSwitch(PLW_CLASS_CPU  pcpuSw);
#endif                                                                  /*  LW_CFG_COROUTINE_EN > 0     */

VOID        archIntCtxLoad(PLW_CLASS_CPU  pcpuSw);
VOID        archSigCtxLoad(const ARCH_REG_CTX  *pregctx);

VOID        archIntCtxSaveReg(PLW_CLASS_CPU  pcpu,
                              ARCH_REG_T     reg0,
                              ARCH_REG_T     reg1,
                              ARCH_REG_T     reg2,
                              ARCH_REG_T     reg3);

PLW_STACK   archCtxStackEnd(const ARCH_REG_CTX  *pregctx);

/*********************************************************************************************************
  RISC-V 处理器调试接口
*********************************************************************************************************/

#if LW_CFG_GDB_EN > 0
VOID    archDbgBpInsert(addr_t   ulAddr, size_t stSize, ULONG  *pulIns, BOOL  bLocal);
VOID    archDbgAbInsert(addr_t   ulAddr, ULONG  *pulIns);
VOID    archDbgBpRemove(addr_t   ulAddr, size_t stSize, ULONG   ulIns, BOOL  bLocal);
VOID    archDbgBpPrefetch(addr_t ulAddr);
UINT    archDbgTrapType(addr_t   ulAddr, PVOID   pvArch);
VOID    archDbgBpAdjust(PVOID  pvDtrace, PVOID   pvtm);
#endif                                                                  /*  LW_CFG_GDB_EN > 0           */

/*********************************************************************************************************
  RISC-V 处理器异常接口
*********************************************************************************************************/

VOID    archIntHandle(ULONG  ulVector, BOOL  bPreemptive);              /*  bspIntHandle 需要调用此函数 */

VOID    archWaitForInterrupt(VOID);

/*********************************************************************************************************
  RISC-V 通用库
*********************************************************************************************************/

INT     archFindLsb(UINT32 ui32);
INT     archFindMsb(UINT32 ui32);

/*********************************************************************************************************
  RISC-V 处理器标准底层库
*********************************************************************************************************/

#define KN_INT_DISABLE()            archIntDisable()
#define KN_INT_ENABLE(intLevel)     archIntEnable(intLevel)
#define KN_INT_ENABLE_FORCE()       archIntEnableForce()

INTREG  archIntDisable(VOID);
VOID    archIntEnable(INTREG  iregInterLevel);
VOID    archIntEnableForce(VOID);

VOID    archPageCopy(PVOID pvTo, PVOID pvFrom);

#define KN_COPY_PAGE(to, from)      archPageCopy(to, from)

VOID    archReboot(INT  iRebootType, addr_t  ulStartAddress);

/*********************************************************************************************************
  RISC-V 处理器 BogoMIPS 循环
*********************************************************************************************************/

VOID    archBogoMipsLoop(ULONG  ulLoop);

#define __ARCH_BOGOMIPS_LOOP            archBogoMipsLoop
#define __ARCH_BOGOMIPS_INS_PER_LOOP    2

/*********************************************************************************************************
  RISC-V 处理器 CACHE 操作
*********************************************************************************************************/

#define RISCV_MACHINE_GENERAL   "general"                               /*  General                     */

#if LW_CFG_CACHE_EN > 0
VOID    archCacheReset(CPCHAR     pcMachineName);
VOID    archCacheInit(CACHE_MODE  uiInstruction, CACHE_MODE  uiData, CPCHAR  pcMachineName);

#define __ARCH_CACHE_INIT   archCacheInit
#endif                                                                  /*  LW_CFG_CACHE_EN > 0         */

/*********************************************************************************************************
  RISC-V 处理器 MMU 操作 (riscvMmu*() 可供给 bsp 使用)
*********************************************************************************************************/

#if LW_CFG_VMM_EN > 0
VOID    archMmuInit(CPCHAR  pcMachineName);

#define __ARCH_MMU_INIT     archMmuInit
#endif                                                                  /*  LW_CFG_VMM_EN > 0           */

/*********************************************************************************************************
  RISC-V 处理器多核自旋锁操作
*********************************************************************************************************/

#if LW_CFG_SMP_EN > 0
VOID    archSpinInit(spinlock_t  *psl);
VOID    archSpinDelay(VOID);
VOID    archSpinNotify(VOID);

#define __ARCH_SPIN_INIT    archSpinInit
#define __ARCH_SPIN_DELAY   archSpinDelay
#define __ARCH_SPIN_NOTIFY  archSpinNotify

INT     archSpinLock(spinlock_t  *psl, VOIDFUNCPTR  pfuncPoll, PVOID  pvArg);
INT     archSpinTryLock(spinlock_t  *psl);
INT     archSpinUnlock(spinlock_t  *psl);

#define __ARCH_SPIN_LOCK    archSpinLock
#define __ARCH_SPIN_TRYLOCK archSpinTryLock
#define __ARCH_SPIN_UNLOCK  archSpinUnlock

INT     archSpinLockRaw(spinlock_t  *psl);
INT     archSpinTryLockRaw(spinlock_t  *psl);
INT     archSpinUnlockRaw(spinlock_t  *psl);

#define __ARCH_SPIN_LOCK_RAW    archSpinLockRaw
#define __ARCH_SPIN_TRYLOCK_RAW archSpinTryLockRaw
#define __ARCH_SPIN_UNLOCK_RAW  archSpinUnlockRaw

ULONG   archMpCur(VOID);
VOID    archMpInt(ULONG  ulCPUId);
#endif                                                                  /*  LW_CFG_SMP_EN               */

/*********************************************************************************************************
  RISC-V 内存屏障
*********************************************************************************************************/

#define KN_BARRIER()            __asm__ __volatile__ ("" : : : "memory")
#define KN_FENCE(p, s)          __asm__ __volatile__ ("fence " #p "," #s : : : "memory")

#define KN_MB()                 KN_FENCE(iorw, iorw)
#define KN_RMB()                KN_FENCE(ir, ir)
#define KN_WMB()                KN_FENCE(ow, ow)

#if LW_CFG_SMP_EN > 0
#define KN_SMP_MB()             KN_FENCE(rw, rw)
#define KN_SMP_RMB()            KN_FENCE(r, r)
#define KN_SMP_WMB()            KN_FENCE(w, w)
#else
#define KN_SMP_MB()             KN_BARRIER()
#define KN_SMP_RMB()            KN_BARRIER()
#define KN_SMP_WMB()            KN_BARRIER()
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */

/*********************************************************************************************************
  RISC-V 处理器浮点运算器
*********************************************************************************************************/

#define RISCV_FPU_NONE      "none"                                      /*  None                        */
#define RISCV_FPU_SP        "sp"                                        /*  Single-Precision VFP        */
#define RISCV_FPU_DP        "dp"                                        /*  Double-Precision VFP        */
#define RISCV_FPU_QP        "qp"                                        /*  Quad-Precision   VFP        */

#if LW_CFG_CPU_FPU_EN > 0
VOID    archFpuPrimaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);
#if LW_CFG_SMP_EN > 0
VOID    archFpuSecondaryInit(CPCHAR  pcMachineName, CPCHAR  pcFpuName);
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */

VOID    archFpuCtxInit(PVOID pvFpuCtx);
VOID    archFpuEnable(VOID);
VOID    archFpuDisable(VOID);
VOID    archFpuSave(PVOID pvFpuCtx);
VOID    archFpuRestore(PVOID pvFpuCtx);

#define __ARCH_FPU_CTX_INIT     archFpuCtxInit
#define __ARCH_FPU_ENABLE       archFpuEnable
#define __ARCH_FPU_DISABLE      archFpuDisable
#define __ARCH_FPU_SAVE         archFpuSave
#define __ARCH_FPU_RESTORE      archFpuRestore

#if LW_CFG_DEVICE_EN > 0
VOID    archFpuCtxShow(INT  iFd, PVOID pvFpuCtx);
#define __ARCH_FPU_CTX_SHOW     archFpuCtxShow
#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */

INT     archFpuUndHandle(PLW_CLASS_TCB  ptcbCur);
#endif                                                                  /*  LW_CFG_CPU_FPU_EN > 0       */

/*********************************************************************************************************
  bsp 需要提供的接口如下:
*********************************************************************************************************/
/*********************************************************************************************************
  RISC-V 处理器异常回调 (可不定义)
*********************************************************************************************************/

#if LW_CFG_CPU_EXC_HOOK_EN > 0
#define ARCH_INSTRUCTION_EXCEPTION  0                                   /*  iExcType                    */
#define ARCH_DATA_EXCEPTION         1
#define ARCH_MACHINE_EXCEPTION      2
#define ARCH_BUS_EXCEPTION          3

INT     bspCpuExcHook(PLW_CLASS_TCB   ptcb,
                      addr_t          ulRetAddr,
                      addr_t          ulExcAddr,
                      INT             iExcType, 
                      INT             iExcInfo);
#endif                                                                  /*  LW_CFG_CPU_EXC_HOOK_EN      */

/*********************************************************************************************************
  RISC-V 处理器中断向量判读
*********************************************************************************************************/

VOID    bspIntInit(VOID);
VOID    bspIntHandle(VOID);

VOID    bspIntVectorEnable(ULONG  ulVector);
VOID    bspIntVectorDisable(ULONG  ulVector);
BOOL    bspIntVectorIsEnable(ULONG  ulVector);

#define __ARCH_INT_VECTOR_ENABLE    bspIntVectorEnable
#define __ARCH_INT_VECTOR_DISABLE   bspIntVectorDisable
#define __ARCH_INT_VECTOR_ISENABLE  bspIntVectorIsEnable

#if LW_CFG_INTER_PRIO > 0
ULONG   bspIntVectorSetPriority(ULONG  ulVector, UINT  uiPrio);
ULONG   bspIntVectorGetPriority(ULONG  ulVector, UINT *puiPrio);

#define __ARCH_INT_VECTOR_SETPRIO   bspIntVectorSetPriority
#define __ARCH_INT_VECTOR_GETPRIO   bspIntVectorGetPriority
#endif                                                                  /*  LW_CFG_INTER_PRIO > 0       */

#if LW_CFG_INTER_TARGET > 0
ULONG   bspIntVectorSetTarget(ULONG  ulVector, size_t  stSize, const PLW_CLASS_CPUSET  pcpuset);
ULONG   bspIntVectorGetTarget(ULONG  ulVector, size_t  stSize, PLW_CLASS_CPUSET  pcpuset);

#define __ARCH_INT_VECTOR_SETTARGET bspIntVectorSetTarget
#define __ARCH_INT_VECTOR_GETTARGET bspIntVectorGetTarget
#endif                                                                  /*  LW_CFG_INTER_TARGET > 0     */

/*********************************************************************************************************
  CPU 定时器时钟
*********************************************************************************************************/

VOID    bspTickInit(VOID);
VOID    bspDelayUs(ULONG ulUs);
VOID    bspDelayNs(ULONG ulNs);

/*********************************************************************************************************
  TICK 高精度时间修正
  如未实现则可定义为空函数, 同时系统初始化前需调用 API_TimeNanoSleepMethod(LW_TIME_NANOSLEEP_METHOD_TICK)
*********************************************************************************************************/

#if LW_CFG_TIME_HIGH_RESOLUTION_EN > 0
VOID    bspTickHighResolution(struct timespec *ptv);
#endif                                                                  /*  LW_CFG_TIME_HIGH_...        */

/*********************************************************************************************************
  内核关键位置回调函数
*********************************************************************************************************/

VOID    bspTaskCreateHook(LW_OBJECT_ID  ulId);
VOID    bspTaskDeleteHook(LW_OBJECT_ID  ulId, PVOID  pvReturnVal, PLW_CLASS_TCB  ptcb);
VOID    bspTaskSwapHook(LW_OBJECT_HANDLE  hOldThread, LW_OBJECT_HANDLE  hNewThread);
VOID    bspTaskIdleHook(VOID);
VOID    bspTickHook(INT64  i64Tick);
VOID    bspWdTimerHook(LW_OBJECT_ID  ulId);
VOID    bspTCBInitHook(LW_OBJECT_ID  ulId, PLW_CLASS_TCB   ptcb);
VOID    bspKernelInitHook(VOID);

/*********************************************************************************************************
  系统重启操作 (ulStartAddress 参数用于调试, BSP 可忽略)
*********************************************************************************************************/

VOID    bspReboot(INT  iRebootType, addr_t  ulStartAddress);

/*********************************************************************************************************
  系统关键信息打印 (打印信息不可依赖任何操作系统 api)
*********************************************************************************************************/

VOID    bspDebugMsg(CPCHAR pcMsg);

/*********************************************************************************************************
  BSP 信息
*********************************************************************************************************/

CPCHAR  bspInfoCpu(VOID);
CPCHAR  bspInfoCache(VOID);
CPCHAR  bspInfoPacket(VOID);
CPCHAR  bspInfoVersion(VOID);
ULONG   bspInfoHwcap(VOID);
addr_t  bspInfoRomBase(VOID);
size_t  bspInfoRomSize(VOID);
addr_t  bspInfoRamBase(VOID);
size_t  bspInfoRamSize(VOID);

/*********************************************************************************************************
  RISC-V 处理器 CACHE 操作
*********************************************************************************************************/

/*********************************************************************************************************
  RISC-V 处理器 MMU 操作
*********************************************************************************************************/

#if LW_CFG_VMM_EN > 0
ULONG   bspMmuPgdMaxNum(VOID);
#if LW_CFG_CPU_WORD_LENGHT == 64
ULONG   bspMmuPmdMaxNum(VOID);
ULONG   bspMmuPtsMaxNum(VOID);
#endif                                                                  /*  LW_CFG_CPU_WORD_LENGHT == 64*/
ULONG   bspMmuPteMaxNum(VOID);
#endif                                                                  /*  LW_CFG_VMM_EN > 0           */

/*********************************************************************************************************
  RISC-V 处理器多核操作
*********************************************************************************************************/

#if LW_CFG_SMP_EN > 0
VOID    bspMpInt(ULONG  ulCPUId);
VOID    bspCpuUp(ULONG  ulCPUId);                                       /*  启动一个 CPU                */

#if LW_CFG_SMP_CPU_DOWN_EN > 0
VOID    bspCpuDown(ULONG  ulCPUId);                                     /*  停止一个 CPU                */
#endif                                                                  /*  LW_CFG_SMP_CPU_DOWN_EN > 0  */

#if LW_CFG_CPU_ARCH_SMT > 0
ULONG   bspCpuLogic2Physical(ULONG  ulCPUId);
#endif
#endif                                                                  /*  LW_CFG_SMP_EN > 0           */

extern  ULONG  _G_ulRiscvCpuIdToHartId[];
#define RISCV_CPUID_TO_HARTID(cpuid)  _G_ulRiscvCpuIdToHartId[cpuid]

/*********************************************************************************************************
  RISC-V 处理器 CPU 功耗 (升降主频)
*********************************************************************************************************/

#if LW_CFG_POWERM_EN > 0
VOID    bspSuspend(VOID);                                               /*  系统休眠                    */
VOID    bspCpuPowerSet(UINT  uiPowerLevel);                             /*  设置 CPU 主频等级           */
VOID    bspCpuPowerGet(UINT *puiPowerLevel);                            /*  获取 CPU 主频等级           */
#endif                                                                  /*  LW_CFG_POWERM_EN > 0        */

/*********************************************************************************************************
  Trusted computing
*********************************************************************************************************/

#if LW_CFG_TRUSTED_COMPUTING_EN > 0
VOID    bspTrustedModuleInit(VOID);
INT     bspTrustedModuleCheck(const PCHAR  pcPath);
VOID    bspTrustedModuleLoad(PVOID  pvModule);
VOID    bspTrustedModuleUnload(PVOID  pvModule);
#endif                                                                  /*  LW_CFG_TRUSTED_COMPUTING_EN */

#endif                                                                  /*  __ARCH_RISCV_SUPPORT_H      */
/*********************************************************************************************************
  END
*********************************************************************************************************/
