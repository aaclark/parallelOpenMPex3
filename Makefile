#========
# Streamlined Makefile
#========

# Find and specify build files
SRCS 			= 		$(shell find . -type f -iname '*.cc' -o -iname '*.cpp')
HDFS			= 		$(shell find . -type f -iname '*.h' -o -iname '*.hpp')
OBJS			=		$(SRCS:%.cpp=%.o)
DEPS			=		$(shell find . -type f -iname '*.cpp') #$(OBJS:%.o=%.d)

# Determine Build Target
UNAME_S	= $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OMP_INC		=		-I"/opt/libomp/include"
	OMP_LIB		=		-L"/opt/libomp/lib"
endif
ifeq ($(UNAME_S),Darwin)
	OMP_INC		=		-I"$(shell brew --prefix libomp)/include"
	OMP_LIB		=		-L"$(shell brew --prefix libomp)/lib"
endif

# Ensures IDEs can see local includes as a last resort
LOC_INC 		=		-I"$(shell realpath ./include)"
LOC_LIB			=		-L"$(shell realpath ./lib)"

# Feature Toggles (can be overridden from the command line)
DEBUG			?=		0
PTHREAD			?=		1
OPENMP			?=		1

# Basic requirements
CXXFLAGS		+=		-std=c++11

# Toggle Flags Conditionally
ifeq ($(DEBUG),1)
	CXXFLAGS	+=		-g -Og -v
else
	CXXFLAGS	+=		-O3
endif

ifeq ($(PTHREAD),1)
	PTHREAD_FLAGS =		-pthread
else
	PTHREAD_FLAGS =		# No pthread
endif

ifeq ($(OPENMP),1)
	OPENMP_FLAGS  =		-Xpreprocessor -fopenmp
	OMP_LIBS	  =		-lomp
else
	OPENMP_FLAGS  =		# No OpenMP
	OMP_LIBS	  =		# No OpenMP
endif

# Include/Lib Directories
INCLUDES		=		$(OMP_INC) $(LOC_INC)
LIBS			=		$(OMP_LIB) $(LOC_LIB)

# Warnings and Strict Flags
WARN			=		-Wall -Wextra -Wthread-safety
CXXFLAGS		+=		$(PTHREAD_FLAGS) $(WARN) $(INCLUDES)
CPPFLAGS		+=		$(OPENMP_FLAGS)
LDFLAGS			+=		$(LIBS) $(INCLUDES) -lm $(OMP_LIBS)

# Compiler Choice
CXX				=		clang++
CC				=		$(CXX)	# Hack to force make to use clang++ (instead of cc)

# Build Targets
TARGETS	= 	matrix_multiply

.PHONY: all
all: clean depend $(TARGETS)

-include *.d

#matrix_multiply: matrix_t.o


%.o: %.cpp %.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	$(RM) *.o *.d $(TARGETS)
	@echo "Cleanup complete!"


.PHONEY: depend
depend: $(SRCS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -MMD $^
	@sed -i'~' "s|^\(.*\)\.o: \(.*\)\1|\2\1.o: \2\1|g" $(DEPS)

.DEFAULT_GOAL=all
