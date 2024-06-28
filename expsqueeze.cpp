#include <fstream>
#include <iostream>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <complex>
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
};

struct Encounter
{
	int chance;
	int minlevel;
	int maxlevel;
	string pokemonname;
};

struct EncounterTable
{
	string method;
	string placename;
	vector<Encounter> encounters;
	int filenumber;
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

int Setup(Settings *settings)
{
	string answer;

	cout << "Enter game name. Only include the main word(s) of the game's name and replace spaces with hyphens (e.g. black-2, yellow, soulsilver)\nGens 8+, spinoffs, Pokemon Green, ORAS, and Let's Go games not available.\n";
	getline(cin, answer);
	
	//answer = "pearl";
	settings->wantedgame = answer;

	//validate game choice
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
	settings->generation = 0;

	if (settings->wantedgame == "blue" || settings->wantedgame == "red" || settings->wantedgame == "yellow")
	{
		settings->expfile = "gen1_exp.txt";
		settings->firstfolder = 258;
		settings->lastfolder = 833;
		settings->generation = 1;
	}

	if (settings->wantedgame == "gold" || settings->wantedgame == "silver" || settings->wantedgame == "crystal")
	{
		settings->expfile = "gen2_exp.txt";
		settings->firstfolder = 184;
		settings->lastfolder = 841;
		settings->generation = 2;
	}

	if (settings->wantedgame == "ruby" || settings->wantedgame == "sapphire" || settings->wantedgame == "emerald")
	{
		settings->firstfolder = 350;
		settings->lastfolder = 834;
		rse = true;
	}

	if (settings->wantedgame == "firered" || settings->wantedgame == "leafgreen")
	{
		settings->firstfolder = 258;
		settings->lastfolder = 833;
		frlg = true;
	}

	if (rse || frlg)
	{
		settings->expfile = "gen3_exp.txt";
		settings->generation = 3;
	}

	if (settings->wantedgame == "diamond" || settings->wantedgame == "pearl" || settings->wantedgame == "platinum")
	{
		settings->firstfolder = 1;
		settings->lastfolder = 840;
		dpp = true;
	}

	if (settings->wantedgame == "heartgold" || settings->wantedgame == "soulsilver")
	{
		settings->firstfolder = 184;
		settings->lastfolder = 842;
		hgss = true;
	}

	if (dpp || hgss)
	{
		settings->expfile = "gen4_exp.txt";
		settings->generation = 4;
	}

	if (settings->wantedgame == "black" || settings->wantedgame == "white")
	{
		settings->expfile = "gen5bw1_exp.txt";
		settings->firstfolder = 576;
		settings->lastfolder = 775;
		bw1 = true;
	}

	if (settings->wantedgame == "black-2" || settings->wantedgame == "white-2")
	{
		settings->expfile = "gen5bw2_exp.txt";
		settings->firstfolder = 576;
		settings->lastfolder = 792;
		bw2 = true;
	}

	if (bw1 || bw2)
		settings->generation = 5;

	if (settings->wantedgame == "x" || settings->wantedgame == "y")
	{
		settings->expfile = "gen6_exp.txt";
		settings->firstfolder = 708;
		settings->lastfolder = 780;
		settings->generation = 6;
		xy = true;
	}

	if (settings->wantedgame == "sun" || settings->wantedgame == "moon")
	{
		settings->expfile = "gen7sm_exp.txt";
		sm = true;
	}

	if (settings->wantedgame == "ultra-sun" || settings->wantedgame == "ultra-moon")
	{
		settings->expfile = "gen7usum_exp.txt";
		usum = true;
	}

	if (sm || usum)
	{
		settings->firstfolder = 1035;
		settings->lastfolder = 1164;
		settings->generation = 7;
	}

	if (settings->wantedgame == "all")
	{
		settings->firstfolder = 1;
		settings->lastfolder = 1164;
		allgames = true;
	}

	if (settings->generation == 0 && !allgames)
	{
		cout << "\nGame '" << settings->wantedgame << "' not valid. Here is the full list of games. Please restart and enter the game's name as shown here.\n";
		cout << "blue, red, yellow\ngold, silver, crystal\nruby, sapphire, emerald, firered, leafgreen\n";
		cout << "diamond, pearl, platinum, heartgold, soulsilver\nblack, white, black-2, white-2\n";
		cout << "x, y,\nsun, moon, ultra-sun, ultra-moon";
		cin.get();
		return 0;
	}

	//conditions https://github.com/PokeAPI/api-data/blob/master/data/api/v2/encounter-condition-value/index.json
	//encounter slots that get replaced are marked with things like "off" or "none"
	//all slots in a table that change based on conditions will still add up to the same probability. for instance if there's a 10% chance on a morning encounter, there must necessarily be a day and night encounter with 10% as well.
	//even though we don't know specifically which slots are replacing which, the chances will always add up to 100% regardless of the condition's value.
	//ask user about these things for relevant games
	//time: morning/day/night (all gens except 1, 3)
	settings->wantedtime = "NA";
	if (allgames || (settings->generation != 1 && settings->generation != 3))
	{
		cout << "\n\nEnter time of day. morning, day, or night\n";
		getline(cin, answer);
		//answer = "day";
		if (answer == "morning")
			settings->wantedtime = "time-morning";
		else if (answer == "day")
			settings->wantedtime = "time-day";
		else if (answer == "night")
			settings->wantedtime = "time-night";
		else
		{
			cout << "Didn't understand answer, going with default day\n";
			settings->wantedtime = "time-day";
		}
	}
	//season: spring/summer/autumn/winter (gen 5)
	settings->wantedseason = "NA";
	if (allgames || settings->generation == 5)
	{
		cout << "\n\nEnter season. spring, summer, autumn, or winter\n";
		getline(cin, answer);
		//answer = "spring";
		if (answer == "spring")
			settings->wantedseason = "season-spring";
		else if (answer == "summer")
			settings->wantedseason = "season-summer";
		else if (answer == "autumn")
			settings->wantedseason = "season-autumn";
		else if (answer == "winter")
			settings->wantedseason = "season-winter";
		else
		{
			cout << "Didn't understand answer, going with default spring\n";
			settings->wantedseason = "season-spring";
		}

	}
	//swarm: yes/no (gen 2-5)
	settings->wantswarm = false;
	if (allgames || (settings->generation >= 2 && settings->generation <= 5))
	{
		cout << "\n\nPretend mass outbreaks are active? yes or no\n";
		getline(cin, answer);
		//answer = "no";
		settings->wantswarm = answer == "yes";
		if (answer != "yes" && answer != "no")
		{
			cout << "Didn't understand answer, going with default no\n";
		}
	}
	//radar: on/off (DPP, XY)
	settings->wantradar = false;
	if (allgames || dpp || xy)
	{
		cout << "\n\nPoke radar status? on or off (don't know if this matters for X/Y)\n";
		getline(cin, answer);
		//answer = "off";
		settings->wantradar = answer == "on";
		if (answer != "on" && answer != "off")
		{
			cout << "Didn't understand answer, going with default off\n";
		}
	}
	//slot2: none/ruby/sapphire/emerald/firered/leafgreen (gen 4)
	settings->wantedslot2game = "NA";
	if (allgames || settings->generation == 4)
	{
		cout << "\n\nEnter game for DS slot 2. (Pal Park) none, ruby, sapphire, emerald, firered, or leafgreen\n";
		getline(cin, answer);
		//answer = "none";
		if (answer == "none")
			settings->wantedslot2game = "slot2-none";
		else if (answer == "ruby")
			settings->wantedslot2game = "slot2-ruby";
		else if (answer == "sapphire")
			settings->wantedslot2game = "slot2-sapphire";
		else if (answer == "emerald")
			settings->wantedslot2game = "slot2-emerald";
		else if (answer == "firered")
			settings->wantedslot2game = "slot2-firered";
		else if (answer == "leafgreen")
			settings->wantedslot2game = "slot2-leafgreen";
		else
		{
			cout << "Didn't understand answer, going with default none\n";
			settings->wantedslot2game = "slot2-none";
		}
	}
	//radio: off/hoenn/sinnoh (HGSS)
	settings->wantedradiostation = "NA";
	if (allgames || hgss)
	{
		cout << "\n\nEnter radio status. (Hoenn Sound and Sinnoh Sound) off, hoenn, or sinnoh\n";
		getline(cin, answer);
		//answer = "off";
		if (answer == "off")
			settings->wantedradiostation = "radio-off";
		else if (answer == "hoenn")
			settings->wantedradiostation = "radio-hoenn";
		else if (answer == "sinnoh")
			settings->wantedradiostation = "radio-sinnoh";
		else
		{
			cout << "Didn't understand answer, going with default off\n";
			settings->wantedradiostation = "radio-off";
		}
	}

	//these conditions are not used for encounters that are repeatable, at least easily, so we don't care about them:
	//starter
	//tv-option
	//story-progress
	//other
	//item
	//weekday
	//first-party-pokemon
	return 1;
	
}

int ParseLocationDataFile(string basepath, int i, Settings *settings, vector<EncounterTable> *maintables)
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
						makenewtable = false;
						//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
						table.encounters.push_back(newEnc);
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
					//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
					newTable.encounters.push_back(newEnc);

