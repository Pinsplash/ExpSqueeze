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
//#include "implot_demo.cpp"
//#include "implot.cpp"
//#include "implot_items.cpp"
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
	METHOD_SEAWEED,
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
	MethodFilterFlags_Seaweed		= 1 << 7,
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
	Reason_LackingDuplicateMons
};

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
	//int movefiltertypeflags = 0;
	int scalinglevel = 0;
	bool abilitywarn = true;
	std::vector<float> minAvgEV = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::vector<float> maxAvgEV = {3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f};
	std::vector<int> minSingleMonEV = {0, 0, 0, 0, 0, 0, 0};
	std::vector<int> maxSingleMonEV = {3, 3, 3, 3, 3, 3, 3};
};

struct SettingsWindowData
{
	int generation_lastframe = 0;
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
	vector<Encounter> encounters;
	int filenumber = 0;
	__int64 expectedtotalpercent = 0;
	double totalavgexp = 0;
	__int64 totalchance = 0;
	int filterReason = Reason_None;
	int version_index = 0;
	string header;
	string warning;
	__int64 lowestlevel = 0;
	__int64 highestlevel = 0;
	bool goodtype = false;
	bool goodEVs = false;
	std::vector<double> averageEVs = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	EfficientEVData efficientEVs[7];
};

