// Dear ImGui: standalone example application for DirectX 9

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
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

struct Settings
{
	int firstfolder;
	int lastfolder;
	string wantedgame;
	string wantedtime;
	string wantedseason;
	bool wantswarm;
	bool wantradar;
	string wantedslot2game;
	string wantedradiostation;
	string expfile;
	int generation;
	int repellevel;
	int maxlevel;
	bool printtext;
};

struct SettingsWindowData
{
	int generation_lastframe;
	int time_chosen;
	bool running = false;
};

struct Encounter
{
	int chance;
	int minlevel;
	int maxlevel;
	string pokemonname;
	double avgexp;
	double avgexpweighted;
};

struct EncounterTable
{
	string method;
	string placename;
	vector<Encounter> encounters;
	int filenumber;
	int expectedtotalpercent;
	double totalavgexp;
	int totalchance;
	bool knownerror;
};

string remove_whitespace(string str)
{
	string str1 = str;

	auto noSpaceEnd = remove(str1.begin(), str1.end(), ' ');

	str1.erase(noSpaceEnd, str1.end());

	string str2 = str;
	str2.erase(remove_if(str2.begin(),
		str2.end(),
		[](unsigned char x) { return isspace(x); }),
		str2.end());

	vector<complex<double>> nums{ { 2, 2 }, { 1, 3 }, { 4, 8 } };
#ifdef __cpp_lib_algorithm_default_value_type
	nums.erase(remove(nums.begin(), nums.end(), { 1, 3 }), nums.end());
#else
	nums.erase(remove(nums.begin(), nums.end(), complex<double>{1, 3}),
		nums.end());
#endif
	return str2;
}

