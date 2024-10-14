#========
# Streamlined Makefile
#========

# Find and specify build files
SRCS 			= 		$(shell find . -type f -iname '*.cc' -o -iname '*.cpp')
HDFS			= 		$(shell find . -type f -iname '*.h' -o -iname '*.hpp')
OBJS			=		$(SRCS:%.cpp=%.o)
DEPS			=		$(shell find . -type f -iname '*.cpp') #$(OBJS:%.o=%.d)


# Feature Toggles (can be overridden from the command line: make target DEBUG=1 )
# OpenMP implies pthread
DEBUG ?= 0
PTHREAD ?= 1
OPENMP ?= 1


########
# PREPROCESSOR STAGE
########
ifeq ($(OPENMP),1) # -pthread # implied by openMP
UNAME_S	= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
 CPPFLAGS += -fopenmp -I"/opt/libomp/include"
else ifeq ($(UNAME_S),Darwin) # Workaround for OSX
 CPPFLAGS += -Xpreprocessor -fopenmp -lomp -I"$(shell brew --prefix libomp)/include"
endif
endif
# debug flags
ifeq ($(DEBUG),1)
 ifeq ($(OPENMP),1)
 CPPFLAGS  +=
 else # pthreads-only; openMP doesn't support sanitize
 CPPFLAGS  +=  -pthread -fsanitize=thread -fno-omit-frame-pointer
 endif
endif
# Local dylib if necessary
CPPFLAGS  +=  -I"$(shell realpath ./include)"


########
# ASSEMBLE/COMPILE STAGE
########
TOOLCHAIN ?= clang++ # default clang++
CHECK_TOOLCHAIN = $(shell which $(TOOLCHAIN))
ifeq (clang++,$(CHECK_TOOLCHAIN)) # has clang++
 TOOLCHAIN = clang++
 THREADSAFETY = -Wthread-safety # only clang++ supports this
else ifeq (g++,$(CHECK_TOOLCHAIN)) # no clang++
 TOOLCHAIN = g++
 THREADSAFETY =
endif
# toolchain setup
CXX				=		$(TOOLCHAIN)
CC				=		$(CXX)	# Hack to force make to use clan/g++ (instead of cc)
CXXFLAGS		+=		-std=c++11
ifeq ($(OPENMP),1)
CXXFLAGS  += -fopenmp
endif
# debug flags
ifeq ($(DEBUG),1)
 CXXFLAGS +=	-v
 CXXFLAGS +=	-g #-Og
 CXXFLAGS +=	-Wall -Wextra $(THREADSAFETY)
 ifeq ($(OPENMP),1)
  CXXFLAGS +=
 else # pthreads-only; openMP doesn't support sanitize
  CXXFLAGS +=  -pthread -fsanitize=thread -fno-omit-frame-pointer
 endif
else
 CXXFLAGS +=	-O3
endif
ifeq ($(OPENMP),1)
CXXFLAGS  +=  -pthread -fsanitize=thread -fno-omit-frame-pointer
endif


########
# LINKER STAGE
########
ifeq ($(OPENMP),1)
 UNAME_S = $(shell uname -s)
 ifeq ($(UNAME_S),Linux)
  LDFLAGS += -L"/opt/libomp/lib"
  LDFLAGS += -fopenmp #-lpthread
 endif
 ifeq ($(UNAME_S),Darwin) # Workaround for OSX
  LDFLAGS += -L"$(shell brew --prefix libomp)/lib"
  LDFLAGS += -lomp # for some reason "-lomp" is required here
 endif
endif
# debug flags
ifeq ($(DEBUG),1)
 ifeq ($(OPENMP),1)
  LDFLAGS +=
 else # pthreads-only; openMP doesn't support sanitize
  LDFLAGS += -pthread -fsanitize=thread -fno-omit-frame-pointer
 endif
endif
# Local dylib if necessary
LDFLAGS +=  -lm -L"$(shell realpath ./lib)"


# Build Targets
TARGETS	= 	matrix_multiply gaussian_elimination


.PHONY: all
all: depend $(TARGETS)


.PHONY: help
help:
	@echo "You can build parts of this codebase by running make clean; make depend; make all"
	@echo "Concatenate the following options to invocations of make ... as needed:"
	@echo "	[*]	TOOLCHAIN=g++|clang++	"
	@echo "			enforces using g++ (or clang++) as the compiler"
	@echo "	[*]	DEBUG=1	"
	@echo "			enables the debug flags for compilation"
	@echo "Default settings are"
	@echo "[DEBUG:$(DEBUG)]\n[OPENMP:$(OPENMP)]\n[TOOLCHAIN:$(TOOLCHAIN)]\n[CPPFLAGS:$(CPPFLAGS)]\n[CXXFLAGS:$(CXXFLAGS)]\n[LDFLAGS:$(LDFLAGS)]"


.PHONY: echo
echo:
	@echo "[DEBUG:$(DEBUG)]\n[OPENMP:$(OPENMP)]\n[CXX:$(CXX)]\n[CPPFLAGS:$(CPPFLAGS)]\n[CXXFLAGS:$(CXXFLAGS)]\n[LDFLAGS:$(LDFLAGS)]"


.PHONY: clean
clean:
	$(RM) *.o *.d $(TARGETS)
	@echo "Cleanup complete!"


.PHONEY: depend
depend: $(SRCS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -c -MMD $^
	@sed -i'~' "s|^\(.*\)\.o: \(.*\)\1|\2\1.o: \2\1|g" $(DEPS)


-include *.d
.DEFAULT_GOAL=all

