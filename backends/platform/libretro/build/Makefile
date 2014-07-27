##LIBRETRO
DEBUG=0

ifeq ($(platform),)
platform = unix
ifeq ($(shell uname -a),)
   platform = win
else ifneq ($(findstring MINGW,$(shell uname -a)),)
   platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
   platform = osx
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
endif
else ifneq ($(findstring win,$(shell uname -a)),)
   platform = win
endif
endif

ifeq ($(shell uname -a),)
   EXE_EXT = .exe
endif

TARGET_NAME := scummvm

LD        = $(CXX)
AR        = ar cru
RANLIB    = ranlib

FS_TYPE := posix

ifeq ($(platform), unix)
   TARGET  := $(TARGET_NAME)_libretro.so
   DEFINES += -fPIC
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
else ifeq ($(platform), osx)
   TARGET  := $(TARGET_NAME)_libretro.dylib
   DEFINES += -fPIC
   LDFLAGS += -dynamiclib -fPIC
	arch = intel
ifeq ($(shell uname -p),powerpc)
	arch = ppc
endif
else ifeq ($(platform), ios)
   TARGET  := $(TARGET_NAME)_libretro_ios.dylib
   DEFINES += -fPIC -DHAVE_POSIX_MEMALIGN=1 -DIOS
   LDFLAGS += -dynamiclib -fPIC
   CXX      = clang++ -arch armv7 -isysroot $(IOSSDK) -marm
else ifeq ($(platform), qnx)
   TARGET  := $(TARGET_NAME)_libretro_qnx.so
   DEFINES += -fPIC -DSYSTEM_NOT_SUPPORTING_D_TYPE
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   CC = qcc -Vgcc_ntoarmv7le
   CXX = QCC -Vgcc_ntoarmv7le
   LD = QCC -Vgcc_ntoarmv7le
   AR = qcc -Vgcc_ntoarmv7le -A
   RANLIB="${QNX_HOST}/usr/bin/ntoarmv7-ranlib"
else ifeq ($(platform), ps3)
   TARGET  := $(TARGET_NAME)_libretro_ps3.a
   CC = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-gcc.exe
   CXX = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-g++.exe
   LD = ./fake-link.sh $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe
   AR = $(CELL_SDK)/host-win32/ppu/bin/ppu-lv2-ar.exe cru
	FS_TYPE := posix
   DEFINES += -DPLAYSTATION3
else ifeq ($(platform), wii)
   TARGET := $(TARGET_NAME)_libretro_wii.a
   CC = $(DEVKITPPC)/bin/powerpc-eabi-gcc$(EXE_EXT)
   CXX = $(DEVKITPPC)/bin/powerpc-eabi-g++$(EXE_EXT)
   LD = ./fake-link.sh $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT)
   AR = $(DEVKITPPC)/bin/powerpc-eabi-ar$(EXE_EXT) cru
   DEFINES += -DGEKKO -DHW_RVL -mrvl -mcpu=750 -meabi -mhard-float -D__ppc__ -I$(DEVKITPRO)/libogc/include
else ifeq ($(platform), android)
   TARGET  := $(TARGET_NAME)_libretro_android_arm.so
   DEFINES += -fPIC -Wno-multichar
   LDFLAGS += -shared -Wl,--version-script=../link.T -fPIC
   TOOLSET = arm-linux-androideabi-