struct GameObject
{
	string uiname;
	string internalname;
	string expfile;
	//string versiongroup;
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

vector<MethodObject*> g_methods;
vector<EncounterTable> maintables;

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
	if (flags & MethodFilterFlags_Seaweed) cout << "-MethodFilterFlags_Seaweed\n";
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

static void RegisterEncounter(Settings* settings, __int64 chance, __int64 minlevel, __int64 maxlevel, string pokemonname, string placename, int method_index, int version_index, int i, int filterReason, string warning, bool goodtype, bool goodEVs)
{
	//throw out the whole table
	if (settings->maxallowedlevel < maxlevel)
		filterReason = Reason_OverLevelCap;

	if (settings->repellevel <= maxlevel)
	{
		Encounter newEnc;
		newEnc.chance = chance;
		newEnc.maxlevel = maxlevel;
		newEnc.minlevel = minlevel;
		newEnc.pokemonname = pokemonname;
		bool makenewtable = true;
		for (EncounterTable& table : maintables)
		{
			if (table.placename == placename && table.method_index == method_index && (settings->wantedgame_index != ALLGAMES_INDEX || table.version_index == version_index))
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
}

static bool isEqualString(char* initial, char* compare)
{
	return !strcmp(initial, compare);
}

/*
static bool isEqualString(json_value* initial, json_value* compare)
{
	assert(initial->type == json_string);
	return initial->u.string.ptr == compare->u.string.ptr;
}

static bool isEqualString(json_value* initial, char* compare)
{
	assert(initial->type == json_string);
	return initial->u.string.ptr == compare;
}

static bool isEqualInt(json_value* initial, json_value* compare)
{
	assert(initial->type == json_integer);
	return (long)initial->u.integer == compare->u.integer;
}

static bool isEqualDouble(json_value* initial, json_value* compare)
{
	assert(initial->type == json_double);
	return initial->u.dbl == compare->u.dbl;
}

static bool isEqualBool(json_value* initial, json_value* compare)
{
	assert(initial->type == json_boolean);
	return initial->u.boolean == compare->u.boolean;
}

static void ExplainJSONValue(json_value* obj)
{
	const char* str2 = " ";
	long myint;
	double mydub;
	char* mystring;
	bool mybool;
	switch (obj->type)
	{
	case json_none:
		str2 = " type: none";
		break;
	case json_null:
		str2 = " type: null";
		break;
	case json_object:
		str2 = " type: object";
		break;
	case json_array:
		str2 = " type: array";
		break;
	case json_integer:
		myint = (long)obj->u.integer;
		char intbuffer[128];
		snprintf(intbuffer, sizeof(intbuffer), " type: integer (%i)", myint);
		str2 = intbuffer;
		break;
	case json_double:
		mydub = obj->u.dbl;
		char dubbuffer[128];
		snprintf(dubbuffer, sizeof(dubbuffer), " type: double (%lf)", mydub);
		str2 = dubbuffer;
		break;
	case json_string:
		mystring = obj->u.string.ptr;
		char stringbuffer[128];
		snprintf(stringbuffer, sizeof(stringbuffer), " type: string (%s)", mystring);
		str2 = stringbuffer;
		break;
	case json_boolean:
		mybool = obj->u.boolean;
		char booleanbuffer[128];
		snprintf(booleanbuffer, sizeof(booleanbuffer), " type: bool (%s)", mybool ? "TRUE" : "FALSE");
		str2 = booleanbuffer;
		break;
	}
	//cout << str2 << "\n";
}

static void ExplainObjectEntry(json_object_entry obj)
{
	//string combinestring(string("val name: ") + obj.name);
	//const char* str1 = combinestring.c_str();
	//cout << str1;
	ExplainJSONValue(obj.value);
}
*/
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
/*
static int FindObjectInArrayByName(json_value* initialObject, char* name)
{
	assert(initialObject->type == json_array);
	int arrayLength = initialObject->u.array.length;
	//cout << "FindObjectInArrayByName: arrayLength: " << to_string(arrayLength) << "\n";
	for (int i = 0; i < arrayLength; i++)
	{
		json_value* obj = initialObject->u.array.values[i];
		//cout << "FindObjectInArrayByName: " + to_string(i); ExplainJSONValue(obj);
		if (obj->type == json_object)
		{
			int objectArrayLength = obj->u.object.length;
			//cout << "FindObjectInArrayByName: " + to_string(i) + " objectArrayLength: " << to_string(objectArrayLength) << "\n";
			for (int j = 0; j < objectArrayLength; j++)
			{
				json_object_entry val = obj->u.object.values[j];
				//cout << "FindObjectInArrayByName: " + to_string(i) + " " + to_string(j) + " "; ExplainObjectEntry(val);
				if (isEqualString(val.name, name))
				{
					//return index object is at in array
					//cout << "FindObjectInArrayByName: " + to_string(i) + " " + to_string(j) + " MATCHED\n";
					return i;
				}
			}
		}
	}
	return -1;
}
*/
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
/*
static bool isEqual(json_value* initial, json_value* compare)
{
	switch (initial->type)
	{
	case json_integer:
		isEqualInt(initial, compare);
		break;
	case json_double:
		isEqualDouble(initial, compare);
		break;
	case json_string:
		isEqualString(initial, compare);
		break;
	case json_boolean:
		isEqualBool(initial, compare);
		break;
	default:
		assert(0);
	}
}
*/
static bool InvalidateCondition(Settings* settings, string condition, int iFile)
{
	if (condition == "time-morning" || condition == "time-day" || condition == "time-night")
	{
		string wantedtime = settings->wantedtime;
		//no morning in gen 7. if the wanted game is All and the wanted time is morning, pretend we wanted day instead
		if (settings->wantedtime == "time-morning" && iFile >= 1035)
			wantedtime = "time-day";
		//time: morning/day/night (relevant in gens 2, 4, 7)
		if (wantedtime != condition)
			return true;
	}
	if (condition == "season-spring" || condition == "season-summer" || condition == "season-autumn" || condition == "season-winter")
	{
		//season: spring/summer/autumn/winter (gen 5)
		if (settings->wantedseason != condition)
			return true;
	}
	if (condition == "swarm-yes" || condition == "swarm-no")
	{
		//swarm: yes/no (gen 2-5)
		if (settings->wantswarm != (condition == "swarm-yes"))
			return true;
	}
	if (condition == "radar-on" || condition == "radar-off")
	{
		//radar: on/off (DPP)
		if (settings->wantradar != (condition == "radar-on"))
			return true;
	}
	if (condition == "slot2-none" || condition == "slot2-ruby" || condition == "slot2-sapphire" || condition == "slot2-emerald" || condition == "slot2-firered" || condition == "slot2-leafgreen")
	{
		//slot2: none/ruby/sapphire/emerald/firered/leafgreen (DPP)
		if (settings->wantedslot2game != condition)
			return true;
	}
	if (condition == "radio-off" || condition == "radio-hoenn" || condition == "radio-sinnoh")
	{
		//radio: off/hoenn/sinnoh (HGSS)
		if (settings->wantedradiostation != condition)
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

static bool IsPokemonMatchingType(Settings* settings, string path, string version, int flags, string pokemonname, string* warning)
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
		if (settings->wantedgame_index == ALLGAMES_INDEX)
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
			gameindex = settings->wantedgame_index;
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
/*
static bool PokemonHasBadMove(Settings* settings, string basepath, string path, int version_index, int flags)
{
	path = basepath + path;
	FILE* fp;
	struct stat filestatus;
	int file_size;
	char* file_contents;
	json_char* json;
	json_value* file;
	if (stat(path.c_str(), &filestatus) != 0)
	{
		//cout << "File " + path + " not found\n";
		//return quietly
		assert(0);
		return 1;
	}
	file_size = filestatus.st_size;
	file_contents = (char*)malloc(filestatus.st_size);
	if (!file_contents)
	{
		cout << "Memory error: unable to allocate " + to_string(file_size) + " bytes\n";
		assert(0);
		return 0;
	}
	fp = fopen(path.c_str(), "rb");
	if (!fp)
	{
		cout << "Unable to open " + path + "\n";
		//fclose(fp);
		free(file_contents);
		assert(0);
		return 0;
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
		return 0;
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
		return 0;
	}
	//there are only 3 attacking moves that have ever changed type. it's 100% not worth checking for altered types dynamically.
	//2:  karate chop: normal -> fighting
	//16: gust:		   normal -> flying
	//44: bite:		   normal -> dark
	//all 3 were changed in gen 2
	bool gen1 = (version_index <= 2);
	//find all level-up moves with a level less than or equal to the encounter's max level
	json_value* moves = FindArrayInObjectByName(file, "moves");
	if (!moves)
	{
		json_value_free(file);
		free(file_contents);
		assert(0);
		return 0;
	}
	for (size_t movesIdx = 0; movesIdx < moves->u.array.length; movesIdx++)
	{
		json_value* moveEntry = moves->u.array.values[movesIdx];
		if (!moveEntry)
		{
			json_value_free(file);
			free(file_contents);
			assert(0);
			return 0;
		}
		json_value* versionGroupDetails = FindArrayInObjectByName(moveEntry, "version_group_details");
		if (!versionGroupDetails)
		{
			json_value_free(file);
			free(file_contents);
			assert(0);
			return 0;
		}
		for (size_t VGDIdx = 0; VGDIdx < versionGroupDetails->u.array.length; VGDIdx++)
		{
			json_value* VGDEntry = versionGroupDetails->u.array.values[VGDIdx];
			if (!VGDEntry)
			{
				json_value_free(file);
				free(file_contents);
				assert(0);
				return 0;
			}
			__int64 moveLevel = FindValueInObjectByKey(VGDEntry, "level_learned_at")->u.integer;

			//current working model of how movesets are made for wild pokemon:
			//-4 most recent level up moves
			//-evolution, egg, tutor, TM/HM, and other more niche move types not included
			//-the only exceptions are encounters not in the scope of this program (such as USUM necrozma reportedly or dexnav)
			
			//old games have a bug where a pokemon will not know a given move if the relevant slot (A) is 2 or 3 slots away from a slot (B) where the pokemon also learns that same move.
			//There is most likely a caveat where the move will in fact be learned if slot B was affected by this bug earlier.
			//The working model for the original way the game generated movesets is this:
			//for (int i = 1; i < chosen_level; i++)
			//{
			//  If we want to learn a new move x
			//	{
			//    If move x is the same as a move we currently know, don't learn it.
			// 
			//    If there are 4 moves learned, forget the oldest move.
			//	}
			//}
			//proof:
			//https://www.youtube.com/watch?v=8CfVFjQk6Jg yellow, cerulean cave. golbat can be seen using supersonic, confuse ray, wing attack, and haze. they should have bite instead of supersonic.
			//gen 2: ___
			//report of this being fixed by gen 3 but still want to personally confirm
			
			//easy way to tell if a move is non-level-up.
			if (moveLevel == 0)
		}
	}
	
	//sort the list by level (high to low)
	//ignore everything after four slots once the minimum level is reached

	json_value_free(file);
	free(file_contents);
	assert(0);
	return 0;
}
*/
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

static bool ParseEncounterDetails(Settings* settings, json_value* encdetailblock, string pokemonname, string placename, int version_index, int iFile, bool filterReason/*, string basepath, string url*/, string warning, bool goodtype, bool goodEVs)
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
			if (InvalidateCondition(settings, condition, iFile))
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
	int method_index = ValidateMethod(settings->methodflags, method);
	if (method_index == -1)
		return false;

	//all good
	__int64 chance = FindValueInObjectByKey(encdetailblock, "chance")->u.integer;
	__int64 maxlevel = FindValueInObjectByKey(encdetailblock, "max_level")->u.integer;
	__int64 minlevel = FindValueInObjectByKey(encdetailblock, "min_level")->u.integer;
	RegisterEncounter(settings, chance, minlevel, maxlevel, pokemonname, placename, method_index, version_index, iFile, filterReason, warning, goodtype, goodEVs);
	return true;
}

#define OFFSET_BEY 0
#define OFFSET_HP 1
#define OFFSET_ATTACK 2
#define OFFSET_DEFENSE 3
#define OFFSET_SP_ATTACK 4
#define OFFSET_SP_DEFENSE 5
#define OFFSET_SPEED 6
#define OFFSET_TOTAL 7
static bool FindInExpFile(int offset, string basepath, string expfile, string pokemonname, int* stat)
{
	string path = basepath + "exp-gain-stats/" + expfile;
	ifstream ReadFile(path);
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
				s2Start = s2End + 1;
				s2End = textLine.find(',', s2Start + 1);
				str2 = textLine.substr(s2Start, s2End - s2Start);
			}
			foundmon = true;
			*stat = stoi(str2);
			return true;
		}
	}
	return false;
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

static bool IsPokemonInEVRangeGen3(Settings* settings, string basepath, string version, string pokemonname)
{
	string expfile;
	if (settings->wantedgame_index == ALLGAMES_INDEX)
	{
		for (int i = 0; i < GAMES_TOTAL; i++)
		{
			if (version == g_games[i]->internalname)
			{
				expfile = g_games[i]->expfile;
				break;
			}
		}
	}
	else
		expfile = g_games[settings->wantedgame_index]->expfile;
	for (int i = 1; i < 8; i++)
	{
		int stat;
		if (!FindInExpFile(i, basepath, expfile, pokemonname, &stat))
		{
			cout << "ERROR: Could not find pokemon named '" << pokemonname << "' in " << expfile << "\n";
			continue;
		}
		if (stat < settings->minSingleMonEV[i - 1] || stat > settings->maxSingleMonEV[i - 1])
			return false;
	}
	return true;
}

static int ParseLocationDataFile(string basepath, int iFile, Settings* settings)
{
	//if (iFile != 57)
	//	return 1;
	string path = basepath + "api\\v2\\location-area\\" + to_string(iFile) + "\\index.json";
	//cout << path << "\n";
	GameObject* game = g_games[settings->wantedgame_index];
	string placename;//only one place name per file
	FILE* fp;
	struct stat filestatus;
	int file_size;
	char* file_contents;
	json_char* json;
	json_value* file;
	if (stat(path.c_str(), &filestatus) != 0)
	{
		//cout << "File " + path + " not found\n";
		//for whatever reason, some folders are missing from pokeapi.
		//the first missing file as of right now is #65. this folder is also missing in their live api (https://pokeapi.co/api/v2/location-area/65/)
		//so it's not simply an omission on my part or the api-data repo.
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
	fp = fopen(path.c_str(), "rb");
	if (!fp)
	{
		cout << "Unable to open " + path + "\n";
		//fclose(fp);
		free(file_contents);
		return 0;
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
			if (givengame == game->internalname || settings->wantedgame_index == ALLGAMES_INDEX)
			{
				//do NOT remove these {} brackets, or the else statement will be linked up to the wrong if statement
				int gameindex;
				if (settings->wantedgame_index == ALLGAMES_INDEX)
				{
					for (gameindex = 0; gameindex < GAMES_TOTAL; gameindex++)
						if (givengame == g_games[gameindex]->internalname)
							break;
				}
				else
					gameindex = settings->wantedgame_index;
				//it was. get pokemon name
				json_value* pokemon = FindObjectInObjectByName(encounterblock, "pokemon");
				if (!pokemon)
				{
					assert(0);
					return 0;
				}
				string pokemonname = FindValueInObjectByKey(pokemon, "name")->u.string.ptr;
				bool filterReason = Reason_None;
				string warning = "";
				if (settings->pkmnfiltertypeflags != 0)
				{
					string url = FindValueInObjectByKey(pokemon, "url")->u.string.ptr;
					if (IsPokemonMatchingType(settings, basepath + url + "index.json", givengame, settings->pkmnfiltertypeflags, pokemonname, &warning))
					{
						//pokemon is a type we don't allow
						//cout << pokemonname << " is bad type\n";
						filterReason = Reason_BadType;
					}
				}
				bool goodtype = false;
				if (settings->pkmnrequiretypeflags != 0)
				{
					string url = FindValueInObjectByKey(pokemon, "url")->u.string.ptr;
					goodtype = IsPokemonMatchingType(settings, basepath + url + "index.json", givengame, settings->pkmnrequiretypeflags, pokemonname, &warning);
				}
				bool goodEVs = g_games[gameindex]->generation >= 3 ? IsPokemonInEVRangeGen3(settings, basepath, givengame, pokemonname) : true;
				//go back up to version details to get encounter details block
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

					if (!ParseEncounterDetails(settings, encdetailblock, pokemonname, placename, gameindex, iFile, filterReason/*, basepath, url*/, warning, goodtype, goodEVs))
						continue;//encounter was bad for some reason
				}
			}
			else
				continue;
		}
	}
	json_value_free(file);
	free(file_contents);
	return 1;
}

static bool compareByExp(const EncounterTable a, const EncounterTable b)
{
	//cout << a.totalavgexp << " > " << b.totalavgexp << "\n";
	//if a table is filtered out then totalavgexp is probably 0
	assert(a.totalavgexp > 0 || a.filterReason != Reason_None);
	assert(b.totalavgexp > 0 || b.filterReason != Reason_None);
	return a.totalavgexp > b.totalavgexp;
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
	return strcmp(g_methods[a.method_index]->uiname.c_str(), g_methods[b.method_index]->uiname.c_str()) < 0;
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
	return a.averageEVs[OFFSET_HP - 1] > b.averageEVs[OFFSET_HP - 1];
}

static bool compareByAverageAttackEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageEVs[OFFSET_ATTACK - 1] > b.averageEVs[OFFSET_ATTACK - 1];
}

