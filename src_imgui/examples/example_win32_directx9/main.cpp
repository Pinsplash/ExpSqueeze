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

const int ALLGAMES_INDEX = 26;//make sure this always matches the last switch case in dosettingswindow()!
const int GAMES_TOTAL = 27;

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
	MethodFilterFlags_RoughTerrain	= 1 << 10,
	MethodFilterFlags_BubblingSpots = 1 << 11,
	MethodFilterFlags_Ambush		= 1 << 12,
	MethodFilterFlags_Last			= 1 << 13,
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
	int maxlevel = 100;
	bool printtext = false;
	int methodflags = MethodFilterFlags_Last - 1;
	int pkmnfiltertypeflags = 0;
};

struct SettingsWindowData
{
	int generation_lastframe = 0;
	int time_chosen = 0;
	bool running = false;
};

struct Encounter
{
	__int64 chance = 0;
	__int64 minlevel = 0;
	__int64 maxlevel = 0;
	string pokemonname;
	double avgexp = 0;
	double avgexpweighted = 0;
};

struct EncounterTable
{
	string method;
	string placename;
	vector<Encounter> encounters;
	int filenumber = 0;
	__int64 expectedtotalpercent = 0;
	double totalavgexp = 0;
	__int64 totalchance = 0;
	bool filterout = false;
	int version_index = 0;
	string header;
};

struct GameObject
{
	string uiname;
	string internalname;
	string expfile;
	int generation = 0;
	vector<int> folderRanges;
};

