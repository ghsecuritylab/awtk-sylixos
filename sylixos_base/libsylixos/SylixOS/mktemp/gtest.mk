#*********************************************************************************************************
#
#                                    中国软件开源组织
#
#                                   嵌入式实时操作系统
#
#                                SylixOS(TM)  LW : long wing
#
#                               Copyright All Rights Reserved
#
#--------------文件信息--------------------------------------------------------------------------------
#
# 文   件   名: gtest.mk
#
# 创   建   人: Jiao.JinXing(焦进星)
#
# 文件创建日期: 2016 年 08 月 24 日
#
# 描        述: gtest 应用程序类目标 makefile 模板
#*********************************************************************************************************

#*********************************************************************************************************
# Include common.mk
#*********************************************************************************************************
include $(MKTEMP)/common.mk

#*********************************************************************************************************
# Depend and compiler parameter (cplusplus in kernel MUST NOT use exceptions and rtti)
#*********************************************************************************************************
ifeq ($($(target)_USE_CXX_EXCEPT), yes)
$(target)_CXX_EXCEPT  := $(TOOLCHAIN_CXX_EXCEPT_CFLAGS)
else
$(target)_CXX_EXCEPT  := $(TOOLCHAIN_NO_CXX_EXCEPT_CFLAGS)
endif

ifeq ($($(target)_USE_GCOV), yes)
$(target)_GCOV_FLAGS  := $(TOOLCHAIN_GCOV_CFLAGS)
else
$(target)_GCOV_FLAGS  := $(TOOLCHAIN_NO_GCOV_CFLAGS)
endif

ifeq ($($(target)_USE_OMP), yes)
$(target)_OMP_FLAGS   := $(TOOLCHAIN_OMP_CFLAGS)
else
$(target)_OMP_FLAGS   := $(TOOLCHAIN_NO_OMP_CFLAGS)
endif

$(target)_CPUFLAGS    := $(ARCH_CPUFLAGS)
$(target)_COMMONFLAGS := $($(target)_CPUFLAGS) $(ARCH_COMMONFLAGS) $(TOOLCHAIN_OPTIMIZE) $(TOOLCHAIN_COMMONFLAGS) $($(target)_GCOV_FLAGS) $($(target)_OMP_FLAGS)
$(target)_ASFLAGS     := $($(target)_COMMONFLAGS) $(ARCH_PIC_ASFLAGS) $(TOOLCHAIN_ASFLAGS) $($(target)_DSYMBOL) $($(target)_INC_PATH)
$(target)_CFLAGS      := $($(target)_COMMONFLAGS) $(ARCH_PIC_CFLAGS) $($(target)_DSYMBOL) $($(target)_INC_PATH) $($(target)_CFLAGS)
$(target)_CXXFLAGS    := $($(target)_COMMONFLAGS) $(ARCH_PIC_CFLAGS) $($(target)_DSYMBOL) $($(target)_INC_PATH) $($(target)_CXX_EXCEPT) $($(target)_CXXFLAGS)

#*********************************************************************************************************
# Targets
#*********************************************************************************************************
$(target)_EXE       := $(OUTPATH)/$(LOCAL_TARGET_NAME)
$(target)_STRIP_EXE := $(OUTPATH)/strip/$(LOCAL_TARGET_NAME)

#*********************************************************************************************************
# Depend base library search paths (Select the user specified optimization level library first)
#*********************************************************************************************************
$(target)_DEPEND_LIB_PATH := $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libsylixos/$(OUTDIR)"

ifeq ($(DEBUG_LEVEL), debug)
$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libsylixos/Release"
else
$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libsylixos/Debug"
endif

ifeq ($($(target)_USE_CXX), yes)
$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libcextern/$(OUTDIR)"

ifeq ($(DEBUG_LEVEL), debug)
$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libcextern/Release"
else
$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libcextern/Debug"
endif
endif

$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libgtest/$(OUTDIR)"

ifeq ($(DEBUG_LEVEL), debug)
$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libgtest/Release"
else
$(target)_DEPEND_LIB_PATH += $(TOOLCHAIN_LIB_INC)"$(SYLIXOS_BASE_PATH)/libgtest/Debug"
endif

#*********************************************************************************************************
# Depend user library search paths
#*********************************************************************************************************
$(target)_DEPEND_LIB_PATH += $(LOCAL_DEPEND_LIB_PATH)

#*********************************************************************************************************
# Depend libraries
#*********************************************************************************************************
$(target)_DEPEND_LIB := $(LOCAL_DEPEND_LIB)

$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_GTEST)

$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_VPMPDM)

ifeq ($($(target)_USE_GCOV), yes)
$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_PIC_GCOV)
endif

ifeq ($($(target)_USE_OMP), yes)
$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_PIC_OMP)
endif

ifeq ($($(target)_USE_CXX), yes)
$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_CEXTERN) $(TOOLCHAIN_LINK_PIC_CXX) $(TOOLCHAIN_LINK_DSOHANDLE)
endif

$(target)_DEPEND_LIB += $(TOOLCHAIN_LINK_PIC_M) $(TOOLCHAIN_LINK_PIC_GCC)

#*********************************************************************************************************
# Link object files
#*********************************************************************************************************
ifeq ($(ARCH), c6x)
$($(target)_EXE): $($(target)_OBJS) $($(target)_DEPEND_TARGET)
		@rm -f $@
		$(__PRE_LINK_CMD)
		$(__LD) $(__CPUFLAGS) $(ARCH_PIC_LDFLAGS) $(__LINKFLAGS) $(__OBJS) $(__LIBRARIES) -o $@
		@mv $@ $@.c6x
		@nm $@.c6x > $@_nm.txt
		@$(DIS) $(TOOLCHAIN_DIS_FLAGS) $@.c6x > $@_dis.txt
		@$(LINK) $@_nm.txt $@_dis.txt $@.c6x
		@mv $@.c6x $@
		@rm -f $@_nm.txt $@_dis.txt
		$(__POST_LINK_CMD)
else
$($(target)_EXE): $($(target)_OBJS) $($(target)_DEPEND_TARGET)
		@rm -f $@
		$(__PRE_LINK_CMD)
		$(__LD) $(__CPUFLAGS) $(ARCH_PIC_LDFLAGS) $(__LINKFLAGS) $(__OBJS) $(__LIBRARIES) -o $@
		$(__POST_LINK_CMD)
endif

#*********************************************************************************************************
# Strip target
#*********************************************************************************************************
$($(target)_STRIP_EXE): $($(target)_EXE)
		@if [ ! -d "$(dir $@)" ]; then mkdir -p "$(dir $@)"; fi
		@rm -f $@
		$(__PRE_STRIP_CMD)
		$(STRIP) $(TOOLCHAIN_STRIP_FLAGS) $< -o $@
		$(__POST_STRIP_CMD)

#*********************************************************************************************************
# Add targets
#*********************************************************************************************************
TARGETS := $(TARGETS) $($(target)_EXE) $($(target)_STRIP_EXE)

#*********************************************************************************************************
# End
#*********************************************************************************************************
