include make.settings

all: dirs build

dirs:
	if [ -d $(BINDIR) ] ; then true; else mkdir -p $(BINDIR) ; fi; \

build:
	cd ./pearson-calc && $(MAKE)
	cd ./rank-calc && $(MAKE)
	cd ./trade-calc && $(MAKE)
	cd ./favor-calc && $(MAKE)
	cd ./transform-calc && $(MAKE)

clean:
	cd ./pearson-calc && $(MAKE) clean
	cd ./rank-calc && $(MAKE) clean
	cd ./trade-calc && $(MAKE) clean
	cd ./favor-calc && $(MAKE) clean
	cd ./transform-calc && $(MAKE) clean
