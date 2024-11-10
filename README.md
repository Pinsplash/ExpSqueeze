# [Download from here](https://github.com/Pinsplash/ExpSqueeze/releases)

# ExpSqueeze
ExpSqueeze finds the average amount of experience in Pokemon that can be gained from a single wild encounter within an encounter table. It prints out data for every encounter table in the specified game at once and the output can easily be externally filtered and sorted to analyze it. ExpSqueeze only considers the most straightforward training possible and does not consider more niche methods like SOS encounters or training on Ditto. ExpSqueeze works for most mainline Pokemon games except Generation 8+.

The files in the pkmndata folder were taken from the [PokeAPI database](https://github.com/PokeAPI/api-data/). The exp-gain-stats files were made using tables from [Bulbapedia](https://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_effort_value_yield_(Generation_III)).

ExpSqueeze uses [Imgui](https://github.com/ocornut/imgui) for its UI.

The data generated by ExpSqueeze can be filtered and manipulated in many different ways to make it easier to find the perfect place to train a pokemon or for other analytical uses.

* Accounting for factors that change encounter tables, like the time of day.
* Ability to filter by the types of pokemon.
* Adjusting calculated experience when repels are used.
* And a lot more!

## How it works
Encounter tables consist of encounter slots. Every slot has a Pokemon, minimum and maximum level, and % chance value. ExpSqueeze finds the average level given the minimum and maximum level, solves the leftmost fraction of the [experience gain formula](https://bulbapedia.bulbagarden.net/wiki/Experience#Gain_formula), and then weighs this according to the slot's % chance. This is done for every slot. When the numbers are added up, you get the encounter table's average experience per encounter.

![image](https://github.com/user-attachments/assets/1b4d884e-7554-4d87-b0b1-f4b2e5a1b7ba)
