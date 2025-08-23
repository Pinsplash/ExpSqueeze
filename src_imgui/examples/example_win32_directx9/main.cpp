// Dear ImGui: standalone example application for DirectX 9

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "..\json-parser\json.h"
#include "implot.h"
#include "implot_internal.h"
#include <stdio.h>
#include <sys/stat.h>
#include <d3d9.h>
#include <tchar.h>
#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <complex>
#include <future>

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static bool                     g_DeviceLost = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace std;

enum
{
	GAME_RED,
	GAME_BLUE,
	GAME_YELLOW,
	GAME_GOLD,
	GAME_SILVER,
	GAME_CRYSTAL,
	GAME_RUBY,
	GAME_SAPPHIRE,
	GAME_EMERALD,
	GAME_FIRERED,
	GAME_LEAFGREEN,
	GAME_DIAMOND,
	GAME_PEARL,
	GAME_PLATINUM,
	GAME_HEARTGOLD,
	GAME_SOULSILVER,
	GAME_BLACK1,
	GAME_WHITE1,
	GAME_BLACK2,
	GAME_WHITE2,
	GAME_X,
	GAME_Y,
	GAME_SUN,
	GAME_MOON,
	GAME_ULTRASUN,
	GAME_ULTRAMOON,
	ALLGAMES_INDEX,
	GAMES_TOTAL
};
enum
{
	METHOD_WALK,
	METHOD_SURF,
	METHOD_OLDROD,
	METHOD_GOODROD,
	METHOD_SUPERROD,
	METHOD_ROCKSMASH,
	METHOD_HEADBUTTLOW,
	METHOD_HEADBUTTHIGH,
	METHOD_UNUSED,
	METHOD_DARK_GRASS,
	METHOD_PHEN_GRASS,
	METHOD_PHEN_DUST,
	METHOD_PHEN_SHADOW,
	METHOD_PHEN_WATER_FISH,
	METHOD_PHEN_WATER_SURF,
	METHOD_FLOWER_RED,
	METHOD_FLOWER_YELLOW,
	METHOD_FLOWER_PURPLE,
	METHOD_SWAMP,
	METHOD_BUBBLE_ROCK,
	METHOD_AMBUSH_GRASS,
	METHOD_AMBUSH_BUSH,
	METHOD_AMBUSH_SPLASH,
	METHOD_AMBUSH_TREE,
	METHOD_AMBUSH_DIRT,
	METHOD_AMBUSH_SHADOW,
	METHOD_AMBUSH_CHASE,
	METHOD_AMBUSH_SAND,
	METHODS_TOTAL
};

enum MethodFilterFlags
{
	MethodFilterFlags_None			= 0,
	MethodFilterFlags_Walk			= 1 << 0,
	MethodFilterFlags_Surf			= 1 << 1,
	MethodFilterFlags_RodOld		= 1 << 2,
	MethodFilterFlags_RodGood		= 1 << 3,
	MethodFilterFlags_RodSuper		= 1 << 4,
	MethodFilterFlags_RockSmash		= 1 << 5,
	MethodFilterFlags_Headbutt		= 1 << 6,
	MethodFilterFlags_Unused		= 1 << 7,
	MethodFilterFlags_DarkGrass		= 1 << 8,
	MethodFilterFlags_Phenomena		= 1 << 9,
	MethodFilterFlags_BubblingSpots = 1 << 10,
	MethodFilterFlags_Ambush		= 1 << 11,
	MethodFilterFlags_Last			= 1 << 12,
};

enum TypeFlags
{
	TypeFlags_None		= 0,
	TypeFlags_Normal	= 1 << 0,
	TypeFlags_Fighting	= 1 << 1,
	TypeFlags_Flying	= 1 << 2,
	TypeFlags_Poison	= 1 << 3,
	TypeFlags_Ground	= 1 << 4,
	TypeFlags_Rock		= 1 << 5,
	TypeFlags_Bug		= 1 << 6,
	TypeFlags_Ghost		= 1 << 7,
	TypeFlags_Steel		= 1 << 8,
	TypeFlags_Fire		= 1 << 9,
	TypeFlags_Water		= 1 << 10,
	TypeFlags_Grass		= 1 << 11,
	TypeFlags_Electric	= 1 << 12,
	TypeFlags_Psychic	= 1 << 13,
	TypeFlags_Ice		= 1 << 14,
	TypeFlags_Dragon	= 1 << 15,
	TypeFlags_Dark		= 1 << 16,
	TypeFlags_Fairy		= 1 << 17,
};

enum FilterReasons
{
	Reason_None,
	Reason_BadType,
	Reason_NoGoodTypes,
	Reason_BadEVs,
	Reason_NoGoodEVs,
	Reason_OverLevelCap,
	Reason_BadProgress,
	Reason_LackingDuplicateMons
};

enum MilestoneType
{
	MILESTONE_NONE,
	MILESTONE_CHECKPOINT,
	MILESTONE_ONEWAY,
	MILESTONE_CHECKBOX
};

//offsets for bulba's yield tables and EncounterTable's averageYields vector
#define OFFSET_HP 0
#define OFFSET_ATTACK 1
#define OFFSET_DEFENSE 2
#define OFFSET_SP_ATTACK 3//gen 1 special stat goes in both spa and spd
#define OFFSET_SP_DEFENSE 4//gen 1 special stat goes in both spa and spd
#define OFFSET_SPEED 5
#define OFFSET_TOTAL 6
#define OFFSET_EXP 7
#define OFFSET_BEY 8

#define GENERATION_ALL 0

struct Settings
{
	int wantedgame_index = 0;
	string wantedtime;
	string wantedseason;
	bool wantswarm = false;
	bool wantradar = false;
	string wantedslot2game;
	string wantedradiostation;
	int repellevel = 0;
	int maxallowedlevel = 100;
	bool printtext = false;
	int methodflags = MethodFilterFlags_Last - 1;
	int pkmnfiltertypeflags = 0;
	int pkmnrequiretypeflags = 0;
	bool pkmntypewarn = false;
	int scalinglevel = 0;
	bool abilitywarn = true;
	bool useprogressfilter = false;
	int selected_checkpoint_slot = 0;
	std::vector<float> minAvgEV = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::vector<float> maxAvgEV = {255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 680.0f};
	std::vector<int> minSingleMonEV = {0, 0, 0, 0, 0, 0, 0};
	std::vector<int> maxSingleMonEV = {255, 255, 255, 255, 255, 255, 680 };
	std::vector<int> partyYields = { 1, 0, 0, 0, 0, 0, 1, -10000, 61 };
};

struct SettingsWindowData
{
	int generation_lastframe = 0;
	int wantedgame_index_lastframe = 0;
	int time_chosen = 0;
	bool running = false;
	double progress = 0;
};

struct Encounter
{
	__int64 chance = 0;
	__int64 minlevel = 0;
	__int64 maxlevel = 0;
	string pokemonname;
	double avgexp = 0;
	double avgexpweighted = 0;
	int baseExp = 0;
	bool specialswarm = false;
};

struct EfficientEVData
{
	string pokemonname;
	double expPerEV = 0.0;
};

struct EncounterTable
{
	int method_index = 0;
	string placename;
	string walkstring;
	vector<Encounter> encounters;
	int filenumber = 0;
	__int64 expectedtotalpercent = 0;
	__int64 totalchance = 0;
	int filterReason = Reason_None;
	int version_index = 0;
	string header;
	string warning;
	__int64 lowestlevel = 0;
	__int64 highestlevel = 0;
	bool goodtype = false;
	bool goodEVs = false;
	std::vector<double> averageYields = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	EfficientEVData efficientEVs[7];
};

struct GameObject
{
	string uiname;
	string internalname;
	string expfile;
	string progressfile;
	int generation = 0;
	vector<int> folderRanges;
};

vector<GameObject*> g_games;

struct MethodObject
{
	string uiname;
	string internalname;
	int flag = 0;
};

struct MethodExclude
{
	size_t slot;//slot in the tables vector of a Milestone that this exclusion applies to
	string str;//method(s) to exclude. multiple methods can be delimited by commas.
};

struct Milestone
{
	string name;
	MilestoneType type = MILESTONE_NONE;
	size_t parentslot = ((std::size_t)(-1));//it's not suitable to simply look at the previous slot in the Milestone list as that may be a checkbox
	int id = 0;
	vector<int> tables;
	vector<int> removes;
	vector<int> cancels;
	vector<string> unlocks;
	vector<MethodExclude*> excludes;
	bool userselected = false;
};

vector<MethodObject*> g_methods;
vector<EncounterTable> maintables;
vector<Milestone> g_milestones;
vector<string> g_checkpointnames;
string g_pkmndatapath = "pkmndata/";
Settings g_settings, g_newsettings;
SettingsWindowData g_settingswindowdata;

#ifdef _DEBUG
vector<string> g_debugdata;
//dumb thing for collecting whatever data i need for some purpose
static void RecordCustomData(string condition)
{
	bool found = false;
	for (string str : g_debugdata)
	{
		if (str == condition)
		{
			found = true;
		}
	}
	if (!found)
	{
		g_debugdata.push_back(condition);
	}
}

static void PrintCustomData(/*Settings* settings*/)
{
	//cout << settings->wantedgame << "\n";
	for (string str : g_debugdata)
	{
		//cout << "condition " << str << "\n";
	}
}
#endif //_DEBUG
/*
static void PrintMethodFlags(int flags)
{
	cout << "Method Flags:\n";
	if (flags == 0) cout << "-MethodFilterFlags_None\n";
	if (flags & MethodFilterFlags_Walk) cout << "-MethodFilterFlags_Walk\n";
	if (flags & MethodFilterFlags_Surf) cout << "-MethodFilterFlags_Surf\n";
	if (flags & MethodFilterFlags_RodOld) cout << "-MethodFilterFlags_RodOld\n";
	if (flags & MethodFilterFlags_RodGood) cout << "-MethodFilterFlags_RodGood\n";
	if (flags & MethodFilterFlags_RodSuper) cout << "-MethodFilterFlags_RodSuper\n";
	if (flags & MethodFilterFlags_RockSmash) cout << "-MethodFilterFlags_RockSmash\n";
	if (flags & MethodFilterFlags_Headbutt) cout << "-MethodFilterFlags_Headbutt\n";
	if (flags & MethodFilterFlags_DarkGrass) cout << "-MethodFilterFlags_DarkGrass\n";
	if (flags & MethodFilterFlags_Phenomena) cout << "-MethodFilterFlags_Phenomena\n";
	if (flags & MethodFilterFlags_BubblingSpots) cout << "-MethodFilterFlags_BubblingSpots\n";
	if (flags & MethodFilterFlags_Ambush) cout << "-MethodFilterFlags_Ambush\n";
}

static void PrintTypeFlags(int flags)
{
	if (flags == 0) cout << "-TypeFlags_None\n";
	if (flags & TypeFlags_Normal) cout << "-TypeFlags_Normal\n";
	if (flags & TypeFlags_Fighting) cout << "-TypeFlags_Fighting\n";
	if (flags & TypeFlags_Flying) cout << "-TypeFlags_Flying\n";
	if (flags & TypeFlags_Poison) cout << "-TypeFlags_Poison\n";
	if (flags & TypeFlags_Ground) cout << "-TypeFlags_Ground\n";
	if (flags & TypeFlags_Rock) cout << "-TypeFlags_Rock\n";
	if (flags & TypeFlags_Bug) cout << "-TypeFlags_Bug\n";
	if (flags & TypeFlags_Ghost) cout << "-TypeFlags_Ghost\n";
	if (flags & TypeFlags_Steel) cout << "-TypeFlags_Steel\n";
	if (flags & TypeFlags_Fire) cout << "-TypeFlags_Fire\n";
	if (flags & TypeFlags_Water) cout << "-TypeFlags_Water\n";
	if (flags & TypeFlags_Grass) cout << "-TypeFlags_Grass\n";
	if (flags & TypeFlags_Electric) cout << "-TypeFlags_Electric\n";
	if (flags & TypeFlags_Psychic) cout << "-TypeFlags_Psychic\n";
	if (flags & TypeFlags_Ice) cout << "-TypeFlags_Ice\n";
	if (flags & TypeFlags_Dragon) cout << "-TypeFlags_Dragon\n";
	if (flags & TypeFlags_Dark) cout << "-TypeFlags_Dark\n";
	if (flags & TypeFlags_Fairy) cout << "-TypeFlags_Fairy\n";
}
*/

