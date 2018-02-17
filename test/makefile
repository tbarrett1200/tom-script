TEST_SRC = $(wildcard test/src/*.cpp)
TEST_OBJ = $(patsubst test/src/%.cpp, test/obj/%.o, $(TEST_SRC))

OBJ = $(filter-out obj/main.o, $(wildcard obj/*.o))

CXX = clang++
CXXFLAGS = -c -g -std=c++11  -I include

test/test: $(TEST_OBJ) $(OBJ)
	$(CXX) -o $@ $^ -g -lpthread -lgtest -lgtest_main -std=c++14

DEPDIR := test/.d
TEST_OBJDIR := test/obj

$(shell mkdir -p $(DEPDIR) >/dev/null)
$(shell mkdir -p $(TEST_OBJDIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

test/obj/%.o : test/src/%.cpp
test/obj/%.o : test/src/%.cpp $(DEPDIR)/%.d
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d
