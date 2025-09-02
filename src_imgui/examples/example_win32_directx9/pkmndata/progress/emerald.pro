//game progress description file. see gen1.pro for explanations of most features.

checkpoint Met Professor Birch
unlock walk
393//hoenn-route-101
395//hoenn-route-103

checkpoint Obtained Running Shoes
394//hoenn-route-102

//a guy forces player to go to gym before leaving westward
checkpoint Completed catching tutorial
//petalburg actually accessible after defeating Youngster Calvin on Route 102, but no old rod yet, so the checkpoint would be useless clutter in the UI
350//petalburg-city
396//hoenn-route-104
365//petalburg-woods

//twins don't "notice" you in r/s
checkpoint Defeated Twins Gina & Mia (Route 104 North) or avoid by having 1 Pokemon in party
//no wild pokemon in rustboro city
407 exclude walk,rematch//hoenn-route-115
//karen is in grass now
408 exclude remnum:2//hoenn-route-116

checkpoint Defeated Hiker Devan (Route 116)
360//rusturf-tunnel

checkbox 3 Obtained ability to use Cut
408//hoenn-route-116

checkpoint Obtained Mr. Stone's letter
433//dewford-town
unlock old-rod
399 exclude rematch//hoenn-route-107
398//hoenn-route-106
361//granite-cave-1f
362//granite-cave-b1f
363//granite-cave-b2f
364//granite-cave-1fsmall-room

checkpoint Delivered letter to Steven
401//hoenn-route-109
351//slateport-city
402//hoenn-route-110

checkpoint Defeated Psychic Edward and Triathlete Alyssa (Route 110)
//no wild pokemon in mauville city
409//hoenn-route-117
//no wild pokemon in verdanturf town
403 exclude walk//hoenn-route-111

checkbox 5 Defeated Aroma Lady Rose and Youngster Deandre (Route 118)
410 exclude walk//hoenn-route-118

checkpoint Obtained ability to use Rock Smash
unlock rock-smash
404//hoenn-route-112
367//fiery-path
//cooltrainer brooke is avoidable in emerald
405//hoenn-route-113
//no wild pokemon in fallarbor town
406//hoenn-route-114

//picknicker angelina is avoidable, hiker is not
checkpoint Defeated Hiker Lucas
356//meteor-falls-1f

checkpoint Defeated Team Magma at Mt. Chimney
417//mt-chimney
366//jagged-pass
//no wild pokemon in lavaridge town

checkpoint Obtained Go-Goggles
403//hoenn-route-111

checkpoint Obtained Balance Badge
unlock rematch
//remnum means exclude the specified trainer (indices based on order in file in trainers/ folder)
//expert timothy only reachable by surf, but two others are rematchable now
407 exclude walk,remnum:4//hoenn-route-115

//hm given by wally's dad after getting petalburg badge
checkpoint Obtained Surf HM
unlock surf
387//new-mauville-entrance
397//hoenn-route-105
400//hoenn-route-108
389//abandoned-ship
407//hoenn-route-115
unlock good-rod
411//hoenn-route-119
428//hoenn-route-134
//excluded rematch before; have to surf to reach swimmer tony
399//hoenn-route-107
cancel 5

checkbox 6 Defeated Aroma Lady Rose and Youngster Deandre (Route 118)
410//hoenn-route-118

checkbox 1 Obtained Basement Key
388//new-mauville-interior

checkpoint Defeated Rival on Route 119
//no wild pokemon in fortree city
412 exclude surf,old-rod,good-rod,super-rod,remnum:3//hoenn-route-120
cancel 6
410//hoenn-route-118

checkpoint Obtained Devon Scope
412//hoenn-route-120
//no wild pokemon in scorched slab
//double double battles can be avoided by grass path
413//hoenn-route-121
368//mt-pyre-1f
369//mt-pyre-2f
374//mt-pyre-outside
375//mt-pyre-summit
414//hoenn-route-122
415 exclude remnum:4//hoenn-route-123
cancel 3

checkbox 4 Obtained ability to use Cut
408//hoenn-route-116
415//hoenn-route-123

checkbox 7 Defeated Black Belt Zander (Mt. Pyre)
370//mt-pyre-3f
371//mt-pyre-4f
372//mt-pyre-5f
373//mt-pyre-6f

checkpoint Defeated Cooltrainer Cristin (Route 121)
352//lilycove-city

checkpoint Defeated Matt at Aqua Hideout
416 exclude walk,rematch//hoenn-route-124

checkpoint Defeated Swimmers Grace and Declan (Route 124)
416 exclude walk//hoenn-route-124
418//hoenn-route-125
353//mossdeep-city
385//shoal-cave
386//shoal-cave-b1f
unlock super-rod
419 exclude walk//hoenn-route-126
421//hoenn-route-127
422//hoenn-route-128
423//hoenn-route-129
424//hoenn-route-130
425//hoenn-route-131
434//pacifidlog-town
426//hoenn-route-132
427//hoenn-route-133
355 exclude rematch//ever-grande-city

checkpoint Obtained ability to use Dive
416//hoenn-route-124
419//hoenn-route-126
354//sootopolis-city
376//seafloor-cavern

checkpoint Defeated Archie at Seafloor Cavern
377//cave-of-origin-entrance
378//cave-of-origin-1f
379//cave-of-origin-b1f
380//cave-of-origin-b2f
381//cave-of-origin-b3f

checkpoint Met Wallace in Cave of Origin
390//sky-pillar-1f
391//sky-pillar-3f
392//sky-pillar-5f

checkpoint Awakened Rayquaza
cancel 377//cave-of-origin-entrance
cancel 378//cave-of-origin-1f
cancel 379//cave-of-origin-b1f
cancel 380//cave-of-origin-b2f
cancel 381//cave-of-origin-b3f

checkpoint Obtained Rain Badge
382 exclude rematch//hoenn-victory-road-1f
//you can just walk by the old couple
357//meteor-falls-1f-back
358 exclude surf,old-rod,good-rod,super-rod//meteor-falls-b1f

checkbox 2 Defeated Dragon Tamer Nicolas (Meteor Falls)
358//meteor-falls-b1f
359//meteor-falls-b1f-back

checkpoint Defeated Cooltrainer Hope (Victory Road)
383//hoenn-victory-road-b1f

checkpoint Defeated Cooltrainer Samuel or Shannon (Victory Road)
384//hoenn-victory-road-b2f

checkpoint Became Champion
//wally rematch is available now
382//hoenn-victory-road-1f
//league rematch
355//ever-grande-city