static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static void WarnMarker(const char* desc)
{
	ImGui::TextWarn("(!)");
	if (ImGui::BeginItemTooltip())
	{
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static bool MilestoneCancelsID(int slot, int subjectID)
{
	Milestone ms = g_milestones[slot];
	for (int cancel : ms.cancels)
	{
		if (cancel == subjectID)
		{
			if (ms.id == subjectID)
			{
				//milestone cannot cancel itself!!
				assert(false);
			}
			return true;
		}
	}
	return false;
}

//see that no milestone between #0 and checkpoint_current is suppressing id (not including the effect of oneways)
static bool MilestoneIsRelevant(int subjectslot, int start, int checkpoint_current)
{
	int subjectID = g_milestones[subjectslot].id;

	if (!subjectID)
		return true;

	bool pastownslot = subjectslot <= start;

	//keep iterating through milestones so we can set foundself = true when the checkpoint_current is the parent of a checkbox
	bool gountilnextcheckpoint = false;

	for (int slot = start; (slot < checkpoint_current + 1 || gountilnextcheckpoint) && slot < g_milestones.size(); slot++)
	{
		Milestone ms = g_milestones[slot];

		if (ms.id == subjectID)
		{
			for (int cancel : ms.cancels)
			{
				if (cancel == subjectID)
				{
					//milestone cannot cancel itself!!
					assert(false);
				}
			}
		}

		if (subjectslot <= slot)
		{
			pastownslot = true;
		}

		switch (ms.type)
		{
		case MILESTONE_ONEWAY:
			if (gountilnextcheckpoint)
				return pastownslot;
			if (MilestoneCancelsID(slot, subjectID) && MilestoneIsRelevant(slot, slot + 1, checkpoint_current))
				return false;
			break;
		case MILESTONE_CHECKBOX:
			if (ms.userselected && MilestoneCancelsID(slot, subjectID) && MilestoneIsRelevant(ms.id, slot + 1, checkpoint_current))
				return false;
			break;
		case MILESTONE_CHECKPOINT:
			if (gountilnextcheckpoint)
				return pastownslot;
			if (MilestoneCancelsID(slot, subjectID))
				return false;
			break;
		case MILESTONE_NONE:
		default:
			assert(false);
			break;
		}

		if (slot == checkpoint_current)
			gountilnextcheckpoint = true;
	}
	return pastownslot;
}

static bool EncounterIsAccessible(int tablenum, string method)
{
	bool excludebymethod = false;
	bool foundtable = false;
	bool removed = false;
	vector<string> unlockedmethods;
	for (int slot = 0; slot < g_milestones.size(); slot++)
	{
		Milestone ms = g_milestones[slot];
		if (ms.userselected && MilestoneIsRelevant(slot, 0, g_newsettings.selected_checkpoint_slot))
		{
			if (foundtable && ms.type == MILESTONE_ONEWAY)
			{
				return false;
			}
			for (int Jslot = 0; Jslot < ms.tables.size(); Jslot++)
			{
				int table = ms.tables[Jslot];
				if (tablenum == table)
				{
					foundtable = true;
					removed = false;
					if (!ms.excludes.empty())
					{
						bool thistableexcludes = false;
						for (MethodExclude* exclude : ms.excludes)
						{
							if (exclude->slot == Jslot)
							{
								if (exclude->str.find(method) != string::npos)
								{
									//don't return yet. there might be a later milestone that unlocks our target method.
									excludebymethod = true;
									thistableexcludes = true;
								}
							}
						}
						if (!thistableexcludes)
						{
							excludebymethod = false;
						}
					}
					else
					{
						excludebymethod = false;
					}
				}
			}
			if (!ms.removes.empty())
			{
				for (int remove : ms.removes)
				{
					if (tablenum == remove)
					{
						//don't return yet. there might be a later milestone that re-adds this table.
						removed = true;
					}
				}
			}
			for (int Jslot = 0; Jslot < ms.unlocks.size(); Jslot++)
			{
				unlockedmethods.push_back(ms.unlocks[Jslot]);
			}
		}
	}
	bool methodavailable = std::find(unlockedmethods.begin(), unlockedmethods.end(), method) != unlockedmethods.end();
	return foundtable && !excludebymethod && methodavailable && !removed;
}

static void RegisterEncounter(__int64 chance, __int64 minlevel, __int64 maxlevel, string pokemonname, string placename, string walkstring, int method_index, int version_index, int i, int filterReason, string warning, bool goodtype, bool goodEVs)
{
	if (g_settings.repellevel > maxlevel)
		return;
	
	if (g_settings.useprogressfilter && !EncounterIsAccessible(i, g_methods[method_index]->internalname))
		filterReason = Reason_BadProgress;

	if (g_settings.maxallowedlevel < maxlevel)
		filterReason = Reason_OverLevelCap;

	Encounter newEnc;
	newEnc.chance = chance;
	newEnc.maxlevel = maxlevel;
	newEnc.minlevel = minlevel;
	newEnc.pokemonname = pokemonname;
	bool makenewtable = true;
	for (EncounterTable& table : maintables)
	{
		if (table.placename == placename && table.method_index == method_index && (g_settings.wantedgame_index != ALLGAMES_INDEX || table.version_index == version_index))
		{
			//don't lose our reason just because another encounter was ok
			//prioritize OverLevelCap because BadType may simply be a warning
			if (filterReason != Reason_None && table.filterReason != Reason_OverLevelCap)
				table.filterReason = filterReason;

			makenewtable = false;
			//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
			table.encounters.push_back(newEnc);
			table.expectedtotalpercent += chance;
			table.lowestlevel = min(table.lowestlevel, minlevel);
			table.highestlevel = max(table.highestlevel, maxlevel);
			//if any type is good, the table is good
			if (goodtype)
				table.goodtype = true;
			if (goodEVs)
				table.goodEVs = true;

			if (!warning.empty())
			{
				if (table.warning.empty())
				{
					table.warning = warning;
				}
				else
				{
					table.warning += "\n" + warning;
				}
			}
			break;
		}
	}
	if (makenewtable)
	{
		//cout << "Line " << linenum << ": new table\n";
		EncounterTable* newTable = new EncounterTable;
		newTable->method_index = method_index;
		newTable->placename = placename;
		newTable->walkstring = walkstring;
		newTable->filenumber = i;
		newTable->expectedtotalpercent = chance;
		newTable->version_index = version_index;
		newTable->filterReason = filterReason;
		newTable->warning = warning;
		newTable->lowestlevel = minlevel;
		newTable->highestlevel = maxlevel;
		newTable->goodtype = goodtype;
		newTable->goodEVs = goodEVs;
		//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ". (new table)\n";
		newTable->encounters.push_back(newEnc);

		maintables.push_back(*newTable);
	}
}

static bool isEqualString(char* initial, char* compare)
{
	return !strcmp(initial, compare);
}

static json_value* FindArrayInObjectByName(json_value* initialObject, char* name)
{
	assert(initialObject->type == json_object);
	int arrayLength = initialObject->u.object.length;
	//cout << "FindArrayInObjectByName: arrayLength: " << to_string(arrayLength) << "\n";
	for (int i = 0; i < arrayLength; i++)
	{
		json_object_entry obj = initialObject->u.object.values[i];
		//cout << "FindArrayInObjectByName: " + to_string(i) + " "; ExplainObjectEntry(obj);
		if (obj.value->type == json_array)
		{
			if (isEqualString(obj.name, name))
			{
				//return index array is at in object
				//cout << "FindArrayInObjectByName: " + to_string(i) + " MATCHED\n";
				return obj.value;
			}
		}
	}
	return NULL;
}

static json_value* FindValueInObjectByKey(json_value* initialObject, char* key)
{
	assert(initialObject->type == json_object);
	int arrayLength = initialObject->u.object.length;
	//cout << "FindValueInObjectByKey: arrayLength: " << to_string(arrayLength) << "\n";
	for (int i = 0; i < arrayLength; i++)
	{
		json_object_entry obj = initialObject->u.object.values[i];
		//cout << "FindValueInObjectByKey: " + to_string(i) + " "; ExplainObjectEntry(obj);
		if (isEqualString(obj.name, key))
		{
			//cout << "FindValueInObjectByKey: " + to_string(i) + " MATCHED\n";
			return obj.value;
		}
	}
	return NULL;
}

static json_value* FindObjectInObjectByName(json_value* initialObject, char* name)
{
	assert(initialObject->type == json_object);
	int arrayLength = initialObject->u.object.length;
	//cout << "FindObjectInObjectByName: arrayLength: " << to_string(arrayLength) << "\n";
	for (int i = 0; i < arrayLength; i++)
	{
		json_object_entry obj = initialObject->u.object.values[i];
		//cout << "FindObjectInObjectByName: " + to_string(i) + " "; ExplainObjectEntry(obj);
		if (obj.value->type == json_object && isEqualString(obj.name, name))
		{
			//return index object is at in object
			//cout << "FindObjectInObjectByName: " + to_string(i) + " MATCHED\n";
			return obj.value;
		}
	}
	return NULL;
}

static bool InvalidateCondition(string condition, int iFile)
{
	if (condition == "time-morning" || condition == "time-day" || condition == "time-night")
	{
		string wantedtime = g_settings.wantedtime;
		//no morning in gen 7. if the wanted game is All and the wanted time is morning, pretend we wanted day instead
		if (g_settings.wantedtime == "time-morning" && iFile >= 1035)
			wantedtime = "time-day";
		//time: morning/day/night (relevant in gens 2, 4, 7)
		if (wantedtime != condition)
			return true;
	}
	if (condition == "season-spring" || condition == "season-summer" || condition == "season-autumn" || condition == "season-winter")
	{
		//season: spring/summer/autumn/winter (gen 5)
		if (g_settings.wantedseason != condition)
			return true;
	}
	if (condition == "swarm-yes" || condition == "swarm-no")
	{
		//swarm: yes/no (gen 2-5)
		if (g_settings.wantswarm != (condition == "swarm-yes"))
			return true;
	}
	if (condition == "radar-on" || condition == "radar-off")
	{
		//radar: on/off (DPP)
		if (g_settings.wantradar != (condition == "radar-on"))
			return true;
	}
	if (condition == "slot2-none" || condition == "slot2-ruby" || condition == "slot2-sapphire" || condition == "slot2-emerald" || condition == "slot2-firered" || condition == "slot2-leafgreen")
	{
		//slot2: none/ruby/sapphire/emerald/firered/leafgreen (DPP)
		if (g_settings.wantedslot2game != condition)
			return true;
	}
	if (condition == "radio-off" || condition == "radio-hoenn" || condition == "radio-sinnoh")
	{
		//radio: off/hoenn/sinnoh (HGSS)
		if (g_settings.wantedradiostation != condition)
			return true;
	}
	return false;
}

static void StringFlagsContainStringFlag(string strgiven, int flagsgiven, string strcompare, int flagcompare, int* result)
{
	if (strgiven == strcompare)
	{
		*result = 1;
		if (flagsgiven & flagcompare)
			*result = 2;
	}
}

static bool TypeMatches(int flags, string type)
{
	int result = 0;
	StringFlagsContainStringFlag(type, flags, "normal", TypeFlags_Normal, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "fighting", TypeFlags_Fighting, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "flying", TypeFlags_Flying, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "poison", TypeFlags_Poison, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "ground", TypeFlags_Ground, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "rock", TypeFlags_Rock, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "bug", TypeFlags_Bug, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "ghost", TypeFlags_Ghost, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "steel", TypeFlags_Steel, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "fire", TypeFlags_Fire, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "water", TypeFlags_Water, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "grass", TypeFlags_Grass, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "electric", TypeFlags_Electric, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "psychic", TypeFlags_Psychic, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "ice", TypeFlags_Ice, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "dragon", TypeFlags_Dragon, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "dark", TypeFlags_Dark, &result);
	if (result == 0) StringFlagsContainStringFlag(type, flags, "fairy", TypeFlags_Fairy, &result);

	if (result == 0)
		assert(0);//this type is unaccounted for
	else if (result == 1)
		return false;//not the type
	return true;
}

static string FindMatchingType(json_value* containerobj, int flags, size_t typeIdx)
{
	json_value* types = FindArrayInObjectByName(containerobj, "types");
	if (!types)
	{
		assert(0);
		return "No types array";
	}
	assert(typeIdx == 0 || typeIdx == 1);
	if (typeIdx < types->u.array.length)
	{
		json_value* typeobj = types->u.array.values[typeIdx];
		if (!typeobj)
		{
			assert(0);
			return "No type object (1)";
		}
		json_value* typeobj2 = FindObjectInObjectByName(typeobj, "type");
		if (!typeobj2)
		{
			assert(0);
			return "No type object (2)";
		}
		string nameoftype = FindValueInObjectByKey(typeobj2, "name")->u.string.ptr;
		if (TypeMatches(flags, nameoftype)) return nameoftype;
	}
	return "";
}

static string CreateWarning(json_value* containerobj, string pokemonname, int flags)
{
	string result = FindMatchingType(containerobj, flags, 0);
	if (!result.empty())
	{
		return pokemonname + " has a matching type: " + result;
	}
	string result2 = FindMatchingType(containerobj, flags, 1);
	if (!result2.empty())
	{
		if (!result.empty())
		{
			return pokemonname + " has 2 matching types: " + result + " and " + result2;
		}
		else
		{
			return pokemonname + " has a matching type: " + result;
		}
	}
	return "";//no match
}

static bool IsPokemonMatchingType(string path, string version, int flags, string pokemonname, string* warning)
{
	FILE* fp;
	struct stat filestatus;
	int file_size;
	char* file_contents;
	json_char* json;
	json_value* file;
	if (stat(path.c_str(), &filestatus) != 0)
	{
		cout << "File " + path + " not found\n";
		assert(0);
		*warning = "File " + path + " not found";
		return false;
	}
	file_size = filestatus.st_size;
	file_contents = (char*)malloc(filestatus.st_size);
	if (!file_contents)
	{
		cout << "Memory error: unable to allocate " + to_string(file_size) + " bytes\n";
		assert(0);
		*warning = "Memory error: unable to allocate " + to_string(file_size) + " bytes";
		return false;
	}
	fp = fopen(path.c_str(), "rb");
	if (!fp)
	{
		cout << "Unable to open " + path + "\n";
		//fclose(fp);
		free(file_contents);
		assert(0);
		*warning = "Unable to open " + path;
		return false;
	}
	size_t readNum = fread(file_contents, 1, file_size, fp);
	if (readNum != file_size)
	{
		cout << "Unable to read content of " + path + " ret " + to_string(readNum) + "\n";
		cout << "ferror " + to_string(ferror(fp)) + "\n";
		cout << "feof " + to_string(feof(fp)) + "\n";
		cout << "file_size " + to_string(file_size) + "\n";
		fclose(fp);
		free(file_contents);
		assert(0);
		*warning = "Unable to read content of " + path + " ret " + to_string(readNum);
		return false;
	}
	fclose(fp);
	json = (json_char*)file_contents;
	string error_buf;
	file = json_parse(json, file_size, &error_buf);
	if (file == NULL)
	{
		cout << "File " << path << ": Unable to parse data: " << error_buf << "\n";
		free(file_contents);
		assert(0);
		*warning = "File " + path + ": Unable to parse data: " + error_buf;
		return false;
	}

	json_value* pasttypes = FindArrayInObjectByName(file, "past_types");
	if (!pasttypes)
	{
		json_value_free(file);
		free(file_contents);
		assert(0);
		*warning = "No past_types array (should be 0 length when no past typing)";
		return false;
	}
	if (pasttypes->u.array.length == 0)
	{
		string result = CreateWarning(file, pokemonname, flags);
		json_value_free(file);
		free(file_contents);
		*warning = result;
		return !result.empty();
	}
	else
	{
		//since pokemon types can vary by generation, make sure we're looking at the data for the correct generation
		//and change generation we look for based on version we found in json file
		//we only evaluate a pokemon as it pertains to one game at a time
		int gameindex = 0;
		if (g_settings.wantedgame_index == ALLGAMES_INDEX)
		{
			for (gameindex = 0; gameindex < GAMES_TOTAL; gameindex++)
			{
				//cout << version << " == " << g_games[gameindex]->internalname << "\n";
				if (version == g_games[gameindex]->internalname)
				{
					break;
				}
			}
		}
		else
		{
			gameindex = g_settings.wantedgame_index;
		}
		assert(gameindex != GAMES_TOTAL);
		for (size_t pasttypeIdx = 0; pasttypeIdx < pasttypes->u.array.length; pasttypeIdx++)
		{
			json_value* pasttypeobj = pasttypes->u.array.values[pasttypeIdx];
			if (!pasttypeobj)
			{
				json_value_free(file);
				free(file_contents);
				assert(0);
				*warning = "No past_types object (we thought there was a past typing)";
				return false;
			}
			json_value* generation = FindObjectInObjectByName(pasttypeobj, "generation");
			if (!generation)
			{
				json_value_free(file);
				free(file_contents);
				assert(0);
				*warning = "No generation object (for past typing)";
				return false;
			}
			string generationname = FindValueInObjectByKey(generation, "name")->u.string.ptr;
			bool ok = false;
			int currentgen = g_games[gameindex]->generation;
			if (generationname == "generation-i" && currentgen == 1) ok = true;
			if (generationname == "generation-ii" && currentgen <= 2) ok = true;
			if (generationname == "generation-iii" && currentgen <= 3) ok = true;
			if (generationname == "generation-iv" && currentgen <= 4) ok = true;
			if (generationname == "generation-v" && currentgen <= 5) ok = true;
			if (generationname == "generation-vi" && currentgen <= 6) ok = true;
			if (generationname == "generation-vii" && currentgen <= 7) ok = true;
			//gen 7 was the last time a pokemon's type changed
			if (ok)
			{
				//obey old type
				string result = CreateWarning(pasttypeobj, pokemonname, flags);
				json_value_free(file);
				free(file_contents);
				*warning = result;
				return !result.empty();
			}
			else
			{
				//use pokemon's new type instead
				string result = CreateWarning(file, pokemonname, flags);
				json_value_free(file);
				free(file_contents);
				*warning = result;
				return !result.empty();
			}
		}
	}
	//this should not be possible
	json_value_free(file);
	free(file_contents);
	assert(0);
	*warning = "";
	return false;
}

static int ValidateMethod(int flags, string method)
{
	//these encounter methods are not very useful, if even applicable.
	//sos encounters may come back, if the alola tables are ever fixed.
	if (method == "gift" || method == "gift-egg" || method == "headbutt-normal" || method == "only-one" || method == "pokeflute"
		|| method == "squirt-bottle" || method == "wailmer-pail" || method == "devon-scope"
		|| method == "island-scan" || method == "sos-encounter" || method == "berry-piles"
		|| method == "npc-trade" || method == "sos-from-bubbling-spot" || method == "roaming-grass"
		|| method == "roaming-water" || method == "feebas-tile-fishing")
		return -1;
	int result = 0;
	int i = 0;
	while (i < METHODS_TOTAL)
	{
		//cout << method << " == " << g_methods[i]->internalname << "\n";
		StringFlagsContainStringFlag(method, flags, g_methods[i]->internalname, g_methods[i]->flag, &result);
		if (result != 0)
			break;
		i++;
	}

	if (result == 0)
		assert(0);//this encounter method is unaccounted for
	else if (result == 1)
		return -1;//bad method
	//cout << to_string(i) << "\n";
	return i;
}

static bool ParseEncounterDetails(json_value* encdetailblock, string pokemonname, string placename, string walkstring, int version_index, int iFile, bool filterReason, string warning, bool goodtype, bool goodEVs)
{
	json_value* conditionvalues = FindArrayInObjectByName(encdetailblock, "condition_values");

	if (conditionvalues)
	{
		for (size_t conditionIdx = 0; conditionIdx < conditionvalues->u.array.length; conditionIdx++)
		{
			json_value* condobj = conditionvalues->u.array.values[conditionIdx];
			string condition = FindValueInObjectByKey(condobj, "name")->u.string.ptr;
#ifdef _DEBUG
			RecordCustomData(condition);
#endif //_DEBUG
			//make sure encounter meets applicable parameters
			if (InvalidateCondition(condition, iFile))
				return false;
		}
	}
	else
	{
		assert(0);
		return false;
	}

	json_value* methodobj = FindObjectInObjectByName(encdetailblock, "method");

	if (!methodobj)
	{
		assert(0);
		return false;
	}

	string method = FindValueInObjectByKey(methodobj, "name")->u.string.ptr;
	int method_index = ValidateMethod(g_settings.methodflags, method);
	if (method_index == -1)
		return false;

	//all good
	__int64 chance = FindValueInObjectByKey(encdetailblock, "chance")->u.integer;
	__int64 maxlevel = FindValueInObjectByKey(encdetailblock, "max_level")->u.integer;
	__int64 minlevel = FindValueInObjectByKey(encdetailblock, "min_level")->u.integer;
	RegisterEncounter(chance, minlevel, maxlevel, pokemonname, placename, walkstring, method_index, version_index, iFile, filterReason, warning, goodtype, goodEVs);
	return true;
}

static bool FindInExpFile(int offset, string expfile, string pokemonname, int* stat)
{
	string expfilepath = g_pkmndatapath + "exp-gain-stats/" + expfile;
	ifstream ReadFile(expfilepath);
	string textLine;
	bool foundmon = false;
	while (getline(ReadFile, textLine))
	{
		size_t s1End = textLine.find(',');
		string str1 = textLine.substr(0, s1End);

		//cout << "mon '" << str2 << "'\n";
		if (str1 == pokemonname)
		{
			size_t s2Start = s1End + 1;
			size_t s2End = textLine.find(',', s2Start + 1);
			string str2 = textLine.substr(s2Start, s2End - s2Start);
			for (int i = 0; i < offset; i++)
			{
				//last cell of line doesn't end with a comma, it ends with nothing
				if (s2End == -1)
				{
					str2 = textLine.substr(s2Start);
				}
				else
				{
					s2Start = s2End + 1;
					s2End = textLine.find(',', s2Start + 1);
					str2 = textLine.substr(s2Start, s2End - s2Start);
				}
			}
			foundmon = true;
			*stat = stoi(str2);
			return true;
		}
	}
	return false;
}

static bool GameHasProgressFile(int index)
{
	return !g_games[index]->progressfile.empty();
}

static double CalculateExperienceCore(int generation, double level, int baseExp)
{
	int factor = (generation == 5 || generation >= 7) ? 5 : 7;
	return (baseExp * level) / factor;
}

static double ExperienceScaleForLevel(int generation, double defeatedLevel, double winnerLevel, double avgExp)
{
	double a = 2 * defeatedLevel + 10;
	double b = defeatedLevel + winnerLevel + 10;
	if (generation == 5)
		return avgExp * (sqrt(a) * pow(a, 2)) / (sqrt(b) * pow(b, 2));
	if (generation >= 7)
		return avgExp * pow((a) / (b), 2.5);
	else
		return avgExp;
}

static bool IsPokemonInEVRange(string version, string pokemonname)
{
	string expfile;
	int gameindex = 0;
	if (g_settings.wantedgame_index == ALLGAMES_INDEX)
	{
		for (gameindex = 0; gameindex < GAMES_TOTAL; gameindex++)
		{
			if (version == g_games[gameindex]->internalname)
			{
				expfile = g_games[gameindex]->expfile;
				break;
			}
		}
	}
	else
	{
		gameindex = g_settings.wantedgame_index;
		expfile = g_games[g_settings.wantedgame_index]->expfile;
	}
	assert(gameindex != GAMES_TOTAL);
	for (int i = 0; i < OFFSET_TOTAL + 1; i++)
	{
		if (g_settings.minSingleMonEV[i] == 0 &&
			(g_games[g_settings.wantedgame_index]->generation == 1 || g_games[g_settings.wantedgame_index]->generation == 2) ?
			(i == OFFSET_TOTAL ? g_settings.maxSingleMonEV[i] == 680 : g_settings.maxSingleMonEV[i] == 255) : (g_settings.maxSingleMonEV[i] == 3))
			continue;
		int stat;
		if (!FindInExpFile(i, expfile, pokemonname, &stat))
		{
			cout << "ERROR: Could not find pokemon named '" << pokemonname << "' in " << expfile << "\n";
			continue;
		}
		if (stat < g_settings.minSingleMonEV[i] || stat > g_settings.maxSingleMonEV[i])
			return false;
	}
	return true;
}

static void ProcessStat(Encounter* encounter, EncounterTable* table, int offset, int stat, int generation, double chancescale)
{
	if (offset == OFFSET_BEY)
	{
		encounter->baseExp = stat;
		//school form BEY introduces special behavior for wishiwashi
		//school form is only accessible when level 20+
		__int64 pctlost = 0;
		if ((generation >= 7 || generation == GENERATION_ALL) && encounter->pokemonname == "wishiwashi")
		{
			encounter->minlevel = max(encounter->minlevel, g_settings.repellevel);
			for (__int64 level = encounter->minlevel; level <= encounter->maxlevel; level++)
			{
				int BEY = level < 20 ? 61 : encounter->baseExp;
				double exp = CalculateExperienceCore(generation, (double)level, BEY);
				//level scaling
				if (g_settings.scalinglevel != 0)
					exp = ExperienceScaleForLevel(generation, (double)level, g_settings.scalinglevel, exp);
				//since we're going level-by-level, we weigh exp here by what fraction of the range this level is
				encounter->avgexp += exp / (encounter->maxlevel - encounter->minlevel + 1);
				assert(encounter->avgexp > 0);
			}
		}
		else
		{
			double avglevel;
			if (generation == 2 && table->method_index == METHOD_SURF)
			{
				assert(encounter->maxlevel == encounter->minlevel + 4);
				//https://github.com/pret/pokegold/blob/0b8a81212eda44a6b54a06d7c35628629003b29a/engine/overworld/wildmons.asm#L317
				//disregard maxlevel. we actually take minlevel and add n levels onto it
				//35%: n = 0
				//30%: n = 1
				//20%: n = 2
				//10%: n = 3
				// 5%: n = 4
				//which means on average, n = 1.2 = (0 * .35) + (1 * .30) + (2 * .20) + (3 * .10) + (4 * .05)
				__int64 skiplevels = g_settings.repellevel - encounter->minlevel;
				double n = 0;
				if (skiplevels >= 0)
				{
					//for every level skipped, redistribute the odds to remaining levels
					switch (skiplevels)
					{
					case 0:
						n = 1.2;//(0 * (.35+.00/5)) + (1 * (.30+.00/5)) + (2 * (.20+.00/5)) + (3 * (.10+.00/5)) + (4 * (.05+.00/5))
						pctlost = 0;
						break;
					case 1:
						n = 2.075;//				  (1 * (.30+.35/4)) + (2 * (.20+.35/4)) + (3 * (.10+.35/4)) + (4 * (.05+.35/4))
						pctlost = 35;
						break;
					case 2:
						n = 2.85;//										  (2 * (.20+.65/3)) + (3 * (.10+.65/3)) + (4 * (.05+.65/3))
						pctlost = 65;
						break;
					case 3:
						n = 3.475;//														  (3 * (.10+.85/2)) + (4 * (.05+.85/2))
						pctlost = 85;
						break;
					case 4:
						n = 4;//																				  (4 * (.05+.95/1))
						pctlost = 95;
						break;
					}
				}
				avglevel = encounter->minlevel + n;
				encounter->minlevel = max(encounter->minlevel, g_settings.repellevel);
			}
			else
			{
				encounter->minlevel = max(encounter->minlevel, g_settings.repellevel);
				avglevel = (double)(encounter->maxlevel + encounter->minlevel) / 2;
			}
			encounter->avgexp = CalculateExperienceCore(generation, avglevel, encounter->baseExp);
			//level scaling
			if (g_settings.scalinglevel != 0)
				encounter->avgexp = ExperienceScaleForLevel(generation, avglevel, g_settings.scalinglevel, encounter->avgexp);
		}
		assert(encounter->avgexp > 0);
		encounter->avgexpweighted = (double)(encounter->avgexp * (encounter->chance - pctlost) * chancescale) / table->expectedtotalpercent;
		if (g_settings.printtext) cout << encounter->pokemonname << " has " << encounter->chance << "% chance between level " << encounter->minlevel << " and " << encounter->maxlevel << ". avgexp " << encounter->avgexp << ", weighted " << encounter->avgexpweighted << "\n";
#ifdef _DEBUG
		//if (settings->printtext) cout << "total avg exp " << table->averageYields[OFFSET_EXP] << " += " << encounter.avgexpweighted << "\n";
		//if (settings->printtext) cout << "totalchance " << table->totalchance << " += " << encounter.chance << "\n\n";
#endif //_DEBUG
		table->averageYields[OFFSET_EXP] += encounter->avgexpweighted;
		assert(table->averageYields[OFFSET_EXP] > 0);
		assert(encounter->avgexpweighted > 0);
		table->totalchance += encounter->chance;
	}
	else
	{
		table->averageYields[offset] += (double)(stat * encounter->chance * chancescale) / table->expectedtotalpercent;
		double lowestExp = CalculateExperienceCore(generation, (double)encounter->minlevel, encounter->baseExp);
		if (stat > 0)
		{
			if (table->efficientEVs[offset].expPerEV == 0.0f)
			{
				table->efficientEVs[offset].expPerEV = lowestExp / stat;
				table->efficientEVs[offset].pokemonname = encounter->pokemonname;
			}
			else
			{
				if (table->efficientEVs[offset].expPerEV > lowestExp / stat)
				{
					table->efficientEVs[offset].expPerEV = lowestExp / stat;
					table->efficientEVs[offset].pokemonname = encounter->pokemonname;
				}
			}
		}
	}
}

//find gen 3 or 5 swarm encounter. these are different than others because they make the table into X percent the special encounter and 100-X everything else. no slots are overridden.
static int AddHoennUnovaSwarm(EncounterTable* table, int generation)
{
	string swarmdatapath = g_pkmndatapath + "swarm-data/" + to_string(table->filenumber) + ".txt";
	ifstream ReadFile(swarmdatapath);
	string textLine;
	while (getline(ReadFile, textLine))
	{
		size_t s1End = textLine.find(',');
		string str1 = textLine.substr(0, s1End);//pokemon's name - not verified yet

		size_t s2Start = s1End + 1;
		size_t s2End = textLine.find(',', s2Start + 1);
		string str2 = textLine.substr(s2Start, s2End - s2Start);//level

		if (generation == 5 && str1 == "croagunk" && g_settings.wantedseason == "season-winter")
			break;//special case. croagunk will not appear in winter even by swarm. when the game tries to spawn a croagunk swarm in winter, it simply will not do anything.

		string targetgame;
		if (table->version_index == GAME_RUBY || table->version_index == GAME_SAPPHIRE)
			targetgame = "rubysapphire";
		else if (table->version_index == GAME_EMERALD)
			targetgame = "emerald";
		else if (table->version_index == GAME_BLACK1)
			targetgame = "b1";
		else if (table->version_index == GAME_BLACK2)
			targetgame = "b2";
		else if (table->version_index == GAME_WHITE1)
			targetgame = "w1";
		else if (table->version_index == GAME_WHITE2)
			targetgame = "w2";
		else
		{
			cout << "ERROR: tried to get gen 3/5 swarm encounter for invalid game '" << g_games[table->version_index]->uiname << "' file number " << to_string(table->filenumber) << "\n";
			continue;
		}
		if (textLine.find(targetgame) != string::npos)
		{
			int extrachance = (generation == 5) ? 40 : 50;
			Encounter newEnc;
			newEnc.chance = extrachance;
			if (table->version_index == GAME_RUBY || table->version_index == GAME_SAPPHIRE || table->version_index == GAME_EMERALD)
			{
				newEnc.maxlevel = stoi(str2);
				newEnc.minlevel = stoi(str2);
			}
			else if (table->version_index == GAME_BLACK1 || table->version_index == GAME_WHITE1)
			{
				newEnc.minlevel = 15;
				newEnc.maxlevel = 55;
			}
			else if (table->version_index == GAME_BLACK2 || table->version_index == GAME_WHITE2)
			{
				newEnc.minlevel = 40;
				newEnc.maxlevel = 55;
			}
			newEnc.pokemonname = str1;
			newEnc.specialswarm = true;
			table->encounters.push_back(newEnc);
			return 1;
		}
		//didn't find an encounter? that's ok
	}
	return 0;
}

static int ParseEncounterBlock(json_value* versiondetails, json_value* encounterblock, string placename, string walkstring, int iFile, GameObject* game)
{
	for (size_t verdetailsIdx = 0; verdetailsIdx < versiondetails->u.array.length; verdetailsIdx++)
	{
		json_value* verdetailblock = versiondetails->u.array.values[verdetailsIdx];
		if (!verdetailblock)
		{
			assert(0);
			return 0;
		}
		//version
		json_value* version = FindObjectInObjectByName(verdetailblock, "version");
		if (!version)
		{
			assert(0);
			return 0;
		}
		string givengame = FindValueInObjectByKey(version, "name")->u.string.ptr;

		//ensure this pokemon is in our game version before doing anything else
		if (!(givengame == game->internalname || g_settings.wantedgame_index == ALLGAMES_INDEX))
			continue;

		//do NOT remove these {} brackets, or the else statement will be linked up to the wrong if statement
		int gameindex;
		if (g_settings.wantedgame_index == ALLGAMES_INDEX)
		{
			for (gameindex = 0; gameindex < GAMES_TOTAL; gameindex++)
				if (givengame == g_games[gameindex]->internalname)
					break;
		}
		else
			gameindex = g_settings.wantedgame_index;
		
		json_value* pokemon = FindObjectInObjectByName(encounterblock, "pokemon");
		if (!pokemon)
		{
			assert(0);
			return 0;
		}
		string pokemonname = FindValueInObjectByKey(pokemon, "name")->u.string.ptr;
		bool filterReason = Reason_None;
		string warning = "";
		if (g_settings.pkmnfiltertypeflags != 0)
		{
			string url = FindValueInObjectByKey(pokemon, "url")->u.string.ptr;
			if (IsPokemonMatchingType(g_pkmndatapath + url + "index.json", givengame, g_settings.pkmnfiltertypeflags, pokemonname, &warning))
			{
				//pokemon is a type we don't allow
				//cout << pokemonname << " is bad type\n";
				filterReason = Reason_BadType;
			}
		}
		bool goodtype = false;
		if (g_settings.pkmnrequiretypeflags != 0)
		{
			string url = FindValueInObjectByKey(pokemon, "url")->u.string.ptr;
			goodtype = IsPokemonMatchingType(g_pkmndatapath + url + "index.json", givengame, g_settings.pkmnrequiretypeflags, pokemonname, &warning);
		}
		bool goodEVs = IsPokemonInEVRange(givengame, pokemonname);

		json_value* encounterdetails = FindArrayInObjectByName(verdetailblock, "encounter_details");
		if (!encounterdetails)
		{
			assert(0);
			return 0;
		}
		for (size_t encdetailsIdx = 0; encdetailsIdx < encounterdetails->u.array.length; encdetailsIdx++)
		{
			json_value* encdetailblock = encounterdetails->u.array.values[encdetailsIdx];
			if (!encdetailblock)
			{
				assert(0);
				return 0;
			}

			if (!ParseEncounterDetails(encdetailblock, pokemonname, placename, walkstring, gameindex, iFile, filterReason, warning, goodtype, goodEVs))
				continue;//encounter was bad for some reason
		}
	}
	return 1;
}

static int ParseLocationDataFile(int iFile)
{
	string locationareapath = g_pkmndatapath + "api\\v2\\location-area\\" + to_string(iFile) + "\\index.json";
	//cout << path << "\n";
	GameObject* game = g_games[g_settings.wantedgame_index];
	string placename;//only one place name per file
	FILE* fp;
	struct stat filestatus;
	int file_size;
	char* file_contents;
	json_char* json;
	json_value* file;
	if (stat(locationareapath.c_str(), &filestatus) != 0)
	{
		//cout << "File " + locationareapath + " not found\n";
		//we delete some folders because they're redundant, and sometimes pokeapi is just missing folders on its own
		//return quietly
		return 1;
	}
	file_size = filestatus.st_size;
	file_contents = (char*)malloc(filestatus.st_size);
	if (!file_contents)
	{
		cout << "Memory error: unable to allocate " + to_string(file_size) + " bytes\n";
		return 0;
	}
	fp = fopen(locationareapath.c_str(), "rb");
	if (!fp)
	{
		cout << "Unable to open " + locationareapath + "\n";
		//fclose(fp);
		free(file_contents);
		return 0;
	}
	size_t readNum = fread(file_contents, 1, file_size, fp);
	if (readNum != file_size)
	{
		cout << "Unable to read content of " + locationareapath + " ret " + to_string(readNum) + "\n";
		cout << "ferror " + to_string(ferror(fp)) + "\n";
		cout << "feof " + to_string(feof(fp)) + "\n";
		cout << "file_size " + to_string(file_size) + "\n";
		fclose(fp);
		free(file_contents);
		return 0;
	}
	fclose(fp);
	json = (json_char*)file_contents;
	string error_buf;
	file = json_parse(json, file_size, &error_buf);
	if (file == NULL)
	{
		cout << "File " << to_string(iFile) << ": Unable to parse data: " << error_buf << "\n";
		free(file_contents);
		return 0;
	}
	//find place name
	json_value* names = FindArrayInObjectByName(file, "names");
	if (!names)
	{
		assert(0);
		return 0;
	}
	for (size_t nameIdx = 0; nameIdx < names->u.array.length; nameIdx++)
	{
		json_value* localname = names->u.array.values[nameIdx];
		if (!localname)
		{
			assert(0);
			return 0;
		}
		json_value* language = FindObjectInObjectByName(localname, "language");
		if (!language)
		{
			assert(0);
			return 0;
		}
		json_value* langname = FindValueInObjectByKey(language, "name");
		if (isEqualString(langname->u.string.ptr, "en"))
			placename = FindValueInObjectByKey(localname, "name")->u.string.ptr;
	}
	//walk string (not all files have one)
	json_value* walkstringjson = FindValueInObjectByKey(file, "walk_string");
	string walkstring;
	if (walkstringjson)
	{
		walkstring = walkstringjson->u.string.ptr;
	}
	//get encounter info
	json_value* encounters = FindArrayInObjectByName(file, "pokemon_encounters");
	if (!encounters)
	{
		assert(0);
		return 0;
	}
	for (size_t encounterIdx = 0; encounterIdx < encounters->u.array.length; encounterIdx++)
	{
		json_value* encounterblock = encounters->u.array.values[encounterIdx];
		if (!encounterblock)
		{
			assert(0);
			return 0;
		}
		//version details block
		json_value* versiondetails = FindArrayInObjectByName(encounterblock, "version_details");
		if (!versiondetails)
		{
			assert(0);
			return 0;
		}
		int result = ParseEncounterBlock(versiondetails, encounterblock, placename, walkstring, iFile, game);
		if (result == 0)
			return 0;
	}
	json_value_free(file);
	free(file_contents);
	return 1;
}

static bool compareByExp(const EncounterTable a, const EncounterTable b)
{
	//cout << a.averageYields[OFFSET_EXP] << " > " << b.averageYields[OFFSET_EXP] << "\n";
	//if a table is filtered out then total avg exp is probably 0
	assert(a.averageYields[OFFSET_EXP] > 0 || a.filterReason != Reason_None);
	assert(b.averageYields[OFFSET_EXP] > 0 || b.filterReason != Reason_None);
	return a.averageYields[OFFSET_EXP] > b.averageYields[OFFSET_EXP];
}

static bool compareByLevelRange(const EncounterTable a, const EncounterTable b)
{
	return a.highestlevel - a.lowestlevel > b.highestlevel - b.lowestlevel;
}

static bool compareByPlacename(const EncounterTable a, const EncounterTable b)
{
	return strcmp(a.placename.c_str(), b.placename.c_str()) < 0;
}

static bool compareByMethod(const EncounterTable a, const EncounterTable b)
{
	string str_a = (a.method_index == METHOD_WALK && b.method_index == METHOD_WALK && !a.walkstring.empty()) ? a.walkstring : g_methods[a.method_index]->uiname;
	string str_b = (b.method_index == METHOD_WALK && a.method_index == METHOD_WALK && !b.walkstring.empty()) ? b.walkstring : g_methods[b.method_index]->uiname;
	return strcmp(str_a.c_str(), str_b.c_str()) < 0;
}

static bool compareByVersion(const EncounterTable a, const EncounterTable b)
{
	return strcmp(g_games[a.version_index]->uiname.c_str(), g_games[b.version_index]->uiname.c_str()) < 0;
}

static bool compareByMonName(const Encounter& a, const Encounter& b)
{
	return strcmp(a.pokemonname.c_str(), b.pokemonname.c_str()) < 0;
}

static bool compareByChance(const Encounter& a, const Encounter& b)
{
	return a.chance > b.chance;
}

static bool compareByAvgLevel(const Encounter& a, const Encounter& b)
{
	double avglevelA = static_cast<double>(a.maxlevel + a.minlevel) / 2;
	double avglevelB = static_cast<double>(b.maxlevel + b.minlevel) / 2;
	return avglevelA > avglevelB;
}

static bool compareByAvgExp(const Encounter& a, const Encounter& b)
{
	return a.avgexp > b.avgexp;
}

static bool compareByAEW(const Encounter& a, const Encounter& b)
{
	return a.avgexpweighted > b.avgexpweighted;
}

static bool compareByAverageHPEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageYields[OFFSET_HP] > b.averageYields[OFFSET_HP];
}

static bool compareByAverageAttackEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageYields[OFFSET_ATTACK] > b.averageYields[OFFSET_ATTACK];
}

