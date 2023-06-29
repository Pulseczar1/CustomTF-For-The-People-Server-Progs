PROGS_TOPLEVEL=..

include $(PROGS_TOPLEVEL)/makefile.inc

#CFLAGS	= $(PRJCFLAGS) -I..
#OBJLIBS	= ../libsub.a ../libsubsub.a
#PROGS_ARCHIVES = progs.a frikbot.a idmonsters.a
OBJS	= bot_ai.o  bot_fight.o  bot_move.o  bot_qw.o  \
          bot_ed.o  bot_misc.o   bot_phys.o  bot_way.o

#all : $(PROGS_ARCHIVES)

frikbot.a : $(OBJS)
#	ar rv frikbot.a $?
	ranlib frikbot.a

clean :
	-rm -f *.o *.a

# Pattern Rule
%.o: %.cpp
	g++ $(CXXFLAGS) -I$(PROGS_TOPLEVEL) -c $< -o $@
	ar -rvT frikbot.a $*.o
#	$(RM) $*.o
