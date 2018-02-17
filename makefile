SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

TEST_SRC = $(wildcard src/*.cpp)
TEST_OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC))

CXX = clang++
CXXFLAGS = -c -g -std=c++11 -I include

all: project

test: project FORCE
	make -f test/makefile

FORCE:

project: $(OBJ)
	$(CXX) -o $@ -g $^

DEPDIR := .d
OBJDIR := obj

$(shell mkdir -p $(OBJDIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

obj/%.o : src/%.cpp
obj/%.o : src/%.cpp $(DEPDIR)/%.d
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
