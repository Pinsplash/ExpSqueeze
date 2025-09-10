//NOTE: this progress file does not detail which trainers, if any, are only accessible after beating other trainers as the value is perceived as too low for how much work that would entail. There are also too many checkboxes for my liking as is.

//game progress description file. see gen1.pro for explanations of most features.
checkpoint Battled Rival for the first time
unlock walk

//these places are reachable at the start of the game
285//pallet-town
295//kanto-route-1
280//viridian-city
313//kanto-route-22

checkpoint Delivered Oak's Parcel
296//kanto-route-2-south-towards-viridian-city
321//viridian-forest

checkpoint Defeated Bug Catcher Sammy (Viridian Forest)
320//kanto-route-2-north-towards-pewter-city
//no wild pokemon in pewter city

checkpoint Defeated Bug Catcher James (Route 3)
297//kanto-route-3
290//mt-moon-1f
326//mt-moon-b1f
327//mt-moon-b2f

oneway 1 Crossed over final ledge on Route 4
//single trainer on route requires surf
298 exclude rematch//kanto-route-4
281//cerulean-city
346//cerulean-gym

checkpoint Defeated all of Nugget Bridge
314//kanto-route-24
315//kanto-route-25

//after visiting bill, officer jenny arbitrarily decides to stop standing in the way
checkpoint Defeated TM thief
299//kanto-route-5
300//kanto-route-6

checkpoint Defeated Camper Jeff (Route 6)
282//vermilion-city
unlock old-rod
349//vermilion-city-port
305//kanto-route-11
317//digletts-cave

checkbox 25 Obtained Vs. Seeker
unlock rematch
progresspoint 2

checkbox 2 S.S. Anne departed
remove 349//vermilion-city-port

checkbox 6 Obtained ability to use Cut
//if you get the cascade badge first, you technically have the ability to use Cut immediately after getting the HM, so don't remove the dock
cancel 1

checkpoint Defeated Picnicker Alicia (Route 9)
//we clearly have cut by now, so we can go back to places before the route 4 one-way
cancel 1
cancel 6
//ss anne is clearly gone by now
cancel 2
remove 349//vermilion-city-port

checkbox 5 Defeated Bug Catcher Brent (Route 9)
303//kanto-route-9

checkbox 12 Defeated Camper Drew (Route 9)
303//kanto-route-9

checkpoint Bug Catcher Conner (Route 9)
304//kanto-route-10
292//rock-tunnel-1f

checkpoint Defeated Pokemaniac Ashton (Rock Tunnel)
293//rock-tunnel-b1f

checkpoint Defeated Picnicker Dana (Rock Tunnel)
//no wild pokemon in lavender town or first two floors of pokemon tower
//here we exclude the walk table, because the grass isn't accessible until later
276 exclude walk//kanto-route-12

checkbox 10 Defeated Gamer Rich or Lass Julia (Route 8)
//east approach
302//kanto-route-8 (no grass until after this checkpoint)
301//kanto-route-7
283//celadon-city
309//kanto-route-16
//doing this from west grants us nothing new

checkbox 24 Reached Celadon City
progresspoint 3

checkbox 9 Obtained Rainbow Badge
329 exclude walk//kanto-route-23

checkpoint Defeated Rival in Pokemon Tower
336//pokemon-tower-3f
337//pokemon-tower-4f
338//pokemon-tower-5f

checkpoint Defeated Channeler Tammy on Pokemon Tower 5F
339//pokemon-tower-6f

checkpoint Defeated Marowak
340//pokemon-tower-7f

checkpoint Woke Snorlax
//after you get the poke flute, you have two paths to fuchsia:
//route 12 to 15 in the east, or
//the cycling road in the west
//the only requirement for either way is obtaining the bike for cycling road
//no mandatory trainers at all
unlock super-rod
//grass now reachable
276//kanto-route-12
306//kanto-route-13
//bikers are avoidable
307//kanto-route-14
308//kanto-route-15

checkbox 24 Reached Fuchsia City
progresspoint 4
311 exclude surf,old-rod,good-rod,super-rod//kanto-route-18
284//fuchsia-city
unlock good-rod
277//kanto-sea-route-19

checkbox 13 Obtained Bicycle
310//kanto-route-17
311//kanto-route-18

