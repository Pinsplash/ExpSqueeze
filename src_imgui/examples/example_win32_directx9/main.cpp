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

enum MethodFilterFlags
{
	MethodFilterFlags_None = 0,
	MethodFilterFlags_Walk = 1 << 0,
	MethodFilterFlags_Surf = 1 << 1,
	MethodFilterFlags_RodOld = 1 << 2,
	MethodFilterFlags_RodGood = 1 << 3,
	MethodFilterFlags_RodSuper = 1 << 4,
	MethodFilterFlags_RockSmash = 1 << 5,
	MethodFilterFlags_Headbutt = 1 << 6,
	MethodFilterFlags_Seaweed = 1 << 7,
	MethodFilterFlags_DarkGrass = 1 << 8,
	MethodFilterFlags_Phenomena = 1 << 9,
	MethodFilterFlags_RoughTerrain = 1 << 10,
	MethodFilterFlags_BubblingSpots = 1 << 11,
	MethodFilterFlags_Ambush = 1 << 12,
	MethodFilterFlags_Last = 1 << 13,
};

struct Settings
{
	vector<int> folderRanges;
	string wantedgame;
	string wantedtime;
	string wantedseason;
	bool wantswarm = false;
	bool wantradar = false;
	string wantedslot2game;
	string wantedradiostation;
	string expfile;
	int generation = 0;
	int repellevel = 0;
	int maxlevel = 100;
	bool printtext = false;
	int methodflags = MethodFilterFlags_Last - 1;
};

struct SettingsWindowData
{
	int generation_lastframe = 0;
	int time_chosen = 0;
	bool running = false;
};

struct Encounter
{
	int chance = 0;
	int minlevel = 0;
	int maxlevel = 0;
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
	int expectedtotalpercent = 0;
	double totalavgexp = 0;
	int totalchance = 0;
	bool overlevellimit = false;
};

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

static void PrintCustomData(Settings* settings)
{
	//cout << settings->wantedgame << "\n";
	for (string str : g_debugdata)
	{
		//cout << "condition " << str << "\n";
	}
}
#endif //_DEBUG

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

static void RegisterEncounter(Settings* settings, vector<EncounterTable>* maintables, int chance, int minlevel, int maxlevel, string pokemonname, string placename, string method, int i)
{
	bool tablebad = false;
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
			if (table.placename == placename && table.method == method)
			{
				if (tablebad)
				{
					table.overlevellimit = true;
				}
				else
				{
					makenewtable = false;
					//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
					table.encounters.push_back(newEnc);
					table.expectedtotalpercent += chance;
					break;
				}
			}
		}
		if (makenewtable && !tablebad)
		{
			//cout << "Line " << linenum << ": new table\n";
			EncounterTable newTable;
			newTable.method = method;
			newTable.placename = placename;
			newTable.filenumber = i;
			newTable.expectedtotalpercent = chance;
			//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
			newTable.encounters.push_back(newEnc);

			maintables->push_back(newTable);
		}
	}
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

