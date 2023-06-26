/*=======================================================//
// mtfents.QC - CustomTF 3.2.OfN           - 30/1/2001 - //
// by Sergio Fumaña Grunwaldt - OfteN [cp]               //
=========================================================//
 I'll add mega-tf map entities support here
 sure there are differences with my implementation.
 Unsupported ones are simply removed after reporting it.
=========================================================*/

#include "progs.h"
#include "mtfents.h"
#include "ofndefs.h"
#include "debug.h"
#include "sprites.h"
#include "weapons.h"

namespace Progs {

#define PR_MEGATFGLASS_NUM   5 // Number of glasses

// PZ: From MegaTF code.
float RandomRange(float min, float max)
{
	float width;
	float offset;
	float result;
	width = max - min;
	offset = random() * width;
	result = offset + min;
	return result;
}
void Effect_Attack();
// PZ: END

void UnsupMegaTF()
{
	string st;

	st = "Warning: Unsupported MegaTF \"";
	st = strcat(st,self->classname);
	st = strcat(st,"\" entity is removed from the edicts.\n");

	RPrint(st);

	dremove(self);
}

float MegaTFMapEntExistence()
{
	string st;

	if (megatf & PR_MEGATF_MAPENTS)
		return PR_TRUE;

	st = "Warning: MegaTF entities are disabled, \"";
	st = strcat(st,self->classname);
	st = strcat(st,"\" entity is removed.\n");

	RPrint(st);

	dremove(self);
	return PR_FALSE;
}

// PZ: actual MegaTF code
#ifdef PR_0
void ambient_sound() // spawn function
{
	if (CheckExistence() == 0)
	{
		dremove(self);
		return;
	}
	if ((self->noise) == "")
	{
		objerror("ambient_sound without noise");
	}
	if (!(self->volume))
	{
		self->volume = 1;
	}
	precache_sound(self->noise);
	if (self->spawnflags == 4)
	{
		ambientsound(self->origin, self->noise, self->volume, 0);
	}
	else
	{
		ambientsound(self->origin, self->noise, self->volume, 1);
	}
}
#endif

void ambient_sound() // spawn function
{
	/*if (CheckExistence() == #FALSE)
	{
		dremove(self);
		return;
	}*/

	if (!MegaTFMapEntExistence()) return;

	if (self->noise != "")
	{
		if (self->volume == 0)
			self->volume = 1;

		float attn;
		// PZ: This is the original code.
		/*if (self.option == 0)
			attn = #ATTN_NORM;
		else
			attn = self.option;*/
		// PZ: This is like the MegaTF code. I went and got this because haloween.bsp seemed too quiet in this mod.
		if (self->spawnflags == 4)
			attn = PR_ATTN_NONE;   // 0
		else
			attn = PR_ATTN_NORM;   // 1

		precache_sound(self->noise);
		//precache_sound2(self.noise);

		ambientsound(self->origin, self->noise, self->volume, attn);
	}
	else
		dremove(self);
}

// PZ: actual MEGATF code
#ifdef PR_0
void Random_Play()
{
	if (self->spawnflags == 4)
	{
		sound(self, 0, self->noise, 1, 0);
	}
	else
	{
		sound(self, 0, self->noise, 1, 1);
	}
	self->nextthink = time + RandomRange(self->option, self->option2);
}
void effect_random_sound() // spawn function
{
	if (CheckExistence() == 0)
	{
		dremove(self);
		return;
	}
	if ((self->noise) == "")
	{
		objerror("random_sound without noise");
	}
	if (!(self->volume))
	{
		self->volume = 1;
	}
	precache_sound(self->noise);
	self->nextthink = time + RandomRange(self->option, self->option2);
	self->think = Random_Play;
}
#endif

void ers_think()
{
	float nextsound;
	nextsound = self->option + random()*(self->option2 - self->option);

	self->nextthink = time + nextsound;

	// PZ: Added this to make it like MegaTF code -- make the sound able to be heard all around the level.
	float attn;
	if (self->spawnflags == 4)
		attn = PR_ATTN_NONE;   // 0
	else
		attn = PR_ATTN_NORM;   // 1

	if (self->noise == "weaponfire")
	{
		float fsound;
		string thesound;
		fsound = random();
		if (fsound < 0.20)
			thesound = "effects/explode1.wav";
		else if (fsound < 0.40)
			thesound = "effects/explode2.wav";
		else if (fsound < 0.60)
			thesound = "effects/minigun.wav";
		else if (fsound < 0.80)
			thesound = "effects/m60.wav";
		else
			thesound = "effects/rocket.wav";

		sound(self, PR_CHAN_AUTO,   thesound, self->volume, attn); // PZ: was #CHAN_MISC; AUTO avoids overriding sounds; "attn" was #ATTN_NORM
	}
	else
		sound(self, PR_CHAN_AUTO, self->noise, self->volume, attn); // PZ: was #CHAN_MISC; AUTO avoids overriding sounds; "attn" was #ATTN_NORM
}
void effect_random_sound() // spawn function
{
	/*if (CheckExistence() == #FALSE)
	{
		dremove(self);
		return;
	}*/

	if (!MegaTFMapEntExistence()) return;

	if (self->noise != "")
	{
		if (self->volume == 0)
			self->volume = 1;

		precache_sound(self->noise);
		//precache_sound2(self.noise);

		if (self->option == 0)
			self->option = 5;
		if (self->option2 == 0)
			self->option2 = 10;
		if (self->option > self->option2)
			self->option = self->option2;

		float nextsound;
		nextsound = self->option + random()*(self->option2 - self->option);

		self->think = ers_think;
		self->nextthink = time + nextsound;

		//sound(self, #CHAN_MISC, self.noise, self.volume, #ATTN_NORM);
	}
	else
		dremove(self);
}

void ambient_jungle() // spawn function
{
	/*if (CheckExistence() == #FALSE)
	{
		dremove(self);
		return;
	}*/

	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/jungle.wav");
	ambientsound (self->origin, "ambience/jungle.wav", self->volume, PR_ATTN_NORM);
}

void ambient_diesel() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/diesel.wav");
	ambientsound (self->origin, "ambience/diesel.wav", self->volume, PR_ATTN_NORM);
}

