include ../../make.settings

SRC_DIR=./
OBJ_DIR=obj/

INCLUDEDIR = -I../inc -I../../rank-calc/inc

SRCS=$(wildcard ./*.cpp)
OBJS=$(patsubst $(SRC_DIR)%.cpp,$(OBJ_DIR)%.o,$(SRCS))

DLL=$(LIBDIR)/libfavor-calc$(LIBSUFFIX)

all: dirs $(DLL)

dirs:
	if [ -d $(OBJ_DIR) ] ; then true; else mkdir -p $(OBJ_DIR) ; fi; \

$(DLL): $(OBJS)
	@echo
	@echo
	@echo "link $^    ---->    $@"
	$(CC) $(LIBLINK) -L$(LIBDIR) -lrank-calc -o $(DLL) $(OBJS)

$(OBJS):$(OBJ_DIR)%.o:$(SRC_DIR)%.cpp
	@echo
	@echo
	@echo "compile $<         ---->      $@"
	$(CC) $(LIBCOMPILE) $(INCLUDEDIR) $< -o $@

clean:
	rm -rf $(DLL) $(OBJS)

disp:
	@echo "src:"
	@echo $(SRCS)
	@echo
	@echo "objs:"
	@echo $(OBJS)
