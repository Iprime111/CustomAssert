srcDir = src
outDir = build
libDirs = headers ColorConsole/headers

sources = CustomAssert.cpp Logger.cpp

ifndef buildDir

libFlags = $(addprefix -I, $(libDirs))

.PHONY: all debug release

all: debug

debug: export CXX = g++
debug: export CXXFLAGS = $(libFlags) -D_DEBUG -D_SHOW_STACK_TRACE -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -Wno-write-strings -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr #-flto-odr-type-merging
debug: export buildDir = $(outDir)/debug
debug: export libObjects = ConsoleParser/CustomAssert/build/debug/Logger.o ConsoleParser/CustomAssert/build/debug/CustomAssert.o ConsoleParser/CustomAssert/ColorConsole/build/debug/ColorConsole.o ConsoleParser/build/debug/ConsoleParser.o
debug: export makeMode = debug

release: export CXX = g++
release: export CXXFLAGS = $(libFlags) -std=c++17 -Wnowrite-strings -O3
release: export buildDir = $(outDir)/release
release: export libObjects = ConsoleParser/CustomAssert/build/release/Logger.o ConsoleParser/CustomAssert/build/release/CustomAssert.o ConsoleParser/CustomAssert/ColorConsole/build/release/ColorConsole.o ConsoleParser/build/release/ConsoleParser.o
release: export makeMode = release


debug release:
	@$(MAKE)

else

srcObjects = $(addprefix $(buildDir)/, $(sources:.cpp=.o))

.PHONY: all clean prepare build_docs

all: prepare $(srcObjects)

# TODO -M -MD -MMD -MF ...

$(srcObjects): $(buildDir)/%.o: $(srcDir)/%.cpp
	@echo [CXX] -c $< -o $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

prepare:
	@mkdir -p $(buildDir)

clean:
	@rm -f $(outDir)/debug/*.o all
	@rm -f $(outDir)/release/*.o all

	cd ColorConsole && make clean

build_docs:
	doxygen Doxyfile

endif