static bool compareByAverageDefenseEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageEVs[OFFSET_DEFENSE - 1] > b.averageEVs[OFFSET_DEFENSE - 1];
}

static bool compareByAverageSpAtkEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageEVs[OFFSET_SP_ATTACK - 1] > b.averageEVs[OFFSET_SP_ATTACK - 1];
}

static bool compareByAverageSpDefEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageEVs[OFFSET_SP_DEFENSE - 1] > b.averageEVs[OFFSET_SP_DEFENSE - 1];
}

static bool compareByAverageSpeedEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageEVs[OFFSET_SPEED - 1] > b.averageEVs[OFFSET_SPEED - 1];
}

static bool compareByAverageTotalEV(const EncounterTable a, const EncounterTable b)
{
	return a.averageEVs[OFFSET_TOTAL - 1] > b.averageEVs[OFFSET_TOTAL - 1];
}

static bool compareByExpPerHPEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_HP - 1].expPerEV > b.efficientEVs[OFFSET_HP - 1].expPerEV;
}

static bool compareByExpPerAttackEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_ATTACK - 1].expPerEV > b.efficientEVs[OFFSET_ATTACK - 1].expPerEV;
}

static bool compareByExpPerDefenseEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_DEFENSE - 1].expPerEV > b.efficientEVs[OFFSET_DEFENSE - 1].expPerEV;
}

static bool compareByExpPerSpAtkEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_SP_ATTACK - 1].expPerEV > b.efficientEVs[OFFSET_SP_ATTACK - 1].expPerEV;
}

static bool compareByExpPerSpDefEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_SP_DEFENSE - 1].expPerEV > b.efficientEVs[OFFSET_SP_DEFENSE - 1].expPerEV;
}

static bool compareByExpPerSpeedEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_SPEED - 1].expPerEV > b.efficientEVs[OFFSET_SPEED - 1].expPerEV;
}

static bool compareByExpPerTotalEV(const EncounterTable a, const EncounterTable b)
{
	return a.efficientEVs[OFFSET_TOTAL - 1].expPerEV > b.efficientEVs[OFFSET_TOTAL - 1].expPerEV;
}

static bool ReadTables(Settings* settings, SettingsWindowData* settingswindowdata, string basepath)
{
	if (settings->printtext) cout << "Reading encounter data\n";
	GameObject* game = g_games[settings->wantedgame_index];
	
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
		if (settings->printtext) cout << "|     PROGRESS     |\n";
		for (int j = game->folderRanges[i]; j <= game->folderRanges[a]; j++)
		{
			filesread++;
			settingswindowdata->progress = (1 - (static_cast<float>(totalfiles) - filesread) / totalfiles) * 0.5;
			if (settingswindowdata->progress >= (notch * 0.05))
			{
				if (settings->printtext) cout << "-";
				notch++;
			}
			if (ParseLocationDataFile(basepath, j, settings) == 0)
				return true;
		}
	}

	if (settings->printtext) cout << "\n";
	int numTablesProcessed = 0;
	for (EncounterTable& table : maintables)
	{
		string expfile = game->expfile;
		int generation = game->generation;
		if (settings->wantedgame_index == ALLGAMES_INDEX)
		{
			//change exp file based on table's game
			expfile = g_games[table.version_index]->expfile;
			generation = g_games[table.version_index]->generation;
		}
		//progress bar
		numTablesProcessed++;
		//cout << to_string(settingswindowdata->progress) << "\n";
		if (settingswindowdata->progress >= (notch * 0.05))
		{
			if (settings->printtext) cout << "-";
			notch++;
		}
		//really prefer to not save tables that i know are bad, but this is by far the least painful way to take care of this
		if (table.filterReason == Reason_None || (settings->pkmntypewarn && table.filterReason == Reason_BadType))
		{
			if (settings->printtext) cout << "\n" << table.placename << ", " << g_methods[table.method_index]->uiname << ", " << g_games[table.version_index]->uiname << "\n";
			table.totalavgexp = 0;
			table.totalchance = 0;//sanity check: this number should always = 100 or expectedtotalpercent at the end of the table.

			bool specialswarm = false;
			int extrachance = 0;
			if (settings->wantswarm && table.method_index == METHOD_WALK && (generation == 5 || table.version_index == GAME_RUBY || table.version_index == GAME_SAPPHIRE || table.version_index == GAME_EMERALD))
			{
				//find gen 3 or 5 swarm encounter. these are different than others because they make the table into X percent the special encounter and 100-X everything else. no slots are overridden.
				string path = basepath + "swarm-data/" + to_string(table.filenumber) + ".txt";
				ifstream ReadFile(path);
				string textLine;
				while (getline(ReadFile, textLine))
				{
					size_t s1End = textLine.find(',');
					string str1 = textLine.substr(0, s1End);//pokemon's name - not verified yet

					size_t s2Start = s1End + 1;
					size_t s2End = textLine.find(',', s2Start + 1);
					string str2 = textLine.substr(s2Start, s2End - s2Start);//level

					if (generation == 5 && str1 == "croagunk" && settings->wantedseason == "season-winter")
						break;//special case. croagunk will not appear in winter even by swarm. when the game tries to spawn a croagunk swarm in winter, it simply will not do anything.

					string targetgame;
					if (table.version_index == GAME_RUBY || table.version_index == GAME_SAPPHIRE)
						targetgame = "rubysapphire";
					else if (table.version_index == GAME_EMERALD)
						targetgame = "emerald";
					else if (table.version_index == GAME_BLACK1)
						targetgame = "b1";
					else if (table.version_index == GAME_BLACK2)
						targetgame = "b2";
					else if (table.version_index == GAME_WHITE1)
						targetgame = "w1";
					else if (table.version_index == GAME_WHITE2)
						targetgame = "w2";
					else
					{
						cout << "ERROR: tried to get gen 3/5 swarm encounter for invalid game '" << g_games[table.version_index]->uiname << "' file number " << to_string(table.filenumber) << "\n";
						continue;
					}
					if (textLine.find(targetgame) != string::npos)
					{
						specialswarm = true;
						extrachance = (generation == 5) ? 40 : 50;

						Encounter newEnc;
						newEnc.chance = extrachance;
						if (table.version_index == GAME_RUBY || table.version_index == GAME_SAPPHIRE || table.version_index == GAME_EMERALD)
						{
							newEnc.maxlevel = stoi(str2);
							newEnc.minlevel = stoi(str2);
						}
						else if (table.version_index == GAME_BLACK1 || table.version_index == GAME_WHITE1)
						{
							newEnc.minlevel = 15;
							newEnc.maxlevel = 55;
						}
						else if (table.version_index == GAME_BLACK2 || table.version_index == GAME_WHITE2)
						{
							newEnc.minlevel = 40;
							newEnc.maxlevel = 55;
						}
						newEnc.pokemonname = str1;
						newEnc.specialswarm = true;
						table.encounters.push_back(newEnc);
						break;
					}
					//didn't find an encounter? that's ok
				}
			}

			for (Encounter& encounter : table.encounters)
			{
				//get experience yield from stripped down bulba tables
				if (!FindInExpFile(OFFSET_BEY, basepath, expfile, encounter.pokemonname, &encounter.baseExp))
				{
					cout << "ERROR: Could not find pokemon named '" << encounter.pokemonname << "' in " << expfile << "\n";
					continue;
				}
				double chancescale = 1;
				if (specialswarm)
					chancescale = (generation == 5) ? 0.6 : 0.5;
				encounter.minlevel = max(encounter.minlevel, settings->repellevel);
				double avglevel = (double)(encounter.maxlevel + encounter.minlevel) / 2;
				encounter.avgexp = CalculateExperienceCore(generation, avglevel, encounter.baseExp);
				//level scaling
				if (settings->scalinglevel != 0)
				{
					encounter.avgexp = ExperienceScaleForLevel(generation, avglevel, settings->scalinglevel, encounter.avgexp);
				}
				assert(encounter.avgexp > 0);
				if (generation >= 3)
				{
					for (int j = 1; j < 8; j++)
					{
						int stat;
						FindInExpFile(j, basepath, expfile, encounter.pokemonname, &stat);
						table.averageEVs[j - 1] += (double)(stat * encounter.chance * chancescale) / table.expectedtotalpercent;
						double lowestExp = CalculateExperienceCore(generation, (double)encounter.minlevel, encounter.baseExp);
						if (stat > 0)
						{
							if (table.efficientEVs[j - 1].expPerEV == 0.0f)
							{
								table.efficientEVs[j - 1].expPerEV = lowestExp / stat;
								table.efficientEVs[j - 1].pokemonname = encounter.pokemonname;
							}
							else
							{
								if (table.efficientEVs[j - 1].expPerEV > lowestExp / stat)
								{
									table.efficientEVs[j - 1].expPerEV = lowestExp / stat;
									table.efficientEVs[j - 1].pokemonname = encounter.pokemonname;
								}
							}
						}
					}
				}
				encounter.avgexpweighted = (double)(encounter.avgexp * encounter.chance * chancescale) / table.expectedtotalpercent;
				if (settings->printtext) cout << encounter.pokemonname << " has " << encounter.chance << "% chance between level " << encounter.minlevel << " and " << encounter.maxlevel << ". avgexp " << encounter.avgexp << ", weighted " << encounter.avgexpweighted << "\n";
#ifdef _DEBUG
				//if (settings->printtext) cout << "totalavgexp " << table.totalavgexp << " += " << encounter.avgexpweighted << "\n";
				//if (settings->printtext) cout << "totalchance " << table.totalchance << " += " << encounter.chance << "\n\n";
#endif //_DEBUG
				table.totalavgexp += encounter.avgexpweighted;
				assert(table.totalavgexp > 0);
				assert(encounter.avgexpweighted > 0);
				table.totalchance += encounter.chance;
			}
			if (settings->printtext) cout << table.totalavgexp << " average EXP in " << table.placename << ", " << g_methods[table.method_index]->uiname << ", " << g_games[table.version_index]->uiname << "\n";
			std::sort(table.encounters.begin(), table.encounters.end(), compareByAEW);

			//unless we're using repel or max level, the table's total chance should always be 100.
			bool errorfound = false;
			if ((settings->repellevel == 0 && settings->maxallowedlevel == 100) && table.totalchance != 100 + extrachance)
				errorfound = true;
			//this check was to find tables that were being deleted incorrectly. now that we don't delete tables for this purpose, this appears to be pointless, but testing is needed.
			else if (table.totalchance != table.expectedtotalpercent + extrachance)
				errorfound = true;
			if (errorfound)
			{
				cout << "ERROR: Total chance was " << table.totalchance << "! File number " << table.filenumber << "\n";
				cout << "wantedgame: " << game->uiname << " totalchance: " << to_string(table.totalchance) << "\n";
				cout << "repellevel: " << to_string(settings->repellevel) << " maxallowedlevel: " << to_string(settings->maxallowedlevel) << "\n";
				cout << "expectedtotalpercent: " << to_string(table.expectedtotalpercent) << "\n";
				cin.get();
				return true;
			}
		}
		settingswindowdata->progress = (1 - (static_cast<float>(maintables.size()) - numTablesProcessed) / maintables.size()) * 0.5 + 0.5;
	}
	if (settings->printtext)
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