void RegisterEncounter(Settings* settings, vector<EncounterTable>* maintables, int chance, int minlevel, int maxlevel, string pokemonname, string placename, string method, int i)
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
		int index = -1;
		for (EncounterTable& table : *maintables)
		{
			index++;
			//cout << "Trying table. " << table.placename << " == " << placename << " && " << table.method << " == " << method << "\n";
			if (table.placename == placename && table.method == method)
			{
				if (tablebad)
				{
					maintables->erase(maintables->begin() + index);
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

int ParseLocationDataFile(string basepath, int i, Settings* settings, vector<EncounterTable>* maintables)
{
	string path = basepath + "\\" + to_string(i) + "\\index.json";
	//cout << path << "\n";
	ifstream ReadFile(path);
	string textfileLine;
	vector<string> fileLines;

	//since getline is apparently not very good at handling skipping ahead in a file, we're going to copy every line of the file into a vector.
	//line 0 blank
	fileLines.push_back("");
	while (getline(ReadFile, textfileLine))
	{
		textfileLine = remove_whitespace(textfileLine);

		////code for finding folder numbers associated with each game
		//size_t s1End = textfileLine.find(':');
		//string str1 = textfileLine.substr(0, s1End);
		//if (str1 == "\"name\"")
		//{
		//	size_t s2Start = s1End + 2;
		//	size_t s2End = textfileLine.find('\"', s2Start + 1);
		//	string str2 = textfileLine.substr(s2Start, s2End - s2Start);
		//	if (str2 == settings->wantedgame)
		//		cout << settings->wantedgame << " " << i << "\n";
		//}

		fileLines.push_back(textfileLine);
	}

	string placename;//only one place name per file
	string pokemonname;
	int chance;
	int maxlevel;
	int minlevel;
	string method;
	bool inmainbrackets = false;
	bool inencountermethodrates = false;
	bool inencountermethod = false;
	bool inversiondetails = false;
	bool inversion = false;
	bool inlocation = false;
	bool innames = false;
	bool inlanguage = false;
	bool inpokemonencounters = false;
	bool inpokemon = false;
	bool inencounterdetails = false;
	bool inmethod = false;
	bool inconditionvalues = false;
	bool inencounterentry = false;
	bool approachingplacename = false;
	bool encounterinvalid = false;
	string badencounterreason;
	bool readingcorrectgame = false;
	for (int linenum = 1; linenum < (int)fileLines.size(); linenum++)
	{
		string textLine = fileLines[linenum];
		bool finishedline = false;
		//cout << "Line " << linenum << ": start (line is '" << textLine << "')\n";
		//skip any blank lines
		if (textLine.size() == 0)
		{
			cout << "Line " << linenum << ": Blank\n";
			cin.get();
			ReadFile.close();
			return 0;
		}

		if (textLine == "{")
		{
			if (inversiondetails && !inencounterdetails && !inconditionvalues && !inencountermethodrates && !readingcorrectgame)
			{
				//skip ahead to our version name. don't bother reading in data for the wrong game
				//streampos current_pos = ReadFile.tellg();
				int oldline = linenum;
				for (int j = oldline + 1; j < (int)fileLines.size(); j++)
				{
					string aheadLine = fileLines[j];
					linenum++;
					//cout << "Skip hit line " << linenum << "! A (line is '" << aheadLine << "')\n";
					if (aheadLine == "\"version\":{")
					{
						string aheadLine = fileLines[j + 1];
						linenum++;
						//cout << "Skip hit line " << linenum << "! B (line is '" << aheadLine << "')\n";
						size_t s1End = aheadLine.find(':');
						string str1 = aheadLine.substr(0, s1End);
						if (str1 == "\"name\"")
						{
							size_t s2Start = s1End + 2;
							size_t s2End = aheadLine.find('\"', s2Start + 1);
							string str2 = aheadLine.substr(s2Start, s2End - s2Start);
							if (str2 == settings->wantedgame || settings->wantedgame == "all")
							{
								//good, go back to the top and read it in
								linenum = oldline;
								readingcorrectgame = true;
								//cout << "Line " << oldline << ", wanted " << settings->wantedgame << ", got " << str2 << ", game is right\n";
								break;
							}
							else
							{
								//not our version, skip down
								linenum += 4;
								//cout << "Line " << oldline << ", wanted " << settings->wantedgame << ", got " << str2 << ", wrong game\n";
								break;
							}
						}
						else
						{
							//cout << "Line " << linenum << ": hit something we don't know about when trying to get game name\n";
							cin.get();
							ReadFile.close();
							return 0;
						}
					}
				}
				//if (readingcorrectgame)
				//	continue;
				//cout << "Line " << linenum << ": resuming here (line is '" << textLine << "')\n";
				linenum--;
				continue;
			}
			readingcorrectgame = false;
			if (inencounterdetails && encounterinvalid)
			{
				//badencounterreason = "";
				//encounterinvalid = false;
			}
			if (!inmainbrackets)
			{
				inmainbrackets = true;
				finishedline = true;
			}
			if (inencountermethodrates)
				finishedline = true;
			if (innames)
				finishedline = true;
			if (textLine != "]" && inpokemonencounters)
				finishedline = true;
			if (!inencounterentry)
			{
				inencounterentry = true;
				finishedline = true;
			}
			if (inencounterdetails)
				finishedline = true;
		}
		if (textLine == "\"encounter_method_rates\":[],")
			finishedline = true;
		if (textLine == "\"encounter_method_rates\":[")
		{
			inencountermethodrates = true;
			finishedline = true;
		}

		if (textLine == "\"encounter_method\":{")
		{
			if (!inencountermethod)
			{
				inencountermethod = true;
				finishedline = true;
			}
		}

		if (textLine == "},")
		{
			if (inencounterdetails && !encounterinvalid && !inconditionvalues && !inmethod)
			{
				RegisterEncounter(settings, maintables, chance, minlevel, maxlevel, pokemonname, placename, method, i);
				finishedline = true;
			}
			/*
			if (inencounterdetails && encounterinvalid && !inconditionvalues && !inmethod && placename == "CherrygroveCity")
			{
				cout << "Line " << linenum << "\n";
				cout << placename << " " << method << " " << pokemonname << " was invalid encounter.\n";
				cout << "inmethod " << (inmethod ? "TRUE" : "FALSE") << "\n";
				cout << "inencountermethod " << (inencountermethod ? "TRUE" : "FALSE") << "\n";
				cout << "inencountermethodrates " << (inencountermethodrates ? "TRUE" : "FALSE") << "\n";
				cout << "inlocation " << (inlocation ? "TRUE" : "FALSE") << "\n";
				cout << "inlanguage " << (inlanguage ? "TRUE" : "FALSE") << "\n";
				cout << "innames " << (innames ? "TRUE" : "FALSE") << "\n";
				cout << "inpokemon " << (inpokemon ? "TRUE" : "FALSE") << "\n";
				cout << "inencounterentry " << (inencounterentry ? "TRUE" : "FALSE") << "\n";
				cout << "inversiondetails " << (inversiondetails ? "TRUE" : "FALSE") << "\n";
				cout << "inpokemonencounters " << (inpokemonencounters ? "TRUE" : "FALSE") << "\n";
				cout << "inversion " << (inversion ? "TRUE" : "FALSE") << "\n";
				cout << "inmainbrackets " << (inmainbrackets ? "TRUE" : "FALSE") << "\n";
				cout << "}REASON: " << badencounterreason << "\n\n";
			}
			*/
			if (inmethod)
			{
				inmethod = false;
				finishedline = true;
			}
			if (inencountermethod)
			{
				inencountermethod = false;
				finishedline = true;
			}
			//idk why i had put this here but it was making us think we left version details before we should, which made us think we left pokemon encounters before we should

			//if (inversiondetails)
			//{
			//if (!inencounterdetails && !inencountermethodrates)
			//inversiondetails = false;
			//continue;
			//}

			if (inencountermethodrates)
				finishedline = true;
			if (inlocation)
			{
				inlocation = false;
				finishedline = true;
			}
			if (inlanguage)
			{
				inlanguage = false;
				finishedline = true;
			}
			if (innames)
				finishedline = true;
			if (inpokemon)//no point in setting this back to be false. once we hit the pokemon section of a file, there's no data after that to read
				finishedline = true;
			if (inencounterentry)
			{
				inencounterentry = false;
				finishedline = true;
			}
		}

		if (textLine == "}")
		{
			if (inencounterdetails && !encounterinvalid && !inconditionvalues)
			{
				RegisterEncounter(settings, maintables, chance, minlevel, maxlevel, pokemonname, placename, method, i);
				finishedline = true;
			}
			/*
			if (inencounterdetails && encounterinvalid && !inconditionvalues && placename == "CherrygroveCity")
			{
				cout << "Line " << linenum << "\n";
				cout << placename << " " << method << " " << pokemonname << " was invalid encounter.\n";
				cout << "inmethod " << (inmethod ? "TRUE" : "FALSE") << "\n";
				cout << "inencountermethod " << (inencountermethod ? "TRUE" : "FALSE") << "\n";
				cout << "inencountermethodrates " << (inencountermethodrates ? "TRUE" : "FALSE") << "\n";
				cout << "inlocation " << (inlocation ? "TRUE" : "FALSE") << "\n";
				cout << "inlanguage " << (inlanguage ? "TRUE" : "FALSE") << "\n";
				cout << "innames " << (innames ? "TRUE" : "FALSE") << "\n";
				cout << "inpokemon " << (inpokemon ? "TRUE" : "FALSE") << "\n";
				cout << "inencounterentry " << (inencounterentry ? "TRUE" : "FALSE") << "\n";
				cout << "inversiondetails " << (inversiondetails ? "TRUE" : "FALSE") << "\n";
				cout << "inpokemonencounters " << (inpokemonencounters ? "TRUE" : "FALSE") << "\n";
				cout << "inversion " << (inversion ? "TRUE" : "FALSE") << "\n";
				cout << "inmainbrackets " << (inmainbrackets ? "TRUE" : "FALSE") << "\n";
				cout << "}REASON: " << badencounterreason << "\n\n";
			}
			*/
			if (inversiondetails)
				finishedline = true;
			if (inencountermethodrates)
				finishedline = true;
			if (innames)
				finishedline = true;
			if (inpokemonencounters)
				finishedline = true;
			if (inencounterentry)
			{
				inencounterentry = false;
				finishedline = true;
			}
			if (inversion)
			{
				inversion = false;
				finishedline = true;
			}
			//end of file
			if (!finishedline && inmainbrackets)
			{
				//cout << "Line " << linenum << ": left main brackets\n";
				inmainbrackets = false;
				finishedline = true;
			}
		}

		if (textLine == "\"version_details\":[")
		{
			if (!inversiondetails)
			{
				inversiondetails = true;
				finishedline = true;
			}
		}

		if (textLine == "\"version\":{")
		{
			inversion = true;
			if (inversiondetails && inencountermethodrates)
				finishedline = true;
			if (inpokemonencounters)
				finishedline = true;
		}

		if (textLine == "\"location\":{")
		{
			if (!inlocation)
			{
				inlocation = true;
				finishedline = true;
			}
		}

		if (textLine == "\"language\":{")
		{
			if (!inlanguage)
			{
				inlanguage = true;
				finishedline = true;
			}
		}

		if (textLine == "\"names\":[],")
			finishedline = true;

		if (textLine == "\"names\":[")
		{
			innames = true;
			finishedline = true;
		}

		if (textLine == "\"pokemon_encounters\":[]")
			finishedline = true;

		if (textLine == "\"pokemon_encounters\":[")
		{
			inpokemonencounters = true;
			finishedline = true;
		}

		if (textLine == "\"encounter_details\":[")
		{
			inencounterdetails = true;
			finishedline = true;
		}

		if (textLine == "\"pokemon\":{")
		{
			inpokemon = true;
			finishedline = true;
		}

		if (textLine == "]")
		{
			if (inversiondetails)
			{
				inversiondetails = false;
				finishedline = true;
			}
			else if (inpokemonencounters)
			{
				inpokemonencounters = false;
				//cout << "Line " << linenum << ": left pokemon encounters\n";
				finishedline = true;
			}
		}

		if (textLine == "],")
		{
			if (inconditionvalues)
			{
				inconditionvalues = false;
				finishedline = true;
			}
			else if (inencounterdetails)
			{
				inencounterdetails = false;
				finishedline = true;
			}
			if (inencountermethodrates)
			{
				inencountermethodrates = false;
				finishedline = true;
			}
			if (innames)
			{
				innames = false;
				finishedline = true;
			}
		}

		size_t s1End = textLine.find(':');
		string str1 = textLine.substr(0, s1End);

		if (str1 == "\"name\"")
		{
			size_t s2Start = s1End + 2;
			size_t s2End = textLine.find('\"', s2Start + 1);
			string str2 = textLine.substr(s2Start, s2End - s2Start);

			if (inconditionvalues)
			{
				if (!encounterinvalid)
				{
					//make sure encounter meets applicable parameters
					//time: morning/day/night (all gens except 1, 3)
					if (str2 == "time-morning" || str2 == "time-day" || str2 == "time-night")
					{
						if (settings->wantedtime != str2)
						{
							badencounterreason = "Needed time " + settings->wantedtime + " but got " + str2;
							encounterinvalid = true;
						}
					}
					//season: spring/summer/autumn/winter (gen 5)
					if (str2 == "season-spring" || str2 == "season-summer" || str2 == "season-autumn" || str2 == "season-winter")
					{
						if (settings->wantedseason != str2)
						{
							badencounterreason = "Needed season " + settings->wantedseason + " but got " + str2;
							encounterinvalid = true;
						}
					}
					//swarm: yes/no (gen 2-5)
					if (str2 == "swarm-yes" || str2 == "swarm-no")
					{
						if (settings->wantswarm != (str2 == "swarm-yes"))
						{
							string swarmanswer = (settings->wantswarm ? "swarm-yes" : "swarm-no");
							badencounterreason = "Needed swarm " + swarmanswer + " but got " + str2;
							encounterinvalid = true;
						}
					}
					//radar: on/off (DPP, XY)
					if (str2 == "radar-on" || str2 == "radar-off")
					{
						if (settings->wantradar != (str2 == "radar-on"))
						{
							string radaranswer = (settings->wantradar ? "radar-on" : "radar-off");
							badencounterreason = "Needed radar " + radaranswer + " but got " + str2;
							encounterinvalid = true;
						}
					}
					//slot2: none/ruby/sapphire/emerald/firered/leafgreen (gen 4)
					if (str2 == "slot2-none" || str2 == "slot2-ruby" || str2 == "slot2-sapphire" || str2 == "slot2-emerald" || str2 == "slot2-firered" || str2 == "slot2-leafgreen")
					{
						if (settings->wantedslot2game != str2)
						{
							badencounterreason = "Needed slot2game " + settings->wantedslot2game + " but got " + str2;
							encounterinvalid = true;
						}
					}
					//radio: off/hoenn/sinnoh (HGSS)
					if (str2 == "radio-off" || str2 == "radio-hoenn" || str2 == "radio-sinnoh")
					{
						if (settings->wantedradiostation != str2)
						{
							badencounterreason = "Needed radio " + settings->wantedradiostation + " but got " + str2;
							encounterinvalid = true;
						}
					}
				}
				finishedline = true;
			}

			if (inmethod)
			{
				if (!encounterinvalid)
				{
					//these encounter types are impossible to repeat or take a long time. they are not ideal for training.
					//some are not even battle encounters.
					//sos-encounter is because it's a specific and distinct way to train.
					if (str2 == "gift" || str2 == "gift-egg" || str2 == "only-one" || str2 == "pokeflute"
						|| str2 == "squirt-bottle" || str2 == "wailmer-pail" || str2 == "devon-scope"
						|| str2 == "island-scan" || str2 == "sos-encounter" || str2 == "berry-piles"
						|| str2 == "npc-trade" || str2 == "sos-from-bubbling-spot" || str2 == "roaming-grass"
						|| str2 == "roaming-water" || str2 == "feebas-tile-fishing")
					{
						badencounterreason = "Bad encounter type: " + str2;
						encounterinvalid = true;
					}
					method = str2;
					//cout << "Line " << linenum << ": method name\n";
				}
				finishedline = true;
			}
			if (inpokemon && !inconditionvalues && !inversiondetails)
			{
				pokemonname = str2;
				//cout << "Line " << linenum << ": pokemon name " << str2 << "\n";
				finishedline = true;
			}
			//only care for english name. data is not very robust for other langs
			if (approachingplacename)
			{
				placename = str2;
				approachingplacename = false;
				//cout << "Line " << linenum << ": place name\n";
				finishedline = true;
			}
			if (inlanguage)
			{
				if (textLine == "\"name\":\"en\",")
					approachingplacename = true;
				//cout << "Line " << linenum << ": language name\n";
				finishedline = true;
			}
			if (innames && !approachingplacename)
				finishedline = true;
			if (inencountermethod || inversion || inlocation)
				finishedline = true;
			//Alola entries apparently don't have the nice capitalized names that come from the language block
			//we have to use the crummy looking strings instead, which just kinda sit there in the main block
			if (!finishedline)
			{
				placename = str2;
				finishedline = true;
			}
			finishedline = true;
		}

		if (inencounterdetails)
		{

			if (str1 == "\"chance\"")
			{
				size_t s2Start = s1End + 1;
				size_t s2End = textLine.find('\"', s2Start + 1);
				string str2 = textLine.substr(s2Start, s2End - s2Start);
				chance = stoi(str2);
				//cout << "chance is " << chance << "\n";
				finishedline = true;
			}
			if (str1 == "\"max_level\"")
			{
				size_t s2Start = s1End + 1;
				size_t s2End = textLine.find('\"', s2Start + 1);
				string str2 = textLine.substr(s2Start, s2End - s2Start);
				maxlevel = stoi(str2);
				//cout << "maxlevel is " << maxlevel << "\n";
				finishedline = true;
			}
			if (str1 == "\"min_level\"")
			{
				size_t s2Start = s1End + 1;
				size_t s2End = textLine.find('\"', s2Start + 1);
				string str2 = textLine.substr(s2Start, s2End - s2Start);
				minlevel = stoi(str2);
				//cout << "minlevel is " << minlevel << "\n";
				finishedline = true;
			}
			if (textLine == "\"condition_values\":[],")
			{
				badencounterreason = "";
				encounterinvalid = false;
				finishedline = true;
			}
			if (textLine == "\"condition_values\":[")
			{
				inconditionvalues = true;
				badencounterreason = "";
				encounterinvalid = false;
				finishedline = true;
			}
			if (textLine == "\"method\":{")
			{
				inmethod = true;
				finishedline = true;
			}
		}

		if (str1 == "\"rate\"")
		{
			if (inversiondetails)
				finishedline = true;
		}

		if (str1 == "\"url\"")
			finishedline = true;

		if (str1 == "\"game_index\"")
			finishedline = true;

		if (str1 == "\"id\"")
			finishedline = true;

		if (str1 == "\"max_chance\"")
			finishedline = true;

		if (finishedline)
			continue;

		cout << "File " << i << ", line " << linenum << ": Didn't know what to do with:\n" << textLine << "\n";
		cin.get();
		ReadFile.close();
		return 0;
	}
	ReadFile.close();
	return 1;
}

void ReadTables(Settings* settings, vector<EncounterTable>* maintables, string basepath)
{
	if (settings->printtext) cout << "Reading encounter data\n";
	//go through every file
	string tablepath = basepath + "location-area";
	int notch = 1;
	double range = settings->lastfolder - settings->firstfolder;
	if (settings->printtext) cout << "|     PROGRESS     |\n";
	for (int i = settings->firstfolder; i <= settings->lastfolder; i++)
	{
		if ((i - settings->firstfolder) / range >= (notch * 0.05))
		{
			if (settings->printtext) cout << "-";
			notch++;
		}
		if (ParseLocationDataFile(tablepath, i, settings, maintables) == 0)
			return;
	}
	if (settings->printtext) cout << "\n";
	vector<string> warnings;
	for (EncounterTable &table : *maintables)
	{
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
		table.knownerror = false;
		if (table.placename == "S.S.Annedock" //swarm conditions missing
			|| table.placename == "Road42" //probably incorrect golbat 1% encounter in crystal during day/morning
			|| table.placename == "Road13" //crystal entries seem to be duplicated? added up to 300%
			)
		{
			table.knownerror = true;
		}
		if (table.filenumber >= 1035) //starting in alola we're missing seemingly all day/night conditions
		{
			table.knownerror = true;
		}
		//if we're getting data for all games, then there are multiple fully correct encounter tables inside a file
		//this means if the table is correct it will be a multiple of 100
		//long story short we can't know what that multiple will be, so % is our best option
		//also, again, because all is meant for debugging only, we're just going to not expect it to be used in conjunction with repellevel because that gets hairy
		if (settings->wantedgame == "all" ? (table.totalchance % 100 != 0) : (table.totalchance != table.expectedtotalpercent))
		{
			if (table.knownerror)
			{
				warnings.push_back("WARNING: Data for " + table.placename + ", " + table.method + " known to be inaccurate. Total percent chance reported as " + to_string(table.totalchance) + "% when it should be " + to_string(table.expectedtotalpercent) + " after considering levels for repel.\n");
			}
			else
			{
				cout << "ERROR: Total chance was " << table.totalchance << "! File number " << table.filenumber << "\n";
				cin.get();
				return;
			}
		}
	}
	if (settings->printtext) cout << "\n";
	for (string warning : warnings)
		cout << warning;
	if (settings->printtext)
	{
		cout << "\n";
		cout << "Done. Press ENTER or the X button to close.\n";
		cout << "To view data more neatly, copy the text output above and put it into any website or program that can sort text (http://www.unit-conversion.info/texttools/sort-lines/)\n";
		cout << "You may also want to filter the lines in some way (http://www.unit-conversion.info/texttools/filter-lines/)\n";
	}
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

bool compareByExp(const EncounterTable& a, const EncounterTable& b)
{
	return a.totalavgexp > b.totalavgexp;
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

void dosettingswindow(Settings* settings, SettingsWindowData* settingswindowdata, vector<EncounterTable>* maintables, string basepath)
{
	static bool use_work_area = false;
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

	// We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
	// Based on your use case you may want one or the other.
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
#ifndef _DEBUG
	ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
	ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);
#endif
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
		newsettings.firstfolder = 258;
		newsettings.lastfolder = 833;
		newsettings.generation = 1;
	}

	if (newsettings.wantedgame == "gold" || newsettings.wantedgame == "silver" || newsettings.wantedgame == "crystal")
	{
		newsettings.expfile = "gen2_exp.txt";
		newsettings.firstfolder = 184;
		newsettings.lastfolder = 841;
		newsettings.generation = 2;
	}

	if (newsettings.wantedgame == "ruby" || newsettings.wantedgame == "sapphire" || newsettings.wantedgame == "emerald")
	{
		newsettings.firstfolder = 350;
		newsettings.lastfolder = 834;
		rse = true;
	}

	if (newsettings.wantedgame == "firered" || newsettings.wantedgame == "leafgreen")
	{
		newsettings.firstfolder = 258;
		newsettings.lastfolder = 833;
		frlg = true;
	}

	if (rse || frlg)
	{
		newsettings.expfile = "gen3_exp.txt";
		newsettings.generation = 3;
	}

	if (newsettings.wantedgame == "diamond" || newsettings.wantedgame == "pearl" || newsettings.wantedgame == "platinum")
	{
		newsettings.firstfolder = 1;
		newsettings.lastfolder = 840;
		dpp = true;
	}

	if (newsettings.wantedgame == "heartgold" || newsettings.wantedgame == "soulsilver")
	{
		newsettings.firstfolder = 184;
		newsettings.lastfolder = 842;
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
		newsettings.firstfolder = 576;
		newsettings.lastfolder = 775;
		bw1 = true;
	}

	if (newsettings.wantedgame == "black-2" || newsettings.wantedgame == "white-2")
	{
		newsettings.expfile = "gen5bw2_exp.txt";
		newsettings.firstfolder = 576;
		newsettings.lastfolder = 792;
		bw2 = true;
	}

	if (bw1 || bw2)
		newsettings.generation = 5;

	if (newsettings.wantedgame == "x" || newsettings.wantedgame == "y")
	{
		newsettings.expfile = "gen6_exp.txt";
		newsettings.firstfolder = 708;
		newsettings.lastfolder = 780;
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

	if (sm || usum)
	{
		newsettings.firstfolder = 1035;
		newsettings.lastfolder = 1164;
		newsettings.generation = 7;
	}

	if (newsettings.wantedgame == "all")
	{
		newsettings.firstfolder = 1;
		newsettings.lastfolder = 1164;
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

	if (allgames || (newsettings.generation >= 2 && newsettings.generation <= 5))
	{
		static bool wantswarm = false;
		ImGui::Checkbox("Mass Outbreaks", &wantswarm);
		newsettings.wantswarm = wantswarm;
	}

	if (allgames || dpp || xy)
	{
		static bool wantradar = false;
		ImGui::Checkbox("PokeRadar", &wantradar);
		newsettings.wantradar = wantradar;
	}

	if (allgames || newsettings.generation == 4)
	{
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

	static bool printtext = false;
	ImGui::Checkbox("Text Output", &printtext);
	newsettings.printtext = printtext;

	if (settingswindowdata->running)
	{
		//only save the new settings once the button is pressed. this prevents changing the settings mid-iteration.
		settings->firstfolder = newsettings.firstfolder;
		settings->lastfolder = newsettings.lastfolder;
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
		ReadTables(settings, maintables, basepath);
		sort(maintables->begin(), maintables->end(), compareByExp);
		settingswindowdata->running = false;
	}

	if (ImGui::Button("Go!"))
	{
		string imgoing = "Searching through " + to_string(newsettings.lastfolder - newsettings.firstfolder) + " files.";
		ImGui::SameLine(); ImGui::Text(imgoing.c_str());
		maintables->clear();
		settingswindowdata->running = true;
	}

	//detect when settings have changed
	if (settings->firstfolder != newsettings.firstfolder ||
		settings->lastfolder != newsettings.lastfolder ||
		settings->wantedgame != newsettings.wantedgame ||
		settings->wantedtime != newsettings.wantedtime ||
		settings->wantedseason != newsettings.wantedseason ||
		settings->wantswarm != newsettings.wantswarm ||
		settings->wantradar != newsettings.wantradar ||
		settings->wantedslot2game != newsettings.wantedslot2game ||
		settings->wantedradiostation != newsettings.wantedradiostation ||
		settings->expfile != newsettings.expfile ||
		settings->generation != newsettings.generation ||
		settings->repellevel != newsettings.repellevel ||
		settings->maxlevel != newsettings.maxlevel)
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
    bool show_demo_window = true;
    bool show_another_window = false;
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
        ImGui::ShowDemoWindow(&show_demo_window);
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