static bool compareByAverageDefenseEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageYields[OFFSET_DEFENSE] > b.averageYields[OFFSET_DEFENSE];
}

static bool compareByAverageSpAtkEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageYields[OFFSET_SP_ATTACK] > b.averageYields[OFFSET_SP_ATTACK];
}

static bool compareByAverageSpDefEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageYields[OFFSET_SP_DEFENSE] > b.averageYields[OFFSET_SP_DEFENSE];
}

static bool compareByAverageSpeedEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageYields[OFFSET_SPEED] > b.averageYields[OFFSET_SPEED];
}

static bool compareByAverageTotalEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageYields[OFFSET_TOTAL] > b.averageYields[OFFSET_TOTAL];
}

static bool compareByExpPerHPEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_HP].expPerEV > b.efficientEVs[OFFSET_HP].expPerEV;
}

static bool compareByExpPerAttackEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_ATTACK].expPerEV > b.efficientEVs[OFFSET_ATTACK].expPerEV;
}

static bool compareByExpPerDefenseEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_DEFENSE].expPerEV > b.efficientEVs[OFFSET_DEFENSE].expPerEV;
}

static bool compareByExpPerSpAtkEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_SP_ATTACK].expPerEV > b.efficientEVs[OFFSET_SP_ATTACK].expPerEV;
}