static void dosettingswindow(Settings* settings, Settings* newsettings, SettingsWindowData* settingswindowdata, string basepath)
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
	newsettings->wantedgame_index = game_current;

	bool rse = false;
	bool dpp = false;
	bool hgss = false;
	bool allgames = false;

	switch (newsettings->wantedgame_index)
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

	GameObject *game = g_games[newsettings->wantedgame_index];

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
			if (settingswindowdata->generation_lastframe != game->generation)
				settingswindowdata->time_chosen = 0;
		}
		else
		{
			times = "Morning\0Day\0Night\0";
			internal_times = "time-morning\0time-day\0time-night\0";
			height = 3;
			if (settingswindowdata->generation_lastframe != game->generation)
				settingswindowdata->time_chosen = 1;
		}
#pragma warning(suppress: 6384)
		ImGui::Combo("Time of Day", &settingswindowdata->time_chosen, times, IM_ARRAYSIZE(times));
		newsettings->wantedtime = Items_SingleStringGetter((void*)internal_times, settingswindowdata->time_chosen);
	}

	if (allgames || game->generation == 5)
	{
		const char* seasons[] = { "Spring", "Summer", "Autumn", "Winter" };
		const char* internal_seasons[] = { "season-spring", "season-summer", "season-autumn", "season-winter" };
		static int season_current = 0;
		ImGui::Combo("Season", &season_current, seasons, IM_ARRAYSIZE(seasons));
		newsettings->wantedseason = internal_seasons[season_current];
	}

	if (allgames || (game->generation == 2 || rse || game->generation == 4 || game->generation == 5))
	{
		static bool wantswarm = false;
		ImGui::Checkbox("Mass Outbreaks", &wantswarm);
		newsettings->wantswarm = wantswarm;
	}

	if (allgames || dpp)
	{
		static bool wantradar = false;
		ImGui::Checkbox("PokeRadar", &wantradar);
		newsettings->wantradar = wantradar;

		const char* slotgames[] = { "None", "Ruby", "Sapphire", "Emerald", "FireRed", "LeafGreen" };
		const char* internal_slotgames[] = { "slot2-none", "slot2-ruby", "slot2-sapphire", "slot2-emerald", "slot2-firered", "slot2-leafgreen" };
		static int slotgame_current = 0;
		ImGui::Combo("DS Slot 2", &slotgame_current, slotgames, IM_ARRAYSIZE(slotgames));
		ImGui::SameLine(); HelpMarker("Some DS models have a slot for GameBoy games. Having a Generation 3 game in the slot while playing a Generation 4 game can change some encounter tables. Emulators may or may not have a feature for this.");
		newsettings->wantedslot2game = internal_slotgames[slotgame_current];
	}

	if (allgames || hgss)
	{
		const char* stations[] = { "Off/Other", "Hoenn Sound", "Sinnoh Sound" };
		const char* internal_stations[] = { "radio-off", "radio-hoenn", "radio-sinnoh" };
		static int station_current = 0;
		ImGui::Combo("Radio Station", &station_current, stations, IM_ARRAYSIZE(stations));
		newsettings->wantedradiostation = internal_stations[station_current];
	}

	if (game->generation == 5 || game->generation >= 7)
	{
		static int scalinglevel = 0;
		ImGui::InputInt("Level for Scaling Math", &scalinglevel);
		ImGui::SameLine(); HelpMarker("This game scales experience by the difference in levels between the victorious pokemon and defeated pokemon. Enter your pokemon's level to factor in level scaling. Otherwise, use 0.");
		newsettings->scalinglevel = scalinglevel;
	}
	else
		newsettings->scalinglevel = 0;

	static int repellevel = 0;
	ImGui::InputInt("Repel Level", &repellevel);
	ImGui::SameLine();
	if (allgames || (game->generation >= 2 && game->generation <= 5))
		HelpMarker("Repels keep away wild Pokemon who are a lower level than the first NON-FAINTED Pokemon in the party.");
	else
		HelpMarker("Repels keep away wild Pokemon who are a lower level than the first Pokemon in the party.");
	newsettings->repellevel = repellevel;

	static int maxallowedlevel = 100;
	ImGui::InputInt("Maximum Level", &maxallowedlevel);
	ImGui::SameLine(); HelpMarker("Encounter tables with Pokemon above this level will not be shown.");
	newsettings->maxallowedlevel = maxallowedlevel;

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

		if (allgames || rse)
			ImGui::CheckboxFlags("Seaweed", &methodflags, MethodFilterFlags_Seaweed);

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

		newsettings->methodflags = methodflags;
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

		newsettings->pkmnfiltertypeflags = pkmnFilterTypeFlags;

		static bool pkmntypewarn = false;
		ImGui::Checkbox("Don't Hide, Just Warn", &pkmntypewarn);
		newsettings->pkmntypewarn = pkmntypewarn;

		//////////////////////////////////////////////////////////////////////////////
		ImGui::Text(" ");
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

		newsettings->pkmnrequiretypeflags = pkmnRequireTypeFlags;
	}
	/*
	if (ImGui::CollapsingHeader("Move Types", ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Tables with pokemon with ATTACKING MOVES of the selected types will not be shown.");
		ImGui::Text("This can take a long time!");
		static int moveFilterTypeFlags = 0;
		if (ImGui::BeginTable("typetable2", 5, ImGuiTableFlags_None))
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Normal", &moveFilterTypeFlags, 1);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fighting", &moveFilterTypeFlags, 2);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Flying", &moveFilterTypeFlags, 4);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Poison", &moveFilterTypeFlags, 8);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ground", &moveFilterTypeFlags, 16);
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Rock", &moveFilterTypeFlags, 32);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Bug", &moveFilterTypeFlags, 64);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ghost", &moveFilterTypeFlags, 128);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fire", &moveFilterTypeFlags, 512);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Water", &moveFilterTypeFlags, 1024);
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Grass", &moveFilterTypeFlags, 2048);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Electric", &moveFilterTypeFlags, 4096);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Psychic", &moveFilterTypeFlags, 8192);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ice", &moveFilterTypeFlags, 16384);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dragon", &moveFilterTypeFlags, 32768);
			if (allgames || game->generation >= 2)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Steel", &moveFilterTypeFlags, 256);
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dark", &moveFilterTypeFlags, 65536);
				if (allgames || game->generation >= 6)
				{
					ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fairy", &moveFilterTypeFlags, 131072);
				}
			}
		}
		ImGui::EndTable();

		newsettings->movefiltertypeflags = moveFilterTypeFlags;
	}
	*/
	if (game->generation >= 3 && ImGui::CollapsingHeader("Effort Values", ImGuiTreeNodeFlags_None))
	{
		static std::vector<std::vector<float>> statColors = {
			{93.0f / 360, .56f, .90f},
			{50.0f / 360, .57f, .96f},
			{22.0f / 360, .57f, .94f},
			{192.0f / 360, .58f, .96f},
			{227.0f / 360, .41f, .91f},
			{313.0f / 360, .52f, .89f},
			{0.0f, 0.0f, 0.8f}};
		ImGui::Text("Average EV range filtering: Tables must have average EV yields\nin the defined ranges. This may allow you to EV train faster by\nfinding places with multiple Pokemon who yield the desired stat.");
		static std::vector<const char*> avgstatLabels = {"%.1f\nHiP", "%.1f\nAtk", "%.1f\nDef", "%.1f\nSpA", "%.1f\nSpD", "%.1f\nSpe", " %.1f\nTotal"};
		static std::vector<float> minAvgEV = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
		static std::vector<float> maxAvgEV = {3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f};
		ImGui::BeginTable("averageEVfiltering", 2);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Minimum average EV");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Maximum average EV");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		for (int i = 0; i < 7; i++)
		{
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderFloat("##minavg", ImVec2(i == 6 ? 40.0f : 30.0f, 80.0f), &minAvgEV[i], 0.0f, 3.0f, avgstatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::TableSetColumnIndex(1);
		for (int i = 0; i < 7; i++)
		{
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderFloat("##maxavg", ImVec2(i == 6 ? 40.0f : 30.0f, 80.0f), &maxAvgEV[i], 0.0f, 3.0f, avgstatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::EndTable();
		newsettings->minAvgEV = minAvgEV;
		newsettings->maxAvgEV = maxAvgEV;
		//////////////////////////////////////////////////////////////////
		ImGui::Text(" ");
		ImGui::Text("At least one pokemon in each table must have EV yields in these ranges to be shown.\nUse this for more standard EV training.");
		static std::vector<const char*> singlemonStatLabels = {" %i\nHiP", " %i\nAtk", " %i\nDef", " %i\nSpA", " %i\nSpD", " %i\nSpe", "  %i\nTotal"};
		static std::vector<int> minSingleMonEV = {0, 0, 0, 0, 0, 0, 0};
		static std::vector<int> maxSingleMonEV = {3, 3, 3, 3, 3, 3, 3};
		ImGui::BeginTable("singleMonEVfiltering", 2);
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		ImGui::Text("Minimum EVs");
		ImGui::TableSetColumnIndex(1);
		ImGui::Text("Maximum EVs");
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		for (int i = 0; i < 7; i++)
		{
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderInt("##minsingle", ImVec2(i == 6 ? 40.0f : 30.0f, 80.0f), &minSingleMonEV[i], 0, 3, singlemonStatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::TableSetColumnIndex(1);
		for (int i = 0; i < 7; i++)
		{
			if (i > 0) ImGui::SameLine();
			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2] * 0.7f));
			ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor::HSV(statColors[i][0], statColors[i][1], statColors[i][2]));
			ImGui::VSliderInt("##maxsingle", ImVec2(i == 6 ? 40.0f : 30.0f, 80.0f), &maxSingleMonEV[i], 0, 3, singlemonStatLabels[i]);
			ImGui::PopStyleColor(5);
			ImGui::PopID();
		}
		ImGui::EndTable();
		newsettings->minSingleMonEV = minSingleMonEV;
		newsettings->maxSingleMonEV = maxSingleMonEV;
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

		if (game->generation >= 3)
		{
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
			if (ImGui::Button("SpA"))
				std::sort(maintables.begin(), maintables.end(), compareByAverageSpAtkEV);
			ImGui::SameLine();
			if (ImGui::Button("SpD"))
				std::sort(maintables.begin(), maintables.end(), compareByAverageSpDefEV);
			ImGui::SameLine();
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
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_HP - 1].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_HP - 1].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
			if (ImGui::Button("Atk"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerAttackEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_ATTACK - 1].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_ATTACK - 1].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
			if (ImGui::Button("Def"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerDefenseEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_DEFENSE - 1].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_DEFENSE - 1].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
			if (ImGui::Button("SpA"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerSpAtkEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_SP_ATTACK - 1].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_SP_ATTACK - 1].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
			if (ImGui::Button("SpD"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerSpDefEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_SP_DEFENSE - 1].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_SP_DEFENSE - 1].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
			if (ImGui::Button("Spe"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerSpeedEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_SPEED - 1].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_SPEED - 1].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::SameLine();
			if (ImGui::Button("Total"))
			{
				std::sort(maintables.begin(), maintables.end(), compareByExpPerTotalEV);
				for (EncounterTable& table : maintables)
					table.header = to_string((int)trunc(table.efficientEVs[OFFSET_TOTAL - 1].expPerEV)) + " EXP/EV from " + table.efficientEVs[OFFSET_TOTAL - 1].pokemonname + ", " + table.placename + ", " + g_methods[table.method_index]->uiname + ", " + g_games[table.version_index]->uiname;
			}
			ImGui::PopID();
		}

		ImGui::Text("\nSort slots within tables by...");

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
		newsettings->printtext = printtext;

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

	if (settingswindowdata->running)
	{
		//cout << to_string(settingswindowdata->progress) << "\n";
		if (settingswindowdata->progress == 1.00)
		{
			settingswindowdata->running = false;
			bool requiringtype = settings->pkmnrequiretypeflags;
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
				table.header = to_string((int)trunc(table.totalavgexp)) + " EXP, " + table.placename + ", " + methodnamestring + ", " + g_games[table.version_index]->uiname;
				if (!table.goodtype && requiringtype)
					table.filterReason = Reason_NoGoodTypes;
				if (!table.goodEVs)
					table.filterReason = Reason_NoGoodEVs;
				for (int i = 0; i < 7; i++)
				{
					if (table.averageEVs[i] < settings->minAvgEV[i] || table.averageEVs[i] > settings->maxAvgEV[i])
					{
						table.filterReason = Reason_BadEVs;
						break;
					}
				}
			}
		}
	}

	if (ImGui::Button("Go!") && !settingswindowdata->running)
	{
		//only save the new settings once the button is pressed. this prevents changing the settings mid-iteration.
		settings->wantedgame_index = newsettings->wantedgame_index;
		settings->wantedtime = newsettings->wantedtime;
		settings->wantedseason = newsettings->wantedseason;
		settings->wantswarm = newsettings->wantswarm;
		settings->wantradar = newsettings->wantradar;
		settings->wantedslot2game = newsettings->wantedslot2game;
		settings->wantedradiostation = newsettings->wantedradiostation;
		settings->repellevel = newsettings->repellevel;
		settings->maxallowedlevel = newsettings->maxallowedlevel;
		settings->printtext = newsettings->printtext;
		settings->methodflags = newsettings->methodflags;
		settings->pkmnfiltertypeflags = newsettings->pkmnfiltertypeflags;
		settings->pkmnrequiretypeflags = newsettings->pkmnrequiretypeflags;
		settings->pkmntypewarn = newsettings->pkmntypewarn;
		//settings->movefiltertypeflags = newsettings->movefiltertypeflags;
		settings->scalinglevel = newsettings->scalinglevel;
		settings->minAvgEV = newsettings->minAvgEV;
		settings->maxAvgEV = newsettings->maxAvgEV;
		settings->minSingleMonEV = newsettings->minSingleMonEV;
		settings->maxSingleMonEV = newsettings->maxSingleMonEV;

		maintables.clear();
		maintables.shrink_to_fit();
#ifdef _DEBUG
		g_debugdata.clear();
#endif //_DEBUG
		settingswindowdata->running = true;
		thread task(ReadTables, settings, settingswindowdata, basepath);
		task.detach();
	}

	if (settingswindowdata->running)
	{
		int totalrange = 0;
		for (int i = 0; i < game->folderRanges.size(); i += 2)
		{
			int a = i + 1;
			totalrange += (game->folderRanges[a] - game->folderRanges[i]) + 1;
		}
		ImGui::SameLine(); ImGui::ProgressBar((float)settingswindowdata->progress, ImVec2(ImGui::GetFontSize() * 25.0f, 0.0f));
		string imgoing = "Searching through " + to_string(totalrange) + " locations.";
		ImGui::Text(imgoing.c_str());
	}

	//detect when settings have changed
	if (settings->wantedgame_index != newsettings->wantedgame_index ||
		settings->wantedseason != newsettings->wantedseason ||
		settings->wantswarm != newsettings->wantswarm ||
		settings->wantradar != newsettings->wantradar ||
		settings->wantedslot2game != newsettings->wantedslot2game ||
		settings->wantedradiostation != newsettings->wantedradiostation ||
		settings->repellevel != newsettings->repellevel ||
		settings->maxallowedlevel != newsettings->maxallowedlevel ||
		settings->methodflags != newsettings->methodflags ||
		settings->pkmnfiltertypeflags != newsettings->pkmnfiltertypeflags ||
		settings->pkmnrequiretypeflags != newsettings->pkmnrequiretypeflags/* ||
		settings->movefiltertypeflags != newsettings->movefiltertypeflags*/ ||
		settings->scalinglevel != newsettings->scalinglevel ||
		settings->minAvgEV != newsettings->minAvgEV ||
		settings->maxAvgEV != newsettings->maxAvgEV ||
		settings->minSingleMonEV != newsettings->minSingleMonEV ||
		settings->maxSingleMonEV != newsettings->maxSingleMonEV)
	{
		if (!maintables.empty())
		{
			ImGui::SameLine(); ImGui::Text("Settings changed");
		}
	}

	if (!settingswindowdata->running)
	{
		for (EncounterTable& table : maintables)
		{
			bool showWarning = (settings->pkmntypewarn && table.filterReason == Reason_BadType);
			if (table.filterReason == Reason_None || showWarning)
			{
				if (showWarning)
				{
					WarnMarker(table.warning.c_str()); ImGui::SameLine();
				}
				if (ImGui::CollapsingHeader(table.header.c_str()))
				{
					if (ImGui::BeginTable("showencountertable", 6, ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable))
					{
						ImGui::TableSetupColumn("Pokemon");
						ImGui::TableSetupColumn("Chance");
						ImGui::TableSetupColumn("Level");
						ImGui::TableSetupColumn("Base Exp. Yield");
						ImGui::TableSetupColumn("Avg. Exp.");
						ImGui::TableSetupColumn("Weighted Avg. Exp.");
						ImGui::TableHeadersRow();
						for (Encounter encounter : table.encounters)
						{
							ImGui::TableNextRow();

							//pokemon
							ImGui::TableNextColumn();
							ImGui::Text(encounter.pokemonname.c_str());

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
							string baseexp = to_string((long)trunc(encounter.baseExp));
							ImGui::Text(baseexp.c_str());

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
						}
						ImGui::EndTable();
					}
					if (game->generation >= 3)
					{
						static const char* ilabels[] = { "Hit Points", "Physical Attack", "Physical Defense", "Special Attack", "Special Defense", "Speed", "Total" };
						static const char* emptylabels[] = { "", "", "", "", "", "", "" };
						double graphmax = ceil(table.averageEVs[OFFSET_TOTAL - 1]);
						int ticks = floor(graphmax * 5) + 1;
						ImPlot::PushColormap(ImPlotColormap_PKMNstats);
						if (ImPlot::BeginPlot("##xxAverage EV yields", ImVec2(-1, 0), ImPlotFlags_NoInputs))
						{
							ImPlot::SetupAxisTicks(ImAxis_X1, -0.5, 6.5, 0, emptylabels);
							ImPlot::SetupAxisTicks(ImAxis_Y1, 0.0, graphmax, ticks);
							ImPlot::SetupAxesLimits(-0.5, 6.5, 0, graphmax);
							ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
							for (int i = 0; i < 7; i++)
							{
								ImPlot::PlotBars(ilabels[i], &table.averageEVs[i], 1, 1, i);
							}
							ImPlot::EndPlot();
						}
						ImPlot::PopColormap();
					}
				}
			}
		}
	}
	ImGui::End();
	settingswindowdata->generation_lastframe = game->generation;
}

