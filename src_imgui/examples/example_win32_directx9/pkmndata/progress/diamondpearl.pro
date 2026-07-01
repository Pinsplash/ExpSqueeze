//NOTE: this progress file does not detail which trainers, if any, are only accessible after beating other trainers as the value is perceived as too low for how much work that would entail. There are also too many checkboxes for my liking as is.

//game progress description file. see gen1.pro for explanations of most features.
checkpoint Got Running Shoes
unlock walk
//water not immediately accessible. the water level rises during the team galactic events with no real explanation.
135//lake-verity-before-galactic-intervention
141//sinnoh-route-201
177//twinleaf-town

checkpoint Completed catching tutorial
//technically this happens once you meet rowan properly, but since it's a water-only route, a milestone for that wouldn't make any visible difference, so we shove it in here
169 exclude rematch//sinnoh-route-219
142//sinnoh-route-202
unlock old-rod
168 exclude walk,rematch//sinnoh-route-218
144//sinnoh-route-204-south-towards-jubilife-city
54 exclude old-rod//ravaged-path

checkpoint Defeated Barry on Route 203
143//sinnoh-route-203
55//oreburgh-gate-1f
149//sinnoh-route-207
6//oreburgh-mine-1f
7//oreburgh-mine-b1f

checkbox 1 Obtained ability to use Rock Smash
56//oreburgh-gate-b1f

checkpoint Defeated Galactic Grunts in Jubilife City
54//ravaged-path
145//sinnoh-route-204-north-towards-floaroma-town
//water accessible from bridge
146 exclude walk//sinnoh-route-205-south-towards-floaroma-town
8//valley-windworks

checkpoint Obtained ability to use Rock Smash
cancel 1
56//oreburgh-gate-b1f

checkpoint Defeated Commander Mars
146//sinnoh-route-205-south-towards-floaroma-town
9//eterna-forest
147//sinnoh-route-205-east-towards-eterna-city
159//sinnoh-route-211-west-towards-eterna-city
2//eterna-city
22//mt-coronet-1f-route-211

checkpoint Obtained ability to use Cut
126//old-chateau
133//old-chateau-2f-right-room

checkpoint Obtained Bicycle and Explorer Kit
148//sinnoh-route-206
113//wayward-cave-1f

checkpoint Obtained Vs. Seeker
unlock rematch
progresspoint 2
11//mt-coronet-1f-route-207
150 exclude remnum:8,remnum:9,//sinnoh-route-208

checkpoint Visited Contest Hall then defeated Barry
151//sinnoh-route-209
unlock good-rod

checkpoint Defeated Twins Emma & Lil (Route 209) or avoid by having 1 Pokemon in party//confirmed possible
30//solaceon-ruins
157 exclude remnum:3,remnum:4,remnum:5,//sinnoh-route-210-south-towards-solaceon-town
165//sinnoh-route-215
152//lost-tower-1f
153//lost-tower-2f
154//lost-tower-3f
155//lost-tower-4f
156//lost-tower-5f

checkpoint Reached Solaceon Town
161//sinnoh-route-212-north-towards-hearthome-city
118//trophy-garden
//only a bit of grass accessible before the trainers
162 exclude rematch,old-rod,good-rod//sinnoh-route-212-east-towards-pastoria-city