static bool compareByExpPerSpDefEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_SP_DEFENSE].expPerEV > b.efficientEVs[OFFSET_SP_DEFENSE].expPerEV;
}

static bool compareByExpPerSpeedEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_SPEED].expPerEV > b.efficientEVs[OFFSET_SPEED].expPerEV;
}

static bool compareByExpPerTotalEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_TOTAL].expPerEV > b.efficientEVs[OFFSET_TOTAL].expPerEV;
}

static bool ReadTables()
{
	if (g_settings.printtext) cout << "Reading encounter data\n";
	GameObject* game = g_games[g_settings.wantedgame_index];
	
	//look at ranges to find how many files we're looking through for the progress bar
	int totalfiles = 0;
	for (int i = 0; i < game->folderRanges.size(); i += 2)
	{
		int a = i + 1;
		totalfiles += (game->folderRanges[a] - game->folderRanges[i]) + 1;
	}

	//read files
	int filesread = 0;
	int notch = 1;
	for (int i = 0; i < game->folderRanges.size(); i += 2)
	{
		int a = i + 1;
		if (g_settings.printtext) cout << "|     PROGRESS     |\n";
		for (int j = game->folderRanges[i]; j <= game->folderRanges[a]; j++)
		{
			filesread++;
			g_settingswindowdata.progress = (1 - (static_cast<float>(totalfiles) - filesread) / totalfiles) * 0.5;
			if (g_settingswindowdata.progress >= (notch * 0.05))
			{
				if (g_settings.printtext) cout << "-";
				notch++;
			}
			if (ParseLocationDataFile(j) == 0)
				return true;
		}
	}

	if (g_settings.printtext) cout << "\n";
	int numTablesProcessed = 0;
	for (EncounterTable& table : maintables)
	{
		string expfile = game->expfile;
		int generation = game->generation;
		if (g_settings.wantedgame_index == ALLGAMES_INDEX)
		{
			//change exp file based on table's game
			expfile = g_games[table.version_index]->expfile;
			generation = g_games[table.version_index]->generation;
		}
		//progress bar
		numTablesProcessed++;
		//cout << to_string(settingswindowdata->progress) << "\n";
		if (g_settingswindowdata.progress >= (notch * 0.05))
		{
			if (g_settings.printtext) cout << "-";
			notch++;
		}
		//really prefer to not save tables that i know are bad, but this is by far the least painful way to take care of this
		if (table.filterReason == Reason_None || table.filterReason == Reason_BadType)
		{
			if (g_settings.printtext) cout << "\n" << table.placename << ", " << g_methods[table.method_index]->uiname << ", " << g_games[table.version_index]->uiname << "\n";
			table.averageYields[OFFSET_EXP - 1] = 0;
			table.totalchance = 0;//sanity check: this number should always = 100 or expectedtotalpercent at the end of the table.

			bool specialswarm = false;
			int extrachance = 0;
			if (g_settings.wantswarm && table.method_index == METHOD_WALK && (generation == 5 || table.version_index == GAME_RUBY || table.version_index == GAME_SAPPHIRE || table.version_index == GAME_EMERALD))
			{
				if (AddHoennUnovaSwarm(&table, generation))
				{
					specialswarm = true;
					extrachance = (generation == 5) ? 40 : 50;
				}
			}

			for (Encounter& encounter : table.encounters)
			{
				double chancescale = 1;
				if (specialswarm)
					chancescale = (generation == 5) ? 0.6 : 0.5;
				string expfilepath = g_pkmndatapath + "exp-gain-stats/" + expfile;
				ifstream ReadFile(expfilepath);
				string textLine;
				while (getline(ReadFile, textLine))
				{
					size_t s1End = textLine.find(',');
					string str1 = textLine.substr(0, s1End);

					//cout << "mon '" << str1 << "'\n";
					if (str1 != encounter.pokemonname)
						continue;

					size_t commaPos = textLine.length();
					//reverse order because BEY is last in the table, and efficientEVs needs BEY before other stats can be processed
					//downsides of BEY being first in table are far worse
					for (int j = OFFSET_BEY; j >= 0; j--)
					{
						if (j == OFFSET_EXP)//skip exp, this is filled in later based on OFFSET_BEY
							continue;
						commaPos = textLine.find_last_of(',' , commaPos - 1);
						size_t s2End = textLine.find(',', commaPos + 1);
						string str2;
						//last cell of line doesn't end with a comma, it ends with nothing
						if (s2End == string::npos)
						{
							str2 = textLine.substr(commaPos + 1);
						}
						else
						{
							str2 = textLine.substr(commaPos + 1, s2End - commaPos - 1);
						}
						int stat = stoi(str2);
						//ditto reads from setting (because of Transform copying stats)
						if (encounter.pokemonname == "ditto" && game->generation >= 3)
							stat = g_settings.partyYields[j];
						ProcessStat(&encounter, &table, j, stat, generation, chancescale);
					}
				}
			}
			if (g_settings.printtext) cout << table.averageYields[OFFSET_EXP] << " average EXP in " << table.placename << ", " << g_methods[table.method_index]->uiname << ", " << g_games[table.version_index]->uiname << "\n";
			std::sort(table.encounters.begin(), table.encounters.end(), compareByAEW);

			//unless we're using repel or max level, the table's total chance should always be 100.
			bool errorfound = false;
			if ((g_settings.repellevel == 0 && g_settings.maxallowedlevel == 100) && table.totalchance != 100 + extrachance)
				errorfound = true;
			//this check was to find tables that were being deleted incorrectly. now that we don't delete tables for this purpose, this appears to be pointless, but testing is needed.
			else if (table.totalchance != table.expectedtotalpercent + extrachance)
				errorfound = true;
			if (errorfound)
			{
				cout << "ERROR: Total chance was " << table.totalchance << "! File number " << table.filenumber << "\n";
				cout << "wantedgame: " << game->uiname << " totalchance: " << to_string(table.totalchance) << "\n";
				cout << "repellevel: " << to_string(g_settings.repellevel) << " maxallowedlevel: " << to_string(g_settings.maxallowedlevel) << "\n";
				cout << "expectedtotalpercent: " << to_string(table.expectedtotalpercent) << "\n";
				cin.get();
				return true;
			}
		}
		g_settingswindowdata.progress = (1 - (static_cast<float>(maintables.size()) - numTablesProcessed) / maintables.size()) * 0.5 + 0.5;
	}
	if (g_settings.printtext)
	{
		cout << "\n";
		cout << "To view data more neatly, copy the text output above and put it into any website or program that can sort text (http://www.unit-conversion.info/texttools/sort-lines/)\n";
		cout << "You may also want to filter the lines in some way (http://www.unit-conversion.info/texttools/filter-lines/)\n";
	}
#ifdef _DEBUG
	PrintCustomData(/*settings*/);
#endif //_DEBUG
	return true;
}

