//game progress description file. see gen1.pro for explanations of most features.
checkpoint Picked starter
unlock walk
184//new-bark-town
185//johto-route-29
252//johto-route-46
186//cherrygrove-city
187//johto-route-30

checkpoint Defeated Youngster Mikey
188//johto-route-31
253//dark-cave-violet-city-entrance
189//violet-city
190//sprout-tower-2f

checkbox 1 Defeated Sage Edmond (Sprout Tower)
191//sprout-tower-3f

checkbox 2 Solved any puzzle in Ruins of Alph
194//ruins-of-alph-interior

checkpoint Met aide after defeating Falkner
192//johto-route-32

checkpoint Defeated Youngster Albert (Route 32)
unlock old-rod
198//union-cave-1f

checkpoint Defeated Hiker Russell (Union Cave)
199//union-cave-b1f

checkpoint Defeated Firebreather Ray (Union Cave)
201//johto-route-33
798//azalea-town
204 exclude old-rod//ilex-forest

checkpoint Met Kurt
202 exclude surf,old-rod,good-rod,super-rod//slowpoke-well-1f

checkbox 3 Trained a Slowpoke to level 34 to learn Headbutt
unlock headbutt-low
unlock headbutt-high

checkpoint Defeated Team Rocket at Slowpoke Well
remove 204//ilex-forest

//rival always comes before Cut HM
checkpoint Defeated Rival in Azalea Town
204 exclude old-rod//ilex-forest

//if you did rival first, this means you beat bugsy
//if you did bugsy first, this means you beat rival
checkpoint Obtained ability to use Cut
204//ilex-forest
cancel 3
unlock headbutt-low
unlock headbutt-high
205//johto-route-34
206 exclude walk//johto-route-35

checkbox 4 Defeated Camper Ivan or Picnicker Brooke (Route 35)
206//johto-route-35
207//national-park

checkbox 5 Defeated Bug Catcher Arnie (Route 35) or Psychic Mark (Route 36)
209//johto-route-36
//if approaching from violet city, still have to beat one of those two to reach national park
207//national-park

checkpoint Battled the Sudowoodo
206//johto-route-35
209//johto-route-36
unlock rock-smash
210//johto-route-37

checkpoint Defeated Psychic Greg (Route 37)
211//ecruteak-city
236//johto-route-42
//water happens to become accessible at same time as surf unlock
237//mt-mortar-1f-entrance
238//mt-mortar-1f-back
800//mahogany-town
241//johto-route-43
222//johto-route-38
223//johto-route-39
224//olivine-city
348//olivine-city-port
unlock good-rod
225//johto-sea-route-40

checkbox 6 Defeated Pokemaniac Ron (Route 43)
242//lake-of-rage

checkbox 7 Defeated Rival in Burned Tower
212//burned-tower-1f
213//burned-tower-b1f

checkbox 9 Obtained ability to use Strength
//strength hm is just given by a rando in olivine cafe
202//slowpoke-well-1f
203//slowpoke-well-b1f

checkpoint Obtained ability to use Surf
unlock surf
240//mt-mortar-b1f
226//johto-sea-route-41
235//cianwood-city
288 exclude walk//kanto-route-27
316//tohjo-falls
254 exclude old-rod,good-rod,super-rod,surf//dark-cave-blackthorn-city-entrance
193 exclude walk//ruins-of-alph-outside
200//union-cave-b2f

checkbox 10 Defeated Psychic Nathan (Ruins of Alph)
193//ruins-of-alph-outside

checkbox 8 Obtained Glacier Badge
//scripted to get whirlpool hm in team rocket hq
227//whirl-islands-1f
228//whirl-islands-b1f
230//whirl-islands-b2f
232//whirl-islands-b3f

checkpoint Obtained 7 Badges
//you have to beat him to go to lake of rage to access pryce's gym
cancel 6
242//lake-of-rage
cancel 8
227//whirl-islands-1f
228//whirl-islands-b1f
230//whirl-islands-b2f
232//whirl-islands-b3f
//you have to use strength to navigate chuck's gym
cancel 9
202//slowpoke-well-1f
203//slowpoke-well-b1f
243//johto-route-44
244//ice-path-1f
245//ice-path-b1f
246//ice-path-b2f
247//ice-path-b3f
249//blackthorn-city
251//johto-route-45 (water can be reached while avoiding all trainers by taking a certain winding path)
254//dark-cave-blackthorn-city-entrance

