# NMake Makefile for building a DLL with MSVC 6.0 or MSVC 2005
# Usage:#	nmake /f Makefile.msvc ARCH=x86 BUILD=release
#			nmake /f Makefile.msvc ARCH=x64 BUILD=release
#
#			"nmake /f Makefile.msvc" defaults to 
#			"nmake /f Makefile.msvc ARCH=x86 BUILD=debug"

# Project name
TARGET = descript.ion-shellex
TARGET_DLL = $(TARGET).dll
TARGET_PDB = $(TARGET).pdb
INSTALLER_DIR = installer^\


# Compiler and linker
CC = cl
LINK = link
RC = rc
MV = move

!IF "$(ARCH)" == "x64"
MACHINE = AMD64
ARCH_DEFINE = _WIN64
EXPORT_NAME = DllGetClassObject
EXPORT_NAME_INSTALL = install
EXPORT_NAME_INSTALLASADMIN = installAsAdmin
EXPORT_NAME_UNINSTALL = uninstall
EXPORT_NAME_UNINSTALLASADMIN = uninstallAsAdmin
NEEDS_MANIFEST = 1
ARCH_DEPENDENT_CFLAGS = /EHsc
ARCH_DEPENDENT_LDFLAGS = /manifest
MT = mt
MANIFEST = $(TARGET_DLL).manifest
NAME_IN_INSTALLER_DIR = $(TARGET)64.dll

!ELSE
MACHINE = I386
ARCH_DEFINE = WIN32
EXPORT_NAME = _DllGetClassObject@12
EXPORT_NAME_INSTALL = _install@16
EXPORT_NAME_INSTALLASADMIN = _installAsAdmin@16
EXPORT_NAME_UNINSTALL = _uninstall@16
EXPORT_NAME_UNINSTALLASADMIN = _uninstallAsAdmin@16
NEEDS_MANIFEST = 0
ARCH_DEPENDENT_CFLAGS = /GX
ARCH_DEPENDENT_LDFLAGS =
NAME_IN_INSTALLER_DIR = $(TARGET)32.dll

!ENDIF

# Release select
!IF "$(BUILD)" == "release"
RLS_CFLAGS = /O2 /MT /D "NDEBUG"
RLS_LDFLAGS = /OPT:REF /OPT:ICF
!ELSE
RLS_CFLAGS = /Od /MDd /D "_DEBUG" /Zi
RLS_LDFLAGS = /DEBUG /PDB:$(TARGET_PDB)
!ENDIF

# Compiler flags
CFLAGS = /nologo /W3 $(RLS_CFLAGS) /D "$(ARCH_DEFINE)" /D "_UNICODE" /D "UNICODE" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /FD /c $(ARCH_DEPENDENT_CFLAGS)

# Resource compiler flags
RCFLAGS = /l 0x409 /d "NDEBUG"

# Linker flags
LDFLAGS = /nologo /dll /incremental:no \
	/export:DllGetClassObject=$(EXPORT_NAME) \
	/export:install=$(EXPORT_NAME_INSTALL) \
	/export:installAsAdmin=$(EXPORT_NAME_INSTALLASADMIN) \
	/export:uninstall=$(EXPORT_NAME_UNINSTALL) \
	/export:uninstallAsAdmin=$(EXPORT_NAME_UNINSTALLASADMIN) \
	/machine:$(MACHINE) /out:$(TARGET_DLL) $(RLS_LDFLAGS) $(ARCH_DEPENDENT_LDFLAGS)

# Libraries
LIBS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib

# Object files
OBJS = \
	bom_cp_eol_utils.obj \
	dbg.obj \
	description_handler.obj \
	errors_and_settings.obj \
	property_sheet.obj \
	shellext_init.obj \
	context_menu.obj \
	description_file_rw.obj \
	dll_main.obj \
	file_io_wrapper.obj \
	column_provider.obj \
	reg_helper.obj

# Resource object file
RES_OBJ = resource.res

# Default target
all: $(TARGET_DLL)

# Link DLL
!IF "$(NEEDS_MANIFEST)" == "1"
$(TARGET_DLL): $(OBJS) $(RES_OBJ)
	$(LINK) $(LDFLAGS) $(OBJS) $(RES_OBJ) $(LIBS)
	$(MT) -manifest "$(MANIFEST)" -outputresource:"$(TARGET_DLL);#2"
!ELSE
$(TARGET_DLL): $(OBJS) $(RES_OBJ)
	$(LINK) $(LDFLAGS) $(OBJS) $(RES_OBJ) $(LIBS)
!ENDIF
!IF "$(BUILD)" == "release"
	$(MV) $(TARGET_DLL) $(INSTALLER_DIR)$(NAME_IN_INSTALLER_DIR)
!ENDIF

# Suffix rule for .cpp -> .obj
.SUFFIXES: .cpp .obj
.cpp.obj:
	$(CC) $(CFLAGS) $<

# Compile resources
.rc.res:
	$(RC) $(RCFLAGS) /fo $@ $<

# Clean up
clean:
	-del /q $(OBJS) $(RES_OBJ) $(TARGET_DLL) $(TARGET_DLL).manifest *.ilk *.pdb 2>nul
