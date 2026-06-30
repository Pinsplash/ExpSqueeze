Ideally, we'd repeat the FRLG process for DP, but pokediamond isn't decompiled quite as much as we need to do that (and the architecture is of course going to be a bit different) so we need to source some data from other places.

After much struggle, I managed to get the rematch table from https://github.com/pret/pokediamond/blob/038cccaed5de8f013875bc5d734f912d1de08e0f/arm9/overlays/05/asm/ov05_021E1374.s#L744 This took a lot of effort with texttool.

We seem to be lacking data about which teams are where. This should be easily sourceable from Bulbapedia, but it does mean we have to match up trainers by their UI name. Since we would have the data about the pokemon right there, it seems logical to take it from the same files, but we'd rather interact with wikitext and its many edge cases as little as possible.

Data about teams is here: https://github.com/pret/pokediamond/blob/master/files/poketool/trainer/trdata.json
