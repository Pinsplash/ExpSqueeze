I'm currently trying to make the trainer data for Diamond and Pearl and don't know where to begin. The process for RSE is something I can't recall at all. The "frlgtrainerdata" folder gives enough clues on how the process works. Why isn't there an equivalent folder for RSE? I believe that's because I thought I could put that data together manually, which I must have done, but the amount of time and effort kicked my ass so I decided to change my approach for FRLG. In any case, here are some observations.


A team means a trainer's team that you battle against (eg TRAINER_YOUNGSTER_TIMMY). If a trainer has a new team upon being rematched, it is considered a separate team (eg TRAINER_YOUNGSTER_TIMMY, _2, _3, _4). The original team is used in some places to represent all of the trainer's teams collectively (like in rematch_party_names.csv).

A party is basically the same thing as a team, but the names are a different kind of symbol used elsewhere in code (eg sParty_YoungsterTimmy). They are preferable because they're formatted closer to plain English, which made it sufficiently easy to manually edit them into plain English.

A script refers to a specific bit of code to start a battle (eg Route24_EventScript_Timmy). These are referenced in map data files. That code refers to trainers by... trainer name, but first it checks a table (vs_seeker.c, link below) to see if the trainer should use an upgraded team.

See code for definition of a progress point in the Party struct.


trainer_to_party_names.txt: Starting from 1, odd lines are teams and even lines are the associated party names. All files such as this were certainly used in the "Replace Table" function in my tool, "texttool". Surely this was used to help make rematch_party_names.csv and script_to_party_names.txt. Derived from https://github.com/pret/pokefirered/blob/df4449a27cd78dd747ce269e47d3ab4a0149d8f4/src/data/trainers.h

script_to_trainer_names.txt: Starting from 1, odd lines are script names and even lines are the associated trainers. Derived from https://github.com/pret/pokefirered/blob/4f5fe2a27941770cb1d7c33fcc1fd4c9495838af/data/scripts/trainers.inc#L1310

script_to_party_names.txt: Starting from 1, odd lines are script names and even lines are the associated parties. Derived from script_to_trainer_names.txt and trainer_to_party_names.txt.

overworld_party_names.txt: Assigns every trainer to a location by initial party name. The numbers are PokeAPI table indices. This is basically what you would get if you combined all the relevant map JSON files (eg https://github.com/pret/pokefirered/blob/df4449a27cd78dd747ce269e47d3ab4a0149d8f4/data/maps/Route24/map.json), stripped them to just map names and script names, and manually replaced map names with PokeAPI's indices. "Relevant" means ones in areas that have trainers and allow the rematch gadget to be used, i.e. outside.

party_defs.txt: Tells the team composition of every party. Derived from https://github.com/pret/pokefirered/blob/df4449a27cd78dd747ce269e47d3ab4a0149d8f4/src/data/trainer_parties.h

rematch_party_names.csv: A table telling the points at which a trainer's team upgrades. If a party has no entry in here, the trainer keeps the same team forever. Some entries are redundant and don't actually result in a trainer's team changing. This is fine. Used to create rematchX.csv files. Derived from https://github.com/pret/pokefirered/blob/df4449a27cd78dd747ce269e47d3ab4a0149d8f4/src/vs_seeker.c

rematchX.csv: Tells what parties become available once the progress point in question is reached. The number in the file name represents the progress point. Each lines tells the party, followed by the PokeAPI table associated with its location. Taking rematch_party_names.csv and using "Column Extract" in texttool should create these files very simply.


Also see the "Make FRLG Trainer Data" button's code.