static const char* Items_SingleStringGetter(void* data, int idx)
{
	const char* items_separated_by_zeros = (const char*)data;
	int items_count = 0;
	const char* p = items_separated_by_zeros;
	while (*p)
	{
		if (idx == items_count)
			break;
		p += strlen(p) + 1;
		items_count++;
	}
	return *p ? p : NULL;
}

static void TablePostProcess()
{
	g_settingswindowdata.running = false;
	bool requiringtype = g_settings.pkmnrequiretypeflags;
	std::sort(maintables.begin(), maintables.end(), compareByExp);
	for (EncounterTable& table : maintables)
	{
		string methodnamestring = g_methods[table.method_index]->uiname;
		if (g_games[table.version_index]->generation == 7 && table.method_index == METHOD_SUPERROD)
		{
			//three-rod distinction is gone in gen 7, so we manually change the name here
			//sucks but i don't see a better way
			methodnamestring = "Fishing";
		}
		if (table.method_index == METHOD_WALK && !table.walkstring.empty())
			methodnamestring = table.walkstring;
		table.header = to_string((int)trunc(table.averageYields[OFFSET_EXP])) + " EXP, " + table.placename + ", " + methodnamestring + ", " + g_games[table.version_index]->uiname;
		if (!table.goodtype && requiringtype)
			table.filterReason = Reason_NoGoodTypes;
		if (!table.goodEVs)
			table.filterReason = Reason_NoGoodEVs;
		for (int i = 0; i < OFFSET_TOTAL + 1; i++)
		{
			if (table.averageYields[i] < g_settings.minAvgEV[i] || table.averageYields[i] > g_settings.maxAvgEV[i])
			{
				table.filterReason = Reason_BadEVs;
				break;
			}
		}
	}
}

static void UIMiscSettings(GameObject* game, bool allgames, bool rse, bool dpp, bool hgss)
{
	if (game->generation == 2)
	{
		if (ImGui::BeginTable("gen2timetable", 3))
		{
			ImGui::TableSetupColumn("Morning");
			ImGui::TableSetupColumn("Day");
			ImGui::TableSetupColumn("Night");
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("4:00 AM-9:59 AM\n(04:00-09:59)");
			ImGui::TableNextColumn();
			ImGui::Text("10:00 AM-5:59 PM\n(10:00-17:59)");
			ImGui::TableNextColumn();
			ImGui::Text("6:00 PM-3:59 AM\n(18:00-03:59)");
			ImGui::EndTable();
		}
	}

	if (game->generation == 4)
	{
		if (ImGui::BeginTable("gen4timetable", 3))
		{
			ImGui::TableSetupColumn("Morning");
			ImGui::TableSetupColumn("Day");
			ImGui::TableSetupColumn("Night");
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("4:00 AM-9:59 AM\n(04:00-09:59)");
			ImGui::TableNextColumn();
			ImGui::Text("10:00 AM-7:59 PM\n(10:00-19:59)");
			ImGui::TableNextColumn();
			ImGui::Text("8:00 PM-3:59 AM\n(20:00-03:59)");
			ImGui::EndTable();
		}
	}

	if (game->generation == 7)
	{
		ImGui::Text("In Moon/Ultra Moon, time is inverse of the normal.");
		ImGui::Text("In all games, the time can be inverted at the Altar of the Sunne/Moone.");
		if (ImGui::BeginTable("gen7timetable", 2))
		{
			ImGui::TableSetupColumn("Day");
			ImGui::TableSetupColumn("Night");
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text("6:00 AM-4:59 PM\n(06:00-17:59)");
			ImGui::TableNextColumn();
			ImGui::Text("6:00 PM-4:59 AM\n(18:00-05:59)");
			ImGui::EndTable();
		}
	}

	//1 and 3 don't have day/night cycles, 5 and 6's are cosmetic for most purposes
	if (allgames || (game->generation == 2 || game->generation == 4 || game->generation == 7))
	{
		const char* times;
		const char* internal_times;
		int height;
		if (game->generation == 7)
		{
			times = "Day\0Night\0";
			internal_times = "time-day\0time-night\0";
			height = 2;
			if (g_settingswindowdata.generation_lastframe != game->generation)
				g_settingswindowdata.time_chosen = 0;
		}
		else
		{
			times = "Morning\0Day\0Night\0";
			internal_times = "time-morning\0time-day\0time-night\0";
			height = 3;
			if (g_settingswindowdata.generation_lastframe != game->generation)
				g_settingswindowdata.time_chosen = 1;
		}
#pragma warning(suppress: 6384)
		ImGui::Combo("Time of Day", &g_settingswindowdata.time_chosen, times, IM_ARRAYSIZE(times));
		g_newsettings.wantedtime = Items_SingleStringGetter((void*)internal_times, g_settingswindowdata.time_chosen);
	}

	if (allgames || game->generation == 5)
	{
		const char* seasons[] = { "Spring", "Summer", "Autumn", "Winter" };
		const char* internal_seasons[] = { "season-spring", "season-summer", "season-autumn", "season-winter" };
		static int season_current = 0;
		ImGui::Combo("Season", &season_current, seasons, IM_ARRAYSIZE(seasons));
		g_newsettings.wantedseason = internal_seasons[season_current];
	}

	if (allgames || (game->generation == 2 || rse || game->generation == 4 || game->generation == 5))
	{
		static bool wantswarm = false;
		ImGui::Checkbox("Mass Outbreaks", &wantswarm);
		g_newsettings.wantswarm = wantswarm;
	}

	if (allgames || dpp)
	{
		static bool wantradar = false;
		ImGui::Checkbox("PokeRadar", &wantradar);
		g_newsettings.wantradar = wantradar;

		const char* slotgames[] = { "None", "Ruby", "Sapphire", "Emerald", "FireRed", "LeafGreen" };
		const char* internal_slotgames[] = { "slot2-none", "slot2-ruby", "slot2-sapphire", "slot2-emerald", "slot2-firered", "slot2-leafgreen" };
		static int slotgame_current = 0;
		ImGui::Combo("DS Slot 2", &slotgame_current, slotgames, IM_ARRAYSIZE(slotgames));
		ImGui::SameLine(); HelpMarker("Some DS models have a slot for GameBoy games. Having a Generation 3 game in the slot while playing a Generation 4 game can change some encounter tables. Emulators may or may not have a feature for this.");
		g_newsettings.wantedslot2game = internal_slotgames[slotgame_current];
	}

	if (allgames || hgss)
	{
		const char* stations[] = { "Off/Other", "Hoenn Sound", "Sinnoh Sound" };
		const char* internal_stations[] = { "radio-off", "radio-hoenn", "radio-sinnoh" };
		static int station_current = 0;
		ImGui::Combo("Radio Station", &station_current, stations, IM_ARRAYSIZE(stations));
		g_newsettings.wantedradiostation = internal_stations[station_current];
	}

	if (game->generation == 5 || game->generation >= 7)
	{
		static int scalinglevel = 0;
		ImGui::InputInt("Level for Scaling Math", &scalinglevel);
		ImGui::SameLine(); HelpMarker("This game scales experience by the difference in levels between the victorious pokemon and defeated pokemon. Enter your pokemon's level to factor in level scaling. Otherwise, use 0.");
		g_newsettings.scalinglevel = scalinglevel;
	}
	else
		g_newsettings.scalinglevel = 0;

	static int repellevel = 0;
	ImGui::InputInt("Repel Level", &repellevel);
	ImGui::SameLine();
	if (allgames || (game->generation >= 2 && game->generation <= 5))
		HelpMarker("Repels keep away wild Pokemon who are a lower level than the first NON-FAINTED Pokemon in the party.");
	else
		HelpMarker("Repels keep away wild Pokemon who are a lower level than the first Pokemon in the party.");
	g_newsettings.repellevel = repellevel;

	static int maxallowedlevel = 100;
	ImGui::InputInt("Maximum Level", &maxallowedlevel);
	ImGui::SameLine(); HelpMarker("Encounter tables with Pokemon above this level will not be shown.");
	g_newsettings.maxallowedlevel = maxallowedlevel;
}