static bool isEqualString(char* initial, char* compare)
{
	return !strcmp(initial, compare);
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
		snprintf(dubbuffer, sizeof(dubbuffer), " type: double (%ld)", mydub);
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
	string combinestring(string("val name: ") + obj.name);
	const char* str1 = combinestring.c_str();
	//cout << str1;
	ExplainJSONValue(obj.value);
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

static bool InvalidateCondition(Settings* settings, string condition)
{
	if (condition == "time-morning" || condition == "time-day" || condition == "time-night")
	{
		//time: morning/day/night (all gens except 1, 3)
		if (settings->wantedtime != condition)
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
		//slot2: none/ruby/sapphire/emerald/firered/leafgreen (FPP)
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

static bool ValidateMethod(Settings* settings, string method)
{
	//these encounter methods are not very useful, if even applicable.
	//sos encounters may come back, if the alola tables are ever fixed.
	if (method == "gift" || method == "gift-egg" || method == "only-one" || method == "pokeflute"
		|| method == "squirt-bottle" || method == "wailmer-pail" || method == "devon-scope"
		|| method == "island-scan" || method == "sos-encounter" || method == "berry-piles"
		|| method == "npc-trade" || method == "sos-from-bubbling-spot" || method == "roaming-grass"
		|| method == "roaming-water" || method == "feebas-tile-fishing")
		return false;
	bool foundmethod = false;
	bool methodgood = false;
	if (method == "walk" || method == "red-flowers" || method == "yellow-flowers" || method == "purple-flowers")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_Walk)
			methodgood = true;
	}
	if (!foundmethod && method == "surf")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_Surf)
			methodgood = true;
	}
	if (!foundmethod && method == "old-rod")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_RodOld)
			methodgood = true;
	}
	if (!foundmethod && method == "good-rod")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_RodGood)
			methodgood = true;
	}
	if (!foundmethod && method == "super-rod")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_RodSuper)
			methodgood = true;
	}
	if (!foundmethod && method == "rock-smash")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_RockSmash)
			methodgood = true;
	}
	if (!foundmethod && (method == "headbutt-low" || method == "headbutt-normal" || method == "headbutt-high"))
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_Headbutt)
			methodgood = true;
	}
	if (!foundmethod && method == "seaweed")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_Seaweed)
			methodgood = true;
	}
	if (!foundmethod && method == "dark-grass")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_DarkGrass)
			methodgood = true;
	}
	if (!foundmethod && (method == "grass-spots" || method == "cave-spots" || method == "bridge-spots" || method == "super-rod-spots" || method == "surf-spots"))
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_Phenomena)
			methodgood = true;
	}
	if (!foundmethod && method == "rough-terrain")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_RoughTerrain)
			methodgood = true;
	}
	if (!foundmethod && method == "bubbling-spots")
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_BubblingSpots)
			methodgood = true;
	}
	if (!foundmethod && (method == "ambush-grass" || method == "ambush-bush" || method == "ambush-splash" || method == "ambush-tree"
		|| method == "ambush-dirt" || method == "ambush-shadow" || method == "ambush-chase" || method == "ambush-sand"))
	{
		foundmethod = true;
		if (settings->methodflags & MethodFilterFlags_Ambush)
			methodgood = true;
	}
	if (!foundmethod)
		assert(0);//this encounter method is unaccounted for
	else if (!methodgood)
		return false;//bad method
	return true;
}

