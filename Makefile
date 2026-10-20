CFLAGS=-Wall -Wextra -std=c99 -pedantic -Isubmodules/pine/pine/include $(shell pkg-config --cflags cglm) -Iinclude -MMD
LDFLAGS=-Lsubmodules/pine/pine/lib -lpine -lwayland-client -lwayland-egl -lEGL -lxkbcommon -lGL -lGLU $(shell pkg-config --libs cglm)

OBJ=src/main.o\
	src/element.o
BIN=bin/maple
PINE=submodules/pine/pine/lib/libpine.a

SHADERS=$(wildcard src/shaders/*.vert src/shaders/*.frag)
SHADER_HEADER=src/element_shader.h
GLSLANG?=glslangValidator


.PHONY: all
all: $(BIN)

$(BIN): $(OBJ) $(PINE) | bin
	$(CC) $(OBJ) $(LDFLAGS) -o $@

$(OBJ):%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

src/element.o: $(SHADER_HEADER)

$(SHADER_HEADER): $(SHADERS) tools/embed_shaders.sh
	$(GLSLANG) -l $(SHADERS)
	tools/embed_shaders.sh $@ $(SHADERS)

$(PINE): submodules/pine/pine/Makefile
	cd submodules/pine/pine && make $(FLAGS)

bin:
	mkdir -p $@

.PHONY: clean
clean:
	rm -f $(OBJ) $(OBJ:.o=.d) $(SHADER_HEADER) $(BIN)

-include $(OBJ:.o=.d)