checkpoint Defeated Team Rocket at Radio Tower
214//bell-tower-2f
215//bell-tower-3f
216//bell-tower-4f
217//bell-tower-5f
218//bell-tower-6f
219//bell-tower-7f
220//bell-tower-8f
221//bell-tower-9f
221//bell-tower-9f
268//bell-tower-10f
793//bell-tower-roof

checkpoint Defeated Clair
250//dragons-den

checkpoint Obtained ability to use Waterfall
239//mt-mortar-2f
288//kanto-route-27

checkpoint Defeated Cooltrainer Blake (Route 27)
287//kanto-route-26

checkpoint Defeated Cooltrainer Beth (Route 26)
294//kanto-victory-road-1f
318//kanto-victory-road-2f
319//kanto-victory-road-3f

checkpoint Became Champion
//rematch league
328//indigo-plateau

checkpoint Deboarded the S.S. Aqua
349//vermilion-city-ss-anne-dock
282//vermilion-city
300//kanto-route-6
//no wild pokemon in saffron city
//our old friend, the east and west approach...
//west approach
301//kanto-route-7
283//celadon-city
309//kanto-route-16
//east approach
302//kanto-route-8
//no wild pokemon in lavender town
276//kanto-route-12
unlock super-rod
306 exclude walk//kanto-route-13
//north
299//kanto-route-5
281//cerulean-city
346//cerulean-gym
298//kanto-route-4
314//kanto-route-24
315 exclude walk//kanto-route-25

checkbox 11 Obtained Bicycle
//west approach
310//kanto-route-17
311//kanto-route-18
//nothing new for east (route 16 already reachable from celadon)

checkbox 12 Defeated Bird Keeper Bob (Route 18)
//west approach
284//fuchsia-city
308//kanto-route-15
//east approach
311//kanto-route-18

checkbox 13 Defeated Pokefan Trevor (Route 14)
//either direction
307//kanto-route-14

checkbox 14 Defeated Hiker Kenny (Route 13)
//west approach
306 exclude surf,old-rod,good-rod,super-rod//kanto-route-13
//east approach
308//kanto-route-15
284//fuchsia-city

//bird keepers on route 13 cannot be manipulated to allow passage
checkbox 15 Defeated Pokefan Joshua (Route 13)
//from west side, you had grass already and now you have water
//from east side, you had water already and now you have grass
306//kanto-route-13

//relevant until woke snorlax
checkbox 16 Defeated Psychic Herman (Route 11)
305//kanto-route-11

checkbox 17 Defeated Camper Lloyd (Route 25)
315//kanto-route-25

//northeast kanto now has a similar situation to the mid-south
//go east from cerulean to reach lavender, or
//go north from lavender to reach cerulean

//east approach
checkbox 18 Defeated Camper Sid or Camper Dean (Route 9)
303 exclude surf,old-rod,good-rod,super-rod//kanto-route-9

checkbox 19 Defeated Picnicker Heidi (Route 9)
303//kanto-route-9
304//kanto-route-10
//power plant doesn't have wild pokemon. grass is part of route 10.
292//rock-tunnel-1f
293//rock-tunnel-b1f

//north approach
//a table can have excludes added to it even after another milestone added the table with no exclude
//this means if Defeated Hiker Jim (20) came before Defeated Camper Sid or Camper Dean (18), and both are checked, 303 would incorrectly exclude water
//therefore this checkbox is deliberately placed after 18
checkbox 20 Defeated Hiker Jim (Route 10)
292//rock-tunnel-1f
293//rock-tunnel-b1f
304//kanto-route-10
//power plant doesn't have wild pokemon. grass is part of route 10.
303//kanto-route-9

checkpoint Woke Snorlax
cancel 16
305//kanto-route-11
317//digletts-cave
296//kanto-route-2-south-towards-viridian-city
320//kanto-route-2-north-towards-pewter-city
//no wild pokemon in pewter city (not even headbutt trees)
280//viridian-city
313//kanto-route-22
295//kanto-route-1
285//pallet-town
312//kanto-sea-route-21
279//cinnabar-island
278//kanto-sea-route-20
277//kanto-sea-route-19
284//fuchsia-city
308//kanto-route-15

checkbox 22 Defeated Youngster Jimmy (Route 3)
297//kanto-route-3

checkbox 21 Defeated Rival in Mt. Moon
290//mt-moon-1f

checkpoint Obtained All Badges
289//kanto-route-28
269//mt-silver-outside-before-cave
270//mt-silver-1f
263//mt-silver-2f
274//mt-silver-item-rooms
273//mt-silver-top