static bool ParseEncounterDetails(Settings* settings, vector<EncounterTable>* maintables, json_value* encdetailblock, string pokemonname, string placename, int iFile)
{
	json_value* conditionvalues = FindArrayInObjectByName(encdetailblock, "condition_values");

	if (conditionvalues)
	{
		bool encounterinvalid = false;

		for (int conditionIdx = 0; conditionIdx < conditionvalues->u.array.length; conditionIdx++)
		{
			if (encounterinvalid)
				break;

			json_value* condobj = conditionvalues->u.array.values[conditionIdx];
			string condition = FindValueInObjectByKey(condobj, "name")->u.string.ptr;
#ifdef _DEBUG
			RecordCustomData(condition);
#endif //_DEBUG
			//make sure encounter meets applicable parameters
			if (InvalidateCondition(settings, condition))
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
	if (!ValidateMethod(settings, method))
		return false;

	//all good
	int chance = FindValueInObjectByKey(encdetailblock, "chance")->u.integer;
	int maxlevel = FindValueInObjectByKey(encdetailblock, "max_level")->u.integer;
	int minlevel = FindValueInObjectByKey(encdetailblock, "min_level")->u.integer;
	RegisterEncounter(settings, maintables, chance, minlevel, maxlevel, pokemonname, placename, method, iFile);
	return true;
}

static int ParseLocationDataFile(string basepath, int iFile, Settings* settings, vector<EncounterTable>* maintables)
{
	//if (iFile != 57)
	//	return 1;
	string path = basepath + "\\" + to_string(iFile) + "\\index.json";
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
		fclose(fp);
		free(file_contents);
		return 0;
	}
	int readNum = fread(file_contents, 1, file_size, fp);
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
	for (int nameIdx = 0; nameIdx < names->u.array.length; nameIdx++)
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
	for (int encounterIdx = 0; encounterIdx < encounters->u.array.length; encounterIdx++)
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
		for (int verdetailsIdx = 0; verdetailsIdx < versiondetails->u.array.length; verdetailsIdx++)
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
			if (givengame == settings->wantedgame || settings->wantedgame == "all")
			{
				//it was. get pokemon name
				string pokemonname;
				json_value* pokemon = FindObjectInObjectByName(encounterblock, "pokemon");
				if (!pokemon)
				{
					assert(0);
					return 0;
				}
				pokemonname = FindValueInObjectByKey(pokemon, "name")->u.string.ptr;
				//go back up to version details to get encounter details block
				json_value* encounterdetails = FindArrayInObjectByName(verdetailblock, "encounter_details");
				if (!encounterdetails)
				{
					assert(0);
					return 0;
				}
				for (int encdetailsIdx = 0; encdetailsIdx < encounterdetails->u.array.length; encdetailsIdx++)
				{
					json_value* encdetailblock = encounterdetails->u.array.values[encdetailsIdx];
					if (!encdetailblock)
					{
						assert(0);
						return 0;
					}
					if (!ParseEncounterDetails(settings, maintables, encdetailblock, pokemonname, placename, iFile))
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

static void ReadTables(Settings* settings, vector<EncounterTable>* maintables, string basepath)
{
	if (settings->printtext) cout << "Reading encounter data\n";
	//go through every file
	string tablepath = basepath + "location-area";
	for (int i = 0; i < settings->folderRanges.size(); i += 2)
	{
		int notch = 1;
		double range = settings->folderRanges[i + 1] - settings->folderRanges[i];
		if (settings->printtext) cout << "|     PROGRESS     |\n";
		for (int j = settings->folderRanges[i]; j <= settings->folderRanges[i + 1]; j++)
		{
			if ((j - settings->folderRanges[i]) / range >= (notch * 0.05))
			{
				if (settings->printtext) cout << "-";
				notch++;
			}
			if (ParseLocationDataFile(tablepath, j, settings, maintables) == 0)
				return;
		}
	}
	if (settings->printtext) cout << "\n";
	for (EncounterTable &table : *maintables)
	{
		//really prefer to not save tables that i know are bad, but this is by far the least painful way to take care of this
		if (table.overlevellimit)
			continue;
		if (settings->printtext) cout << "\n" << table.placename << ", " << table.method << "\n";
		table.totalavgexp = 0;
		table.totalchance = 0;//sanity check: this number should always = expectedtotalpercent at the end of the table.
		for (Encounter &encounter : table.encounters)
		{
			//get experience yield from stripped down bulba tables
			int baseexp = 0;
			if (settings->wantedgame == "all")
			{
				//this is a debugging feature only.
				//since different gens have different BEY tables and we have no way to discern which tables are for which gens, give up
				//our only interest in this case is the percent total adding up to 100
			}
			else
			{
				string path = basepath + "exp-gain-stats/" + settings->expfile;
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
					if (str1 == encounter.pokemonname)
					{
						foundmon = true;
						baseexp = stoi(str2);
						break;
					}
				}
				if (!foundmon)
				{
					cout << "ERROR: Could not find pokemon named '" << encounter.pokemonname << "'\n";
					continue;
				}
			}
			encounter.minlevel = max(encounter.minlevel, settings->repellevel);
			//30-30: 1
			//30-31: 2
			//30-40: 11
			int numlevels = (encounter.maxlevel - encounter.minlevel) + 1;
			double levelsum = 0;
			for (int i = 0; i < numlevels; i++)
				levelsum += encounter.minlevel + i;
			double avglevel = levelsum / numlevels;
			int factor = (settings->generation == 5 || settings->generation >= 7) ? 5 : 7;
			encounter.avgexp = (baseexp * avglevel) / factor;
			encounter.avgexpweighted = (encounter.avgexp * encounter.chance) / table.expectedtotalpercent;
			if (settings->printtext) cout << encounter.pokemonname << " has " << encounter.chance << "% chance between level " << encounter.minlevel << " and " << encounter.maxlevel << ". avgexp " << encounter.avgexp << ", weighted " << encounter.avgexpweighted << "\n";
			table.totalavgexp += encounter.avgexpweighted;
			table.totalchance += encounter.chance;
		}
		if (settings->printtext) cout << table.totalavgexp << " average EXP in " << table.placename << ", " << table.method << "\n";
		sort(table.encounters.begin(), table.encounters.end(), compareByAEW);
		//if we're getting data for all games, then there are multiple fully correct encounter tables inside a file
		//this means if the table is correct it will be a multiple of 100
		//long story short we can't know what that multiple will be, so % is our best option
		//also, again, because all is meant for debugging only, we're just going to not expect it to be used in conjunction with repellevel because that gets hairy
		bool errorfound = false;
		if (settings->wantedgame == "all" && table.totalchance % 100 != 0)
			errorfound = true;
		else if ((settings->repellevel == 0 && settings->maxlevel == 100) && table.totalchance != 100)
			errorfound = true;
		else if (table.totalchance != table.expectedtotalpercent)
			errorfound = true;
		if (errorfound)
		{
			cout << "ERROR: Total chance was " << table.totalchance << "! File number " << table.filenumber << "\n";
			cout << "wantedgame: " << settings->wantedgame << " totalchance: " << to_string(table.totalchance) << "\n";
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
	PrintCustomData(settings);
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
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	Settings newsettings;
	ImGui::Begin("Options", false, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
	const char* games[] = { "Blue", "Red", "Yellow",
		"Gold", "Silver", "Crystal",
		"Ruby", "Sapphire", "Emerald", "FireRed", "LeafGreen",
		"Diamond", "Pearl", "Platinum", "HeartGold", "SoulSilver",
		"Black", "White", "Black 2", "White 2",
		"X", "Y",
#ifdef _DEBUG
		"Sun", "Moon", "Ultra Sun", "Ultra Moon"
		, "All"//quick and dirty debug option only, not perfectly functional
#endif //_DEBUG
	};
	const char* internal_games[] = { "blue", "red", "yellow",
		"gold", "silver", "crystal",
		"ruby", "sapphire", "emerald", "firered", "leafgreen",
		"diamond", "pearl", "platinum", "heartgold", "soulsilver",
		"black", "white", "black-2", "white-2",
		"x", "y",
#ifdef _DEBUG
		"sun", "moon", "ultra-sun", "ultra-moon"
		, "all"//quick and dirty debug option only, not perfectly functional
#endif //_DEBUG
	};
	static int game_current = 0;
	ImGui::Combo("Game", &game_current, games, IM_ARRAYSIZE(games));
	//ImGui::SameLine(); HelpMarker("dobedobedo");
	newsettings.wantedgame = internal_games[game_current];
	bool rse = false;
	bool frlg = false;
	bool dpp = false;
	bool hgss = false;
	bool bw1 = false;
	bool bw2 = false;
	bool xy = false;
	bool sm = false;
	bool usum = false;
	bool allgames = false;

	if (newsettings.wantedgame == "blue" || newsettings.wantedgame == "red" || newsettings.wantedgame == "yellow")
	{
		newsettings.expfile = "gen1_exp.txt";
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(258);
		newsettings.folderRanges.push_back(349);
		newsettings.generation = 1;
	}

	if (newsettings.wantedgame == "gold" || newsettings.wantedgame == "silver" || newsettings.wantedgame == "crystal")
	{
		newsettings.expfile = "gen2_exp.txt";
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(184);
		newsettings.folderRanges.push_back(349);
		newsettings.folderRanges.push_back(798);
		newsettings.folderRanges.push_back(798);
		newsettings.generation = 2;
	}

	if (newsettings.wantedgame == "ruby" || newsettings.wantedgame == "sapphire" || newsettings.wantedgame == "emerald")
	{
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(350);
		newsettings.folderRanges.push_back(449);
		rse = true;
	}

	if (newsettings.wantedgame == "firered" || newsettings.wantedgame == "leafgreen")
	{
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(258);
		newsettings.folderRanges.push_back(572);
		newsettings.folderRanges.push_back(825);
		newsettings.folderRanges.push_back(825);
		frlg = true;
	}

	if (rse || frlg)
	{
		newsettings.expfile = "gen3_exp.txt";
		newsettings.generation = 3;
	}

	if (newsettings.wantedgame == "diamond" || newsettings.wantedgame == "pearl" || newsettings.wantedgame == "platinum")
	{
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(1);
		newsettings.folderRanges.push_back(183);
		dpp = true;
	}

	if (newsettings.wantedgame == "heartgold" || newsettings.wantedgame == "soulsilver")
	{
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(184);
		newsettings.folderRanges.push_back(349);
		hgss = true;
	}

	if (dpp || hgss)
	{
		newsettings.expfile = "gen4_exp.txt";
		newsettings.generation = 4;
	}

	if (newsettings.wantedgame == "black" || newsettings.wantedgame == "white")
	{
		newsettings.expfile = "gen5bw1_exp.txt";
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(576);
		newsettings.folderRanges.push_back(655);
		bw1 = true;
	}

	if (newsettings.wantedgame == "black-2" || newsettings.wantedgame == "white-2")
	{
		newsettings.expfile = "gen5bw2_exp.txt";
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(576);
		newsettings.folderRanges.push_back(707);
		bw2 = true;
	}

	if (bw1 || bw2)
		newsettings.generation = 5;

	if (newsettings.wantedgame == "x" || newsettings.wantedgame == "y")
	{
		newsettings.expfile = "gen6_exp.txt";
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(708);
		newsettings.folderRanges.push_back(760);
		newsettings.generation = 6;
		xy = true;
	}

	if (newsettings.wantedgame == "sun" || newsettings.wantedgame == "moon")
	{
		newsettings.expfile = "gen7sm_exp.txt";
		sm = true;
	}

	if (newsettings.wantedgame == "ultra-sun" || newsettings.wantedgame == "ultra-moon")
	{
		newsettings.expfile = "gen7usum_exp.txt";
		usum = true;
	}
	//TODO: optimize gen 7 folder ranges
	if (sm || usum)
	{
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(1035);
		newsettings.folderRanges.push_back(1156);
		newsettings.generation = 7;
	}

	if (newsettings.wantedgame == "all")
	{
		newsettings.folderRanges.clear();
		newsettings.folderRanges.push_back(1);
		newsettings.folderRanges.push_back(1156);
		allgames = true;
	}

	if (newsettings.generation == 2)
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

	if (newsettings.generation == 4)
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

	if (newsettings.generation == 7)
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
	if (allgames || (newsettings.generation == 2 || newsettings.generation == 4 || newsettings.generation == 7))
	{
		const char* times;
		const char* internal_times;
		int height;
		if (newsettings.generation == 7)
		{
			times = "Day\0Night\0";
			internal_times = "time-day\0time-night\0";
			height = 2;
			if (settingswindowdata->generation_lastframe != newsettings.generation)
				settingswindowdata->time_chosen = 0;
		}
		else
		{
			times = "Morning\0Day\0Night\0";
			internal_times = "time-morning\0time-day\0time-night\0";
			height = 3;
			if (settingswindowdata->generation_lastframe != newsettings.generation)
				settingswindowdata->time_chosen = 1;
		}
#pragma warning(suppress: 6384)
		ImGui::Combo("Time of Day", &settingswindowdata->time_chosen, times, IM_ARRAYSIZE(times));
		//ImGui::SameLine(); HelpMarker("dobedobedo");
		newsettings.wantedtime = Items_SingleStringGetter((void*)internal_times, settingswindowdata->time_chosen);//internal_times[settingswindowdata->time_chosen];
	}

	if (allgames || newsettings.generation == 5)
	{
		const char* seasons[] = { "Spring", "Summer", "Autumn", "Winter" };
		const char* internal_seasons[] = { "season-spring", "season-summer", "season-autumn", "season-winter" };
		static int season_current = 0;
		ImGui::Combo("Season", &season_current, seasons, IM_ARRAYSIZE(seasons));
		newsettings.wantedseason = internal_seasons[season_current];
	}

	//mass outbreaks exist in gen 3 and 5, but we have no data for them! braugh!
	if (allgames || (newsettings.generation == 2 || newsettings.generation == 4))
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
	if (allgames || (newsettings.generation >= 2 && newsettings.generation <= 5))
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
		if (newsettings.generation != 5 && newsettings.generation != 7)
		{
			ImGui::CheckboxFlags("Old Rod", &methodflags, MethodFilterFlags_RodOld);
			ImGui::CheckboxFlags("Good Rod", &methodflags, MethodFilterFlags_RodGood);
		}

		if (newsettings.generation == 7)
			ImGui::CheckboxFlags("Fishing at regular rock", &methodflags, MethodFilterFlags_RodSuper);
		else
			ImGui::CheckboxFlags("Super Rod", &methodflags, MethodFilterFlags_RodSuper);

		if (allgames || newsettings.generation == 2 || newsettings.generation == 3 || newsettings.generation == 6 || hgss)
			ImGui::CheckboxFlags("Rock Smash", &methodflags, MethodFilterFlags_RockSmash);

		if (allgames || newsettings.generation == 2/* || hgss*/)
			ImGui::CheckboxFlags("Headbutt", &methodflags, MethodFilterFlags_Headbutt);

		if (allgames || rse)
			ImGui::CheckboxFlags("Seaweed", &methodflags, MethodFilterFlags_Seaweed);

		if (allgames || newsettings.generation == 5)
		{
			ImGui::CheckboxFlags("Dark Grass", &methodflags, MethodFilterFlags_DarkGrass);
			ImGui::CheckboxFlags("Phenomena", &methodflags, MethodFilterFlags_Phenomena);
			ImGui::SameLine(); HelpMarker("Rustling grass, dust clouds, flying pokemon's shadows, and rippling water.");
		}

		if (allgames || xy)
		{
			ImGui::CheckboxFlags("Rough Terrain", &methodflags, MethodFilterFlags_RoughTerrain);
			ImGui::SameLine(); HelpMarker("\"Rough Terrain\" encompasses several different kinds of encounters, most of them being unique to a single location, like the snow on Route 17.");
		}

		if (allgames || newsettings.generation == 7)
		{
			ImGui::CheckboxFlags("Fishing at bubbling rock", &methodflags, MethodFilterFlags_BubblingSpots);
			ImGui::CheckboxFlags("Ambush", &methodflags, MethodFilterFlags_Ambush);
			ImGui::SameLine(); HelpMarker("Cases where pokemon have an overworld presence and move, like flying pokemon shadows or rustling grass.");
		}

		newsettings.methodflags = methodflags;
	}

	static bool printtext = false;
	ImGui::Checkbox("Text Output", &printtext);
	newsettings.printtext = printtext;

	if (settingswindowdata->running)
	{
		//only save the new settings once the button is pressed. this prevents changing the settings mid-iteration.
		settings->folderRanges = newsettings.folderRanges;
		settings->wantedgame = newsettings.wantedgame;
		settings->wantedtime = newsettings.wantedtime;
		settings->wantedseason = newsettings.wantedseason;
		settings->wantswarm = newsettings.wantswarm;
		settings->wantradar = newsettings.wantradar;
		settings->wantedslot2game = newsettings.wantedslot2game;
		settings->wantedradiostation = newsettings.wantedradiostation;
		settings->expfile = newsettings.expfile;
		settings->generation = newsettings.generation;
		settings->repellevel = newsettings.repellevel;
		settings->maxlevel = newsettings.maxlevel;
		settings->printtext = newsettings.printtext;
		settings->methodflags = newsettings.methodflags;

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
		*/

		ReadTables(settings, maintables, basepath);
		sort(maintables->begin(), maintables->end(), compareByExp);
		settingswindowdata->running = false;
	}

	if (ImGui::Button("Go!"))
	{
		int totalrange = 0;
		for (int i = 0; i < newsettings.folderRanges.size(); i += 2)
		{
			totalrange += (newsettings.folderRanges[i + 1] - newsettings.folderRanges[i]) + 1;
		}
		string imgoing = "Searching through " + to_string(totalrange) + " tables.";
		ImGui::SameLine(); ImGui::Text(imgoing.c_str());
		maintables->clear();
#ifdef _DEBUG
		g_debugdata.clear();
#endif //_DEBUG
		settingswindowdata->running = true;
	}

	//detect when settings have changed
	if (settings->wantedgame != newsettings.wantedgame ||
		settings->wantedtime != newsettings.wantedtime ||
		settings->wantedseason != newsettings.wantedseason ||
		settings->wantswarm != newsettings.wantswarm ||
		settings->wantradar != newsettings.wantradar ||
		settings->wantedslot2game != newsettings.wantedslot2game ||
		settings->wantedradiostation != newsettings.wantedradiostation ||
		settings->expfile != newsettings.expfile ||
		settings->generation != newsettings.generation ||
		settings->repellevel != newsettings.repellevel ||
		settings->maxlevel != newsettings.maxlevel ||
		settings->methodflags != newsettings.methodflags)
	{
		if (!maintables->empty())
		{
			ImGui::SameLine(); ImGui::Text("Settings changed");
		}
	}

	for (EncounterTable table : *maintables)
	{
		string header = to_string((int)trunc(table.totalavgexp)) + " EXP, " + table.placename + ", " + table.method;
		if (ImGui::CollapsingHeader(header.c_str()))
		{
			if (ImGui::BeginTable("tablee", 5, ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable))
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
					string avgexp = to_string((int)trunc(encounter.avgexp));
					ImGui::Text(avgexp.c_str());

					//avg exp weighted
					ImGui::TableNextColumn();
					string avgexpweighted = to_string((int)trunc(encounter.avgexpweighted));
					ImGui::Text(avgexpweighted.c_str());
				}
				ImGui::EndTable();
			}
		}
	}
	ImGui::End();
	settingswindowdata->generation_lastframe = newsettings.generation;
}

// Main code
int main(int, char**)
{
	cout << "Oh hi, I'm the text output window. If you use the text output option, a purely text-based\n";
	cout << "output will be printed inside me. This was the program's output from before it had a GUI.\n";
	cout << "You may want to use the old output if you have some kind of specific analytical use.\n";
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