static void UISettingSections(GameObject* game, bool allgames, bool hgss)
{
	if (g_settingswindowdata.generation_lastframe != game->generation)
	{
		if (game->generation <= 4)
			g_newsettings.partyYields[OFFSET_BEY] = 61;
		else
			g_newsettings.partyYields[OFFSET_BEY] = 101;
		if (game->generation < 3 && g_settingswindowdata.generation_lastframe >= 3)
		{
			g_newsettings.maxAvgEV = { 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 680.0f };
			g_newsettings.maxSingleMonEV = { 255, 255, 255, 255, 255, 255, 680 };
		}
	}

	if (ImGui::CollapsingHeader("Encounter Methods", ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Choose which encounter methods to include in the analysis.");
		static int methodflags = MethodFilterFlags_Last - 1;
		ImGui::CheckboxFlags("Walk", &methodflags, MethodFilterFlags_Walk);
		ImGui::SameLine(); HelpMarker("\"Walk\" generally means whatever the most obvious method of encounters is in an area. This means grass in most outdoor places and means walking anywhere in caves and dungeons.");
		ImGui::CheckboxFlags("Surf", &methodflags, MethodFilterFlags_Surf);
		if (game->generation != 5 && game->generation != 7)
		{
			ImGui::CheckboxFlags("Old Rod", &methodflags, MethodFilterFlags_RodOld);
			ImGui::CheckboxFlags("Good Rod", &methodflags, MethodFilterFlags_RodGood);
		}

		if (game->generation == 7)
			ImGui::CheckboxFlags("Fishing at regular rock", &methodflags, MethodFilterFlags_RodSuper);
		else
			ImGui::CheckboxFlags("Super Rod", &methodflags, MethodFilterFlags_RodSuper);

		if (allgames || game->generation == 2 || game->generation == 3 || game->generation == 6 || hgss)
			ImGui::CheckboxFlags("Rock Smash", &methodflags, MethodFilterFlags_RockSmash);

		if (allgames || game->generation == 2/* || hgss*/)
		{
			ImGui::CheckboxFlags("Headbutt", &methodflags, MethodFilterFlags_Headbutt);
			ImGui::SameLine(); HelpMarker("GSC headbutt encounters are regarded inconsistently online.\n\n"
				"\"Headbutt High\" is:\n-\"Moderate chances of battle\" on Bulba individual route pages\n-\"High-encounter trees\" on Bulba's \"Headbutt tree\" page\n-\"Headbutt\" on Serebii\n\n"
				"\"Headbutt Low\" is:\n-\"Low chances of battle\" on Bulba individual route pages\n-\"Moderate-encounter trees\" on Bulba's \"Headbutt tree\" page\n-\"Headbutt - Special Trees\" on Serebii");
		}

		if (allgames || game->generation == 5)
		{
			ImGui::CheckboxFlags("Dark Grass", &methodflags, MethodFilterFlags_DarkGrass);
			ImGui::CheckboxFlags("Phenomena", &methodflags, MethodFilterFlags_Phenomena);
			ImGui::SameLine(); HelpMarker("Rustling grass, dust clouds, flying pokemon's shadows, and rippling water.");
		}

		if (allgames || game->generation == 6 || game->generation == 7)
		{
			ImGui::CheckboxFlags("Ambush", &methodflags, MethodFilterFlags_Ambush);
			ImGui::SameLine(); HelpMarker("Cases where pokemon have an overworld presence and move, like flying pokemon shadows or rustling grass.");

			if (game->generation == 7)
				ImGui::CheckboxFlags("Fishing at bubbling rock", &methodflags, MethodFilterFlags_BubblingSpots);
		}

		g_newsettings.methodflags = methodflags;
	}

	if (ImGui::CollapsingHeader("Pokemon Types", ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Tables with pokemon of the selected types will not be shown.");
		ImGui::Text("This can take a long time!");
		static int pkmnFilterTypeFlags = 0;
		if (ImGui::BeginTable("typetable1", 5, ImGuiTableFlags_None))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Normal", &pkmnFilterTypeFlags, 1);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fighting", &pkmnFilterTypeFlags, 2);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Flying", &pkmnFilterTypeFlags, 4);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Poison", &pkmnFilterTypeFlags, 8);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ground", &pkmnFilterTypeFlags, 16);
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Rock", &pkmnFilterTypeFlags, 32);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Bug", &pkmnFilterTypeFlags, 64);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ghost", &pkmnFilterTypeFlags, 128);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fire", &pkmnFilterTypeFlags, 512);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Water", &pkmnFilterTypeFlags, 1024);
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Grass", &pkmnFilterTypeFlags, 2048);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Electric", &pkmnFilterTypeFlags, 4096);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Psychic", &pkmnFilterTypeFlags, 8192);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ice", &pkmnFilterTypeFlags, 16384);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dragon", &pkmnFilterTypeFlags, 32768);
			if (allgames || game->generation >= 2)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Steel", &pkmnFilterTypeFlags, 256);
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dark", &pkmnFilterTypeFlags, 65536);
				if (allgames || game->generation >= 6)
				{
					ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fairy", &pkmnFilterTypeFlags, 131072);
				}
			}
		}
		ImGui::EndTable();

		g_newsettings.pkmnfiltertypeflags = pkmnFilterTypeFlags;

		static bool pkmntypewarn = false;
		ImGui::Checkbox("Don't Hide, Just Warn", &pkmntypewarn);
		g_settings.pkmntypewarn = pkmntypewarn;

		ImGui::Separator();

		ImGui::Text("Tables must have at least one pokemon of the selected types.\nOnly one of the selected types will be required per table.\nIf Bug and Water are both selected, a table only needs one\nBug type or one Water type, not necessarily both.");
		ImGui::Text("This can take a long time!");
		static int pkmnRequireTypeFlags = 0;
		if (ImGui::BeginTable("typetable2", 5, ImGuiTableFlags_None))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Normal", &pkmnRequireTypeFlags, 1);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fighting", &pkmnRequireTypeFlags, 2);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Flying", &pkmnRequireTypeFlags, 4);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Poison", &pkmnRequireTypeFlags, 8);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ground", &pkmnRequireTypeFlags, 16);
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Rock", &pkmnRequireTypeFlags, 32);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Bug", &pkmnRequireTypeFlags, 64);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ghost", &pkmnRequireTypeFlags, 128);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fire", &pkmnRequireTypeFlags, 512);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Water", &pkmnRequireTypeFlags, 1024);
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Grass", &pkmnRequireTypeFlags, 2048);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Electric", &pkmnRequireTypeFlags, 4096);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Psychic", &pkmnRequireTypeFlags, 8192);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ice", &pkmnRequireTypeFlags, 16384);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dragon", &pkmnRequireTypeFlags, 32768);
			if (allgames || game->generation >= 2)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Steel", &pkmnRequireTypeFlags, 256);
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dark", &pkmnRequireTypeFlags, 65536);
				if (allgames || game->generation >= 6)
				{
					ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fairy", &pkmnRequireTypeFlags, 131072);
				}
			}
		}
		ImGui::EndTable();

		g_newsettings.pkmnrequiretypeflags = pkmnRequireTypeFlags;
	}

	if (GameHasProgressFile(g_newsettings.wantedgame_index) && ImGui::CollapsingHeader("Game Progress", ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Hide locations that are not normally accessible at a certain point in the game.\nSelect the action above the first one you have not done.");
		static bool useprogressfilter = false;
		ImGui::Checkbox("Use", &useprogressfilter);
		ImGui::SameLine(); HelpMarker("In some games, some areas can become permanently inaccessible, so you are given the option to use this feature at all.");
		g_newsettings.useprogressfilter = useprogressfilter;

		//main milestone list
		float w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.y);
		ImGui::SetNextItemWidth(w);
		static int selected_checkpoint_slot = 0;
		if (ImGui::BeginListBox("##"))
		{
			for (int slot = 0; slot < g_milestones.size(); slot++)
			{
				Milestone ms = g_milestones[slot];
				if (ms.type != MILESTONE_CHECKBOX)
				{
					if (ImGui::Selectable(ms.name.c_str(), ms.userselected) && !g_settingswindowdata.running)
					{
						bool pastmyself = false;
						for (Milestone& ms2 : g_milestones)
						{
							if (ms2.type != MILESTONE_CHECKBOX)
							{
								if (pastmyself)
									ms2.userselected = false;
								else
								{
									ms2.userselected = true;
									if (ms.name == ms2.name)
									{
										pastmyself = true;
										selected_checkpoint_slot = slot;
										g_newsettings.selected_checkpoint_slot = selected_checkpoint_slot;
									}
								}
							}
						}
					}
				}
			}
			ImGui::EndListBox();
		}

		//checkboxes
		static bool checked[32] = {
			false, false, false, false,
			false, false, false, false,
			false, false, false, false,
			false, false, false, false,
			false, false, false, false,
			false, false, false, false,
			false, false, false, false,
			false, false, false, false};
		for (int slot = 0; slot < g_milestones.size(); slot++)
		{
			Milestone& ms = g_milestones[slot];
			//running check is because we don't want the user to change settings during iteration
			if (!g_settingswindowdata.running && ms.type == MILESTONE_CHECKBOX)
			{
				if (MilestoneIsRelevant(slot, 0, selected_checkpoint_slot))
				{
					ImGui::Checkbox(ms.name.c_str(), &checked[ms.id]);
					ms.userselected = checked[ms.id];
				}
				else
				{
					//critical: a checkbox that is not relevant must be regarded as unselected
					ms.userselected = false;
					checked[ms.id] = false;
				}
			}
		}
	}

	static std::vector<std::vector<float>> statColors = {
		{93.0f / 360, .56f, .90f},
		{50.0f / 360, .57f, .96f},
		{22.0f / 360, .57f, .94f},
		{192.0f / 360, .58f, .96f},
		{227.0f / 360, .41f, .91f},
		{313.0f / 360, .52f, .89f},
		{0.0f, 0.0f, 0.8f} };
	if (ImGui::CollapsingHeader("Effort Values", ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Average EV range filtering: Tables must have average EV yields\nin the defined ranges. This may allow you to EV train faster by\nfinding places with multiple Pokemon who yield the desired stat.");
		static std::vector<const char*> Gen1avgstatLabels = { "%.f\nHiP", "%.f\nAtk", "%.f\nDef", "%.f\nSpc", "%.f\nSpD", "%.f\nSpe", " %.f\nTotal" };
		static std::vector<const char*> Gen2avgstatLabels = { "%.f\nHiP", "%.f\nAtk", "%.f\nDef", "%.f\nSpA", "%.f\nSpD", "%.f\nSpe", " %.f\nTotal" };
		static std::vector<const char*> Gen3avgstatLabels = { "%.1f\nHiP", "%.1f\nAtk", "%.1f\nDef", "%.1f\nSpA", "%.1f\nSpD", "%.1f\nSpe", " %.1f\nTotal" };
		static std::vector<float> minAvgEV = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
		static std::vector<float> Gen1maxAvgEV = { 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 255.0f, 680.0f };
		static std::vector<float> Gen3maxAvgEV = { 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f };
		ImGui::BeginTable("averageEVfiltering", 2);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Minimum average EV");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Maximum average EV");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		int maxi = 7;
		for (int i = 0; i < maxi; i++)
		{
			if (i == OFFSET_SP_DEFENSE && game->generation == 1) continue;
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderFloat("##minavg", ImVec2(i == maxi - 1 ? 40.0f : 30.0f, game->generation >= 3 ? 80.0f : 255.0f), &minAvgEV[i],
				0.0f, game->generation >= 3 ? 3.0f : 255.0f, game->generation == 1 ? Gen1avgstatLabels[i] : game->generation == 2 || game->generation == GENERATION_ALL ? Gen2avgstatLabels[i] : Gen3avgstatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::TableSetColumnIndex(1);
		for (int i = 0; i < maxi; i++)
		{
			if (i == OFFSET_SP_DEFENSE && game->generation == 1) continue;
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderFloat("##maxavg", ImVec2(i == maxi - 1 ? 40.0f : 30.0f, game->generation >= 3 ? 80.0f : 255.0f),
				game->generation >= 3 ? &Gen3maxAvgEV[i] : &Gen1maxAvgEV[i],
				0.0f, game->generation >= 3 ? 3.0f : 255.0f, game->generation == 1 ? Gen1avgstatLabels[i] : game->generation == 2 || game->generation == GENERATION_ALL ? Gen2avgstatLabels[i] : Gen3avgstatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::EndTable();
		g_newsettings.minAvgEV = minAvgEV;
		g_newsettings.maxAvgEV = game->generation >= 3 ? Gen3maxAvgEV : Gen1maxAvgEV;
		
		ImGui::Separator();

		ImGui::Text("At least one pokemon in each table must have EV yields in these ranges to be shown.\nUse this for more standard EV training.");
		static std::vector<const char*> Gen1singlemonStatLabels = { "%i\nHiP", "%i\nAtk", "%i\nDef", "%i\nSpc", "%i\nSpD", "%i\nSpe", " %i\nTotal" };
		static std::vector<const char*> Gen2singlemonStatLabels = { "%i\nHiP", "%i\nAtk", "%i\nDef", "%i\nSpA", "%i\nSpD", "%i\nSpe", " %i\nTotal" };
		static std::vector<const char*> Gen3singlemonStatLabels = { " %i\nHiP", " %i\nAtk", " %i\nDef", " %i\nSpA", " %i\nSpD", " %i\nSpe", "  %i\nTotal" };
		static std::vector<int> minSingleMonEV = { 0, 0, 0, 0, 0, 0, 0 };
		static std::vector<int> Gen1maxSingleMonEV = { 255, 255, 255, 255, 255, 255, 680 };
		static std::vector<int> Gen3maxSingleMonEV = { 3, 3, 3, 3, 3, 3, 3 };
		ImGui::BeginTable("singleMonEVfiltering", 2);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Minimum EVs");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Maximum EVs");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		for (int i = 0; i < maxi; i++)
		{
			if (i == OFFSET_SP_DEFENSE && game->generation == 1) continue;
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderInt("##minsingle", ImVec2(i == maxi - 1 ? 40.0f : 30.0f, game->generation >= 3 ? 80.0f : 255.0f), &minSingleMonEV[i],
				0, game->generation >= 3 ? 3 : i == OFFSET_TOTAL ? 680 : 255, game->generation == 1 ? Gen1singlemonStatLabels[i] : game->generation == 2 || game->generation == GENERATION_ALL ? Gen2singlemonStatLabels[i] : Gen3singlemonStatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::TableSetColumnIndex(1);
		for (int i = 0; i < maxi; i++)
		{
			if (i == OFFSET_SP_DEFENSE && game->generation == 1) continue;
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderInt("##maxsingle", ImVec2(i == maxi - 1 ? 40.0f : 30.0f, game->generation >= 3 ? 80.0f : 255.0f),
				game->generation >= 3 ? &Gen3maxSingleMonEV[i] : &Gen1maxSingleMonEV[i],
				0, game->generation >= 3 ? 3 : i == OFFSET_TOTAL ? 680 : 255, game->generation == 1 ? Gen1singlemonStatLabels[i] : game->generation == 2 || game->generation == GENERATION_ALL ? Gen2singlemonStatLabels[i] : Gen3singlemonStatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::EndTable();
		g_newsettings.minSingleMonEV = minSingleMonEV;
		g_newsettings.maxSingleMonEV = game->generation >= 3 ? Gen3maxSingleMonEV : Gen1maxSingleMonEV;
	}

	//Transform does not copy yield stats until gen 3
	//therefore we check for >= 3 here
	if (game->generation >= 3 && ImGui::CollapsingHeader("Party Pokemon", ImGuiTreeNodeFlags_None))
	{
		ImGui::InputInt("Party Base EXP Yield", &g_newsettings.partyYields[OFFSET_BEY]);
		ImGui::SameLine();
		if (game->generation <= 4)
			HelpMarker("When wild Ditto uses Transform, it will copy the base experience yield stat of its target. If you'll allow Ditto to Transform, enter the value it will copy. Default 61.");
		else
			HelpMarker("When wild Ditto uses Transform, it will copy the base experience yield stat of its target. If you'll allow Ditto to Transform, enter the value it will copy. Default 101.");
		g_newsettings.partyYields[OFFSET_BEY] = g_newsettings.partyYields[OFFSET_BEY];
		ImGui::Text("Party EV Yields");
		ImGui::SameLine();
		HelpMarker("When wild Ditto uses Transform, it will copy the EV yield stats of its target. If you'll allow Ditto to Transform, enter the values it will copy. Default is 1 in HP.");
		static std::vector<const char*> statLabels = { " %i\nHiP", " %i\nAtk", " %i\nDef", " %i\nSpA", " %i\nSpD", " %i\nSpe" };
		for (int i = 0; i < OFFSET_SPEED + 1; i++)
		{
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderInt("##partyEVs", ImVec2(i == 6 ? 40.0f : 30.0f, 80.0f), &g_newsettings.partyYields[i], 0, 3, statLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		g_newsettings.partyYields[OFFSET_TOTAL] =
			g_newsettings.partyYields[OFFSET_HP] + g_newsettings.partyYields[OFFSET_ATTACK] + g_newsettings.partyYields[OFFSET_DEFENSE] +
			g_newsettings.partyYields[OFFSET_SP_ATTACK] + g_newsettings.partyYields[OFFSET_SP_DEFENSE] + g_newsettings.partyYields[OFFSET_SPEED];
	}

	if (ImGui::CollapsingHeader("Sorting", ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Sort tables by... (Use after pressing Go!)");
		if (ImGui::Button("Average Experience Per Encounter"))
			std::sort(maintables.begin(), maintables.end(), compareByExp);
		ImGui::SameLine(); ImGui::Text("(Default)");

		if (ImGui::Button("Place Name"))
			std::sort(maintables.begin(), maintables.end(), compareByPlacename);

		if (ImGui::Button("Method Name"))
			std::sort(maintables.begin(), maintables.end(), compareByMethod);

		if (ImGui::Button("Game Name"))
			std::sort(maintables.begin(), maintables.end(), compareByVersion);

		if (ImGui::Button("Level Range"))
			std::sort(maintables.begin(), maintables.end(), compareByLevelRange);

		ImGui::Separator();

		ImGui::Text("Average Selected EVs Per Encounter");

		ImGui::PushID(0);
		if (ImGui::Button("HP"))
			std::sort(maintables.begin(), maintables.end(), compareByAverageHPEV);
		ImGui::SameLine();
		if (ImGui::Button("Atk"))
			std::sort(maintables.begin(), maintables.end(), compareByAverageAttackEV);
		ImGui::SameLine();
		if (ImGui::Button("Def"))
			std::sort(maintables.begin(), maintables.end(), compareByAverageDefenseEV);
		ImGui::SameLine();
		if (game->generation == 1)
		{
			if (ImGui::Button("Spc"))
				std::sort(maintables.begin(), maintables.end(), compareByAverageSpAtkEV);
			ImGui::SameLine();
		}
		else
		{
			if (ImGui::Button("SpA"))
				std::sort(maintables.begin(), maintables.end(), compareByAverageSpAtkEV);
			ImGui::SameLine();
			if (ImGui::Button("SpD"))
				std::sort(maintables.begin(), maintables.end(), compareByAverageSpDefEV);
			ImGui::SameLine();
		}
		if (ImGui::Button("Spe"))
			std::sort(maintables.begin(), maintables.end(), compareByAverageSpeedEV);
		ImGui::SameLine();
		if (ImGui::Button("Total"))
			std::sort(maintables.begin(), maintables.end(), compareByAverageTotalEV);
		ImGui::PopID();

		ImGui::Text("Lowest Experience Per Selected EV");

		ImGui::PushID(1);
		if (ImGui::Button("HP"))
		{
			std::sort(maintables.begin(), maintables.end(), compareByExpPerHPEV);
			for (EncounterTable& table : maintables)
				table.header = to_string((int)trunc(table.efficientEVs[OFFSET_HP].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_HP].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
		}
		ImGui::SameLine();
		if (ImGui::Button("Atk"))
		{
			std::sort(maintables.begin(), maintables.end(), compareByExpPerAttackEV);
			for (EncounterTable& table : maintables)
				table.header = to_string((int)trunc(table.efficientEVs[OFFSET_ATTACK].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_ATTACK].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
		}
		ImGui::SameLine();
		if (ImGui::Button("Def"))
		{
			std::sort(maintables.begin(), maintables.end(), compareByExpPerDefenseEV);
			for (EncounterTable& table : maintables)
				table.header = to_string((int)trunc(table.efficientEVs[OFFSET_DEFENSE].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_DEFENSE].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
		}
		ImGui::SameLine();
		if (game->generation == 1)
		{
			if (ImGui::Button("Spc"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerSpAtkEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_SP_ATTACK].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_SP_ATTACK].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
		}
		else
		{
			if (ImGui::Button("SpA"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerSpAtkEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_SP_ATTACK].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_SP_ATTACK].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
			if (ImGui::Button("SpD"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerSpDefEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_SP_DEFENSE].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_SP_DEFENSE].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
		}
		if (ImGui::Button("Spe"))
		{
			std::sort(maintables.begin(), maintables.end(), compareByExpPerSpeedEV);
			for (EncounterTable& table : maintables)
				table.header = to_string((int)trunc(table.efficientEVs[OFFSET_SPEED].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_SPEED].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
		}
		ImGui::SameLine();
		if (ImGui::Button("Total"))
		{
			std::sort(maintables.begin(), maintables.end(), compareByExpPerTotalEV);
			for (EncounterTable& table : maintables)
				table.header = to_string((int)trunc(table.efficientEVs[OFFSET_TOTAL].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_TOTAL].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
		}
		ImGui::PopID();

		ImGui::Separator();

		ImGui::Text("Sort slots within tables by...");

		if (ImGui::Button("Pokemon Name"))
			for (EncounterTable& table : maintables)
				std::sort(table.encounters.begin(), table.encounters.end(), compareByMonName);

		if (ImGui::Button("Chance"))
			for (EncounterTable& table : maintables)
				std::sort(table.encounters.begin(), table.encounters.end(), compareByChance);

		if (ImGui::Button("Average Level"))
			for (EncounterTable& table : maintables)
				std::sort(table.encounters.begin(), table.encounters.end(), compareByAvgLevel);

		if (ImGui::Button("Average Experience"))
			for (EncounterTable& table : maintables)
				std::sort(table.encounters.begin(), table.encounters.end(), compareByAvgExp);

		if (ImGui::Button("Weighted Average Experience"))
			for (EncounterTable& table : maintables)
				std::sort(table.encounters.begin(), table.encounters.end(), compareByAEW);
		ImGui::SameLine(); ImGui::Text("(Default)"); ImGui::SameLine(); HelpMarker("This tells you who is contributing the most experience the most often.");
	}

	if (ImGui::CollapsingHeader("Misc. Settings", ImGuiTreeNodeFlags_None))
	{
		static bool printtext = false;
		ImGui::Checkbox("Text Output", &printtext);
		g_newsettings.printtext = printtext;

		if (ImGui::Button("Cull Tables Without Multiple of Species"))
		{
			for (EncounterTable& table : maintables)
				std::sort(table.encounters.begin(), table.encounters.end(), compareByMonName);

			for (EncounterTable& table : maintables)
			{
				bool found = false;
				string lastmonname;
				for (Encounter encounter : table.encounters)
				{
					if (lastmonname == encounter.pokemonname)
					{
						//cout << lastmonname << " == " << encounter.pokemonname << "\n";
						found = true;
					}
					lastmonname = encounter.pokemonname;
				}
				if (!found)
				{
					cout << "Culling table " << table.placename << ", " << g_methods[table.method_index]->uiname << "\n";
					table.filterReason = Reason_LackingDuplicateMons;
				}
			}
		}
		ImGui::SameLine(); WarnMarker("This is a one-way operation. Hidden tables will be inaccessible until the Go button is used again.");
	}
}

static void UITableDisplay(EncounterTable table, GameObject* game)
{
	bool showWarning = (g_settings.pkmntypewarn && table.filterReason == Reason_BadType);
	if (table.filterReason == Reason_None || showWarning)
	{
		if (showWarning)
		{
			WarnMarker(table.warning.c_str()); ImGui::SameLine();
		}
		if (ImGui::CollapsingHeader(table.header.c_str()))
		{
			bool wishiwashi = false;
			bool minior = false;
			if (ImGui::BeginTable("showencountertable", 6, ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable))
			{
				ImGui::TableSetupColumn("Pokemon");
				ImGui::TableSetupColumn("Chance");
				ImGui::TableSetupColumn("Level");
				ImGui::TableSetupColumn("Base Exp. Yield");
				ImGui::TableSetupColumn("Avg. Exp.");
				ImGui::TableSetupColumn("Weighted Avg. Exp.");
				ImGui::TableHeadersRow();
				int i = 0;
				for (Encounter encounter : table.encounters)
				{
					if (game->generation >= 7 || game->generation == GENERATION_ALL)
					{
						if (encounter.pokemonname == "wishiwashi")
							wishiwashi = true;
						if (encounter.pokemonname == "minior")
							minior = true;
					}
					ImGui::TableNextRow();

					//pokemon
					ImGui::TableNextColumn();
					//convert pokeapi name to bulbapedia url name
					string wikiName = encounter.pokemonname;
					if (wikiName == "nidoran-f")
						wikiName = "nidoran-F";
					if (wikiName == "nidoran-m")
						wikiName = "nidoran-M";
					if (wikiName == "mr-mime")
						wikiName = "mr-Mime";
					if (wikiName == "mime-jr")
						wikiName = "mime-Jr";
					if (wikiName.find("basculin") != string::npos)
						wikiName = "basculin";
					if (wikiName.find("pumpkaboo") != string::npos)
						wikiName = "pumpkaboo";
					if (wikiName.find("oricorio") != string::npos)
						wikiName = "oricorio";
					if (wikiName.find("lycanroc") != string::npos)
						wikiName = "lycanroc";
					if (wikiName.find("-alola") != string::npos)
						wikiName = wikiName.substr(0, wikiName.length() - wikiName.find("-alola"));
					//keep dash if you're jangmo-o line
					if (wikiName.find("mo-o") == string::npos)
						std::replace(wikiName.begin(), wikiName.end(), '-', '_');
					string url = "https://bulbapedia.bulbagarden.net/wiki/" + wikiName;
					ImGui::PushID(i);
					ImGui::TextLinkOpenURL(encounter.pokemonname.c_str(), url.c_str());
					ImGui::PopID();

					//chance
					ImGui::TableNextColumn();
					string pct = to_string(encounter.chance) + "%";
					ImGui::Text(pct.c_str());

					//level
					if (encounter.minlevel == encounter.maxlevel)
					{
						ImGui::TableNextColumn();
						string level = to_string(encounter.minlevel);
						ImGui::Text(level.c_str());
					}
					else
					{
						ImGui::TableNextColumn();
						string level = to_string(encounter.minlevel) + " - " + to_string(encounter.maxlevel);
						ImGui::Text(level.c_str());
					}

					//BEY
					ImGui::TableNextColumn();
					if ((game->generation >= 7 || game->generation == GENERATION_ALL) && encounter.pokemonname == "wishiwashi")
					{
						ImGui::Text("61/217");
					}
					else
					{
						string baseexp = to_string((long)trunc(encounter.baseExp));
						ImGui::Text(baseexp.c_str());
					}

					//avg exp
					ImGui::TableNextColumn();
					string avgexp = to_string((long)trunc(encounter.avgexp));
					ImGui::Text(avgexp.c_str());

					//avg exp weighted
					ImGui::TableNextColumn();
					if (encounter.avgexpweighted < 10)
					{
						string avgexpweighted = to_string(encounter.avgexpweighted);
						ImGui::Text(avgexpweighted.c_str());
					}
					else
					{
						string avgexpweighted = to_string((long)trunc(encounter.avgexpweighted));
						ImGui::Text(avgexpweighted.c_str());
					}
					i++;
				}
				ImGui::EndTable();
			}
			if (wishiwashi)
			{
				ImGui::Text("Wishiwashi turn into School form starting at level 20, which gives 255%% more EXP.");
				ImGui::Text("The numbers above were calculated using School form where applicable.");
			}
			if (minior)
			{
				ImGui::Text("Minior defeated while in Core form give 13%% more EXP.");
				ImGui::Text("The numbers above assume Minior is defeated in Core form.");
				ImGui::Text("Meteor form gives 1 EV in both defense stats, and Core 1 in both attack stats.");
			}
			static const char* Gen1ilabels[] = { "Hit Points", "Physical Attack", "Physical Defense", "Special", "Speed", "Total" };
			static const char* Gen2ilabels[] = { "Hit Points", "Physical Attack", "Physical Defense", "Special Attack", "Special Defense", "Speed", "Total" };
			static const char* emptylabels[] = { "", "", "", "", "", "", "" };
			int gen = g_games[table.version_index]->generation;
			double graphmax = gen >= 3 ? 3 : 600;
			int ticks = gen >= 3 ? 16 : 13;
			ImPlot::PushColormap(gen == 1 ? ImPlotColormap_PKMNstatsGen1 : ImPlotColormap_PKMNstats);
			if (ImPlot::BeginPlot("##xxAverage EV yields", ImVec2(-1, 0), ImPlotFlags_NoInputs))
			{
				ImPlot::SetupAxisTicks(ImAxis_X1, -0.5, gen == 1 ? 5.5 : 6.5, 0, emptylabels);
				ImPlot::SetupAxisTicks(ImAxis_Y1, 0.0, graphmax, ticks);
				ImPlot::SetupAxesLimits(-0.5, gen == 1 ? 5.5 : 6.5, 0, graphmax);
				ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
				int maxi = 7;
				for (int i = 0; i < maxi; i++)
				{
					if (i == OFFSET_SP_DEFENSE && gen == 1) continue;
					ImPlot::PlotBars(gen == 1 ? Gen1ilabels[i >= OFFSET_SP_DEFENSE ? i - 1 : i] : Gen2ilabels[i], &table.averageYields[i], 1, 1, i >= OFFSET_SP_DEFENSE && gen == 1 ? i - 1 : i);
				}
				ImPlot::EndPlot();
			}
			ImPlot::PopColormap();
		}
	}
}

//progress file describes how areas unlock with game progress. see yellow.pro for an explanation of most features.
void ParseProgressFile(int game_index)
{
	g_milestones.clear();
	g_checkpointnames.clear();
	string progressfilepath = g_pkmndatapath + "progress/" + g_games[game_index]->progressfile;
	//both checkpoints and oneways are Milestones.
	//oneways are a type of checkpoint.
	//checkboxes are Milestones but not checkpoints.
	//the main Milestone list in the UI is made of checkpoints (which includes oneways) exclusively. checkboxes go into a separate list.
	size_t lastcheckpointslot = ((std::size_t)(-1));
	size_t lastmilestoneslot = ((std::size_t)(-1));
	ifstream ReadFile(progressfilepath);
	string textLine;
	while (getline(ReadFile, textLine))
	{
		size_t s1End = textLine.find("//");
		string str1;

		if (s1End == string::npos)
			str1 = textLine;
		else
			str1 = textLine.substr(0, s1End);

		if (str1.empty())
			continue;

		string ms_name;
		MilestoneType ms_type = MILESTONE_NONE;
		int ms_id = 0;
		size_t ms_parent = ((std::size_t)(-1));

		if (str1.find("checkpoint ") != string::npos)
		{
			ms_type = MILESTONE_CHECKPOINT;
			ms_name = str1.substr(11);
		}

		if (ms_type != MILESTONE_CHECKPOINT && lastcheckpointslot == -1)
		{
			cout << "ERROR on line: " + textLine + "\nNo checkpoint to attach this item to!\n";
			return;
		}

		if (str1.find("oneway ") != string::npos)
		{
			ms_type = MILESTONE_ONEWAY;
			size_t s2End = str1.find(" ", 7);
			string str2 = str1.substr(7, s2End - 7);
			ms_id = stoi(str2);
			ms_parent = lastcheckpointslot;
			string str3 = str1.substr(s2End + 1);
			ms_name = str3;
		}

		if (ms_type != MILESTONE_CHECKPOINT && lastmilestoneslot == -1)
		{
			cout << "ERROR on line: " + textLine + "\nNo milestone to attach this item to!\n";
			return;
		}

		if (str1.find("checkbox ") != string::npos)
		{
			ms_type = MILESTONE_CHECKBOX;
			size_t s2End = str1.find(" ", 9);
			string str2 = str1.substr(9, s2End - 9);
			ms_id = stoi(str2);
			ms_parent = lastcheckpointslot;
			string str3 = str1.substr(s2End + 1);
			ms_name = str3;
		}
		
		if (ms_type == MILESTONE_NONE && lastmilestoneslot != -1)
		{
			if (str1.find("unlock ") != string::npos)
			{
				g_milestones[lastmilestoneslot].unlocks.push_back(str1.substr(7));
				continue;
			}

			if (str1.find("remove ") != string::npos)
			{
				size_t s2End = str1.find(" ", 7);
				string str2 = str1.substr(7, s2End - 7);
				g_milestones[lastmilestoneslot].removes.push_back(stoi(str2));
				continue;
			}

			if (str1.find("cancel ") != string::npos)
			{
				size_t s2End = str1.find(" ", 7);
				string str2 = str1.substr(7, s2End - 7);
				int idtocancel = stoi(str2);
				for (Milestone ms : g_milestones)
				{
					if (ms.id == idtocancel)
					{
						if (ms.type == MILESTONE_CHECKBOX)
						{
							//critical: one checkbox is not allowed to cancel another
							assert(g_milestones[lastmilestoneslot].type != MILESTONE_CHECKBOX);
						}
						break;
					}
				}
				g_milestones[lastmilestoneslot].cancels.push_back(stoi(str2));
				continue;
			}

			//only thing left to try is a number, which would indicate a location-area file number
			//some such lines might have the word "exclude" after them, so take that off first
			size_t s2End = str1.find(" ");
			string str2;

			if (s2End == string::npos)
				str2 = str1;
			else
				str2 = str1.substr(0, s2End);

			//check if string is a number
			if (strspn(str2.c_str(), "0123456789") == str2.size())
			{
				g_milestones[lastmilestoneslot].tables.push_back(stoi(str2));
				//now parse exclude word
				if (s2End != string::npos)
				{
					string str3 = str1.substr(s2End + 9);//" exclude " is 9 chars
					MethodExclude* newME = new MethodExclude;
					newME->slot = g_milestones[lastmilestoneslot].tables.size() - 1;
					newME->str = str3;
					g_milestones[lastmilestoneslot].excludes.push_back(newME);
				}
				continue;
			}
			else
			{
				cout << "ERROR on line: " + textLine + "\nString '" + str2 + "' not recognized.\n";
				return;
			}
		}

		if (ms_type == MILESTONE_NONE)
		{
			cout << "ERROR on line: " + textLine + "\nCommand not recognized.\n";
			return;
		}

		Milestone* newMS = new Milestone;
		newMS->name = ms_name;
		newMS->type = ms_type;
		newMS->id = ms_id;
		newMS->parentslot = ms_parent;
		//first checkpoint should always be active
		if (lastcheckpointslot == -1)
			newMS->userselected = true;
		g_milestones.push_back(*newMS);
		if (ms_type == MILESTONE_CHECKPOINT || ms_type == MILESTONE_ONEWAY)
		{
			lastcheckpointslot = g_milestones.size() - 1;
			g_checkpointnames.push_back(ms_name);
		}
		lastmilestoneslot = g_milestones.size() - 1;
	}
}

static void UIMainWindow()
{
#ifndef _DEBUG
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
#endif
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	const char* f_games[] = { "Blue", "Red", "Yellow",//2
		"Gold", "Silver", "Crystal",//5
		"Ruby", "Sapphire", "Emerald", "FireRed", "LeafGreen",//10
		"Diamond", "Pearl", "Platinum", "HeartGold", "SoulSilver",//15
		"Black", "White", "Black 2", "White 2",//19
		"X", "Y",//21
		"Sun", "Moon", "Ultra Sun", "Ultra Moon", "All"//26 (ALLGAMES_INDEX)
	};

	ImGui::Begin("Options", false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
	static int game_current = 0;
	ImGui::Combo("Game", &game_current, f_games, IM_ARRAYSIZE(f_games));
	g_newsettings.wantedgame_index = game_current;

	bool rse = false;
	bool dpp = false;
	bool hgss = false;
	bool allgames = false;

	switch (g_newsettings.wantedgame_index)
	{
	case GAME_RUBY:
	case GAME_SAPPHIRE:
	case GAME_EMERALD:
		rse = true;
		break;
	case GAME_DIAMOND:
	case GAME_PEARL:
	case GAME_PLATINUM:
		dpp = true;
		break;
	case GAME_HEARTGOLD:
	case GAME_SOULSILVER:
		hgss = true;
		break;
	case ALLGAMES_INDEX:
		allgames = true;
		break;
	}

	GameObject *game = g_games[g_newsettings.wantedgame_index];

	if (g_settingswindowdata.wantedgame_index_lastframe != g_newsettings.wantedgame_index)
	{
		if (GameHasProgressFile(g_newsettings.wantedgame_index))
			ParseProgressFile(g_newsettings.wantedgame_index);
	}
	
	UIMiscSettings(game, allgames, rse, dpp, hgss);
	UISettingSections(game, allgames, hgss);

	if (g_settingswindowdata.running && g_settingswindowdata.progress == 1.00)
		TablePostProcess();

	if (ImGui::Button("Go!") && !g_settingswindowdata.running)
	{
		//only save the new settings once the button is pressed. this prevents changing the settings mid-iteration.
		g_settings.wantedgame_index = g_newsettings.wantedgame_index;
		g_settings.wantedtime = g_newsettings.wantedtime;
		g_settings.wantedseason = g_newsettings.wantedseason;
		g_settings.wantswarm = g_newsettings.wantswarm;
		g_settings.wantradar = g_newsettings.wantradar;
		g_settings.wantedslot2game = g_newsettings.wantedslot2game;
		g_settings.wantedradiostation = g_newsettings.wantedradiostation;
		g_settings.repellevel = g_newsettings.repellevel;
		g_settings.maxallowedlevel = g_newsettings.maxallowedlevel;
		g_settings.printtext = g_newsettings.printtext;
		g_settings.methodflags = g_newsettings.methodflags;
		g_settings.pkmnfiltertypeflags = g_newsettings.pkmnfiltertypeflags;
		g_settings.pkmnrequiretypeflags = g_newsettings.pkmnrequiretypeflags;
		g_settings.scalinglevel = g_newsettings.scalinglevel;
		g_settings.minAvgEV = g_newsettings.minAvgEV;
		g_settings.maxAvgEV = g_newsettings.maxAvgEV;
		g_settings.minSingleMonEV = g_newsettings.minSingleMonEV;
		g_settings.maxSingleMonEV = g_newsettings.maxSingleMonEV;
		g_settings.partyYields = g_newsettings.partyYields;
		g_settings.useprogressfilter = g_newsettings.useprogressfilter;
		g_settings.selected_checkpoint_slot = g_newsettings.selected_checkpoint_slot;

		maintables.clear();
		maintables.shrink_to_fit();
#ifdef _DEBUG
		g_debugdata.clear();
#endif //_DEBUG
		g_settingswindowdata.running = true;
		thread task(ReadTables);
		task.detach();
	}

	if (g_settingswindowdata.running)
	{
		int totalrange = 0;
		for (int i = 0; i < game->folderRanges.size(); i += 2)
		{
			int a = i + 1;
			totalrange += (game->folderRanges[a] - game->folderRanges[i]) + 1;
		}
		ImGui::SameLine(); ImGui::ProgressBar((float)g_settingswindowdata.progress, ImVec2(ImGui::GetFontSize() * 25.0f, 0.0f));
		string imgoing = "Searching through " + to_string(totalrange) + " locations.";
		ImGui::Text(imgoing.c_str());
	}

	//detect when settings have changed
	if (g_settings.wantedgame_index != g_newsettings.wantedgame_index ||
		g_settings.wantedseason != g_newsettings.wantedseason ||
		g_settings.wantswarm != g_newsettings.wantswarm ||
		g_settings.wantradar != g_newsettings.wantradar ||
		g_settings.wantedslot2game != g_newsettings.wantedslot2game ||
		g_settings.wantedradiostation != g_newsettings.wantedradiostation ||
		g_settings.repellevel != g_newsettings.repellevel ||
		g_settings.maxallowedlevel != g_newsettings.maxallowedlevel ||
		g_settings.methodflags != g_newsettings.methodflags ||
		g_settings.pkmnfiltertypeflags != g_newsettings.pkmnfiltertypeflags ||
		g_settings.pkmnrequiretypeflags != g_newsettings.pkmnrequiretypeflags ||
		g_settings.scalinglevel != g_newsettings.scalinglevel ||
		g_settings.minAvgEV != g_newsettings.minAvgEV ||
		g_settings.maxAvgEV != g_newsettings.maxAvgEV ||
		g_settings.minSingleMonEV != g_newsettings.minSingleMonEV ||
		g_settings.maxSingleMonEV != g_newsettings.maxSingleMonEV ||
		g_settings.partyYields != g_newsettings.partyYields ||
		g_settings.useprogressfilter != g_newsettings.useprogressfilter ||
		g_settings.selected_checkpoint_slot != g_newsettings.selected_checkpoint_slot)
	{
		if (!maintables.empty())
		{
			ImGui::SameLine(); ImGui::Text("Settings changed");
		}
	}

	if (!g_settingswindowdata.running)
		for (EncounterTable& table : maintables)
			UITableDisplay(table, game);

	ImGui::End();
	g_settingswindowdata.generation_lastframe = game->generation;
	g_settingswindowdata.wantedgame_index_lastframe = g_newsettings.wantedgame_index;
}

static void RegisterGame(const char* uiname, const char* internalname, const char* expfile, const char* progressfile, int generation, vector<int> folderRanges)
{
	GameObject* newGame = new GameObject;
	newGame->uiname = uiname;
	newGame->internalname = internalname;
	newGame->expfile = expfile;
	newGame->progressfile = progressfile;
	newGame->generation = generation;
	newGame->folderRanges = folderRanges;
	g_games.push_back(newGame);
}

static void RegisterGames()
{
	//g1
	RegisterGame("Blue", "blue", "gen1_exp.csv", "gen1.pro", 1, { 258 , 349 });
	RegisterGame("Red", "red", "gen1_exp.csv", "gen1.pro", 1, { 258 , 349 });
	RegisterGame("Yellow", "yellow", "gen1_exp.csv", "gen1.pro", 1, { 258 , 349 });
	//g2
	RegisterGame("Gold", "gold", "gen2_exp.csv", "gold.pro", 2, { 184, 349, 798, 798 });
	RegisterGame("Silver", "silver", "gen2_exp.csv", "silver.pro", 2, { 184, 349, 798, 798 });
	RegisterGame("Crystal", "crystal", "gen2_exp.csv", "crystal.pro", 2, { 184, 349, 798, 798 });
	//g3
	RegisterGame("Ruby", "ruby", "gen3_exp.csv", "", 3, { 350, 449 });
	RegisterGame("Sapphire", "sapphire", "gen3_exp.csv", "", 3, { 350, 449 });
	RegisterGame("Emerald", "emerald", "gen3_exp.csv", "", 3, { 350, 449 });
	RegisterGame("FireRed", "firered", "gen3_exp.csv", "", 3, { 258, 572, 825, 825 });
	RegisterGame("LeafGreen", "leafgreen", "gen3_exp.csv", "", 3, { 258, 572, 825, 825 });
	//g4
	RegisterGame("Diamond", "diamond", "gen4_exp.csv", "", 4, { 1, 183 });
	RegisterGame("Pearl", "pearl", "gen4_exp.csv", "", 4, { 1, 183 });
	RegisterGame("Platinum", "platinum", "gen4_exp.csv", "", 4, { 1, 183 });
	RegisterGame("HeartGold", "heartgold", "gen4_exp.csv", "", 4, { 184, 349 });
	RegisterGame("SoulSilver", "soulsilver", "gen4_exp.csv", "", 4, { 184, 349 });
	//g5
	RegisterGame("Black", "black", "bw1_exp.csv", "", 5, { 576, 655 });
	RegisterGame("White", "white", "bw1_exp.csv", "", 5, { 576, 655 });
	RegisterGame("Black 2", "black-2", "bw2_exp.csv", "", 5, { 576, 707 });
	RegisterGame("White 2", "white-2", "bw2_exp.csv", "", 5, { 576, 707 });
	//g6
	RegisterGame("X", "x", "gen6_exp.csv", "", 6, { 708, 760 });
	RegisterGame("Y", "y", "gen6_exp.csv", "", 6, { 708, 760 });
	//g7
	RegisterGame("Sun", "sun", "gen7_exp.csv", "", 7, { 1035, 1156 });
	RegisterGame("Moon", "moon", "gen7_exp.csv", "", 7, { 1035, 1156 });
	RegisterGame("Ultra Sun", "ultra-sun", "gen7_exp.csv", "", 7, { 1035, 1156 });
	RegisterGame("Ultra Moon", "ultra-moon", "gen7_exp.csv", "", 7, { 1035, 1156 });
	//extras
	RegisterGame("All", "all", "ALL_EXPFILE", "", GENERATION_ALL, { 1, 1156 });
}

static void RegisterMethod(const char* uiname, const char* internalname, int flag)
{
	MethodObject* newMethod = new MethodObject;
	newMethod->uiname = uiname;
	newMethod->internalname = internalname;
	newMethod->flag = flag;
	g_methods.push_back(newMethod);
}

static void RegisterMethods()
{
	//g1
	RegisterMethod("Walk", "walk", MethodFilterFlags_Walk);
	RegisterMethod("Surf", "surf", MethodFilterFlags_Surf);
	RegisterMethod("Old Rod", "old-rod", MethodFilterFlags_RodOld);
	RegisterMethod("Good Rod", "good-rod", MethodFilterFlags_RodGood);
	RegisterMethod("Super Rod", "super-rod", MethodFilterFlags_RodSuper);
	//g2
	RegisterMethod("Rock Smash", "rock-smash", MethodFilterFlags_RockSmash);
	RegisterMethod("Headbutt (Low)", "headbutt-low", MethodFilterFlags_Headbutt);
	RegisterMethod("Headbutt (High)", "headbutt-high", MethodFilterFlags_Headbutt);
	//g5
	RegisterMethod("Dark Grass", "dark-grass", MethodFilterFlags_DarkGrass);
	RegisterMethod("Rustling Grass", "grass-spots", MethodFilterFlags_Phenomena);
	RegisterMethod("Dust Clouds", "cave-spots", MethodFilterFlags_Phenomena);
	RegisterMethod("Shadows", "bridge-spots", MethodFilterFlags_Phenomena);
	RegisterMethod("Rippling Water (Fishing)", "super-rod-spots", MethodFilterFlags_Phenomena);
	RegisterMethod("Rippling Water (Surfing)", "surf-spots", MethodFilterFlags_Phenomena);
	//g6
	RegisterMethod("Red Flowers", "red-flowers", MethodFilterFlags_Walk);
	RegisterMethod("Yellow Flowers", "yellow-flowers", MethodFilterFlags_Walk);
	RegisterMethod("Purple Flowers", "purple-flowers", MethodFilterFlags_Walk);
	RegisterMethod("Swamp", "swamp", MethodFilterFlags_Walk);
	RegisterMethod("Shadows", "ambush-shadow", MethodFilterFlags_Ambush);
	RegisterMethod("Rustling Bushes", "ambush-bush", MethodFilterFlags_Ambush);
	//g7
	RegisterMethod("Bubbling Spots", "bubbling-spots", MethodFilterFlags_BubblingSpots);
	RegisterMethod("Rustling Grass", "ambush-grass", MethodFilterFlags_Ambush);
	RegisterMethod("Water Splashes", "ambush-splash", MethodFilterFlags_Ambush);
	RegisterMethod("Rustling Trees", "ambush-tree", MethodFilterFlags_Ambush);
	RegisterMethod("Dirt Clouds", "ambush-dirt", MethodFilterFlags_Ambush);
	RegisterMethod("Chase", "ambush-chase", MethodFilterFlags_Ambush);
	RegisterMethod("Sand Clouds", "ambush-sand", MethodFilterFlags_Ambush);
}

//most stuff below this line is imgui
int main(int, char**)
{
	cout << "Oh hi, I'm the text output window. If you use the text output option, a purely text-based\n";
	cout << "output will be printed inside me. This was the program's output from before it had a GUI.\n";
	cout << "You may want to use the old output if you have some kind of specific analytical use.\n";

	RegisterGames();
	RegisterMethods();
	
	//must parse blue's progress file on start
	ParseProgressFile(GAME_BLUE);
	
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ExpSqueeze", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"ExpSqueeze", WS_OVERLAPPEDWINDOW, 100, 100, 600, 600, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
	ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Handle lost D3D9 device
        if (g_DeviceLost)
        {
            HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
            if (hr == D3DERR_DEVICELOST)
            {
                ::Sleep(10);
                continue;
            }
            if (hr == D3DERR_DEVICENOTRESET)
                ResetDevice();
            g_DeviceLost = false;
        }

        // Handle window resize (we don't resize directly in the WM_SIZE handler)
        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            g_d3dpp.BackBufferWidth = g_ResizeWidth;
            g_d3dpp.BackBufferHeight = g_ResizeHeight;
            g_ResizeWidth = g_ResizeHeight = 0;
            ResetDevice();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

#ifdef _DEBUG
        ImGui::ShowDemoWindow();
		ImPlot::ShowDemoWindow();
#endif

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        UIMainWindow();

        // Rendering
		ImGui::PopStyleVar();
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*clear_color.w*255.0f), (int)(clear_color.y*clear_color.w*255.0f), (int)(clear_color.z*clear_color.w*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
        if (result == D3DERR_DEVICELOST)
            g_DeviceLost = true;
    }

    // Cleanup
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
	ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