//South Sinnoh has a situation similar to South Kanto: A string of areas can be traversed coming from one direction or the other, and there are mandatory trainers along the way. Fortunately it's not quite as complicated.
//Ace Trainers Dennis and Maya are at the East end of Route 215 and block Veilstone City when coming from Solaceon Town. (These two end up not being mentioned in the final data because they only block access to Veilstone City, which has no wild Pokemon. Defeating them is only important for story reasons, and that's depending on the route you take.)
//Ace Trainers Allison and Jeffrey are near the rainy/clear border of Route 212 and block Pastoria City when coming from Hearthome City.
//Psychic Abigail is on Route 214, blocks everything if coming from Veilstone, and blocks Veilstone (and therefore the Cobble Badge) if coming from Pastoria.
//Northern approach requires beating Dennis and Maya and Abigail to reach Pastoria.
//Southern approach requires beating Allison and Jeffrey and Abigail to reach Veilstone.
//You could also do a rather unnecessary back-and-forth kind of approach where you beat Dennis and Maya to reach Veilstone, and Allison and Jeffrey to reach Pastoria, without ever meeting Abigail.
//This all matters because you need to defeat both Wake and the Veilstone Galactic Grunt Pair to trigger the Team Galactic events in Pastoria, to get the SecretPotion, to open the way to Celestic Town, to get the Surf HM, which triggers Fantina to come back to her Gym to allow you to beat her, to obtain the ability to use Surf outside of battle, to ascend Mt. Coronet later, to make the man on Route 222 stop blocking the way to Sunyshore City (and therefore the E4).

checkpoint Defeated Ace Trainers Allison and Jeffrey or Psychic Abigail//confirmed not avoidable by only having 1 pokemon
3//pastoria-city
163 exclude remnum:5,remnum:6,remnum:7,remnum:8,remnum:9,//sinnoh-route-213
139//valor-lakefront
164//sinnoh-route-214
162//sinnoh-route-212-east-towards-pastoria-city
115//ruin-maniac-cave-0-9-different-unown-seen
116//ruin-maniac-cave-10-25-different-unown-seen
117//maniac-tunnel-26-plus-different-unown-seen

checkpoint Gave SecretPotion to Psyduck on Route 210
158 exclude remnum:8,remnum:9,remnum:10,//sinnoh-route-210-west-towards-celestic-town

checkpoint Defeated Bird Keeper Brianna (North Route 210)
178//celestic-town
160//sinnoh-route-211-east-towards-celestic-town
22//mt-coronet-1f-route-211

checkpoint Defeated Fantina
progresspoint 3
150 exclude remnum:9,//sinnoh-route-208
169//sinnoh-route-219
180//sinnoh-sea-route-220
170//sinnoh-route-221
10//fuego-ironworks
163//sinnoh-route-213
168//sinnoh-route-218

checkpoint Defeated Barry in Canalave City
1//canalave-city
119//iron-island-outside
120//iron-island-1f
121//iron-island-b1f-left
122//iron-island-b1f-right
123//iron-island-b2f-right
124//iron-island-b2f-left
125//iron-island-b3f

//boxes 2-4 can be delayed until the story requires you visit lake acuity

checkbox 2 Obtained ability to use Strength
23//mt-coronet-b1f
21//mt-coronet-1f-route-216
166 exclude remnum:5,remnum:6,remnum:7,remnum:8,remnum:9,remnum:10,remnum:11,//sinnoh-route-216
114//wayward-cave-b1f

checkbox 3 Defeated Ace Trainer Dalton (Route 217)
167//sinnoh-route-217

checkbox 4 Defeated Ace Trainer Olivia (Route 217)
140//acuity-lakefront

checkbox 5 Obtained ability to use Rock Climb
150//sinnoh-route-208
158//sinnoh-route-210-west-towards-celestic-town
166//sinnoh-route-216
12//mt-coronet-2f

checkbox 10 Defeated Commander Saturn at Lake Valor
remove 135

checkbox 11 Defeated 1st Galactic Grunt pair at Lake Verity
136//lake-verity-after-galactic-intervention

checkpoint Defeated Commander Mars at Lake Verity and Candice
138//lake-acuity
cancel 2
23//mt-coronet-b1f
21//mt-coronet-1f-route-216
166 exclude remnum:5,remnum:6,remnum:7,remnum:8,remnum:9,remnum:10,remnum:11,//sinnoh-route-216
114//wayward-cave-b1f
cancel 3
167//sinnoh-route-217
cancel 4
140//acuity-lakefront
cancel 10
remove 135
cancel 11
136//lake-verity-after-galactic-intervention