					maintables->push_back(newTable);
				}
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
						makenewtable = false;
						//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
						table.encounters.push_back(newEnc);
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
					//cout << "///" << placename << ", " << method << " has a " << chance << "% chance of finding a " << pokemonname << " between level " << minlevel << " and " << maxlevel << ".\n";
					newTable.encounters.push_back(newEnc);

					maintables->push_back(newTable);
				}
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

int main(int argc, char* argv[])
{
	vector<EncounterTable> maintables;
	string basepath = "location-area";//argv[1];
	Settings settings;
	if (Setup(&settings) == 0)
		return 0;

	cout << "\n\n";
	cout << "Reading encounter data\n";
	//go through every file
	int notch = 1;
	double range = settings.lastfolder - settings.firstfolder;
	cout << "|     PROGRESS     |\n";
	for (int i = settings.firstfolder; i <= settings.lastfolder; i++)
	{
		if ((i - settings.firstfolder) / range >= (notch * 0.05))
		{
			cout << "-";
			notch++;
		}
		if (ParseLocationDataFile(basepath, i, &settings, &maintables) == 0)
			return 0;
	}
	cout << "\n";
	vector<string> warnings;
	for (EncounterTable table : maintables)
	{
		cout << "\n" << table.placename << ", " << table.method << "\n";
		double totalavgexp = 0;
		int totalchance = 0;//sanity check: this number should always = 100 at the end of the table.
		for (Encounter encounter : table.encounters)
		{
			//get experience yield from stripped down bulba tables
			int baseexp = 0;
			if (settings.wantedgame == "all")
			{
				//this is a debugging feature only.
				//since different gens have different BEY tables and we have no way to discern which tables are for which gens, give up
				//our only interest in this case is the percent total adding up to 100
			}
			else
			{
				string path = settings.expfile;
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
			//30-30: 1
			//30-31: 2
			//30-40: 11
			int numlevels = (encounter.maxlevel - encounter.minlevel) + 1;
			double levelsum = 0;
			for (int i = 0; i < numlevels; i++)
				levelsum += encounter.minlevel + i;
			double avglevel = levelsum / numlevels;
			int factor = (settings.generation == 5 || settings.generation >= 7) ? 5 : 7;
			double avgexpweighted = (((baseexp * avglevel) / factor) * encounter.chance) / 100;
			cout << encounter.pokemonname << " has " << encounter.chance << "% chance between level " << encounter.minlevel << " and " << encounter.maxlevel << ". avgexp " << ((baseexp * avglevel) / factor) << ", weighted " << avgexpweighted << "\n";
			totalavgexp += avgexpweighted;
			totalchance += encounter.chance;
		}
		cout << totalavgexp << " average EXP in " << table.placename << ", " << table.method << "\n";
		bool knownerror = false;
		if (table.placename == "S.S.Annedock" //swarm conditions missing
			|| table.placename == "Road42" //probably incorrect golbat 1% encounter in crystal during day/morning
			|| table.placename == "Road13" //crystal entries seem to be duplicated? added up to 300%
			)
		{
			knownerror = true;
		}
		if (table.filenumber >= 1035) //starting in alola we're missing seemingly all day/night conditions
		{
			knownerror = true;
		}
		//if we're getting data for all games, then there are multiple fully correct encounter tables inside a file
		//this means if the table is correct it will be a multiple of 100
		//long story short we can't know what that multiple will be, so % is our best option
		if (settings.wantedgame == "all" ? (totalchance % 100 != 0) : (totalchance != 100))
		{
			if (knownerror)
			{
				warnings.push_back("WARNING: Data for " + table.placename + ", " + table.method + " known to be inaccurate. Total percent chance reported as " + to_string(totalchance) + "%.\n");
			}
			else
			{
				cout << "ERROR: Total chance was " << totalchance << "! File number " << table.filenumber << "\n";
				cin.get();
				return 0;
			}
		}
	}
	cout << "\n";
	for (string warning : warnings)
		cout << warning;
	cout << "\n";
	cout << "Done. Press ENTER or the X button to close.\n";
	cout << "To view data more neatly, copy the text output above and put it into any website or program that can sort text (http://www.unit-conversion.info/texttools/sort-lines/)\n";
	cout << "You may also want to filter the lines in some way (http://www.unit-conversion.info/texttools/filter-lines/)\n";
	cin.get();
}