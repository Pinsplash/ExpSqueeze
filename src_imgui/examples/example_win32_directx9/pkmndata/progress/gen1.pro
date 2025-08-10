//game progress description file
//this file details which encounter tables are available at what point in the game

//checkpoints go in the main list of milestones. if the user selects a milestone, every table above it is allowed, and so is every table below until the next checkpoint.

//after the first rival battle in oak's lab
checkpoint Battled Rival for the first time

//we start off being able to find pokemon in grass and dungeons
//this allows "walk" encounters
unlock walk

//these places are reachable at the start of the game
285//pallet-town (note that pallet town's only wild pokemon are in the water to the south. those tables are not actually allowed until we unlock the rods.)
295//kanto-route-1
280//viridian-city
313//kanto-route-22

checkpoint Delivered Oak's Parcel
296//kanto-route-2-south-towards-viridian-city
321//viridian-forest

checkpoint Defeated Viridian Forest Trainers
320//kanto-route-2-north-towards-pewter-city
//no wild pokemon in pewter city

checkpoint Defeated 3rd Bug Catcher on Route 3
297//kanto-route-3
290//mt-moon-1f
326//mt-moon-b1f
327//mt-moon-b2f

//a one-way is a checkpoint which is a point of no return. in this case, once you cross over the ledge at the end of route 4, you can't go back to mt moon or anywhere before then until you can reach back to route 2 via diglett cave AND cut a tree.
//1st number: identifier for this one-way so that later we can cancel it when the conditions mentioned above are met.
oneway 1 Crossed over final ledge on Route 4
298//kanto-route-4
281//cerulean-city

checkpoint Defeated all of Nugget Bridge
314//kanto-route-24
315//kanto-route-25

//after visiting bill, officer jenny arbitrarily decides to stop standing in the way
checkpoint Defeated TM thief
299//kanto-route-5
300//kanto-route-6

checkpoint Defeated last Jr. Trainer M on Route 6
282//vermilion-city
unlock old-rod
349//vermilion-city-ss-anne-dock
305//kanto-route-11
317//digletts-cave

//a checkbox is a milestone that is not in the main list, but rather shows up as a checkbox. checkboxes only appear if the selected checkpoint is the one directly above them, or any checkpoint lower down.
//also uses an identifier number like one-ways do

checkbox 2 S.S. Anne departed
//remove means... you can guess i think
remove 349//vermilion-city-ss-anne-dock

checkbox 6 Obtained ability to use Cut
//this cancels the oneway defined earlier with the id 1
//if you get the cascade badge first, you technically have the ability to use Cut immediately after getting the HM, so don't remove the dock
cancel 1

checkbox 5 Defeated 1st Bug Catcher on Route 9
//we clearly have cut by now, so we can go back to places before the route 4 one-way
cancel 1
remove 349//vermilion-city-ss-anne-dock
303//kanto-route-9

checkpoint Defeated 2nd Bug Catcher on Route 9
//we clearly have cut by now, so we can go back to places before the route 4 one-way
cancel 1
//1st bug catcher is irrelevant now because there's another patch of grass
cancel 5
//we clearly have cut by now
cancel 6
//ss anne is clearly gone by now
cancel 2
remove 349//vermilion-city-ss-anne-dock
303//kanto-route-9
304//kanto-route-10
292//rock-tunnel-1f

checkpoint Defeated 1st Pokemaniac in Rock Tunnel
293//rock-tunnel-b1f

checkpoint Defeated last Jr. Trainer F in Rock Tunnel
//no wild pokemon in lavender town or first two floors of pokemon tower
//here we exclude the walk table, because the grass isn't accessible until later
276 exclude walk//kanto-route-12

checkbox 10 Defeated 1st Gambler or Lass on Route 8
//east approach
302//kanto-route-8 (no grass until after this checkpoint)
301//kanto-route-7
283//celadon-city
309//kanto-route-16
//doing this from west grants us nothing new

checkbox 9 Obtained Rainbow Badge
329 exclude walk//kanto-route-23

checkpoint Defeated Rival in Pokemon Tower
336//pokemon-tower-3f
337//pokemon-tower-4f

checkpoint Defeated 1st Channeler on Pokemon Tower 4F
338//pokemon-tower-5f
339//pokemon-tower-6f

checkpoint Defeated Marowak
340//pokemon-tower-7f

//this checkpoint is only to facilitate showing checkboxes at the right time
checkpoint Woke Snorlax
//after you get the poke flute, you have two paths to fuchsia:
//route 12 to 15 in the east, or
//the cycling road in the west

checkbox 7 Defeated Rocker on Route 12
unlock super-rod

checkbox 8 Defeated Jr. Trainer M on Route 12
//east approach
//now we can reach the grass on this route
276//kanto-route-12
306 exclude walk//kanto-route-13
//west approach
unlock super-rod

checkbox 3 Defeated Jr. Trainer F on Route 13
//east approach
306//kanto-route-13
307//kanto-route-14
308//kanto-route-15
311//kanto-route-18
//west approach
276//kanto-route-12
//either way once you hit fuchsia
284//fuchsia-city
unlock good-rod
277//kanto-sea-route-19

//critical to remember this is "obtained bike AND woke snorlax"
checkbox 4 Obtained Bicycle
//either way
310//kanto-route-17
//west approach
311//kanto-route-18
308//kanto-route-15
307//kanto-route-14
306//kanto-route-13
284//fuchsia-city
unlock good-rod
277//kanto-sea-route-19
//east approach
309//kanto-route-16
283//celadon-city
301//kanto-route-7
302//kanto-route-8

checkpoint Obtained ability to use Surf
//cancel several boxes because this checkpoint means we reached Fuchsia and all of those places are now reachable while avoiding any battles
//don't cancel #4 because routes 16 and 17 remain unreachable until you get a bicycle (which is never required)
cancel 3
cancel 7
cancel 8
//put their contents here too
276//kanto-route-12
unlock super-rod
306//kanto-route-13
307//kanto-route-14
308//kanto-route-15
311//kanto-route-18
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

checkbox 11 Obtained Marsh Badge and haven't defeated Giovanni
329//kanto-route-23

checkpoint Defeated Rival on Route 22 (2nd battle)
//battle only possible when all badges are collected
cancel 9
cancel 11
329//kanto-route-23
331//kanto-victory-road-2-1f

checkpoint Obtained Strength HM
332//kanto-victory-road-2-2f
333//kanto-victory-road-2-3f

checkpoint Defeated last trainer in Victory Road
345//loreleis-room

checkpoint Became Champion
323//cerulean-cave-1f
324//cerulean-cave-2f
325//cerulean-cave-b1f