vector<GameObject*> g_games;

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
	if (flags & MethodFilterFlags_RoughTerrain) cout << "-MethodFilterFlags_RoughTerrain\n";
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
static void RegisterEncounter(Settings* settings, vector<EncounterTable>* maintables, __int64 chance, __int64 minlevel, __int64 maxlevel, string pokemonname, string placename, string method, int version_index, int i, bool tablebad)
{
	//throw out the whole table
	if (settings->maxlevel < maxlevel)
		tablebad = true;

	if (settings->repellevel <= maxlevel)
	{
		Encounter newEnc;
		newEnc.chance = chance;
		newEnc.maxlevel = maxlevel;
		newEnc.minlevel = minlevel;
		newEnc.pokemonname = pokemonname;
		bool makenewtable = true;
		for (EncounterTable& table : *maintables)
		{
			//cout << "Trying table. " << table.placename << " == " << placename << " && " << table.method << " == " << method << "\n";
			if (table.placename == placename && table.method == method && (settings->wantedgame_index != ALLGAMES_INDEX || table.version_index == version_index))
			{
				if (tablebad)
					table.filterout = true;
				makenewtable = false;
				//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
				table.encounters.push_back(newEnc);
				table.expectedtotalpercent += chance;
				break;
			}
		}
		if (makenewtable)
		{
			//cout << "Line " << linenum << ": new table\n";
			EncounterTable newTable;
			newTable.method = method;
			newTable.placename = placename;
			newTable.filenumber = i;
			newTable.expectedtotalpercent = chance;
			newTable.version_index = version_index;
			if (tablebad)
				newTable.filterout = true;
			//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ". (new table)\n";
			newTable.encounters.push_back(newEnc);

			maintables->push_back(newTable);
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

static void StringFlagsContainStringFlag(string strgiven, int flagsgiven, vector<string> strcompare, int flagcompare, int* result)
{
	for (string cmp : strcompare)
	{
		if (strgiven == cmp)
		{
			*result = 1;
			if (flagsgiven & flagcompare)
				*result = 2;
		}
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

static bool FindBadType(json_value* containerobj, int flags)
{
	json_value* types = FindArrayInObjectByName(containerobj, "types");
	if (!types)
	{
		assert(0);
		return 0;
	}
	for (size_t typeIdx = 0; typeIdx < types->u.array.length; typeIdx++)
	{
		json_value* typeobj = types->u.array.values[typeIdx];
		if (!typeobj)
		{
			assert(0);
			return 0;
		}
		json_value* typeobj2 = FindObjectInObjectByName(typeobj, "type");
		if (!typeobj2)
		{
			assert(0);
			return 0;
		}
		string nameoftype = FindValueInObjectByKey(typeobj2, "name")->u.string.ptr;
		if (TypeMatches(flags, nameoftype)) return true;
	}
	return false;
}

static bool IsPokemonBadType(Settings* settings, string path, string version, int flags)
{
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

	json_value* pasttypes = FindArrayInObjectByName(file, "past_types");
	if (!pasttypes)
	{
		assert(0);
		return 0;
	}
	if (pasttypes->u.array.length == 0)
	{
		bool result = FindBadType(file, flags);
		json_value_free(file);
		free(file_contents);
		return result;
	}
	else
	{
		//since pokemon types can vary by generation, make sure we're looking at the data for the correct generation
		//and change generation we look for based on version we found in json file
		int gameindex = 0;
		if (settings->wantedgame_index == ALLGAMES_INDEX)
		{
			for (gameindex = 0; gameindex < GAMES_TOTAL; gameindex++)
			{
				cout << version << " == " << g_games[gameindex]->internalname << "\n";
				if (version == g_games[gameindex]->internalname)
				{
					break;
				}
			}
		}
		else
			gameindex = settings->wantedgame_index;
		assert(gameindex != GAMES_TOTAL);
		for (size_t pasttypeIdx = 0; pasttypeIdx < pasttypes->u.array.length; pasttypeIdx++)
		{
			json_value* pasttypeobj = pasttypes->u.array.values[pasttypeIdx];
			if (!pasttypeobj)
			{
				assert(0);
				return 0;
			}
			json_value* generation = FindObjectInObjectByName(pasttypeobj, "generation");
			if (!generation)
			{
				assert(0);
				return 0;
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
				bool result = FindBadType(pasttypeobj, flags);
				json_value_free(file);
				free(file_contents);
				return result;
			}
			else
			{
				//use pokemon's new type instead
				bool result = FindBadType(file, flags);
				json_value_free(file);
				free(file_contents);
				return result;
			}
		}
	}

	json_value_free(file);
	free(file_contents);
	assert(0);
	return 0;
}

static bool ValidateMethod(int flags, string method)
{
	//these encounter methods are not very useful, if even applicable.
	//sos encounters may come back, if the alola tables are ever fixed.
	if (method == "gift" || method == "gift-egg" || method == "only-one" || method == "pokeflute"
		|| method == "squirt-bottle" || method == "wailmer-pail" || method == "devon-scope"
		|| method == "island-scan" || method == "sos-encounter" || method == "berry-piles"
		|| method == "npc-trade" || method == "sos-from-bubbling-spot" || method == "roaming-grass"
		|| method == "roaming-water" || method == "feebas-tile-fishing")
		return false;
	int result = 0;
	StringFlagsContainStringFlag(method, flags, "walk", MethodFilterFlags_Walk, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "surf", MethodFilterFlags_Surf, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "old-rod", MethodFilterFlags_RodOld, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "good-rod", MethodFilterFlags_RodGood, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "super-rod", MethodFilterFlags_RodSuper, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "rock-smash", MethodFilterFlags_RockSmash, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "seaweed", MethodFilterFlags_Seaweed, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "dark-grass", MethodFilterFlags_DarkGrass, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "rough-terrain", MethodFilterFlags_RoughTerrain, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, "bubbling-spots", MethodFilterFlags_BubblingSpots, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, { "red-flowers", "yellow-flowers", "purple-flowers" }, MethodFilterFlags_Walk, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, { "headbutt-low", "headbutt-normal", "headbutt-high" }, MethodFilterFlags_Headbutt, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, { "grass-spots", "cave-spots", "bridge-spots", "super-rod-spots", "surf-spots" }, MethodFilterFlags_Phenomena, &result);
	if (result == 0) StringFlagsContainStringFlag(method, flags, { "ambush-grass", "ambush-bush", "ambush-splash", "ambush-tree", "ambush-dirt", "ambush-shadow", "ambush-chase", "ambush-sand" }, MethodFilterFlags_Ambush, &result);

	if (result == 0)
		assert(0);//this encounter method is unaccounted for
	else if (result == 1)
		return false;//bad method
	return true;
}

static bool ParseEncounterDetails(Settings* settings, vector<EncounterTable>* maintables, json_value* encdetailblock, string pokemonname, string placename, int version_index, int iFile, bool tablebad)
{
	json_value* conditionvalues = FindArrayInObjectByName(encdetailblock, "condition_values");

	if (conditionvalues)
	{
		bool encounterinvalid = false;

		for (size_t conditionIdx = 0; conditionIdx < conditionvalues->u.array.length; conditionIdx++)
		{
			if (encounterinvalid)
				break;

			json_value* condobj = conditionvalues->u.array.values[conditionIdx];
			string condition = FindValueInObjectByKey(condobj, "name")->u.string.ptr;
#ifdef _DEBUG
			RecordCustomData(condition);
#endif //_DEBUG
			//make sure encounter meets applicable parameters
			if (InvalidateCondition(settings, condition, iFile))
				encounterinvalid = true;
		}

		if (encounterinvalid)
			return false;
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
	if (!ValidateMethod(settings->methodflags, method))
		return false;

	//all good
	__int64 chance = FindValueInObjectByKey(encdetailblock, "chance")->u.integer;
	__int64 maxlevel = FindValueInObjectByKey(encdetailblock, "max_level")->u.integer;
	__int64 minlevel = FindValueInObjectByKey(encdetailblock, "min_level")->u.integer;
	RegisterEncounter(settings, maintables, chance, minlevel, maxlevel, pokemonname, placename, method, version_index, iFile, tablebad);
	return true;
}

static int ParseLocationDataFile(string basepath, int iFile, Settings* settings, vector<EncounterTable>* maintables)
{
	//if (iFile != 57)
	//	return 1;
	string path = basepath + "api\\v2\\location-area\\" + to_string(iFile) + "\\index.json";
	cout << path << "\n";
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
				//it was. get pokemon name
				json_value* pokemon = FindObjectInObjectByName(encounterblock, "pokemon");
				if (!pokemon)
				{
					assert(0);
					return 0;
				}
				string pokemonname = FindValueInObjectByKey(pokemon, "name")->u.string.ptr;
				bool tablebad = false;
				if (settings->pkmnfiltertypeflags != 0)
				{
					string url = FindValueInObjectByKey(pokemon, "url")->u.string.ptr;
					if (IsPokemonBadType(settings, basepath + url + "index.json", givengame, settings->pkmnfiltertypeflags))
					{
						//pokemon is a type we don't allow
						tablebad = true;
					}
				}
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
					int gameindex;
					if (settings->wantedgame_index == ALLGAMES_INDEX)
					{
						for (gameindex = 0; gameindex < GAMES_TOTAL; gameindex++)
						{
							if (givengame == g_games[gameindex]->internalname)
								break;
						}
					}
					else
					{
						gameindex = settings->wantedgame_index;
					}
					if (!ParseEncounterDetails(settings, maintables, encdetailblock, pokemonname, placename, gameindex, iFile, tablebad))
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

static bool compareByExp(const EncounterTable& a, const EncounterTable& b)
{
	return a.totalavgexp > b.totalavgexp;
}

static bool compareByAEW(const Encounter& a, const Encounter& b)
{
	return a.avgexpweighted > b.avgexpweighted;
}

static bool FindBEY(string basepath, string expfile, string pokemonname, int *baseexp)
{
	string path = basepath + "exp-gain-stats/" + expfile;
	ifstream ReadFile(path);
	string textLine;
	bool foundmon = false;
	while (getline(ReadFile, textLine))
	{
		size_t s1End = textLine.find('|');
		string str1 = textLine.substr(0, s1End);

		size_t s2Start = s1End + 1;
		size_t s2End = textLine.find('|', s2Start + 1);
		string str2 = textLine.substr(s2Start, s2End - s2Start);
		//cout << "mon '" << str2 << "'\n";
		if (str1 == pokemonname)
		{
			foundmon = true;
			*baseexp = stoi(str2);
			return true;
		}
	}
	return false;
}

static void ReadTables(Settings* settings, vector<EncounterTable>* maintables, string basepath)
{
	if (settings->printtext) cout << "Reading encounter data\n";
	//go through every file
	GameObject* game = g_games[settings->wantedgame_index];
	for (int i = 0; i < game->folderRanges.size(); i += 2)
	{
		int notch = 1;
		int a = i + 1;
		double range = game->folderRanges[a] - game->folderRanges[i];
		if (settings->printtext) cout << "|     PROGRESS     |\n";
		for (int j = game->folderRanges[i]; j <= game->folderRanges[a]; j++)
		{
			if ((j - game->folderRanges[i]) / range >= (notch * 0.05))
			{
				if (settings->printtext) cout << "-";
				notch++;
			}
			if (ParseLocationDataFile(basepath, j, settings, maintables) == 0)
				return;
		}
	}
	if (settings->printtext) cout << "\n";
	for (EncounterTable &table : *maintables)
	{
		//really prefer to not save tables that i know are bad, but this is by far the least painful way to take care of this
		if (table.filterout)
			continue;
		if (settings->printtext) cout << "\n" << table.placename << ", " << table.method << ", " << g_games[table.version_index]->uiname << "\n";
		table.totalavgexp = 0;
		table.totalchance = 0;//sanity check: this number should always = 100 or expectedtotalpercent at the end of the table.
		for (Encounter &encounter : table.encounters)
		{
			if (settings->wantedgame_index == ALLGAMES_INDEX)
			{
				//change exp file based on table's game
				game->expfile = g_games[table.version_index]->expfile;
			}
			//get experience yield from stripped down bulba tables
			int baseexp = 0;
			if (!FindBEY(basepath, game->expfile, encounter.pokemonname, &baseexp))
			{
				cout << "ERROR: Could not find pokemon named '" << encounter.pokemonname << "' in " << game->expfile << "\n";
				continue;
			}
			encounter.minlevel = max(encounter.minlevel, settings->repellevel);
			//30-30: 1
			//30-31: 2
			//30-40: 11
			__int64 numlevels = (encounter.maxlevel - encounter.minlevel) + 1;
			double levelsum = 0;
			for (int i = 0; i < numlevels; i++)
				levelsum += encounter.minlevel + i;
			double avglevel = levelsum / numlevels;
			int factor = (game->generation == 5 || game->generation >= 7) ? 5 : 7;
			encounter.avgexp = (baseexp * avglevel) / factor;
			encounter.avgexpweighted = (encounter.avgexp * encounter.chance) / table.expectedtotalpercent;
			if (settings->printtext) cout << encounter.pokemonname << " has " << encounter.chance << "% chance between level " << encounter.minlevel << " and " << encounter.maxlevel << ". avgexp " << encounter.avgexp << ", weighted " << encounter.avgexpweighted << "\n";
			table.totalavgexp += encounter.avgexpweighted;
			table.totalchance += encounter.chance;
		}
		if (settings->printtext) cout << table.totalavgexp << " average EXP in " << table.placename << ", " << table.method << ", " << g_games[table.version_index]->uiname << "\n";
		std::sort(table.encounters.begin(), table.encounters.end(), compareByAEW);

		//unless we're using repel or max level, the table's total chance should always be 100.
		bool errorfound = false;
		if ((settings->repellevel == 0 && settings->maxlevel == 100) && table.totalchance != 100)
			errorfound = true;
		//this check was to find tables that were being deleted incorrectly. now that we don't delete tables for this purpose, this appears to be pointless, but testing is needed.
		else if (table.totalchance != table.expectedtotalpercent)
			errorfound = true;
		if (errorfound)
		{
			cout << "ERROR: Total chance was " << table.totalchance << "! File number " << table.filenumber << "\n";
			cout << "wantedgame: " << game->uiname << " totalchance: " << to_string(table.totalchance) << "\n";
			cout << "repellevel: " << to_string(settings->repellevel) << " maxlevel: " << to_string(settings->maxlevel) << "\n";
			cout << "expectedtotalpercent: " << to_string(table.expectedtotalpercent) << "\n";
			cin.get();
			return;
		}
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
}

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.md)
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

static void dosettingswindow(Settings* settings, SettingsWindowData* settingswindowdata, vector<EncounterTable>* maintables, string basepath)
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

	Settings newsettings;
	ImGui::Begin("Options", false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
	static int game_current = 0;
	ImGui::Combo("Game", &game_current, f_games, IM_ARRAYSIZE(f_games));
	newsettings.wantedgame_index = game_current;

	bool rse = false;
	bool dpp = false;
	bool hgss = false;
	bool allgames = false;

	switch (newsettings.wantedgame_index)
	{
	case 6:
	case 7:
	case 8:
		rse = true;
		break;
	case 11:
	case 12:
	case 13:
		dpp = true;
		break;
	case 14:
	case 15:
		hgss = true;
		break;
	case 26://should match ALLGAMES_INDEX
		allgames = true;
		break;
	}

	GameObject *game = g_games[newsettings.wantedgame_index];

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
	//missing gen 7 time data
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
		newsettings.wantedtime = Items_SingleStringGetter((void*)internal_times, settingswindowdata->time_chosen);
	}

	if (allgames || game->generation == 5)
	{
		const char* seasons[] = { "Spring", "Summer", "Autumn", "Winter" };
		const char* internal_seasons[] = { "season-spring", "season-summer", "season-autumn", "season-winter" };
		static int season_current = 0;
		ImGui::Combo("Season", &season_current, seasons, IM_ARRAYSIZE(seasons));
		newsettings.wantedseason = internal_seasons[season_current];
	}

	//mass outbreaks exist in gen 3 and 5, but we have no data for them! braugh!
	if (allgames || (game->generation == 2 || game->generation == 4))
	{
		static bool wantswarm = false;
		ImGui::Checkbox("Mass Outbreaks", &wantswarm);
		newsettings.wantswarm = wantswarm;
	}

	if (allgames || dpp)
	{
		static bool wantradar = false;
		ImGui::Checkbox("PokeRadar", &wantradar);
		newsettings.wantradar = wantradar;

		const char* slotgames[] = { "None", "Ruby", "Sapphire", "Emerald", "FireRed", "LeafGreen" };
		const char* internal_slotgames[] = { "slot2-none", "slot2-ruby", "slot2-sapphire", "slot2-emerald", "slot2-firered", "slot2-leafgreen" };
		static int slotgame_current = 0;
		ImGui::Combo("DS Slot 2", &slotgame_current, slotgames, IM_ARRAYSIZE(slotgames));
		ImGui::SameLine(); HelpMarker("Some DS models have a slot for GameBoy games. Having a Generation 3 game in the slot while playing a Generation 4 game can change some encounter tables. Emulators may or may not have a feature for this.");
		newsettings.wantedslot2game = internal_slotgames[slotgame_current];
	}

	if (allgames || hgss)
	{
		const char* stations[] = { "Off/Other", "Hoenn Sound", "Sinnoh Sound" };
		const char* internal_stations[] = { "radio-off", "radio-hoenn", "radio-sinnoh" };
		static int station_current = 0;
		ImGui::Combo("Radio Station", &station_current, stations, IM_ARRAYSIZE(stations));
		newsettings.wantedradiostation = internal_stations[station_current];
	}

	static int repellevel = 0;
	ImGui::InputInt("Repel Level", &repellevel);
	ImGui::SameLine();
	if (allgames || (game->generation >= 2 && game->generation <= 5))
		HelpMarker("Repels keep away wild Pokemon who are a lower level than the first NON-FAINTED Pokemon in the party.");
	else
		HelpMarker("Repels keep away wild Pokemon who are a lower level than the first Pokemon in the party.");
	newsettings.repellevel = repellevel;

	static int maxlevel = 100;
	ImGui::InputInt("Maximum Level", &maxlevel);
	ImGui::SameLine(); HelpMarker("Encounter tables with Pokemon above this level will not be shown.");
	newsettings.maxlevel = maxlevel;

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
			ImGui::CheckboxFlags("Headbutt", &methodflags, MethodFilterFlags_Headbutt);

		if (allgames || rse)
			ImGui::CheckboxFlags("Seaweed", &methodflags, MethodFilterFlags_Seaweed);

		if (allgames || game->generation == 5)
		{
			ImGui::CheckboxFlags("Dark Grass", &methodflags, MethodFilterFlags_DarkGrass);
			ImGui::CheckboxFlags("Phenomena", &methodflags, MethodFilterFlags_Phenomena);
			ImGui::SameLine(); HelpMarker("Rustling grass, dust clouds, flying pokemon's shadows, and rippling water.");
		}

		if (allgames || game->generation == 6)
		{
			ImGui::CheckboxFlags("Rough Terrain", &methodflags, MethodFilterFlags_RoughTerrain);
			ImGui::SameLine(); HelpMarker("\"Rough Terrain\" encompasses several different kinds of encounters, most of them being unique to a single location, like the snow on Route 17.");
		}

		if (allgames || game->generation == 7)
		{
			ImGui::CheckboxFlags("Fishing at bubbling rock", &methodflags, MethodFilterFlags_BubblingSpots);
			ImGui::CheckboxFlags("Ambush", &methodflags, MethodFilterFlags_Ambush);
			ImGui::SameLine(); HelpMarker("Cases where pokemon have an overworld presence and move, like flying pokemon shadows or rustling grass.");
		}

		newsettings.methodflags = methodflags;
	}

	if (ImGui::CollapsingHeader("Filter Pokemon Types", ImGuiTreeNodeFlags_None))
	{
		ImGui::Text("Tables with pokemon of the selected types will not be shown.");
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
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dragon", &pkmnFilterTypeFlags, 32768);
			ImGui::TableNextRow();
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Water", &pkmnFilterTypeFlags, 1024);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Grass", &pkmnFilterTypeFlags, 2048);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Electric", &pkmnFilterTypeFlags, 4096);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Psychic", &pkmnFilterTypeFlags, 8192);
			ImGui::TableNextColumn(); ImGui::CheckboxFlags("Ice", &pkmnFilterTypeFlags, 16384);
			if (allgames || game->generation >= 2)
			{
				ImGui::TableNextRow();
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Dark", &pkmnFilterTypeFlags, 65536);
				ImGui::TableNextColumn(); ImGui::CheckboxFlags("Steel", &pkmnFilterTypeFlags, 256);
				if (allgames || game->generation >= 6)
				{
					ImGui::TableNextColumn(); ImGui::CheckboxFlags("Fairy", &pkmnFilterTypeFlags, 131072);
				}
			}
		}
		ImGui::EndTable();

		newsettings.pkmnfiltertypeflags = pkmnFilterTypeFlags;
	}

	static bool printtext = false;
	ImGui::Checkbox("Text Output", &printtext);
	newsettings.printtext = printtext;

	if (settingswindowdata->running)
	{
		//only save the new settings once the button is pressed. this prevents changing the settings mid-iteration.
		settings->wantedgame_index = newsettings.wantedgame_index;
		settings->wantedtime = newsettings.wantedtime;
		settings->wantedseason = newsettings.wantedseason;
		settings->wantswarm = newsettings.wantswarm;
		settings->wantradar = newsettings.wantradar;
		settings->wantedslot2game = newsettings.wantedslot2game;
		settings->wantedradiostation = newsettings.wantedradiostation;
		settings->repellevel = newsettings.repellevel;
		settings->maxlevel = newsettings.maxlevel;
		settings->printtext = newsettings.printtext;
		settings->methodflags = newsettings.methodflags;
		settings->pkmnfiltertypeflags = newsettings.pkmnfiltertypeflags;

		/*
		cout << "firstfolder" << to_string(newsettings.firstfolder) << "\n";
		cout << "lastfolder" << to_string(newsettings.lastfolder) << "\n";
		cout << "wantedgame" << newsettings.wantedgame << "\n";
		cout << "wantedtime" << newsettings.wantedtime << "\n";
		cout << "wantedseason" << newsettings.wantedseason << "\n";
		cout << "wantswarm" << (newsettings.wantswarm ? "TRUE" : "FALSE") << "\n";
		cout << "wantradar" << (newsettings.wantradar ? "TRUE" : "FALSE") << "\n";
		cout << "wantedslot2game" << newsettings.wantedslot2game << "\n";
		cout << "wantedradiostation" << newsettings.wantedradiostation << "\n";
		cout << "expfile" << newsettings.expfile << "\n";
		cout << "generation" << to_string(newsettings.generation) << "\n";
		cout << "repellevel" << to_string(newsettings.repellevel) << "\n";
		cout << "maxlevel" << to_string(newsettings.maxlevel) << "\n";
		cout << "printtext" << (newsettings.printtext ? "TRUE" : "FALSE") << "\n";
		PrintMethodFlags(newsettings.methodflags);
		cout << "pkmnfiltertypeflags:\n";
		PrintTypeFlags(newsettings.pkmnfiltertypeflags);
		*/

		ReadTables(settings, maintables, basepath);
		std::sort(maintables->begin(), maintables->end(), compareByExp);
		settingswindowdata->running = false;

		//cache things for tables instead of computing it live every imgui frame
		for (EncounterTable &table : *maintables)
		{
			table.header = to_string((int)trunc(table.totalavgexp)) + " EXP, " + table.placename + ", " + table.method + ", " + g_games[table.version_index]->uiname;
		}
	}

	if (ImGui::Button("Go!"))
	{
		int totalrange = 0;
		for (int i = 0; i < game->folderRanges.size(); i += 2)
		{
			int a = i + 1;
			totalrange += (game->folderRanges[a] - game->folderRanges[i]) + 1;
		}
		string imgoing = "Searching through " + to_string(totalrange) + " tables.";
		ImGui::SameLine(); ImGui::Text(imgoing.c_str());
		maintables->clear();
		maintables->shrink_to_fit();
#ifdef _DEBUG
		g_debugdata.clear();
#endif //_DEBUG
		settingswindowdata->running = true;
	}

	//detect when settings have changed
	if (settings->wantedgame_index != newsettings.wantedgame_index ||
		settings->wantedseason != newsettings.wantedseason ||
		settings->wantswarm != newsettings.wantswarm ||
		settings->wantradar != newsettings.wantradar ||
		settings->wantedslot2game != newsettings.wantedslot2game ||
		settings->wantedradiostation != newsettings.wantedradiostation ||
		settings->repellevel != newsettings.repellevel ||
		settings->maxlevel != newsettings.maxlevel ||
		settings->methodflags != newsettings.methodflags ||
		settings->pkmnfiltertypeflags != newsettings.pkmnfiltertypeflags)
	{
		if (!maintables->empty())
		{
			ImGui::SameLine(); ImGui::Text("Settings changed");
		}
	}

	for (EncounterTable table : *maintables)
	{
		if (table.filterout)
			continue;
		if (ImGui::CollapsingHeader(table.header.c_str()))
		{
			if (ImGui::BeginTable("showencountertable", 5, ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable))
			{
				ImGui::TableSetupColumn("Pokemon");
				ImGui::TableSetupColumn("Chance");
				ImGui::TableSetupColumn("Level");
				ImGui::TableSetupColumn("Avg. Exp.");
				ImGui::TableSetupColumn("Avg. Exp. Weighted");
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

					//avg exp
					ImGui::TableNextColumn();
					string avgexp = to_string((long)trunc(encounter.avgexp));
					ImGui::Text(avgexp.c_str());

					//avg exp weighted
					ImGui::TableNextColumn();
					string avgexpweighted = to_string((long)trunc(encounter.avgexpweighted));
					ImGui::Text(avgexpweighted.c_str());
				}
				ImGui::EndTable();
			}
		}
	}
	ImGui::End();
	settingswindowdata->generation_lastframe = game->generation;
}

static void RegisterGame(const char* uiname, const char* internalname, const char* expfile, int generation, vector<int> folderRanges)
{
	GameObject* newGame = new GameObject;
	newGame->uiname = uiname;
	newGame->internalname = internalname;
	newGame->expfile = expfile;
	newGame->generation = generation;
	newGame->folderRanges = folderRanges;
	g_games.push_back(newGame);
}

// Main code
int main(int, char**)
{
	cout << "Oh hi, I'm the text output window. If you use the text output option, a purely text-based\n";
	cout << "output will be printed inside me. This was the program's output from before it had a GUI.\n";
	cout << "You may want to use the old output if you have some kind of specific analytical use.\n";

	RegisterGame("Blue", "blue", "gen1_exp.txt", 1, { 258 , 349 });
	RegisterGame("Red", "red", "gen1_exp.txt", 1, { 258 , 349 });
	RegisterGame("Yellow", "yellow", "gen1_exp.txt", 1, { 258 , 349 });
	RegisterGame("Gold", "gold", "gen2_exp.txt", 2, { 184, 349, 798, 798 });
	RegisterGame("Silver", "silver", "gen2_exp.txt", 2, { 184, 349, 798, 798 });
	RegisterGame("Crystal", "crystal", "gen2_exp.txt", 2, { 184, 349, 798, 798 });
	RegisterGame("Ruby", "ruby", "gen3_exp.txt", 3, { 350, 449 });
	RegisterGame("Sapphire", "sapphire", "gen3_exp.txt", 3, { 350, 449 });
	RegisterGame("Emerald", "emerald", "gen3_exp.txt", 3, { 350, 449 });
	RegisterGame("FireRed", "firered", "gen3_exp.txt", 3, { 258, 572, 825, 825 });
	RegisterGame("LeafGreen", "leafgreen", "gen3_exp.txt", 3, { 258, 572, 825, 825 });
	RegisterGame("Diamond", "diamond", "gen4_exp.txt", 4, { 1, 183 });
	RegisterGame("Pearl", "pearl", "gen4_exp.txt", 4, { 1, 183 });
	RegisterGame("Platinum", "platinum", "gen4_exp.txt", 4, { 1, 183 });
	RegisterGame("HeartGold", "heartgold", "gen4_exp.txt", 4, { 184, 349 });
	RegisterGame("SoulSilver", "soulsilver", "gen4_exp.txt", 4, { 184, 349 });
	RegisterGame("Black", "black", "gen5bw1_exp.txt", 5, { 576, 655 });
	RegisterGame("White", "white", "gen5bw1_exp.txt", 5, { 576, 655 });
	RegisterGame("Black 2", "black-2", "gen5bw2_exp.txt", 5, { 576, 707 });
	RegisterGame("White 2", "white-2", "gen5bw2_exp.txt", 5, { 576, 707 });
	RegisterGame("X", "x", "gen6_exp.txt", 6, { 708, 760 });
	RegisterGame("Y", "y", "gen6_exp.txt", 6, { 708, 760 });
	RegisterGame("Sun", "sun", "gen7sm_exp.txt", 7, { 1035, 1156 });
	RegisterGame("Moon", "moon", "gen7sm_exp.txt", 7, { 1035, 1156 });
	RegisterGame("Ultra Sun", "ultra-sun", "gen7usum_exp.txt", 7, { 1035, 1156 });
	RegisterGame("Ultra Moon", "ultra-moon", "gen7usum_exp.txt", 7, { 1035, 1156 });
	RegisterGame("All", "all", "THIS_SHOULDNT_HAPPEN", 0, { 1, 1156 });

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

	vector<EncounterTable> maintables;
	string basepath = "pkmndata/";
	Settings settings;
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
#endif

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        dosettingswindow(&settings, &settingswindowdata, &maintables, basepath);

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
