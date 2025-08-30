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
//sage edmond is avoidable in crystal
191//sprout-tower-3f
//grass on east side in crystal
209//johto-route-36

checkbox 2 Solved any puzzle in Ruins of Alph
194//ruins-of-alph-interior

checkpoint Met aide after defeating Falkner
192//johto-route-32
//youngster albert is avoidable in crystal
unlock old-rod
198//union-cave-1f
//hiker russel is avoidable in crystal
199//union-cave-b1f
//firebreather ray is avoidable in crystal
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
//camper ivan and picnicker brooke are avoidable in crystal
206//johto-route-35
207//national-park

checkpoint Battled the Sudowoodo
unlock rock-smash
210//johto-route-37
//psychic greg is avoidable in crystal
211//ecruteak-city
212//burned-tower-1f
236//johto-route-42
//water happens to become accessible at same time as surf unlock
237//mt-mortar-1f-entrance
238//mt-mortar-1f-back
800//mahogany-town
241//johto-route-43
222//johto-route-38
//pokemaniac ron lets you walk by in crystal
242//lake-of-rage

checkbox 1 Defeated Pokefan Derek or Pokefan Ruth (Route 39)
223//johto-route-39
224//olivine-city
348//olivine-city-port
unlock good-rod
225//johto-sea-route-40

checkbox 7 Defeated Rival in Burned Tower
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

checkbox 4 Captured Raikou, Entei and Suicune
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

checkpoint Defeated Cooltrainer Darin (Dragon's Den)
250//dragons-den

checkpoint Obtained ability to use Waterfall
239//mt-mortar-2f
288//kanto-route-27
//cooltrainer blake is avoidable in crystal
287//kanto-route-26

checkpoint Defeated Cooltrainer Beth (Route 26)
294//kanto-victory-road-1f
318//kanto-victory-road-2f
319//kanto-victory-road-3f

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
//psychic herman is avoidable in crystal
305//kanto-route-11
unlock super-rod
306 exclude walk//kanto-route-13
//north
299//kanto-route-5
281//cerulean-city
346//cerulean-gym
298//kanto-route-4
314//kanto-route-24
315 exclude walk//kanto-route-25
//camper sid is avoidable in crystal
303 exclude surf,old-rod,good-rod,super-rod//kanto-route-9

checkbox 11 Obtained Bicycle
//west approach
310//kanto-route-17
311//kanto-route-18
//nothing new for east (route 16 already reachable from celadon)

checkbox 12 Defeated Bird Keeper Bob (Route 18)
//west approach
284//fuchsia-city
308//kanto-route-15
//pokefan trevor is avoidable in crystal
307//kanto-route-14
//east approach
311//kanto-route-18

checkbox 14 Defeated Hiker Kenny (Route 13)
//west approach
306 exclude surf,old-rod,good-rod,super-rod//kanto-route-13
//east approach
308//kanto-route-15
284//fuchsia-city
//pokefan trevor is avoidable in crystal
307//kanto-route-14

//bird keepers on route 13 cannot be manipulated to allow passage
checkbox 15 Defeated Pokefan Joshua (Route 13)
//from west side, you had grass already and now you have water
//from east side, you had water already and now you have grass
306//kanto-route-13

checkbox 17 Defeated Camper Lloyd (Route 25)
315//kanto-route-25

//northeast kanto now has a similar situation to the mid-south
//go east from cerulean to reach lavender, or
//go north from lavender to reach cerulean

//east approach
checkbox 18 Defeated Camper Dean (Route 9)
303//kanto-route-9
//picnicker heidi is avoidable in crystal
304//kanto-route-10
//power plant doesn't have wild pokemon. grass is part of route 10.
292//rock-tunnel-1f
293//rock-tunnel-b1f

//north approach
checkbox 20 Defeated Hiker Jim (Route 10)
292//rock-tunnel-1f
293//rock-tunnel-b1f
304//kanto-route-10
//power plant doesn't have wild pokemon. grass is part of route 10.
303//kanto-route-9

checkpoint Woke Snorlax
cancel 16
317//digletts-cave
296//kanto-route-2-south-towards-viridian-city
320//kanto-route-2-north-towards-pewter-city
//no wild pokemon in pewter city (not even headbutt trees)
//youngster jimmy is avoidable in crystal
297//kanto-route-3
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
//pokefan trevor is avoidable in crystal
307//kanto-route-14

checkbox 21 Defeated Rival in Mt. Moon
290//mt-moon-1f

checkpoint Obtained All Badges
289//kanto-route-28
269//mt-silver-outside-before-cave
270//mt-silver-1f
263//mt-silver-2f
274//mt-silver-item-rooms
273//mt-silver-top