else ifneq (,$(findstring armv,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.so
   SHARED := -shared -Wl,--no-undefined
   DEFINES += -fPIC -Wno-multichar
   CC = gcc
ifneq (,$(findstring cortexa8,$(platform)))
   DEFINES += -marm -mcpu=cortex-a8
else ifneq (,$(findstring cortexa9,$(platform)))
   DEFINES += -marm -mcpu=cortex-a9
endif
   DEFINES += -marm
ifneq (,$(findstring neon,$(platform)))
   DEFINES += -mfpu=neon
   HAVE_NEON = 1
endif
ifneq (,$(findstring softfloat,$(platform)))
   DEFINES += -mfloat-abi=softfp
else ifneq (,$(findstring hardfloat,$(platform)))
   DEFINES += -mfloat-abi=hard
endif
   DEFINES += -DARM
else
	CC = gcc
   TARGET  := $(TARGET_NAME)_libretro.dll
   LDFLAGS += -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=../link.T
endif

ifeq ($(DEBUG), 1)
   DEFINES += -O0 -g
else
   DEFINES += -O3
endif

###SCUMM VM
srcdir = ../../../..
retrodir = backends/platform/libretro/
libcodir = $(retrodir)libco/
VPATH = $(srcdir)

# Define toolset
ifdef TOOLSET
    CC        = $(TOOLSET)gcc
    CXX       = $(TOOLSET)g++
    LD        = $(TOOLSET)g++
    AR        = $(TOOLSET)ar cru
    RANLIB    = $(TOOLSET)ranlib
endif

MKDIR         = mkdir -p
RM            = rm -f
RM_REC        = rm -rf

# Define build flags
DEFINES       += -D__LIBRETRO__ -DNONSTANDARD_PORT -DUSE_RGB_COLOR -DUSE_OSD -DDISABLE_COMMAND_LINE -DDISABLE_TEXT_CONSOLE -DFRONTEND_SUPPORTS_RGB565 -Wno-multichar
INCLUDES      += -I$(srcdir)/$(retrodir) -I. -I$(srcdir) -I$(srcdir)/engines
EXECUTABLE    = $(TARGET)
DEPDIR        = .deps
HAVE_GCC3     = true
USE_RGB_COLOR = true

# Define enabled modules
ENABLED=STATIC_PLUGIN
ENABLE_SCUMM = $(ENABLED)
ENABLE_SCUMM_7_8 = $(ENABLED)
ENABLE_SKY = $(ENABLED)
ENABLE_CINE = $(ENABLED)
ENABLE_SWORD1 = $(ENABLED)
ENABLE_SWORD2 = $(ENABLED)
#ENABLE_SWORD25 = $(ENABLED)
ENABLE_AGOS = $(ENABLED)
ENABLE_AGOS2 = $(ENABLED)
ENABLE_KYRA = $(ENABLED)
ENABLE_AGI = $(ENABLED)
ENABLE_SCI = $(ENABLED)
ENABLE_MADE = $(ENABLED)
ENABLE_EOB = $(ENABLED)
ENABLE_LOL = $(ENABLED)
ENABLE_LURE = $(ENABLED)
ENABLE_GOB = $(ENABLED)
ENABLE_TOON = $(ENABLED)
ENABLE_MOHAWK = $(ENABLED)
ENABLE_MYST = $(ENABLED)
ENABLE_CRUISE = $(ENABLED)
ENABLE_BBVS = $(ENABLED)
ENABLE_RIVEN = $(ENABLED)
ENABLE_LASTEXPRESS = $(ENABLED)
ENABLE_GROOVIE = $(ENABLED)
ENABLE_GROOVIE2 = $(ENABLED)
ENABLE_TINSEL = $(ENABLED)
ENABLE_VOYEUR = $(ENABLED)
ENABLE_NEVERHOOD = $(ENABLED)
ENABLE_QUEEN = $(ENABLED)
ENABLE_SAGA = $(ENABLED)
ENABLE_SAGA2 = $(ENABLED)
ENABLE_IHNM = $(ENABLED)
ENABLE_TOUCHE = $(ENABLED)
#ENABLE_ZVISION = $(ENABLED)
ENABLE_TSAGE = $(ENABLED)
ENABLE_DREAMWEB=$(ENABLED)

#
OBJS :=	$(retrodir)libretro.o \
        $(retrodir)os.o
OBJS += $(libcodir)libco.o

ifeq ($(FS_TYPE), posix)
OBJS += backends/fs/posix/posix-fs.o \
        backends/fs/posix/posix-fs-factory.o
DEFINES += -DFS_TYPE_POSIX
else ifeq ($(FS_TYPE), ps3)

OBJS += $(retrodir)ps3-fs.o \
        $(retrodir)ps3-filestream.o \
        $(retrodir)ps3-fs-factory.o

DEFINES += -DFS_TYPE_PS3
endif

ifeq ($(platform), win)
OBJS += $(retrodir)dirent_win32.o
endif

MODULE_DIRS += $(retrodir) $(retrodir)libco

BACKEND := libretro

include $(srcdir)/Makefile.common.libretro