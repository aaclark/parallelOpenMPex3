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
DEBUG ?= 1
PTHREAD ?= 1
OPENMP ?= 1


########
# PREPROCESSOR STAGE
########
ifeq ($(OPENMP),1)
UNAME_S	= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	CPPFLAGS +=	-I"/opt/libomp/include"
endif
ifeq ($(UNAME_S),Darwin) # Workaround for OSX
	CPPFLAGS += -Xpreprocessor -fopenmp -lomp -I"$(shell brew --prefix libomp)/include"
endif
# CPPFLAGS += -pthread # implied by openMP
endif
# Local dylib if necessary
CPPFLAGS  +=  -I"$(shell realpath ./include)"


########
# ASSEMBLE/COMPILE STAGE
########
CXX				=		clang++
CC				=		$(CXX)	# Hack to force make to use clang++ (instead of cc)
CXXFLAGS		+=		-std=c++11
ifeq ($(DEBUG),1)
CXXFLAGS	+=	-v
CXXFLAGS	+=	-g #-Og
CXXFLAGS	+=	-Wall -Wextra -Wthread-safety
else
CXXFLAGS	+=	-O3
endif
ifeq ($(OPENMP),1)
CXXFLAGS  +=  -pthread -fsanitize=thread -fno-omit-frame-pointer
endif


########
# LINKER STAGE
########
ifeq ($(OPENMP),1)
UNAME_S	= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
LDFLAGS   +=  -L"/opt/libomp/lib"
endif
ifeq ($(UNAME_S),Darwin) # Workaround for OSX
LDFLAGS   +=  -L"$(shell brew --prefix libomp)/lib"
endif
LDFLAGS  +=  -pthread -fsanitize=thread -fno-omit-frame-pointer -lomp # for some reason "-lomp" is required here
endif
# Local dylib if necessary
LDFLAGS   +=  -L"$(shell realpath ./lib)"


# Build Targets
TARGETS	= 	matrix_multiply gaussian_elimination


.PHONY: all
all: depend $(TARGETS)


#%.o: %.cpp %.hpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@


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

