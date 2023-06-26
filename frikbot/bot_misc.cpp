/***********************************************
*                                              *
*              FrikBot Misc Code               *
*   "Because you can't name it anything else"  *
*                                              *
***********************************************/

/*
This program is in the Public Domain. My crack legal
team would like to add:

RYAN "FRIKAC" SMITH IS PROVIDING THIS SOFTWARE "AS IS"
AND MAKES NO WARRANTY, EXPRESS OR IMPLIED, AS TO THE
ACCURACY, CAPABILITY, EFFICIENCY, MERCHANTABILITY, OR
FUNCTIONING OF THIS SOFTWARE AND/OR DOCUMENTATION. IN
NO EVENT WILL RYAN "FRIKAC" SMITH BE LIABLE FOR ANY
GENERAL, CONSEQUENTIAL, INDIRECT, INCIDENTAL,
EXEMPLARY, OR SPECIAL DAMAGES, EVEN IF RYAN "FRIKAC"
SMITH HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
DAMAGES, IRRESPECTIVE OF THE CAUSE OF SUCH DAMAGES.

You accept this software on the condition that you
indemnify and hold harmless Ryan "FrikaC" Smith from
any and all liability or damages to third parties,
including attorney fees, court costs, and other
related costs and expenses, arising out of your use
of this software irrespective of the cause of said
liability.

The export from the United States or the subsequent
reexport of this software is subject to compliance
with United States export control and munitions
control restrictions. You agree that in the event you
seek to export this software, you assume full
responsibility for obtaining all necessary export
licenses and approvals and for assuring compliance
with applicable reexport restrictions.

Any reproduction of this software must contain
this notice in its entirety.
*/

#include "progs.h"
#include "frikbot/bot_misc.h"
#include "frikbot/bot_qw.h"
#include "debug.h"

