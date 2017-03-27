# directories
INCLUDEDIR = include
SRCDIR = src
OBJDIR = obj
EXECUTABLEDIR = bin

# compiler
CC = g++
CFLAGS = -c -g -Wall `root-config --cflags` -I$(INCLUDEDIR)

#linker
LINKER = g++
LDFLAGS = `root-config --libs` -lSpectrum

# the := expands the meaning of the expression in the variable assignment
SOURCES := $(wildcard $(SRCDIR)/*.cc) # take all the .cc files in the src folder
OBJECTS := $(SOURCES:$(SRCDIR)/%.cc=$(OBJDIR)/%.o) # in the SOURCES (variable content) what matches $(SRCDIR)/%.cc becomes $(OBJDIR)/%.o where the % is used to create an "entry to entry" correspondance
TARGETS_SOURCES := $(wildcard $(SRCDIR)/*.cpp)
TARGETS_OBJECTS := $(TARGETS_SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGETS := $(TARGETS_SOURCES:$(SRCDIR)/%.cpp=$(EXECUTABLEDIR)/%)

# variables to compile class to be stored in ttree
ROOTINCLUDE = $(INCLUDEDIR)/MeasurementConditions.h
ROOTDICTFILES := $(filter-out $(INCLUDEDIR)/LinkDef.h, $(ROOTINCLUDE)) # LinkDef must be the last file given to rootcint (see where it's used)

all: $(TARGETS)

$(TARGETS): $(EXECUTABLEDIR)/%: $(OBJDIR)/%.o $(OBJECTS) $(OBJDIR)/Dict.so # the dict is necessary to let root use the cluster structure in the root ttree
	@echo "\tLinking "$@
	@$(LINKER) $< $(OBJECTS) $(LDFLAGS) -o $@

$(TARGETS_OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo "\tCompiling "$<
	@$(CC) $(CFLAGS) $< -o $@

# $< is the current input file and $@ is the target of this action the @ at the beginning of the line is to not print out the line
$(OBJECTS): $(OBJDIR)/%.o: $(SRCDIR)/%.cc # create a object $(OBJDIR)/%.o from the file $(SRCDIR)/%.cc for the name matching $(OBJDIR)/%.o in the OBJECT variable
	@echo "\tCompiling "$<
	@$(CC) $(CFLAGS) $< -o $@

$(OBJDIR)/Dict.cxx: $(ROOTDICTFILES) $(INCLUDEDIR)/LinkDef.h
	@echo "\tGenerating "$@
	@rootcling -f $@ $(CFLAGS) -p $^

$(OBJDIR)/Dict.so: $(OBJDIR)/Dict.cxx
	@echo "\tGenerating "$@
	@$(CC) -shared -o $@ `root-config --ldflags` $(CFLAGS) -I. $^
	@cp $(wildcard $(OBJDIR)/*.pcm) $(EXECUTABLEDIR)

clean:
	rm -f $(TARGETS) $(wildcard $(OBJDIR)/*)

clear:
	rm -f $(wildcard $(INCLUDEDIR)/*~) $(wildcard $(SRCDIR)/*~)