static void RegisterGame(const char* uiname, const char* internalname, const char* expfile, /*const char* versiongroup,*/ int generation, vector<int> folderRanges)
{
	GameObject* newGame = new GameObject;
	newGame->uiname = uiname;
	newGame->internalname = internalname;
	newGame->expfile = expfile;
	newGame->generation = generation;
	newGame->folderRanges = folderRanges;
	//newGame->versiongroup = versiongroup;
	g_games.push_back(newGame);
}

static void RegisterMethod(const char* uiname, const char* internalname, int flag)
{
	MethodObject* newMethod = new MethodObject;
	newMethod->uiname = uiname;
	newMethod->internalname = internalname;
	newMethod->flag = flag;
	g_methods.push_back(newMethod);
}

// Main code
int main(int, char**)
{
	cout << "Oh hi, I'm the text output window. If you use the text output option, a purely text-based\n";
	cout << "output will be printed inside me. This was the program's output from before it had a GUI.\n";
	cout << "You may want to use the old output if you have some kind of specific analytical use.\n";

	//games
	//g1
	RegisterGame("Blue", "blue", "gen123_exp.txt", 1, {258 , 349});
	RegisterGame("Red", "red", "gen123_exp.txt", 1, {258 , 349});
	RegisterGame("Yellow", "yellow", "gen123_exp.txt", 1, {258 , 349});
	//g2
	RegisterGame("Gold", "gold", "gen123_exp.txt", 2, {184, 349, 798, 798});
	RegisterGame("Silver", "silver", "gen123_exp.txt", 2, {184, 349, 798, 798});
	RegisterGame("Crystal", "crystal", "gen123_exp.txt", 2, {184, 349, 798, 798});
	//g3
	RegisterGame("Ruby", "ruby", "gen123_exp.txt", 3, {350, 449});
	RegisterGame("Sapphire", "sapphire", "gen123_exp.txt", 3, {350, 449});
	RegisterGame("Emerald", "emerald", "gen123_exp.txt", 3, {350, 449});
	RegisterGame("FireRed", "firered", "gen123_exp.txt", 3, {258, 572, 825, 825});
	RegisterGame("LeafGreen", "leafgreen", "gen123_exp.txt", 3, {258, 572, 825, 825});
	//g4
	RegisterGame("Diamond", "diamond", "gen4_exp.txt", 4, {1, 183});
	RegisterGame("Pearl", "pearl", "gen4_exp.txt", 4, {1, 183});
	RegisterGame("Platinum", "platinum", "gen4_exp.txt", 4, {1, 183});
	RegisterGame("HeartGold", "heartgold", "gen4_exp.txt", 4, {184, 349});
	RegisterGame("SoulSilver", "soulsilver", "gen4_exp.txt", 4, {184, 349});
	//g5
	RegisterGame("Black", "black", "bw1_exp.txt", 5, {576, 655});
	RegisterGame("White", "white", "bw1_exp.txt", 5, {576, 655});
	RegisterGame("Black 2", "black-2", "bw2_exp.txt", 5, {576, 707});
	RegisterGame("White 2", "white-2", "bw2_exp.txt", 5, {576, 707});
	//g6
	RegisterGame("X", "x", "gen6_exp.txt", 6, {708, 760});
	RegisterGame("Y", "y", "gen6_exp.txt", 6, {708, 760});
	//g7
	RegisterGame("Sun", "sun", "gen7_exp.txt", 7, {1035, 1156});
	RegisterGame("Moon", "moon", "gen7_exp.txt", 7, {1035, 1156});
	RegisterGame("Ultra Sun", "ultra-sun", "gen7_exp.txt", 7, {1035, 1156});
	RegisterGame("Ultra Moon", "ultra-moon", "gen7_exp.txt", 7, {1035, 1156});
	//extras
	RegisterGame("All", "all", "ALL_EXPFILE", 0, {1, 1156});

	//methods
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
	//g3
	RegisterMethod("Seaweed", "seaweed", MethodFilterFlags_Seaweed);
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

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	string basepath = "pkmndata/";
	Settings settings, newsettings;
	SettingsWindowData settingswindowdata;

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
        dosettingswindow(&settings, &newsettings, &settingswindowdata, basepath);

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
