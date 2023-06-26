ENGINE_DIR=../
CLIENT_DIR=../../client
PROGS_DIR=./

include $(ENGINE_DIR)makefile.inc

#CFLAGS	= $(PRJCFLAGS) -I..
#OBJLIBS	= ../libsub.a ../libsubsub.a
PROGS_ARCHIVES = progs.a frikbot/frikbot.a idmonsters/idmonsters.a
OBJS	= actions.o  coop_items.o  environ.o   jobs.o      progdefs.o  spectate.o  tforttm.o   \
          admin.o    cpstuff.o     field.o     medic.o     prozac.o    speed.o     tinker.o    \
          agr.o      crusader.o    fight.o     megatf.o    pyro.o      sprites.o   triggers.o  \
          ai.o       ctf.o         fish.o      menu.o      qw.o        spy.o       tsoldier.o  \
          airfist.o  custom.o      gremlin.o   misc.o      rotate.o    status.o    vote.o      \
          army.o     cutfmenu.o    grunty.o    monsters.o  runes.o     subs.o      vote2.o     \
          boss.o     debug.o       guerilla.o  mtfents.o   sbitems.o   teleport.o  warlock.o   \
          buttons.o  defs.o        gweapons.o  obits.o     scout.o     tesla.o     weapons.o   \
          client.o   demoman.o     haxxx.o     ofnents.o   security.o  tfdefs.o    wizard.o    \
          cmnd.o     demon.o       hook.o      optimize.o  sentry.o    tfort.o     world.o     \
          combat.o   doors.o       invade.o    plats.o     shambler.o  tforthlp.o  neo.o       \
          coop.o     engineer.o    items.o     player.o    sniper.o    tfortmap.o

all : $(PROGS_ARCHIVES)

# Whenever a file in frikbot or idmonsters is recompiled, progs.a has to be recreated in entirely, for some reason.
# The following rule isn't working right. So, what I have done is just make it so that progs.a is always recreated,
# by deleting it first. Didn't work here. So, put it in the parent makefile.
progs.a : $(OBJS) frikbot/frikbot.a idmonsters/idmonsters.a
#	-rm -f progs.a
	ar -rvT progs.a $?
	ranlib progs.a

frikbot/frikbot.a : force_look
	cd frikbot; make CFG=$(CFG)

idmonsters/idmonsters.a : force_look
	cd idmonsters; make CFG=$(CFG)

clean :
	-rm -f *.o *.a
	cd frikbot; make clean
	cd idmonsters; make clean

force_look :
	@true

# Pattern Rule
%.o: %.cpp
	g++ $(CPPFLAGS) -I$(PROGS_DIR) -c $< -o $@
