# Makefile by S739_Lucie - January 18th 2023
# ====== Everything Command Calls related ======

MAKEFLAGS += --no-print-directory

USED_OS := ERROR
P_SEP   := /
CLS     := clear

ifdef OS
	USED_OS := Windows
endif

ifeq ($(shell uname), Linux)
	USED_OS := Linux
endif

# ========= Everything project related =========

PROJ    := LuASM
TARGET  := $(PROJ).out
DTARGET := $(PROJ)_debug.out
EXT     := c
COMP    := gcc

ifeq ($(USED_OS), Windows)
	TARGET  := $(PROJ).exe
	DTARGET := $(PROJ)_debug.exe
endif

# ========== Everything files related ==========

INC_DIR   := include
SRC_DIR   := src
OBJ_DIR   := objs

HDS_FILES := $(wildcard $(INC_DIR)/*.h) $(wildcard $(INC_DIR)/**/*.h) 
GCH_FILES := $(addsuffix .gch,$(HDS_FILES))
SRC_FILES := $(wildcard $(SRC_DIR)/*.$(EXT)) $(wildcard $(SRC_DIR)/**/*.$(EXT)) 
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.$(EXT)=$(OBJ_DIR)/$(USED_OS)/%.o)

# ========== Everything flags related ==========

INC_PATHS := $(addprefix -I , $(sort $(dir $(HDS_FILES))))
LIB_PATHS := -L libs/$(USED_OS)
LIB_FLAGS := 
APP_FLAGS := -std=c2x -Wall -Wextra -Werror -Wfatal-errors -pedantic -pedantic-errors -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wstrict-overflow=5 -Wundef -Wno-unused -Wno-variadic-macros -Wno-parentheses -fdiagnostics-show-option

ifeq ($(USED_OS), Windows)
	LIB_FLAGS := 
endif

ifeq ($(USED_OS), Linux)
	LIB_FLAGS := -lLuLib
endif

EXE_FLAGS := $(LIB_PATHS) $(LIB_FLAGS) $(APP_FLAGS) -O3 -static #-mwindows
D_FLAGS   := $(LIB_PATHS) $(LIB_FLAGS) $(APP_FLAGS) -g 
O_FLAGS   := $(INC_PATHS) $(APP_FLAGS) -O3#-g
H_FLAGS   := $(APP_FLAGS)

# =========== Every usable functions ===========

# Basic Build

build: $(TARGET)

$(TARGET): $(OBJ_FILES)
	@echo Linking the executable $@
	$(COMP) $^ -o $@ $(EXE_FLAGS)

start:
	./$(TARGET)

# Debug Build

debug: $(DTARGET)

$(DTARGET) : $(OBJ_FILES)
	$(COMP) $^ -o $@ $(D_FLAGS)

startDebug:
	gdb ./$(DTARGET)

# Obj and Header files compiling

objects: $(OBJ_FILES)
$(OBJ_DIR)/$(USED_OS)/%.o: $(SRC_DIR)/%.$(EXT)
	@echo Compiling $@
	$(COMP) -c $< -o $@ $(O_FLAGS)

headers: $(GCH_FILES)
$(INC_DIR)/%.h.gch: $(INC_DIR)/%.h
	@echo Compiling $@
	$(COMP) -c $< -o $@ $(H_FLAGS)

# File Cleaners

cleanAll:
	$(RM) $(INC_DIR)$(P_SEP)*.h.gch
	$(RM) $(INC_DIR)$(P_SEP)**$(P_SEP)*.h.gch
	$(RM) $(OBJ_DIR)$(P_SEP)$(USED_OS)$(P_SEP)*.o
	$(RM) $(OBJ_DIR)$(P_SEP)$(USED_OS)$(P_SEP)**$(P_SEP)*.o
	$(RM) $(PROJ).out
	$(RM) $(PROJ).exe
	$(RM) $(PROJ)_debug.out
	$(RM) $(PROJ)_debug.exe
	$(CLS)

cleanHeaders:
	$(RM) $(INC_DIR)$(P_SEP)*.h.gch
	$(RM) $(INC_DIR)$(P_SEP)**$(P_SEP)*.h.gch
	$(CLS)

clean: 
	$(RM) $(OBJ_DIR)$(P_SEP)$(USED_OS)$(P_SEP)*.o
	$(RM) $(OBJ_DIR)$(P_SEP)$(USED_OS)$(P_SEP)**$(P_SEP)*.o
	$(RM) $(TARGET)
	$(RM) $(DTARGET)
	$(CLS)

# Makefile Debugging/Usefull Functions

subDir:
	mkdir -p $(sort $(dir $(OBJ_FILES)))
	touch -f $(addsuffix DO_NOT_REMOVE,$(sort $(dir $(OBJ_FILES))))

restart: 
	$(CLS)
	@$(MAKE) --silent build
	@$(MAKE) --silent start

rebuild:
	$(CLS)
	@$(MAKE) --silent clean
	@$(MAKE) --silent build

redebug:
	$(CLS)
	@$(MAKE) --silent debug
	@$(MAKE) --silent startDebug

# Makefile Debugging/Usefull Functions
showOS: 
	@echo $(USED_OS)

showFiles:
	@echo Source files
	@echo $(SRC_FILES)
	@echo 
	@echo Object files
	@echo $(OBJ_FILES)
	@echo 
	@echo Header files
	@echo $(HDS_FILES)
	@echo 
	@echo Compiled Header files
	@echo $(GCH_FILES)
	@echo 
