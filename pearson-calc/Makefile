include ../make.settings

SRC_DIR=./
OBJ_DIR=obj/

INCLUDEDIR = -I./inc

SRCS=$(wildcard ./*.cpp)
OBJS=$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRCS))

TARGET=$(BINDIR)/pearson-calc

all: subs dirs $(TARGET)

subs:
	cd ./src; $(MAKE) \

dirs:
	if [ -d $(OBJ_DIR) ] ; then true; else mkdir -p $(OBJ_DIR) ; fi; \

$(TARGET): $(OBJS)
	@echo
	@echo "compile $^    ---->    $@"
	$(CC) $(EXELINK) -L$(LIBDIR) -lpearson-calc -o $@ $(OBJS)

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