checkpoint Obtained ability to use Rock Climb
cancel 5
150//sinnoh-route-208
158//sinnoh-route-210-west-towards-celestic-town
166//sinnoh-route-216
12//mt-coronet-2f
13//mt-coronet-3f
14//mt-coronet-exterior
20//mt-coronet-tunnel-room
16//mt-coronet-4f
17//mt-coronet-4f-l-room
18//mt-coronet-5f

checkpoint Defeated last Galactic Grunt in Mt. Coronet 5F
19//mt-coronet-6f

checkpoint Defeated Cyrus
171//sinnoh-route-222

checkpoint Defeated Dialga/Palkia
137//lake-valor
progresspoint 4

checkpoint Defeated Sailor Luther
4//sunyshore-city
181//sinnoh-sea-route-223

checkbox 6 Defeated Swimmer Oscar
5 exclude rematch//sinnoh-pokemon-league

checkbox 9 Obtained ability to use Waterfall
15//mt-coronet-4f-small-room

checkpoint Obtained ability to use Waterfall and defeated Swimmer Oscar
cancel 6
5 exclude rematch//sinnoh-pokemon-league
cancel 9
15//mt-coronet-4f-small-room
48//sinnoh-victory-road-1f

checkpoint Defeated Ace Trainer Mariah (Victory Road 1F)
49//sinnoh-victory-road-2f

checkpoint Defeated Black Belt Miles (Victory Road 1F)
50//sinnoh-victory-road-b1f

checkpoint Became Champion
//rematches
5//sinnoh-pokemon-league
progresspoint 5

checkpoint Obtained National Pokedex
52//sinnoh-victory-road-back-entrance
51 exclude surf,old-rod,good-rod,super-rod//sinnoh-victory-road-back-main
107//snowpoint-temple-1f
108//snowpoint-temple-b1f
109//snowpoint-temple-b2f
110//snowpoint-temple-b3f
111//snowpoint-temple-b4f
112//snowpoint-temple-b5f
60//sendoff-spring
61//turnback-cave-pillar-1
62//turnback-cave-pillar-2
63//turnback-cave-pillar-3
64//turnback-cave-before-pillar-1
70//turnback-cave-between-pillars-1-and-2
76//turnback-cave-between-pillars-2-and-3

checkbox 8 Escorted Marley through Victory Road
51//sinnoh-victory-road-back-main
53//sinnoh-victory-road-back-exit
172//sinnoh-route-224

checkpoint Visited Battle Tower
unlock super-rod
173 exclude surf,old-rod,good-rod,super-rod//sinnoh-route-225
183//sinnoh-sea-route-230
176//sinnoh-route-229
179//resort-area
175//sinnoh-route-228

//another "approach the gauntlet two ways" situation
//Ace Trainer Meagan sits on Route 228 and blocks Route 226, 227, Stark Mountain if going CCW. If going CW, everything past her was already accessible.
//Psychic Daisy and Bird Keeper Audrey sit on Route 225 and block the route's water, Route 226, 227, 228, Stark Mountain if going CW. The paths that the two block converge quickly. If going CCW, everything past them was already accessible. Pokemon Ranger Ashlee is situated between them such that it's easy to create a double battle with her and either of the other two. If you battle Daisy or Audrey first by talking directly to them, you can then avoid Ashlee with conventional spinner-avoidance logic.
//These are the only two checkpoints before reaching Stark Mountain and they both unlock the same areas.

checkbox 7 Defeated Ace Trainer Meagan (Route 228), Psychic Daisy, or Bird Keeper Audrey (Route 225)
182//sinnoh-sea-route-226
174//sinnoh-route-227
173//sinnoh-route-225
57//stark-mountain-exterior
58//stark-mountain-first-room
59//stark-mountain-inside

checkpoint Defeated Heatran
progresspoint 6
cancel 7
182//sinnoh-sea-route-226
174//sinnoh-route-227
173//sinnoh-route-225
57//stark-mountain-exterior
58//stark-mountain-first-room
59//stark-mountain-inside