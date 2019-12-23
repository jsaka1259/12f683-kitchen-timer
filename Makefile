HEX    := kitchen_timer
CHIP   := 12F683


SRCDIR := src
CMNDIR := $(SRCDIR)/common
SRCS   := $(SRCDIR)/*.c
SRCS   += $(CMNDIR)/*.c
OUTDIR := build
BIN    := $(OUTDIR)/$(HEX).hex
LOGS   := log.* MPLABXLog.xml*

# Set writing shell script path
IPE    := ./tool/ipecmd.sh

# Set writing TOOL (Pickit3)
TOOL   := PPK3

# Power target from TOOL
VDD    := 5.0

# Compiler
CC     := xc8-cc
CFLAGS := -mwarn=-9

.PHONY: rebuild clean
.PHONY: write   erase  verify  blank
.PHONY: writew  erasew verifyw blankw

# Build
$(BIN): $(SRCS)
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	$(CC) -mcpu=$(CHIP) $(CFLAGS) -o $@ $^

# Clean and Build
rebuild: clean $(BIN)

# Delete OUTDIR and LOGS
clean:
	rm -rf $(OUTDIR)

# Write BIN in CHIP using TOOL
write:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -F$(BIN) -M
	@mv $(LOGS) $(OUTDIR)
# Erase Flash Device
erase:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -E
	@mv $(LOGS) $(OUTDIR)
# Verify Device
verify:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -F$(BIN) -Y
	@mv $(LOGS) $(OUTDIR)
# Blank Check Device
blank:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -C
	@mv $(LOGS) $(OUTDIR)/

# Write BIN in CHIP using TOOL. Power target from TOOL
writew:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -F$(BIN) -M -W$(VDD)
	@mv $(LOGS) $(OUTDIR)
# Erase Flash Device. Power target from TOOL
erasew:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -E -W$(VDD)
	@mv $(LOGS) $(OUTDIR)
# Verify Device. Power target from TOOL
verifyw:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -F$(BIN) -Y -W$(VDD)
	@mv $(LOGS) $(OUTDIR)
# Blank Check Device. Power target from TOOL
blankw:
	@if [ ! -e $(OUTDIR) ]; then mkdir -p $(OUTDIR); fi
	-$(IPE) -P$(CHIP) -T$(TOOL) -C -W$(VDD)
	@mv $(LOGS) $(OUTDIR)
