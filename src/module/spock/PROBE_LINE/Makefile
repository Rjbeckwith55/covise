##########################################################
## Description: Makefile Template for COVISE modules    ##
## (C) 1995 Computer Center University of Stuttgart     ##
##            Allmandring 30  70550 Stuttgart           ##  
## Author: Uwe Woessner, Andreas Wierse  Date: 29.08.97 ##
##########################################################

                          # name of the module executable
APPLICATION 	= ProbeLine
                          # name of the module category
CATEGORY	= Filter
                          # needed for creating distribution tar-files
PROJECT		= Spock

# put all object files that are needed to build the module here:

OBJECTS         = $(OBJDIR)/HandleSet.o


# $(APPLICATION) is also the name of the main source and include file!!!)

##########################################################

# modify this for your own path if necessary
# COVISEDIR = ../../../..

# All common UNIX compiler flags and options are in
# this central makefile.

include $(COVISEDIR)/src/make.env.$(ARCHSUFFIX)

# if you want to modify settings of variables that are set in the
# central makefile, do this here:


# All common rules are in this central makefile.

include $(COVISEDIR)/src/make.rules

############### link rule for application module #########
# can be modified (e.g. for dependencies or libraries)
##########################################################

$(PLATFORM)/bin/$(CATEGORY)/$(APPLICATION): $(OBJDIR)/$(APPLICATION).o $(OBJECTS)
	  $(CC) $(CPPFLAGS) -o $@ $(OBJECTS) $(OBJDIR)/$(APPLICATION).o \
	  $(APPLLIB) $(LDLIBS)

############### general object rule ######################
# can be modified (e.g. for dependencies)
##########################################################

$(OBJDIR)/$(APPLICATION).o: $(APPLICATION).cpp
	$(CC) $(CPPFLAGS) -c $(APPLICATION).cpp -o $(OBJDIR)/$(APPLICATION).o
	$(POSTCC)

############### module specific rules ####################
# rules for objects in $(OBJECTS)
##########################################################

$(OBJDIR)/HandleSet.o: HandleSet.cpp
	$(CC) $(CPPFLAGS) -c HandleSet.cpp -o $(OBJDIR)/HandleSet.o
	$(POSTCC)

