include ../make.settings

SRC_DIR=./
OBJ_DIR=obj/

INCLUDEDIR = -I../rank-calc/inc -I../favor-calc/inc -I./inc -I../pearson-calc/inc

SRCS=$(wildcard ./*.cpp)
OBJS=$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRCS))

TARGET=$(BINDIR)/transform-calc

all: subs dirs $(TARGET)

subs:
	cd ./src; $(MAKE) \

dirs:
	if [ -d $(OBJ_DIR) ] ; then true; else mkdir -p $(OBJ_DIR) ; fi; \

$(TARGET): $(OBJS)
	@echo
	@echo "compile $^    ---->    $@"
	$(CC) $(EXELINK) -L$(LIBDIR) -lrank-calc -L$(LIBDIR) -lfavor-calc -L$(LIBDIR) -ltransform-calc -L$(LIBDIR) -lpearson-calc -lpthread -o $@ $(OBJS)

$(OBJS):$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	@echo
	@echo
	@echo "compile $<         ---->      $@"
	$(CC) $(LIBCOMPILE) $(INCLUDEDIR) $< -o $@

clean:
	cd ./src; $(MAKE) clean
	rm -rf $(TARGET) $(OBJS)

disp:
	@echo "src:"
	@echo $(SRCS)
	@echo
	@echo "objs:"
	@echo $(OBJS)