checkpoint Obtained ability to use Surf
cancel 13//explained this in gen1.pro
cancel 24
progresspoint 4
311 exclude surf,old-rod,good-rod,super-rod//kanto-route-18
284//fuchsia-city
unlock good-rod
277//kanto-sea-route-19
unlock surf
//strength is unneeded. all of these water areas can be reached by surfing south from pallet town.
278//kanto-sea-route-20
258//seafoam-islands-1f
259//seafoam-islands-b1f
260//seafoam-islands-b2f
261//seafoam-islands-b3f
262//seafoam-islands-b4f
279//cinnabar-island
341//pokemon-mansion-1f
342//pokemon-mansion-2f
343//pokemon-mansion-3f
344//pokemon-mansion-b1f
312//kanto-sea-route-21
330//power-plant
//single trainer on route requires surf
298//kanto-route-4

//explained this in gen1.pro
checkbox 26 Obtained Bicycle
310//kanto-route-17
311//kanto-route-18

checkbox 11 Obtained Marsh Badge and haven't defeated Giovanni
329//kanto-route-23

checkbox 14 Traveled to Sevii Islands (first visit)
561//one-island
512//kindle-road
513//treasure-beach

checkbox 15 Obtained ability to use Rock Smash
unlock rock-smash

checkbox 16 Obtained ability to use Strength
488//mt-ember

checkbox 17 Defeated PKMN Ranger Logan (Mt. Ember)
489//mt-ember-summit-path-1-3
490//mt-ember-summit-path-2
825//mt-ember-summit

checkbox 18 Obtained Tri-pass
514//cape-brink

checkbox 19 Defeated Bikers on Three Island
515 exclude surf,old-rod,good-rod,super-rod//bond-bridge

checkbox 20 Defeated Aroma Lady Violet (Bond Bridge)
515//bond-bridge
495//berry-forest

checkpoint Defeated Rival on Route 22 (2nd battle)
//battle only possible when all badges are collected
cancel 9
cancel 11
329//kanto-route-23
294//kanto-victory-road-1f

checkpoint Obtained Strength HM
318//kanto-victory-road-2f
319//kanto-victory-road-3f

checkpoint Became Champion
progresspoint 5
//rematch league
328 exclude remnum:11,remnum:12,remnum:13//indigo-plateau

checkpoint Obtained National Pokedex
//rematch league
328 exclude remnum:8,remnum:9,remnum:10//indigo-plateau
561//one-island
512//kindle-road
513//treasure-beach
488//mt-ember
489//mt-ember-summit-path-1-3
490//mt-ember-summit-path-2
825//mt-ember-summit
514//cape-brink
515//bond-bridge
495//berry-forest
516//three-isle-port

checkpoint Defeated Rocket Grunts on Mt. Ember
491//mt-ember-ruby-path-1f
492//mt-ember-ruby-path-b1f-b5f
493//mt-ember-ruby-path-b2f-b4f
494//mt-ember-ruby-path-b3f
//clearly traveled to sevii islands
cancel 14
561//one-island
512//kindle-road
513//treasure-beach
//clearly got strength badge and hm in order to go through victory road
cancel 16
488//mt-ember

checkpoint Obtained Rainbow Pass
562//four-island
496//icefall-cave-entrance
497//icefall-cave-1f
498//icefall-cave-b1f
563//five-island
519//five-isle-meadow
520//memorial-pillar
518//water-labyrinth
517//resort-gorgeous
501//lost-cave-main-rooms
511//lost-cave-item-rooms
523//water-path
500//pattern-bush
522//green-path
521//outcast-island
441//altering-cave
525//trainer-tower
526//canyon-entrance
527//sevault-canyon
528//tanoby-ruins
//must have tri-pass to obtain rainbow pass
cancel 18
514//cape-brink

checkbox 21 Obtained ability to use Waterfall
499//icefall-cave-back-cave

checkbox 22 Defeated Hiker Earl (Water Path)
524//ruin-valley

checkbox 23 Solved Tanoby Key puzzle
450//tanoby-chambers

checkbox 27 Delivered the Sapphire to Celio
progresspoint 6

checkpoint Saved Lostelle and Delivered the Sapphire to Celio
cancel 27
progresspoint 6
323//cerulean-cave-1f
324//cerulean-cave-2f
325//cerulean-cave-b1f