void ambient_chopper() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/chopper.wav");
	ambientsound (self->origin, "ambience/chopper.wav", self->volume, PR_ATTN_NORM);
}

void ambient_eerie() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/eerie.wav");
	ambientsound (self->origin, "ambience/eerie.wav", self->volume, PR_ATTN_NORM);
}

void ambient_ocean() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/ocean.wav");
	ambientsound (self->origin, "ambience/ocean.wav", self->volume, PR_ATTN_NORM);
}

void ambient_peakwind() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/peakwind.wav");
	ambientsound (self->origin, "ambience/peakwind.wav", self->volume, PR_ATTN_NORM);
}

void ambient_lavapit() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/lavapit.wav");
	ambientsound (self->origin, "ambience/lavapit.wav", self->volume, PR_ATTN_NORM);
}

void ambient_unholy() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/unholy.wav");
	ambientsound (self->origin, "ambience/unholy.wav", self->volume, PR_ATTN_NONE);
}

void ambient_flagflap() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
		self->volume = 1;

	precache_sound ("ambience/flagflap.wav");

	/*local float attn_value;
	if (self.option == 0)
		attn_value = #ATTN_STATIC; // default is ATTN_STATIC
	else attn_value = #ATTN_NORM; // used in ditch2*/

	ambientsound (self->origin, "ambience/flagflap.wav", self->volume, PR_ATTN_NORM);
}

void ambient_high_wind() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/2windy.wav");
	ambientsound (self->origin, "ambience/2windy.wav", self->volume, PR_ATTN_NORM);
}

void ambient_meadow() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/meadow.wav");
	ambientsound (self->origin, "ambience/meadow.wav", self->volume, PR_ATTN_NORM);
}

void ambient_brook() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	float attn_value;
	if (self->option == 0)
		attn_value = PR_ATTN_NORM; // default is ATTN_NORM
	else attn_value = PR_ATTN_IDLE; // used in ditch2

	precache_sound ("ambience/brook.wav");
	ambientsound (self->origin, "ambience/brook.wav", self->volume, attn_value);
}

void ambient_ice_moving() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/icemove.wav");
	ambientsound (self->origin, "ambience/icemove.wav", self->volume, PR_ATTN_NORM);
}

void ambient_nightpond() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/nitepond.wav");
	ambientsound (self->origin, "ambience/nitepond.wav", self->volume, PR_ATTN_NORM);
}

void ambient_alert() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/redalert.wav");
	ambientsound (self->origin, "ambience/redalert.wav", self->volume, PR_ATTN_NONE);
}

