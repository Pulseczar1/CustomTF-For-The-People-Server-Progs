PROGS_TOPLEVEL=..

include $(PROGS_TOPLEVEL)/makefile.inc

#CFLAGS	= $(PRJCFLAGS) -I..
#OBJLIBS	= ../libsub.a ../libsubsub.a
#PROGS_ARCHIVES = progs.a frikbot.a idmonsters.a
OBJS	= boss.o           demon.o     fish.o     ogre.o      shambler.o  wizard.o   \
          common.o         dog.o       hknight.o  oldone.o    soldier.o   zombie.o   \
		  coop_monsters.o  enforcer.o  knight.o   shalrath.o  tarbaby.o

#all : $(PROGS_ARCHIVES)

idmonsters.a : $(OBJS)
#	ar rv idmonsters.a $?
	ranlib idmonsters.a

clean :
	-rm -f *.o *.a


# Pattern Rule
%.o: %.cpp
	g++ $(CXXFLAGS) -I$(PROGS_TOPLEVEL) -c $< -o $@
	ar -rvT idmonsters.a $*.o
#	$(RM) $*.o
