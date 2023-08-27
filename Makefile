.PHONY: all clean skyline

NAME 			:= $(shell basename $(CURDIR))
NAME_LOWER		:= $(shell echo $(NAME) | tr A-Z a-z)

BUILD_DIR 		:= build

MAKE_NSO		:= nso.mk

all: skyline

skyline:
	$(MAKE) all -f $(MAKE_NSO) MAKE_NSO=$(MAKE_NSO) BUILD=$(BUILD_DIR) TARGET=$(NAME)
	@mkdir -p 01007820196A6000/exefs
	@cp main.npdm 01007820196A6000/exefs/main.npdm
	@cp subsdk9 01007820196A6000/exefs/subsdk9

clean:
	$(MAKE) clean -f $(MAKE_NSO)
	rm -r -f 01007820196A6000