void ambient_chant() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/chant.wav");
	ambientsound (self->origin, "ambience/chant.wav", self->volume, PR_ATTN_NORM);
}

void ambient_onboard() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/onboard.wav");
	ambientsound (self->origin, "ambience/onboard.wav", self->volume, PR_ATTN_NONE);
}

void ambient_rocket_engine() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	if (self->volume == 0)
	self->volume = 1;

	precache_sound ("ambience/onboard.wav");
	ambientsound (self->origin, "ambience/onboard.wav", self->volume, PR_ATTN_NORM);
}

void ambient_weaponfire() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	precache_sound("effects/explode1.wav");
	precache_sound("effects/explode2.wav");
	precache_sound("effects/minigun.wav");
	precache_sound("effects/m60.wav");
	precache_sound("effects/rocket.wav");
	//precache_sound2(self.noise);

	if (self->volume == 0)
	self->volume = 1;

	if (self->option==0)
		self->option=5;
	if (self->option2==0)
		self->option2=10;
	if (self->option > self->option2)
		self->option = self->option2;

	self->noise="weaponfire";

	float nextsound;
	nextsound = self->option + random()*(self->option2 - self->option);

	self->think = ers_think;
	self->nextthink = time + nextsound;
}

// TODO:

/*void() rf_splash =
{

};*/

void rf_touch()
{
	if (other->classname!="player" || self->pain_finished > time) return;

	if (other->cutf_items & PR_CUTF_STEALTH) return; // if we got stealth doesnt make the sound

	self->pain_finished = time + 6 + 6*random();
	self->solid = PR_SOLID_TRIGGER;

	if (other->velocity == V({0, 0, 0}))
		return;

	sound (self, PR_CHAN_MISC, "effects/rockfall.wav", 0.5, PR_ATTN_NORM);
}

void effect_rockfall() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	precache_sound("effects/rockfall.wav");
	setsize (self, V({-20, -20, -20}), V({20, 20, 20}));
	self->touch = rf_touch;
	self->solid = PR_SOLID_TRIGGER;
	setorigin (self, self->origin);
}

void ab_exp1()
{
	FRAME_STATE(0, ab_exp2);
	if (self->noise != "")
		sound(self, PR_CHAN_MISC, self->noise, self->owner->volume, PR_ATTN_NONE);

	SetSpriteThink();
}
void ab_exp2() {FRAME_STATE(1, ab_exp3); SetSpriteThink();}
void ab_exp3() {FRAME_STATE(2, ab_exp4); SetSpriteThink();}
void ab_exp4() {FRAME_STATE(3, ab_exp5); SetSpriteThink();}
void ab_exp5() {FRAME_STATE(4, ab_exp6); SetSpriteThink();}
void ab_exp6() {FRAME_STATE(5, SUB_Remove); SetSpriteThink();}

void ab_think()
{
	float nextburst;
	//local string thesound;
	nextburst = self->option + random()*(self->option2 - self->option);
	self->nextthink = time + nextburst;

	// Generate the explosion entity: //
	newmis=spawn();
	setmodel (newmis, "progs/s_explod.spr");
	newmis->origin[Z] = self->origin[Z];
	newmis->origin[X] = self->origin[X] - 300 + random()*600;
	newmis->origin[Y] = self->origin[Y] - 300 + random()*600;
	setorigin(newmis,newmis->origin);
	newmis->solid = PR_SOLID_NOT; // NEEDED?
	newmis->nextthink = time;
	newmis->think = ab_exp1;
	newmis->owner = self;
	newmis->touch = SUB_Null;

	newmis->movetype = PR_MOVETYPE_FLY;
	newmis->velocity = V({0, 0, 300});
	newmis->effects = PR_EF_BRIGHTLIGHT;

	if (random() < 0.65)
	newmis->noise = "effects/explode1.wav";
	else newmis->noise = "effects/explode2.wav";
}

void effect_airburst() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	precache_model ("progs/s_explod.spr");

	precache_sound("effects/explode1.wav");
	precache_sound("effects/explode2.wav");

	if (self->option==0)
		self->option=5;
	if (self->option2==0)
		self->option2=10;

	if (self->option > self->option2)
		self->option = self->option2;

	self->noise="weaponfire";

	if (self->volume == 0)
	self->volume = 1;

	float nextburst;
	nextburst = self->option + random()*(self->option2 - self->option);

	self->think = ab_think;
	self->nextthink = time + nextburst;
}

