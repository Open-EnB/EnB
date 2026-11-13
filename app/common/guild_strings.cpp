// Earth & Beyond client (reconstruction): app\Common\guild_strings.cpp (__FILE__ 0xBAE080). Notes at the end of the file.
#include <stddef.h>
#include "guild_strings.h"
#include "..\shared\g_assert.h"

// A guild message type's sound and text (the text's $OtherName$ / $GuildName$ symbols are substituted by
// the caller, cpacket.cpp's GuildMessageSectorPacket::Process_Packet).
struct GuildStringStruct {	// NAME 1: what it holds (no client string names it)
	const char *Sound;	// +0x00: a sound name ('Guild_Error', 'Guild_Success' ...), or NULL  // NAME 2: what it holds
	const char *Text;	// +0x04  // NAME 2: what it holds
};







static GuildStringStruct _GuildStrings[GMSG_MAX] = {	// 0xBA9B00  // NAME 1: what it holds
	{ "Guild_Error", "INVALID" },
	{ "Guild_Success", "$OtherName$" },
	{ "Guild_Success", "$OtherName$" },
	{ "Guild_Listing", "$OtherName$" },
	{ "Guild_Listing", "$OtherName$" },
	{ "Guild_Listing", "$OtherName$" },
	{ "Guild_Announcement", "Guild MOTD : $OtherName$" },
	{ "Guild_Error", "Guild : Internal error while processing that command." },
	{ "Guild_Error", "Guild : $OtherName$ is not a valid player name." },
	{ "Guild_Error", "Guild : $GuildName$ is not a valid guild name." },
	{ "Guild_Usage", "Guild : You must be in a guild to issue this command." },
	{ "Guild_Usage", "Guild : Not implemented yet." },
	{ "Guild_Error", "Guild : Invalid command." },
	{ "Guild_Listing", "Available Guild Commands:\n-------------------------\n/gc list all\n/gc list offline\n/gc list online\n/gc list active\n/gc list inactive\n/gc create\n/gc recruit\n/gc promote\n/gc demote\n/gc remove\n/gc leave\n/gc disband\n/gc stats\n/gc message\n/gc publicstats\n/gc contributions\n/gc tag\n/gc rankname\n/gc view\n/gc permission[s] list\n/gc permission[s] add\n/gc permission[s] remove\n/gc set maxpromote\n/gc set mindemote\n/gc set maxremove\n/gc set active\n/gc set inactive\n" },
	{ "Guild_Usage", "Guild : Usage /gc promote [player name] - If you do not specify a player name, you must have the player targetted." },
	{ "Guild_Usage", "Guild : Usage /gc demote [player name] - If you do not specify a player name, you must have the player targetted." },
	{ "Guild_Usage", "Guild : Usage /gc recruit <player name> - You must specify the player name." },
	{ "Guild_Usage", "Guild : Usage /gc remove <player name> - You must specify the player name." },
	{ "Guild_Usage", "Guild : Usage /gc stats [guild name] - Request statistics for the named guild, or for your own guild if no name is specified." },
	{ "Guild_Usage", "Guild : Usage /gc publicstats <on | off> - You must specify 'on' or 'off'." },
	{ "Guild_Usage", "Guild : Usage /gc contributions <player name> - You must specify the name of a player in your guild." },
	{ "Guild_Usage", "Guild : Usage /gc tag <player name> \"tag string\" - You must specify both the player name and the tag string. The tag string must be enclosed in double-quotes if it contains more than one word." },
	{ "Guild_Usage", "Guild : Usage /gc rankname <current rank name / number 0-9> [new rank name] - Rank names must be wrapped in double-quotes if they are more than one word. If no new rank name is specified, the current rank name will be displayed. NOTE - $OtherName$ credits will be deducted if you rename a rank." },
	{ "Guild_Usage", "Guild : Usage /gc set maxpromote <rank name / number 0-9> [new max promote rank name / number 0-9] - You must specify the rank to modify. If you do not specify a new max promote rank, that ability will be revoked. Rank names must be wrapped in double-quotes if they are more than one word." },
	{ "Guild_Usage", "Guild : Usage /gc set maxremove <rank name / number 0-9> [new max remove rank name / number 0-9] - You must specify the rank to modify. If you do not specify a new max remove rank, that ability will be revoked. Rank names must be wrapped in double-quotes if they are more than one word." },
	{ "Guild_Usage", "Guild : Usage /gc set mindemote <rank name / number 0-9> [new min demote rank name / number 0-9] - You must specify the rank to modify. If you do not specify a new min demote rank, that ability will be revoked. Rank names must be wrapped in double-quotes if they are more than one word." },
	{ "Guild_Usage", "Guild : Usage /gc view <rank name / number 0-9> - You must specify a rank name (wrapped in double-quotes if more than one word) or a rank number 0-9." },
	{ "Guild_Usage", "Guild : Usage /gc permission[s] add <rank name / number 0-9> <permission name / number> - You must specify valid rank, and a permission to grant. Rank and permission names must be wrapped in double-quotes if they are more than one word. Permission numbers can be seen using /gc permission list." },
	{ "Guild_Usage", "Guild : Usage /gc permission[s] remove <rank name / number 0-9> <permission name / number> - You must specify valid rank, and a permission to remove. Rank and permission names must be wrapped in double-quotes if they are more than one word. Permission numbers can be seen using /gc permission list." },
	{ "Guild_Usage", "Guild : Usage /gc set active <player name> - You must specify the name of a player in your guild." },
	{ "Guild_Usage", "Guild : Usage /gc set inactive <player name> - You must specify the name of a player in your guild." },
	{ "Guild_Usage", "GM Command : Usage /gmgc disband \"guild name\" - You must specify the name of the guild to disband. Enclose in double-quotes if it is more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc rename \"guild name\" \"new guild name\" - You must specify the old and new guild names, and the names must be wrapped in double-quotes." },
	{ "Guild_Usage", "GM Command : Usage /gmgc remove <player name> - You must specify the player name." },
	{ "Guild_Usage", "GM Command : Usage /gmgc promote <player name> - You must specify the player name." },
	{ "Guild_Usage", "GM Command : Usage /gmgc demote <player name> - You must specify the player name." },
	{ "Guild_Usage", "GM Command : Usage /gmgc list [online] \"guild name\" - You must specify the name of the guild to examine. Enclose in double-quotes if it is more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc list offline \"guild name\" - You must specify the name of the guild to examine. Enclose in double-quotes if it is more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc list all \"guild name\" - You must specify the name of the guild to examine. Enclose in double-quotes if it is more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc list active \"guild name\" - You must specify the name of the guild to examine. Enclose in double-quotes if it is more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc list inactive \"guild name\" - You must specify the name of the guild to examine. Enclose in double-quotes if it is more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc rankname \"guild name\" <rank name / number 0-9> [new rank name] - Guild and rank names must be enclosed in double-quotes if they are more than one word. If no new rank name is specified, the current rank name will be displayed." },
	{ "Guild_Usage", "GM Command : Usage /gmgc view \"guild name\" <rank name / number 0-9> - Guild and rank names must be enclosed in double-quotes if they are more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc permission[s] add \"guild name\" <rank name / number 0-9> <permission name / number> - You must specify a valid guild name, rank, and a permission to grant. Guild, rank and permission names must be enclosed in double-quotes if they are more than one word. Permission numbers can be seen using /gmgc permission[s] list." },
	{ "Guild_Usage", "GM Command : Usage /gmgc permission[s] remove \"guild name\" <rank name / number 0-9> <permission name / number> - You must specify a valid guild name, rank, and a permission to remove. Guild, rank and permission names must be enclosed in double-quotes if they are more than one word. Permission numbers can be seen using /gmgc permission[s] list." },
	{ "Guild_Usage", "GM Command : Usage /gmgc modifyguildpoints \"guild name\" <num guild points> - You must enclose the guild name in double-quotes if it is more than one word. You can specify a positive or negative value for the number of guild points (just not zero)." },
	{ "Guild_Usage", "GM Command : Usage /gmgc set maxpromote \"guild name\" <rank name / number 0-9> [new max promote rank name / number 0-9] - You must specify the rank to modify. If you do not specify a new max promote rank, that ability will be revoked. Guild and rank names must be wrapped in double-quotes if they are more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc set maxremove \"guild name\" <rank name / number 0-9> [new max remove rank name / number 0-9] - You must specify the rank to modify. If you do not specify a new max remove rank, that ability will be revoked. Guild and rank names must be wrapped in double-quotes if they are more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc set mindemote \"guild name\" <rank name / number 0-9> [new min demote rank name / number 0-9] - You must specify the rank to modify. If you do not specify a new min demote rank, that ability will be revoked. Guild and rank names must be wrapped in double-quotes if they are more than one word." },
	{ "Guild_Usage", "GM Command : Usage /gmgc set active <player name> - You must specify the name of a player in your guild." },
	{ "Guild_Usage", "GM Command : Usage /gmgc set inactive <player name> - You must specify the name of a player in your guild." },
	{ "Guild_Usage", "GM Command : Usage /gmgc message \"guild name\" \"new message of the day\" - You must enclose the guild name and the new message of the day in double-quotes." },
	{ "Guild_Error", "Guild : All group members must not already be part of other guilds." },
	{ "Guild_Error", "Guild : $OtherName$ is already a founder of another pending guild, unable to continue guild formation." },
	{ "Guild_Error", "Guild : The guild name '$GuildName$' violates the Terms of Service agreement and cannot be used. Please choose another name for your guild." },
	{ "Guild_Error", "Guild : No player named '$OtherName$' was found." },
	{ "Guild_Error", "Guild : That guild name is already in use. Please choose another." },
	{ "Guild_Error", "Guild : A guild of that name is already in the process of being created. Try again later." },
	{ NULL, "Guild : The guild registration fee of $OtherName$ credits has been deducted from your account." },
	{ "Guild_Error", "Guild : You no longer have enough credits to cover the guild registration fee! Guild creation aborted." },
	{ NULL, "Guild : Your guild registration fee has been refunded." },
	{ "Guild_Error", "Guild : You must be in a guild in order to recruit a member!" },
	{ "Guild_Error", "Guild : You cannot recruit someone who is already in a guild." },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to recruit members!" },
	{ "Guild_Error", "Guild : The formation of $GuildName$ was aborted by $OtherName$!" },
	{ "Guild_Error", "Guild : Guild creation aborted because $OtherName$ left the sector!" },
	{ "Guild_Error", "Guild : $OtherName$ has declined your invitation to join $GuildName$!" },
	{ "Guild_Success", "Guild : $OtherName$ has been recruited into $GuildName$!" },
	{ "Guild_Error", "Guild : You must be in the same guild as $OtherName$ to use this command." },
	{ "Guild_Success", "Guild : Your guild rank has been set to '$OtherName$!'" },
	{ "Guild_Error", "Guild : You cannot promote yourself!" },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to promote $OtherName$!" },
	{ "Guild_Success", "Guild : You have promoted $OtherName$ to '$GuildName$' status!" },
	{ "Guild_Error", "Guild : You cannot demote yourself!" },
	{ "Guild_Error", "Guild : $OtherName$ cannot be demoted any further!" },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to demote $OtherName$!" },
	{ "Guild_Success", "Guild : You have demoted $OtherName$ to '$GuildName$' status!" },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to remove $OtherName$ from the guild!" },
	{ "Guild_Success", "Guild : You have removed $OtherName$ from the guild!" },
	{ "Guild_Success", "Guild : You have been removed from the guild by $OtherName$!" },
	{ "Guild_Error", "Guild : You may not leave the guild, you must promote another officer to leader first!" },
	{ "Guild_Success", "Guild : You are about to remove the leader of a Guild.  Make sure you have promoted some other guild member to leader before proceding!" },
	{ "Guild_Success", "Guild : You have left $GuildName$!" },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to disband the guild!" },
	{ "Guild_Success", "Guild : You have disbanded $GuildName$!" },
	{ "Guild_Success", "Guild : Your guild has been disbanded!" },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to set the guild's message of the day!" },
	{ "Guild_Success", "Guild : You have changed $GuildName$'s message of the day." },
	{ "Guild_Error", "Guild : The guild statistics of $GuildName$ are not available to non-members." },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to make the guild's statistics publicly visible!" },
	{ "Guild_Success", "Guild : $GuildName$'s statistics may now be seen by non-members." },
	{ "Guild_Success", "Guild : $GuildName$'s statistics may no longer be seen by non-members." },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to see $OtherName$'s guild contributions!" },
	{ "Guild_Error", "Guild : Your guild rank is not high enough to set $OtherName$'s guild tag!" },
	{ "Guild_Success", "Guild : You have set $OtherName$'s guild tag." },
	{ "Guild_Success", "Guild : You have joined $GuildName$!" },
	{ "Guild_Error", "Guild : You do not have permission to rename guild ranks!" },
	{ NULL, "Guild : Rank renamed to $OtherName$." },
	{ "Guild_Error", "Guild : That rank name violates the Terms of Service agreement. Please choose a different name." },
	{ "Guild_Listing", "Guild : Members of the \"$OtherName$\" rank have the following permissions:\nGuild : $GuildName$" },
	{ "Guild_Error", "Guild : You do not have permission to modify $OtherName$ rank abilites!" },
	{ "Guild_Error", "Guild : You cannot specify a Max Promote rank that is higher than the rank you are modifying." },
	{ "Guild_Error", "Guild : You cannot specify a Max Remove rank that is higher than the rank you are modifying." },
	{ "Guild_Error", "Guild : You can only specify a Min Demote rank that is at least two ranks below than the rank you are modifying." },
	{ "Guild_Success", "Guild : Members of $OtherName$ rank can now promote guild members up to the rank of $GuildName$" },
	{ "Guild_Success", "Guild : Members of $OtherName$ rank can now remove guild members up to the rank of $GuildName$" },
	{ "Guild_Success", "Guild : Members of $OtherName$ rank can now demote guild members down to the rank of $GuildName$" },
	{ "Guild_Success", "Guild : Members of $OtherName$ rank can no longer promote anyone." },
	{ "Guild_Success", "Guild : Members of $OtherName$ rank can no longer remove anyone." },
	{ "Guild_Success", "Guild : Members of $OtherName$ rank can no longer demote anyone." },
	{ "Guild_Listing", "Guild : Permissions that can be assigned to guild ranks:\n$OtherName$" },
	{ "Guild_Error", "Guild : The '$OtherName$' rank already has the '$GuildName$' permission." },
	{ "Guild_Error", "Guild : The '$OtherName$' rank does not have the '$GuildName$' permission." },
	{ "Guild_Success", "Guild : Successfully added the '$GuildName$' permission to '$OtherName$'." },
	{ "Guild_Success", "Guild : Successfully removed the '$GuildName$' permission from '$OtherName$'." },
	{ "Guild_Error", "Guild : You do not have permission to make somebody active." },
	{ "Guild_Error", "Guild : You do not have permission to make somebody inactive." },
	{ "Guild_Error", "Guild : $OtherName$ is already active." },
	{ "Guild_Error", "Guild : $OtherName$ is already inactive." },
	{ "Guild_Success", "Guild : $OtherName$ successfully marked as active." },
	{ "Guild_Success", "Guild : $OtherName$ successfully marked as inactive." },
	{ NULL, "Guild : You have been charged a fee of $OtherName$ credits." },
	{ "Guild_Announcement", "GUILD ANNOUNCEMENT : Your Guild Leader '$OtherName$' has not logged in for the past 30 days. '$GuildName$' has been promoted to Guild Leader in his/her place!" },
	{ "Guild_Announcement", "GUILD ANNOUNCEMENT : Your Guild Leader '$OtherName$' has deleted his/her character. '$GuildName$' has been promoted to Guild Leader in his/her place!" },
	{ "Guild_Announcement", "GUILD ANNOUNCEMENT : '$GuildName$' has been automatically promoted to Guild Leader because your guild was leaderless!" },
	{ "Guild_Error", "GM Command : Your admin level is not high enough to disband a guild!!" },
	{ "Guild_Success", "GM Command : Success! '$GuildName$' is no more!" },
	{ "Guild_Error", "GM Command : Your admin level is not high enough to rename a guild!!" },
	{ "Guild_Success", "GM Command : You have renamed '$OtherName$' to '$GuildName$'" },
	{ "Guild_Error", "GM Command : '$GuildName$' is not a valid guild name." },
	{ "Guild_Success", "GM Command : Guild points have been modified by $OtherName$. New guild point total is $GuildName$." },
	{ "Guild_Success", "GM Command : Successfully changed $GuildName$'s MOTD to '$OtherName$'" },
};

// 0x83D900
const char *Unknown_83D900(int message_type)
{
	FAIL_IF(message_type < GMSG_INVALID) return(NULL);
	FAIL_IF(message_type >= GMSG_MAX) return(NULL);
	return(_GuildStrings[message_type].Text);
}

// 0x83D970
const char *Unknown_83D970(int message_type)
{
	FAIL_IF(message_type < GMSG_INVALID) return(NULL);
	FAIL_IF(message_type >= GMSG_MAX) return(NULL);
	return(_GuildStrings[message_type].Sound);
}

/*
**	Reconstruction notes
**
**	The object: 0x83D900-0x83D9BA, its data the table 0xBA9B00 (132 entries of {sound, text}, one per
**	line: the FAIL_IFs sit on lines 157-158 and 165-166 right after it), the texts, then the __FILE__
**	string 0xBAE080; interrupt_defs.cpp follows (0x83D9E0, its names 0xBAE0B8). No guarded <string>
**	initialiser: the file does not include <string>.
*/
