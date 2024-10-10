#========
# Streamlined Makefile
#========

# Find and specify build files
SRCS 			= 		$(shell find . -type f -iname '*.cc' -o -iname '*.cpp')
HDFS			= 		$(shell find . -type f -iname '*.h' -o -iname '*.hpp')
OBJS			=		$(SRCS:%.cpp=%.o)
DEPS			=		$(shell find . -type f -iname '*.cpp') #$(OBJS:%.o=%.d)


# Feature Toggles (can be overridden from the command line: make target DEBUG=1 )
DEBUG			?=		1
PTHREAD			?=		0 #OpenMP implies pthread
OPENMP			?=		1 #OpenMP implies pthread


########
# PREPROCESSOR STAGE
########
ifeq ($(OPENMP),1)
	UNAME_S	= $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CPPFLAGS	+=	-I"/opt/libomp/include"
	endif
	ifeq ($(UNAME_S),Darwin) # Workaround for OSX
		CPPFLAGS	+=	-Xpreprocessor -fopenmp -lomp -I"$(shell brew --prefix libomp)/include"
	endif
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
	CXXFLAGS	+=	-g -Og
	CXXFLAGS	+=	-Wall -Wextra -Wthread-safety
		ifeq ($(OPENMP),1)
	CXXFLAGS  +=  -fsanitize=thread -fno-omit-frame-pointer
	endif
else
	CXXFLAGS	+=		-O3
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
endif
# Local dylib if necessary
LDFLAGS   +=  -L"$(shell realpath ./lib)"


# Build Targets
TARGETS	= 	matrix_multiply gaussian_elimination


.PHONY: all
all: clean depend $(TARGETS)


#%.o: %.cpp %.hpp
#	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	$(RM) *.o *.d $(TARGETS)
	@echo "Cleanup complete!"


.PHONEY: depend
depend: $(SRCS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -MMD $^
	@sed -i'~' "s|^\(.*\)\.o: \(.*\)\1|\2\1.o: \2\1|g" $(DEPS)


-include *.d
.DEFAULT_GOAL=all

