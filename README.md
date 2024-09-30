# [Download from here](https://github.com/Pinsplash/ExpSqueeze/releases)

# ExpSqueeze
ExpSqueeze finds the average amount of experience in Pokemon that can be gained from a single wild encounter within an encounter table. It prints out data for every encounter table in the specified game at once and the output can easily be externally filtered and sorted to analyze it. ExpSqueeze only considers the most straightforward training possible and does not consider more niche methods like SOS encounters or training on Ditto. ExpSqueeze works for most mainline Pokemon games except Generation 7+.

The files in the [location-area](https://github.com/Pinsplash/ExpSqueeze/tree/main/expsqueeze/expsqueeze/location-area) folder were taken from the [PokeAPI database](https://github.com/PokeAPI/api-data/) in June 2024. It's unlikely the most recent set of files is significantly better, but if you want to update it anyway, you can download it from their repository and replace the location-area folder in this project with that one. The download is not large but has several thousand files and File Explorer will lag a lot when it opens the .zip, so try using 7zip instead. The .txt files were made using tables from [Bulbapedia](https://bulbapedia.bulbagarden.net/wiki/List_of_Pok%C3%A9mon_by_effort_value_yield_(Generation_III)) in June 2024. These are also unlikely to significantly change, but the process for creating these files was much more involved and not worth discussing. If you want to edit these, you can open one and understand the data structure by simply looking at it.

## How it works
Encounter tables consist of encounter slots. Every slot has a Pokemon, minimum and maximum level, and % chance value. ExpSqueeze finds the average level given the minimum and maximum level, solves the leftmost fraction of the [experience gain formula](https://bulbapedia.bulbagarden.net/wiki/Experience#Gain_formula), and then weighs this according to the slot's % chance. This is done for every slot. When the numbers are added up, you get the encounter table's average experience per encounter.

![image](https://github.com/user-attachments/assets/e908de5f-3014-448c-984d-b5731bb69b41)

Now with a UI!