//- THIS IS FROM HYPNOTIC -//
void bobbingwater_think()
{
	vector ang;

	self->count = self->count + self->speed * ( time - self->ltime );

	if ( self->count > 360 )
	{
		self->count = self->count - 360;
	}

	ang[X] = self->count;
	ang[Y] = 0;
	ang[Z] = 0;
	makevectors( ang );
	self->origin[Z] = v_forward[Z] * self->cnt;
	setorigin( self, self->origin );
	self->ltime = time;
	self->nextthink = time + 0.02;
}

void func_bobbingwater() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	self->angles = V({0, 0, 0});
	self->movetype = PR_MOVETYPE_STEP;
	self->solid = PR_SOLID_NOT;

	setmodel (self,self->model);

	self->think = bobbingwater_think;

	self->count = 0;
	self->cnt = self->size[Z] / 2;

	if ( !self->speed )
	{
		self->speed = 4;
	}

	self->speed = 360 / self->speed;

	self->nextthink = time + 0.02;
	self->ltime = time;
}

//===============================================================
// Here comes two more models and 1 sound, sorry! :P

void spawnGLASS()
{
	entity glass;
	vector where;

	where[X] = self->absmin[X] + ((self->absmax[X] - self->absmin[X]) * random());
	where[Y] = self->absmin[Y] + ((self->absmax[Y] - self->absmin[Y]) * random());
	where[Z] = self->absmin[Z] + ((self->absmax[Z] - self->absmin[Z]) * random());

	glass = spawn();

	glass->origin = where;

	if (random() < 0.5)
		glass->mdl = "progs/glass1.mdl";
	else
		glass->mdl = "progs/glass2.mdl";

	setmodel(glass,glass->mdl);

	//setsize(glass,'-5 -5 -5','5 5 5');
	setsize (newmis, V({0, 0, 0}), V({0, 0, 0}));

	glass->classname = "gib";

	glass->velocity[X] = crandom()*60;
	glass->velocity[Y] = crandom()*60;
	glass->velocity[Z] = crandom()*60;

	glass->avelocity[X] = crandom()*450;
	glass->avelocity[Y] = crandom()*450;
	glass->avelocity[Z] = crandom()*450;

	glass->solid = PR_SOLID_NOT;
	glass->movetype = PR_MOVETYPE_BOUNCE;
	glass->takedamage = PR_DAMAGE_NO;

	#ifdef PR_COOL_GIBS
	glass->PR_coolflag = PR_STRFLAG_COOL;
	#endif

	glass->nextthink = time + 1.5 + random()*1.5;
	glass->think = SUB_Remove;
}

void glass_die()
{
	sound(self,PR_CHAN_BODY,"effects/shatter.wav",1,PR_ATTN_NORM);

	float numg;
	numg = 0;

	while (numg < PR_MEGATFGLASS_NUM)
	{
		spawnGLASS();
		numg = numg + 1;
	}

	dremove(self);
}

void func_glass() // spawn function
{
	if (!MegaTFMapEntExistence()) return;

	precache_model("progs/glass1.mdl");
	precache_model("progs/glass2.mdl");
	precache_sound("effects/shatter.wav");

	if (self->model != "") // Is this entity attached to a brush on BSP? (megatf ones)
	{
		self->movetype = PR_MOVETYPE_PUSH;
		self->solid = PR_SOLID_BSP;
		self->takedamage = PR_DAMAGE_YES;

		setmodel(self,self->model);

		setsize(self,self->mins,self->maxs);

		// Make it invisible
		self->model = "";
	}
	else // point entity then.. (this allows to add glasses to any map by only editing ents!)
	{
		self->movetype = PR_MOVETYPE_NONE;
		self->solid = PR_SOLID_BBOX;
		self->takedamage = PR_DAMAGE_YES;

		vector omin, omax;
		omin = self->absmin;
		omax = self->absmax;

		self->origin[X] = self->absmin[X] + (self->absmax[X] - self->absmin[X])/2;
		self->origin[Y] = self->absmin[Y] + (self->absmax[Y] - self->absmin[Y])/2;
		self->origin[Z] = self->absmin[Z] + (self->absmax[Z] - self->absmin[Z])/2;
		setorigin(self,self->origin);
		setsize(self,V({0, 0, 0})-((omax - omin)*0.5),(omax - omin)*0.5);
		self->absmin = omin;
		self->absmax = omax;
	}

	if (!self->health)
		self->health = 1;

	self->th_die = glass_die;
}

