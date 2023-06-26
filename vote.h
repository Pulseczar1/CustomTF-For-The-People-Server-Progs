#ifndef VOTE_H
#define VOTE_H

#include "vector"     // for std::vector
#include "algorithm"  // for std::sort()

namespace Progs {

// vote entity types
// Gizmo - if voting for AGR in coop, TF will be enabled with AGR
#define PR_VOTE_NONE        0   // Gizmo - this MUST be zero!
#define PR_VOTE_PUNISH      1
#define PR_VOTE_NEXTMAP     2   // Gizmo - use VOTEMAP_ macros to determine the type of next or switchmap vote
#define PR_VOTE_SWITCHMAP   3
#define PR_VOTE_USEOBJECT   4   // Gizmo - similar to admin use
#define PR_VOTE_POLL        5   // PZ: generic player polling for admins
#define PR_VOTE_BOT         6   // PZ: for "votebot"

// punish types
#define PR_PUNISH_NONE      0
#define PR_PUNISH_CURSE     1
#define PR_PUNISH_KICK      2
#define PR_PUNISH_BAN       3
#define PR_PUNISH_MUTE      4
#define PR_PUNISH_CUFF      5

// Gizmo - map vote types, nextmap or switchmap is determined by the #VOTE_*** type itself, not this
#define PR_VOTEMAP_DEFAULT       0    // the default mode the server has set
#define PR_VOTEMAP_AGR           1    // if TF is default, this acts normally, if coop is default then it'll be in TF mode with AGR
#define PR_VOTEMAP_INVADE        2    // PZ: Added Invade mode.
#define PR_VOTEMAP_NEO           3    // PZ: Added Neo mode.
#ifdef PR_COOP_MODE_ENHANCED
#define PR_VOTEMAP_TF            4    // only valid if the server has coop as default
#define PR_VOTEMAP_COOP          5    // only valid if the server has TF as default
#define PR_VOTEMAP_INSANECOOP    6    // only valid if the server has TF as default
#endif

// player voting status
#define PR_VOTED_READY           0
#define PR_VOTED_YES             1
#define PR_VOTED_NO              2

// Settings defines

#define PR_VOTE_TIME             45   // PZ: 60 seemed really long
#define PR_VOTE_MIN_TIME         10
#define PR_VOTE_TIME_REFRESH     25
#define PR_VOTE_DELAY            20   // PZ: was 240 // seconds to wait until another vote from same user is allowed

#ifdef PR_VOTING_TEST
#define PR_VOTE_DELAY            0    // for testing
#endif

// Fraction of votes required for each action
#define PR_VOTESNEEDED_CURSE     0.5
#define PR_VOTESNEEDED_MUTE      0.5
#define PR_VOTESNEEDED_KICK      0.5
#define PR_VOTESNEEDED_CUFF      0.5
#define PR_VOTESNEEDED_BAN       0.5
#define PR_VOTESNEEDED_MAP       0.5
#define PR_VOTESNEEDED_NEXTMAP   0.5
#define PR_VOTESNEEDED_BOT       0.5  // PZ
#define PR_VOTESNEEDED_USE       0.5
#define PR_VOTESNEEDED_POLL      0.5  // PZ

//#define PR_BAN_VOTED_TIME      0    // permanent ban
#define PR_BAN_VOTED_TIME        60   // 60 minutes
#define PR_MUTE_VOTED_TIME       30
#define PR_CUFF_VOTED_TIME       15

#define PR_VOTESLEEP_RATECHECK   5    // Rate in seconds of the think for sleeping votes
#define PR_VOTESLEEP_DURATION    10   // Maximum minutes for a vote to be sleeping before getting discarded

// PZ: This stores the votemap menu's data.
struct MapVoteMenuData
{
	std::vector<string> mapList;
	int numOfVoteMapPages;
};

// Functions
float IsMapVote();
void PrintVoteMapType(const string& preString, float voteMapType, const string& postString);
void AllResetMenu();
void Player_Punish();
void Menu_Punish();
void Menu_Punish_Input(float input);
void Player_VoteMap();
void Menu_VoteMap();
void Menu_VoteMap_Input(float input);
void bprintVoteIssue();
void Player_VoteYes();
void Player_VoteNo();
void PunishClientCycle();
void ResetVoteEnt();
float GetNoClients();
void StartPunishmentVote();
void PunishVote_Think();
void StartVoteMap(float nummap, string themap);
void VoteMap_Think();
void StartPoll(const string& question);
void StartUseObjectVote();
void CleanClientVotes();
void ResetClientsMenu();
float VotePassed(float numFor, float numAgainst, float fraction);
float CheckVoting(float final);
float ValidVoteEnt();
string GetMapString(float themap);
void bprintVotesNeeded(float num_for, float num_against);
void bprintVotesLeft();
void VoteToSleep();
void VoteSleep_think();
void CleanUpSleepVote();
entity GetSleepVote();
void SleepVoteResume(entity sleepent, entity targetclient);

} // END namespace Progs

#endif // END VOTE_H