namespace Progs {

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getNameForBotGivenNumber   (PZ: was named "BotName")

Get the name for a bot, given the name's number.
(Warning: This also sets `self`'s shirt and pants colors.)

PZ: keep NUMBER_OF_BOT_NAMES (in bot_qw.qc) updated when you add or remove names

`self` = a temporary entity that will hold the new bot's data before the bot is actually created

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
string frik_getNameForBotGivenNumber(float r)
{
	self->b_num = r;
	// PZ: There were only 16. I extended it to 32 to allow for more bots.
	if (r == 1)
	{
		self->b_pants = 11;
		self->b_shirt = 0;
		return "Vincent";
	}
	else if (r == 2)
	{
		self->b_pants = 1;
		self->b_shirt = 3;
		return "Bishop";
	}
	else if (r == 3)
	{
		self->b_pants = 13;
		self->b_shirt = 2;
		return "Nomad";
	}
	else if (r == 4)
	{
		self->b_pants = 7;
		self->b_shirt = 6;
		return "Vi";         // PZ: The Walking Dead (CDC computer)
	}
	else if (r == 5)
	{
		self->b_pants = 12;
		self->b_shirt = 6;
		return "Lore";
	}
	else if (r == 6)
	{
		self->b_pants = 4;
		self->b_shirt = 4;
		return "Servo";
	}
	else if (r == 7)
	{
		self->b_pants = 2;
		self->b_shirt = 5;
		return "Gort";
	}
	else if (r == 8)
	{
		self->b_pants = 10;
		self->b_shirt = 3;
		return "Kryten";
	}
	else if (r == 9)
	{
		self->b_pants = 9;
		self->b_shirt = 4;
		return "Data";
	}
	else if (r == 10)
	{
		self->b_pants = 4;
		self->b_shirt = 7;
		return "Max";
	}
	else if (r == 11)
	{
		self->b_pants = 3;
		self->b_shirt = 11;
		return "Marvin";
	}
	else if (r == 12)
	{
		self->b_pants = 13;
		self->b_shirt = 12;
		return "Bender";
	}
	else if (r == 13)
	{
		self->b_pants = 11;
		self->b_shirt = 2;
		return "Mr. Roboto";
	}
	else if (r == 14)
	{
		self->b_pants = 0;
		self->b_shirt = 2;
		return "Starscream";
	}
	else if (r == 15)
	{
		self->b_pants = 8;
		self->b_shirt = 9;
		return "Gypsy";
	}
	else if (r == 16)
	{
		self->b_pants = 5;
		self->b_shirt = 10;
		return "HAL";
	}
	// PZ: need up to 32 bots
	else if (r == 17)
	{
		self->b_pants = 11;
		self->b_shirt = 0;
		return "Governator";
	}
	else if (r == 18)
	{
		self->b_pants = 1;
		self->b_shirt = 3;
		return "T-1000";
	}
	else if (r == 19)
	{
		self->b_pants = 13;
		self->b_shirt = 2;
		return "RoboCop";
	}
	else if (r == 20)
	{
		self->b_pants = 7;
		self->b_shirt = 6;
		return "Optimus Prime";
	}
	else if (r == 21)
	{
		self->b_pants = 12;
		self->b_shirt = 6;
		return "Megatron";
	}
	else if (r == 22)
	{
		self->b_pants = 4;
		self->b_shirt = 4;
		return "Ash";
	}
	else if (r == 23)
	{
		self->b_pants = 2;
		self->b_shirt = 5;
		return "Johnny 5";
	}
	else if (r == 24)
	{
		self->b_pants = 10;
		self->b_shirt = 3;
		return "Dot Matrix";
	}
	else if (r == 25)
	{
		self->b_pants = 9;
		self->b_shirt = 4;
		return "David";
	}
	else if (r == 26)
	{
		self->b_pants = 4;
		self->b_shirt = 7;
		return "Teddy";
	}
	else if (r == 27)
	{
		self->b_pants = 3;
		self->b_shirt = 11;
		return "Jerry";
	}
	else if (r == 28)
	{
		self->b_pants = 13;
		self->b_shirt = 12;
		return "KITT";
	}
	else if (r == 29)
	{
		self->b_pants = 11;
		self->b_shirt = 2;
		return "Talus";
	}
	else if (r == 30)
	{
		self->b_pants = 0;
		self->b_shirt = 2;
		return "Hadaly";
	}
	else if (r == 31)
	{
		self->b_pants = 8;
		self->b_shirt = 9;
		return "R2-D2";
	}
	else if (r == 32)
	{
		self->b_pants = 5;
		self->b_shirt = 10;
		return "C-3PO";
	}
	// PZ: if the name's number wasn't one of the above...
	return "INVALID NAME";
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getRandomNameForBot   (PZ: was named "PickARandomName")

Out of all the available bot names, it returns a random
name.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
string frik_getRandomNameForBot()
{
	float runLoop, numberOfName, counter;
	string botName;
	entity e;

	botName = "INVALID NAME"; // PZ: initialize it

	if (bot_count > PR_NUMBER_OF_BOT_NAMES)  // PZ: "#NUMBER_OF_BOT_NAMES" was 32
		return "player";

	// PZ TODO: don't just keep guessing names until you find one not used; first, establish a list that isn't used?
	// PZ FIXME: warning, this might could result in an infinite loop
	// guess names until we find one that isn't being used
	runLoop = PR_TRUE;
	while (runLoop)
	{
		// PZ: fixed a bug here, in the case where random() chooses 0.0
		numberOfName = random() * (PR_NUMBER_OF_BOT_NAMES - 1);   // PZ: generate a number between 0 and NUMBER_OF_BOT_NAMES - 1
		numberOfName = numberOfName + 1;                        // PZ: make it between 1 and NUMBER_OF_BOT_NAMES
		numberOfName = rint(numberOfName);
		// get the name, given the name's number
		botName = frik_getNameForBotGivenNumber(numberOfName);
		// check that this name isn't already being used

		/*e = find(world, netname, botName);
		if (e == world)
			runLoop = #FALSE;*/

		// PZ: make it just look at client entities (a lot faster)
		e = nextent(world); // start at the first client entity
		counter = 1;
		while (counter <= 32)
		{
			if (e->netname == botName) counter = 99999; // this name is being used; try another name
			else
			{
				e = nextent(e);
				counter = counter + 1;
			}
		}
		if (counter == 33) runLoop = PR_FALSE;   // we didn't find this name being used, so return this name
	}
	return botName;
}

// I didn't like the old code so this is very stripped down (PZ: I don't think I wrote this.)

entity b_topicStarter; // PZ: was called "b_originator"
float b_topic;
/* FBX Topics (b_topic)

b_topicStarter == self
 0 -  no topic
 1 -  sign on
 2 -  killed targ
 3 -  team message "friendly eyes"
 4 -  team message "on your back"
 5 -  team message "need back up"
 6 -  excuses
 ----
 7 -  gameover
 ----
 8 -  welcoming someone onto server
 9 -  ridicule lost frag (killed self?)
 10 - ridicule lost frag (lava)
 11 - lag
b_topicStarter == targ
*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botConsiderStartingConversation   (PZ: was named "bot_start_topic")

Gives the bot a random chance of starting a 'conversation'.

`self` = the bot considering starting a conversation

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botConsiderStartingConversation(float topic, float chance)
{
	// PZ: if you are already about to talk, don't change the topic on yourself
	if (self->b_chattime/* && topic != #TOPIC_NONE*/) return;

	if (random() <= chance) // PZ: "chance" was "0.2"
	{
		b_topic = topic;
		b_topicStarter = self;
	}
	else
		b_topic = 0;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_botChat             (PZ: was named "bot_chat")

`self` = this bot

PZ: I made it so that you don't need to type ": " before every bot chat message.

// Would be funny to add somewhere, "less pow wow. more pow pow".

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_botChat()
{
	float r;

	/*local string str1, str2;
	str1 = ftos(self.b_chattime);
	str2 = ftos(b_topic);
	bprint(2, "Called frik_botChat() for ", self.netname, "; .b_chattime == ", str1, "; b_topic = ", str2);
	bprint(2, "; b_topicStarter = ", b_topicStarter.netname, "\n");*/

	if (b_options & PR_OPT_NOCHAT) // allows players to disable bot talking
		return;

	r = ceil(random() * 6);

	// PZ NOTE: bot is currently "typing" (about to talk)
	if (self->b_chattime > time)
	{
		if (self->b_skill < PR_SKILL_HARD)
			self->keys = self->PR_BUTTON_FIRE = self->PR_BUTTON_JUMP = 0; // PZ NOTE: this means crappy players can't type and play at the same time, right?
		return;
	}
	// PZ NOTE: bot can't talk unless his .b_chattime > 0 and <= `time`
	else if (self->b_chattime)
	{
		if (b_topic == PR_TOPIC_SELF_JOINS_GAME) // sign on
		{
			if (b_topicStarter == self)
			{
				if (r == 1)
				{
					frik_botSay("sup?\n");                   // PZ: was "lo all"
					frik_botConsiderStartingConversation(PR_TOPIC_TARG_JOINS_GAME, 0.2);
				}
				else if (r == 2)
				{
					frik_botSay("hey, everyone\n");
					frik_botConsiderStartingConversation(PR_TOPIC_TARG_JOINS_GAME, 0.2);
				}
				else if (r == 3)
				{
					frik_botSay("prepare to die!\n");        // PZ: was "prepare to be fragged!"
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 4)
				{
					frik_botSay("damn lag\n");               // PZ: was "boy this is laggy"
					frik_botConsiderStartingConversation(PR_TOPIC_LAG, 0.2);
				}
				else if (r == 5)
				{
					//frik_botSay(": #mm getting some lag here\n"); // PZ: hash refers to a #define to the CP preprocessor (it doesn't like it)
					frik_botSay("LAG\n");                    // PZ: was "getting some lag here"
					frik_botConsiderStartingConversation(PR_TOPIC_LAG, 0.2);
				}
				else
				{
					frik_botSay("i love this map\n");        // PZ: was "hi, everyone"
					frik_botConsiderStartingConversation(PR_TOPIC_TARG_JOINS_GAME, 0.2);
				}
			}
		}
		else if (b_topic == PR_TOPIC_SELF_GOT_A_KILL) // killed targ
		{
			if (b_topicStarter == self)
			{
				if (r == 1)
					frik_botSay("hahaha\n");   // PZ: was "take that"
				else if (r == 2)
					frik_botSay("sit down, sucka!\n");  // PZ: was "yehaww!"
				else if (r == 3)
					frik_botSay("w00t\n");     // PZ: was "wh00p"
				else if (r == 4)
					frik_botSay("pwnt\n");     // PZ: was "j00_sawk();"
				else if (r == 5)
					frik_botSay("u suck\n");   // PZ: was "i rule"
				else
					frik_botSay("eat it, son!\n"); // PZ: was "eat that"
				frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
			}
		}
		else if (b_topic == PR_TOPIC_SELF_INFORMS_TEAM_INVIS) // team message "friendly eyes"
		{
			if (b_topicStarter == self)
			{
				if (r < 3)
					frik_botSayTeam("friendly eyes\n", -1);
				else
					frik_botSayTeam("team eyes\n", -1);
				frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
			}
		}
		else if (b_topic == PR_TOPIC_SELF_INFORMS_TEAM_COVERING) // team message "got your back"
		{
			if (b_topicStarter != self) // PZ: was "== self"
			{
				if (r == 1)
					frik_botSayTeam("got your back\n", -1);            // PZ: was "on your back" ??
				else if (r == 2)
					frik_botSayTeam("i'm with you\n", -1);
				else if (r == 3)
					frik_botSayTeam("on my way\n", -1);
				else if (r == 4)
					frik_botSayTeam("covering\n", -1);
				else if (r == 5)
					frik_botSayTeam("got your six\n", -1);
				else
					frik_botSayTeam("okay.. hold on\n", -1);
				frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
			}
		}
		else if (b_topic == PR_TOPIC_SELF_INFORMS_TEAM_NEED_BACKUP) // team message "need back up"
		{
			if (b_topicStarter == self)
			{
				if (r == 1)
					frik_botSayTeam("where's my team??\n", -1);        // PZ: was "i need help"
				else if (r == 2)
					frik_botSayTeam("need backup\n", -1);
				else if (r == 3)
					frik_botSayTeam("help\n", -1);
				else if (r == 4)
					frik_botSayTeam("falling back\n", -1);
				else if (r == 5)
					frik_botSayTeam("there're too many!\n", -1);
				else
					frik_botSayTeam("follow me!\n", -1);
				frik_botConsiderStartingConversation(PR_TOPIC_SELF_INFORMS_TEAM_COVERING, 0.2);
			}
		}
		else if (b_topic == PR_TOPIC_SELF_DIED) // excuses for sucking
		{
			if (b_topicStarter == self)
			{
				if (r == 1)
				{
					frik_botSay("cat jumped on the keyboard\n");   // PZ: was "sun got in my eyes"
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 2)
				{
					frik_botSay("i've got to get rid of this ball mouse\n"); // PZ: was "mouse needs cleaning"
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 3)
				{
					frik_botSay("cheater\n");                      // PZ: was "i meant to do that"
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 4)
				{
					frik_botSay("lag\n");
					frik_botConsiderStartingConversation(PR_TOPIC_LAG, 0.2);
				}
				else if (r == 5)
				{
					frik_botSay("killer lag\n");
					frik_botConsiderStartingConversation(PR_TOPIC_LAG, 0.2);
				}
				else
				{
					frik_botSay("100% lag\n");
					frik_botConsiderStartingConversation(PR_TOPIC_LAG, 0.2);
				}
			}
		}
		// PZ: added these
		else if (b_topic == PR_TOPIC_SELF_LEAVES_GAME) // bot quits
		{
			if (b_topicStarter == self)
			{
				if (r == 1)
				{
					/*frik_botSayInit();
					frik_botSay2(b_topicStarter.netname);
					frik_botSay2(" is a cheating mofo\n");*/
					frik_botSay("cheating bastard\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 2)
				{
					frik_botSay("fcuk this\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 3)
				{
					frik_botSay("i'm out\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 4)
				{
					frik_botSay("cyas\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 5)
				{
					frik_botSay("gg, everyone\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else
				{
					frik_botSay("shit! gtg\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
			}
		}
		// PZ: END
		else if (b_topic == PR_TOPIC_MAP_ENDED) // map ended
		{
			if (r == 1)
				frik_botSay("gg\n");
			else if (r == 2)
				frik_botSay("gg all\n");
			else if (r == 3)
				frik_botSay("that was fun\n");
			else if (r == 4)
				frik_botSay("good game\n");
			else if (r == 5)
				frik_botSay("bah\n");           // PZ: was "pah" ?
			else
				frik_botSay("damn\n");          // PZ: was "hrm"
			frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
		}
		else if (b_topic == PR_TOPIC_TARG_JOINS_GAME) // welcoming someone onto server
		{
			if (b_topicStarter != self)
			{
				if (r == 1)
				{
					frik_botSay("heya\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 2)
				{
					frik_botSay("welcome\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 3)
				{
					frik_botSayInit();
					frik_botSay2("hi, ");
					frik_botSay2(b_topicStarter->netname);
					frik_botSay2("\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 4)
				{
					frik_botSayInit();
					frik_botSay2("hey, ");
					frik_botSay2(b_topicStarter->netname);
					frik_botSay2(". ready to get an ass whoopin?");  // PZ: added
					frik_botSay2("\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 5)
				{
					frik_botSay("wazzzzzup?\n");          // PZ: was "howdy"
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else
				{
					frik_botSay("sup?\n");                // PZ: was "lo"
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
			}
		}
		else if (b_topic == PR_TOPIC_TARG_KILLED_THEIRSELF) // ridicule lost frag (killed self?)
		{
			if (b_topicStarter != self)
			{
				if (r == 1)
					frik_botSay("hah\n");
				else if (r == 2)
					frik_botSay("heheh\n");
				else if (r == 3)
				{
					frik_botSayInit();
					frik_botSay2("good work, ");
					frik_botSay2(b_topicStarter->netname);
					frik_botSay2("\n");
				}
				else if (r == 4)
				{
					frik_botSayInit();
					frik_botSay2("nice1, ");
					frik_botSay2(b_topicStarter->netname);
					frik_botSay2("\n");
				}
				else if (r == 5)
					frik_botSay("lol\n");
				else
					frik_botSay(":)\n");
				b_topic = PR_TOPIC_SELF_DIED;  // PZ: was 6
			}
		}
		else if (b_topic == PR_TOPIC_TARG_KILLED_BY_ENVIRONMENT) // ridicule lost frag (lava)
		{
			if (b_topicStarter != self)
			{
				if (r == 1)
					frik_botSay("have a nice dip?\n");
				else if (r == 2)
					frik_botSay("ugh! I hate levels with lava\n");
				else if (r == 3)
				{
					frik_botSayInit();
					frik_botSay2("good job, ");
					frik_botSay2(b_topicStarter->netname);
					frik_botSay2("\n");
				}
				else if (r == 4)
				{
					frik_botSayInit();
					frik_botSay2("10/10.. perfect form, ");    // PZ: was "nice backflip, "
					frik_botSay2(b_topicStarter->netname);
					frik_botSay2("\n");
				}
				else if (r == 5)
					frik_botSay("watch your step\n");
				else
					frik_botSay("hehe\n");
				b_topic = PR_TOPIC_SELF_DIED;  // PZ: was 6
			}
		}
		else if (b_topic == PR_TOPIC_LAG) // lag
		{
			if (b_topicStarter != self)
			{
				if (r == 1)
				{
					frik_botSayInit();
					frik_botSay2("yeah right, ");
					frik_botSay2(b_topicStarter->netname);
					frik_botSay2("\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 2)
				{
					frik_botSay("ping\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 3)
				{
					frik_botSay("shuddup, you're an lpb\n");
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 4)
				{
					frik_botSay("lag, my ass\n");            // PZ: "ass" was "eye" (?)
					frik_botConsiderStartingConversation(PR_TOPIC_NONE, 0.2);
				}
				else if (r == 5)
				{
					frik_botSay("yeah\n");
					frik_botConsiderStartingConversation(PR_TOPIC_LAG, 0.2);
				}
				else
				{
					frik_botSay("less pow wow. more pow pow.\n");          // PZ: was "totally\n"
					frik_botConsiderStartingConversation(PR_TOPIC_LAG, 0.2);
				}
			}
		}
		self->b_chattime = 0; // PZ NOTE: bot is done talking
	}
	else if (b_topic)
	{
		if (random() < 0.5)
		{
			if (b_topicStarter == self)
			{
				if (b_topic <= PR_TOPIC_MAP_ENDED)  // PZ: was "7" rather than macro
					self->b_chattime = time + 2;   // PZ NOTE: simulates the bot typing; he'll say something in 2 seconds
			}
			else
			{
				if (b_topic >= PR_TOPIC_MAP_ENDED)  // PZ: was "7" rather than macro
					self->b_chattime = time + 2;   // PZ NOTE: simulates the bot typing; he'll say something in 2 seconds
			}
		}
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_removeABot      (PZ: was called "KickABot")

Removes the first bot in the list of entities.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
void frik_removeABot()
{
	entity e;
	e = find(world, "classname", "player");
	while (e != world)
	{
		if (!e->ishuman)
		{
			removeBot(e); // PZ: makes SV_DropClient() (C code) get called for the bot (this is going to call ClientDisconnect())
			lastTimeBotCountAdjusted = time; // delay next bot joining
			//frik_handleBotDisconnection(e); // PZ: and this gets called by ClientDisconnect()
			e->ishuman = PR_TRUE;
			e = world;
		}
		else
			e = find(e, "classname", "player");
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getRealOriginOfEntity   (PZ: was "realorigin")

Gets the position of the center of `ent`'s
bounding box.

Simplified origin checking. God, I wish I had inline.

PZ NOTE: This is especially useful on entities that
don't have .origin values, like brush entities. An
example brush entity is the standard teleporter.
Also, the player's origin isn't truly at the player's
center. Their .origin is around their crotch.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
vector frik_getRealOriginOfEntity(entity ent)
{
	return (ent->absmin + ent->absmax) * 0.5;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getDistanceBetween

PZ: Calculates the distance between `e1` and `e2`.
`bool_1toEdge` : measures from edge of `e1`, else from the true center of `e1`
`bool_2toEdge` : measures from edge of `e2`, else from the true center of `e2`

PZ: Nevermind. Calculating to the edge is tricky. Might want to make a C function for that one day, though.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
/*float(entity e1, float bool_1toEdge, entity e2, float bool_2toEdge) frik_getDistanceBetween =
{
	local vector pos1, pos2;

	if (!bool_1toEdge) pos1 = frik_getRealOriginOfEntity(e1); // from true center
	else               pos1 =
	if (!bool_2toEdge) pos2 = frik_getRealOriginOfEntity(e2); // from true center
	else

	v = frik_getRealOriginOfEntity(self.target1) - self.origin; // get distance to target1

};*/

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_isVisible_Simple     (PZ: was named "sisible")

Now this is getting ridiculous. Simple visible:
used when we need just a simple traceline, and nothing else.

`self` = the entity we are viewing from

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_isVisible_Simple(entity targ)
{
	traceline(self->origin + self->view_ofs, targ->origin, PR_TL_BSP_ONLY, self);    // PZ: Added `view_ofs` (12-4-15)
	if (trace_ent == targ)
		return PR_TRUE;
	else if (trace_fraction == 1)
		return PR_TRUE;
	return PR_FALSE;        // PZ: would never return false?? added this statement
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_isVisible_Complex    (PZ: was named "fisible")

A version of visible() that checks for corners
of the bounding boxes, and whether we can see
the target through the water surface.

`self` = the entity we are viewing from

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_isVisible_Complex(entity targ)
{
	vector viewPoint, positionOfTarg;
	float visibleThroughWater, pointContentsOfTarg, pointContentsOfViewPoint, isWaterVisOn;

	isWaterVisOn = cvar("watervis"); // PZ: added

	if (targ->classname == "player")   // dead bodies will have a maxs_z of -9, which could cause problems below where maxs is used for traceline()
		positionOfTarg = targ->origin;
	else
		positionOfTarg = frik_getRealOriginOfEntity(targ);
	viewPoint = self->origin + self->view_ofs;

//if (targ.classname == "holo")
//bprint(2, "frik_isVisible_Complex(): 1\n");
	if (targ->solid == PR_SOLID_BSP)
	{
		traceline(viewPoint, positionOfTarg, PR_TL_BSP_ONLY, self);
		if (trace_ent == targ)
			return PR_TRUE;
		else if (trace_fraction == 1)
			return PR_TRUE;
		return PR_FALSE;
	}
	else
	{
		pointContentsOfTarg = pointcontents(positionOfTarg);
		pointContentsOfViewPoint = pointcontents(viewPoint);
		if (targ->classname == "player")
			visibleThroughWater = PR_FALSE;
		else if (pointContentsOfTarg == PR_CONTENT_LAVA)
			return PR_FALSE;
		else
			visibleThroughWater = PR_TRUE;
	}
	// PZ: All #TL_EVERYTHING's, below, were #TL_BSP_ONLY. Changed 8-28-15.
	// PZ: Bots had trouble seeing you when you were standing in water, but not submerged completely. I fixed that below.
	if (pointContentsOfTarg < -1) // targ's origin is in water or other liquid
	{
//if (targ.classname == "holo")
//bprint(2, "frik_isVisible_Complex(): 2\n");
		if (pointContentsOfViewPoint != pointContentsOfTarg) // if bot is not in the same material
		{
			// look for their head
			traceline(viewPoint, positionOfTarg + targ->maxs, PR_TL_EVERYTHING, self); // PZ: .maxs was .mins
			// cross the water check
			if (trace_inopen)
				if (trace_inwater)
					if (!visibleThroughWater && !isWaterVisOn) // PZ: allow bots to see through water if cvar `watervis` is on
						return PR_FALSE;
			if (trace_ent == targ)
				return PR_TRUE;
			else if (trace_fraction == 1)
				return PR_TRUE;
			return PR_FALSE;
		}
	}
	else // target is in CONTENT_EMPTY (air), likely, since that's the highest, at -1
	{
//if (targ.classname == "holo")
//bprint(2, "frik_isVisible_Complex(): 3\n");
		if (pointContentsOfViewPoint != pointContentsOfTarg) // if bot is in some kind of material other than air
		{
			traceline(viewPoint, positionOfTarg + targ->maxs, PR_TL_EVERYTHING, self); // PZ NOTE: should be targ.mins for feet? Hmm. Going to keep it on head.
			if (trace_inopen)
				if (trace_inwater)
					if (!visibleThroughWater && !isWaterVisOn) // PZ: allow bots to see through water if cvar `watervis` is on
						return PR_FALSE;
			if (trace_ent == targ)
				return PR_TRUE;
			else if (trace_fraction == 1)
				return PR_TRUE;
			return PR_FALSE;
		}
	}
//if (targ.classname == "holo")
//bprint(2, "frik_isVisible_Complex(): 4\n");
	traceline(viewPoint, positionOfTarg, PR_TL_EVERYTHING, self);
	if (trace_ent == targ)
		return PR_TRUE;
	else if (trace_fraction == 1)
		return PR_TRUE;
	traceline(viewPoint, positionOfTarg + targ->maxs, PR_TL_EVERYTHING, self);   // top of its model?
	if (trace_ent == targ)
		return PR_TRUE;
	else if (trace_fraction == 1)
		return PR_TRUE;
	traceline(viewPoint, positionOfTarg + targ->mins, PR_TL_EVERYTHING, self);   // bottom of its model?
	if (trace_ent == targ)
		return PR_TRUE;
	else if (trace_fraction == 1)
		return PR_TRUE;
	return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_isVisible_Waypoint    (PZ: was named "wisible")

Used for waypoints.
Goes through movable brushes/entities.

// PZ NOTE: Appears to be used to see if a waypoint is visible from another waypoint.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// this is used for waypoint stuff....
float frik_isVisible_Waypoint(entity targ1, entity targ2)
{
	vector spot, positionOfTarg2;
	entity ignore;

	spot = targ1->origin;
	positionOfTarg2 = frik_getRealOriginOfEntity(targ2);

	ignore = self;
	do
	{
		traceline(spot, positionOfTarg2, PR_TL_BSP_ONLY, ignore);
		spot = frik_getRealOriginOfEntity(trace_ent);
		ignore = trace_ent;
	} while ((trace_ent != world) && (trace_fraction != 1));
	if (trace_endpos == positionOfTarg2)
		return PR_TRUE;
	else
		return PR_FALSE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_angleSubtraction     (PZ: was named "angcomp")

Subtracts one angle (y2) from another (y1).

// PZ NOTE: answer appears to be in the form of -180 to +180

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_angleSubtraction(float y1, float y2)
{
	y1 = frik_getAngleMod360(y1);
	y2 = frik_getAngleMod360(y2);

	float answer;
	answer = y1 - y2;
	if (answer > 180)
		answer = answer - 360;
	else if (answer < -180)
		answer = answer + 360;
	return answer;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_isInFovOfSelf         (PZ: was named "fov")

Is the entity, `targ`, in `self`'s field of view?

`self` = the entity we are viewing from

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// PZ TODO: adjust the FOV according to bot skill level?
float frik_isInFovOfSelf(entity targ)
{
	vector vec;
	float g;

	// get the angle to `targ` from the perspective of `self`
	vec = frik_getRealOriginOfEntity(targ);
	vec = (vec + targ->view_ofs) - (self->origin + self->view_ofs);
	vec = normalize(vec);
	vec = vectoangles(vec);

	// how far off in pitch are we? (.v_angle_x is pitch, right?)
	g = frik_angleSubtraction((self->v_angle[X] * -1), vec[X]);  // PZ: v_angle_x is negative when pitched up (not true for angles made with vectoangles())
	if (fabs(g) > 45)
		return PR_FALSE;
	// how far off in yaw are we?   (.v_angle_y is yaw, right?)
	g = frik_angleSubtraction(self->v_angle[Y], vec[Y]);
	if (fabs(g) > 60)
		return PR_FALSE;

	return PR_TRUE;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getAngleMod360      (PZ: was "frik_anglemod")

Returns `v` mod 360.
(limits an angle between 0 and 360 deg)

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_getAngleMod360(float v)
{
	return v - floor(v/360) * 360;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

frik_getAngleMod180

PZ: I'm not sure what angles in Quake go up to                         +Y +90
360, but many, like .v_angle, only go to 180.
                                                         -X  �180                    +X  0
Returns `v` mod 180.
(limits an angle between 0 and 180 deg)                                -Y -90

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
float frik_getAngleMod180(float v)
{
	float absV;
	absV = fabs(v);
	if (absV <= 180) return v;

	absV = absV - floor(absV/180) * 180;

	if      (v <= -180)   // if it's negative
		return 180 - absV;
	else if (v >= 180)    // if it's positive
		return -180 + absV;
	// PZ: I added this to satisfy C++ compiler. I don't feel like figuring out what it should return
	//     right now. So, this is what I did.
	else
	{
		string err = "frik_getAngleMod180(): warning, exceptional case (" + std::to_string(v) + "); returned 0\n";
		dprint(err);
		return 0;
	}
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

Generic LinkedList Module

PZ: I created this because I'm sick of writing linked list code that can be written somewhere once and reused.

Fields used by LinkedList nodes:
._next  :  the next node following this node
._prev  :  the prev node before this node

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/
// Adds a node to the end of the list `firstNode`, and returns the added node.
// Also give a string describing the purpose of the LinkedList, for debugging purposes (can see it with
// 'edicts' command and such). Perhaps give it the name of the function using the LinkedList.
// If the list is empty, it just creates a new node and returns it, for you to use as your first node.
entity LinkedList_addNodeToEndOf(entity firstNode, const string& LinkedList_description)
{
	entity ptr;

	if (firstNode == world)
	{
		ptr = spawnServerSide();
		ptr->classname = "LinkedList_Node";
		ptr->netname = LinkedList_description;
		return ptr;
	}
	ptr = firstNode;
	while (ptr->_next != world) ptr = ptr->_next;
	ptr->_next = spawnServerSide();
	ptr->_next->classname = "LinkedList_Node";
	ptr->_next->netname = LinkedList_description;
	ptr->_next->_prev = ptr;
	return ptr->_next;
}
// Removes a node from the list `firstNode`, and returns the first node in the list, in case
// it needs to change now. WARNING: You must use the return value to update your first node pointer.
entity LinkedList_removeNode(entity firstNode, entity nodeToRemove)
{
	entity ptr;

	ptr = firstNode;
	while (ptr != nodeToRemove && ptr->_next != world)
		ptr = ptr->_next;
	if (ptr == nodeToRemove)
	{
		if (ptr->_prev != world) ptr->_prev->_next = ptr->_next;
		if (ptr->_next != world) ptr->_next->_prev = ptr->_prev;
		if (ptr == firstNode) firstNode = ptr->_next;
		dremove(ptr);
	}
	else
		bprint(PR_PRINT_HIGH, "Error: LinkedList_removeNode() couldn't find node to remove.\n");

	return firstNode;
}
// Deletes all of the nodes in the list `firstNode`.
void LinkedList_deleteList(entity firstNode)
{
	entity ptr, prev;

	if (firstNode == world) return;

	ptr = firstNode;
	while (ptr->_next != world)
	{
		prev = ptr;
		ptr = ptr->_next;
		dremove(prev);
	}
	dremove(ptr);
}
// Swaps e1 and e2 in the linked list `firstNode`, and returns the first node in the list, in case
// it needs to change now. WARNING: You must use the return value to update your first node pointer.
entity LinkedList_swapNodes(entity firstNode, entity e1, entity e2)
{
	entity e1_prev_backup;

	e1_prev_backup = e1->_prev;
	//e1_next_backup = e1._next;

	// let's go ahead and do any outer nodes first
	if (e1->_prev != world) e1->_prev->_next = e2;
	if (e2->_next != world) e2->_next->_prev = e1;

	e1->_prev = e2;
	e1->_next = e2->_next;

	e2->_prev = e1_prev_backup;
	e2->_next = e1;

	if      (firstNode == e1) firstNode = e2;
	else if (firstNode == e2) firstNode = e1;

	return firstNode;
}

/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

PZ: Putting bot-related timer think functions down here. (Not every single one will be here.)

`self` : will be the timer itself, unless it's changed in the function

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

// PZ: Called when bot quits. Makes him delay actual quitting, so that he can potentially say goodbye first.
void timerThink_botQuit()
{
	// removeBot() causes change to `self`; so we need to remember `self`
	entity oldSelf;
	oldSelf = self;
	removeBot(self->owner); // the timer's owner is the bot
	//self.owner.ishuman = #TRUE; // PZ NOTE: be careful if you play with .ishuman; easy to break things
	lastTimeBotCountAdjusted = time; // delay next bot joining
	self = oldSelf;
	dremove(self); // Removes timer. DON'T FORGET THIS.
}

// PZ: Called when bot speaks. Allows us to realistically delay his speech, so that he appears to have to actually type it.
void timerThink_botFinishTypingMsg()
{
	if (self->flags == PR_BOT_SAY)
	{

	}
	else if (self->flags == PR_BOT_SAY_TEAM)
	{
		string temp;  entity t;  float i;
		temp = strcat(": ", self->message);           // PZ: let's not require putting ": " before every bot message in the code

		t = nextent(world);
		i = 1;
		while (i <= max_clients)
		{
			if (t->ishuman && t->is_connected && t->team_no == self->owner->team_no) // PZ: CustomTF uses ".team_no" rather than ".team"
			{
				msg_entity = t;                      // PZ NOTE: who to send to when #MSG_ONE
				WriteByte(PR_MSG_ONE, PR_SVC_PRINT);
				WriteByte(PR_MSG_ONE, PR_PRINT_CHAT);
				WriteByte(PR_MSG_ONE, 1);              // PZ NOTE: 1 or 2 as first character makes it colored (doesn't appear to make a difference whether it's 1 or 2)
				WriteByte(PR_MSG_ONE, 40);             // PZ NOTE: '(' character (the ASCII index, too)
				WriteString(PR_MSG_ONE, self->owner->netname);
				WriteByte(PR_MSG_ONE, PR_SVC_PRINT);
				WriteByte(PR_MSG_ONE, PR_PRINT_CHAT);
				WriteByte(PR_MSG_ONE, 2);              // PZ NOTE continuation about colored text: see Con_Print()
				WriteByte(PR_MSG_ONE, 41);             // PZ NOTE: ')' character (the ASCII index, too)
				WriteString(PR_MSG_ONE, temp);         // PZ: temp instead of msg
			}
			t = nextent(t); // PZ: changed
			i = i + 1;
		}

		self->owner->b_chattime = 0; // PZ: bot is done talking
	}
	delstr(self->message);
	dremove(self); // Removes timer. DON'T FORGET THIS.
}

// PZ: Called when bot selects a weapon. This allows the bots to handle weapons that share impulses with other weapons.
// It checks that the desired weapon has been selected. If not, it sets the same impulse again for the bot, so that the
// weapon will get selected eventually. This timer is removed once the bot reaches the desired weapon.
void timerThink_botEnsureWeaponSelection()
{
	if (self->owner->current_weapon == self->weapon/* || !self.owner.is_connected*/) // are we done?
	{
		self->owner->PR_b_bot_weaponCycler = world;
		dremove(self); // Removes timer. DON'T FORGET THIS.
	}
	else
	{
		self->owner->impulse = self->impulse;
		self->nextthink = time + 0.25;
	}
}
// for auto-removal of this timer when bot leaves or dies
// NOTE: timerCleanup functions must regard `self` as the bot that owns the timer
void timerCleanup_botEnsureWeaponSelection()
{
	//self.owner.#b_bot_weaponCycler = world;
	//dremove(self);
	dremove(self->PR_b_bot_weaponCycler);
	self->PR_b_bot_weaponCycler = world;
}

float roundFloat(float x)
{
	float y;
	y = x - floor(x);
	if (y >= 0.5) return ceil(x);
	else          return floor(x);
}

} // END namespace Progs