// PZ: I took this from the actual MegaTF code, which is in megatfents.qc. TODO: At some point, I want to just use all the code in that file for MegaTF entities.
void effect_lightning() // spawn function
{
	precache_sound("ambience/thunder1.wav");
	precache_model("progs/s_null.spr");
	self->classname = "effect_lightning";
	self->nextthink = time + RandomRange(5, 20);
	self->think = Effect_Attack;
}
void Effect_Attack()
{
	vector org;
	vector targ;
	entity Flash;
	entity Flash2;
	sound(self, PR_CHAN_VOICE, "ambience/thunder1.wav", 1, PR_ATTN_NONE);
	org = self->origin;
	org[X] = org[X] + RandomRange(self->spawnflags - 2 * self->spawnflags, self->spawnflags); // PZ NOTE: Why not just multiply spawnflags by -1 to get the minimum range?
	org[Y] = org[Y] + RandomRange(self->spawnflags - 2 * self->spawnflags, self->spawnflags);
	targ = self->origin - V({0, 0, 350});
	targ[X] = targ[X] + RandomRange(self->spawnflags - 2 * self->spawnflags, self->spawnflags);
	targ[Y] = targ[Y] + RandomRange(self->spawnflags - 2 * self->spawnflags, self->spawnflags);
	self->nextthink = time + RandomRange(1, 20);
	self->think = Effect_Attack;
	LightningDamage(org, targ, self, 100);
	Flash2 = spawn();
	setmodel(Flash2, "progs/s_null.spr");
	setorigin(Flash2, org);
	Flash2->movetype = PR_MOVETYPE_NOCLIP;
	Flash2->solid = PR_SOLID_NOT;
	Flash2->effects = PR_EF_BRIGHTLIGHT;
	Flash2->velocity = V({0, 0, -1500});
	Flash2->think = SUB_Remove;
	Flash2->nextthink = time + 0.6;
	setsize(Flash2, V({-8, -8, -8}), V({8, 8, 8}));
	Flash = spawn();
	setmodel(Flash, "progs/s_null.spr");
	setorigin(Flash, targ);
	Flash->movetype = PR_MOVETYPE_NOCLIP;
	Flash->solid = PR_SOLID_NOT;
	Flash->effects = PR_EF_BRIGHTLIGHT;
	Flash->velocity = V({0, 0, 1000});
	Flash->think = SUB_Remove;
	Flash->nextthink = time + 1;
	setsize(Flash, V({-8, -8, -8}), V({8, 8, 8}));
	WriteByte(PR_MSG_MULTICAST, PR_SVC_TEMPENTITY);
	if (random() <= 0.4)
		WriteByte(PR_MSG_MULTICAST, PR_TE_LIGHTNING1);
	else
		WriteByte(PR_MSG_MULTICAST, PR_TE_LIGHTNING3);
	WriteEntity(PR_MSG_MULTICAST, world);
	WriteCoord(PR_MSG_MULTICAST, org[X]);
	WriteCoord(PR_MSG_MULTICAST, org[Y]);
	WriteCoord(PR_MSG_MULTICAST, org[Z]);
	WriteCoord(PR_MSG_MULTICAST, targ[X]);
	WriteCoord(PR_MSG_MULTICAST, targ[Y]);
	WriteCoord(PR_MSG_MULTICAST, targ[Z]);
	multicast(targ, PR_MULTICAST_PVS);
}
// PZ: END

//============================================================
// Dummy spawn functions for unsupported megaTF crap

// Gizmo - real coop monsters now
#ifndef PR_COOP_MODE_ENHANCED
void monster_dog() // spawn function
{
	UnsupMegaTF();
}
#endif

void monster_turret() // spawn function
{
	UnsupMegaTF();
}

#ifndef PR_COOP_MODE_ENHANCED
void monster_fish() // spawn function
{
	UnsupMegaTF();
}

void monster_zombie() // spawn function
{
	UnsupMegaTF();
}
#endif

// PZ: I put the actual MegaTF code in (above) for this.
/*void() effect_lightning =
{
	UnsupMegaTF();
};*/

} // END namespace Progs
