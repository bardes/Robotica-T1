# Aluno: Paulo Bardes
# nrUsp: 8531932

# Compilador de C++
CXX = g++

# Nome do executavel
EXEC = T1

WARNINGS = -pedantic -Wall -Wextra -Wcast-align -Wcast-qual \
	   -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self \
	   -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs \
	   -Wnoexcept -Woverloaded-virtual -Wredundant-decls \
	   -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
	   -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused

# Flags usadas durante a compilacao
CXXFLAGS = $(WARNINGS) -std=c++11 -march=native -O2

# Flags de preprocessamento
CPPFLAGS = $(subst -I, -isystem, $(shell PKG_CONFIG_PATH=/usr/local/lib/pkgconfig pkg-config --cflags-only-I playerc++))

# Flags usado durante a linkagem
LDFLAGS = $(shell PKG_CONFIG_PATH=/usr/local/lib/pkgconfig pkg-config --libs playerc++)

# Arquivos com codigo fonte
SRCS = $(shell find -name '*.cc')
HDRS = $(shell find -name '*.hh')
RES  = 

# Lista de arquivos objeto gerada a partir dos arquivos de implementacao
OBJS = $(SRCS:=.o)

# Lista de arquivos de dependencia
DEPS = $(SRCS:=.dep)

# Alvos que nao devem ser representados por arquivos
.PHONY: clean all run zip

# Alvo padrao do make
all: $(EXEC)

debug: CXXFLAGS += -g -O0
debug: CPPFLAGS += -DDEBUG
debug: all

# Faz a linkagem e cria o executavel
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS) $(LDFLAGS)

# Regra geral para compilar os arquivos fontes
$(OBJS):
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(@:.o=) -o $@

# Gera arquivo de dependencia de cada ".cc"
%.dep: %
	$(CXX) -MM -MT $(@:.dep=.o) $(CPPFLAGS) $< -MF $@

run:
	./$(EXEC)

zip:
	zip -r $(TARGET).zip $(SRCS) $(HDRS) $(RES) Makefile 

clean:
	rm -f $(OBJS) $(DEPS)

# Insere as dependencias no makefile
-include $(DEPS)
