# [Download from here](https://github.com/Pinsplash/ExpSqueeze/releases)

# ExpSqueeze
ExpSqueeze finds the average amount of experience in Pokemon that can be gained from a single wild encounter within an encounter table. It prints out data for every encounter table in the specified game at once and the output can easily be externally filtered and sorted to analyze it. ExpSqueeze only considers the most straightforward training possible and does not consider more niche methods like SOS encounters or training on Ditto. ExpSqueeze works for most mainline Pokemon games except Generation 8+.

Most of the files in the pkmndata folder were taken from the [PokeAPI database](https://github.com/PokeAPI/api-data/) in June 2024. It has been edited to an extent that makes it somewhat incompatible with any updates to their data repository. Any updates to their data have to be mirrored manually.

The exp-gain-stats files were made using tables from [Bulbapedia](https://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_effort_value_yield_(Generation_III)) in June 2024. If you want to edit these, you can open one and understand the data structure by simply looking at it.

## How it works
Encounter tables consist of encounter slots. Every slot has a Pokemon, minimum and maximum level, and % chance value. ExpSqueeze finds the average level given the minimum and maximum level, solves the leftmost fraction of the [experience gain formula](https://bulbapedia.bulbagarden.net/wiki/Experience#Gain_formula), and then weighs this according to the slot's % chance. This is done for every slot. When the numbers are added up, you get the encounter table's average experience per encounter.

![image](https://github.com/user-attachments/assets/59ec320c-b9ab-4b63-86e6-8c7c5bc284e2)
