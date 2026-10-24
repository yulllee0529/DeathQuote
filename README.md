# Random Everything (prototype)

A Geode mod prototype targeting Geometry Dash 2.2081 on Android64 and Windows.

## Current behavior
- Intercepts the level's object setup data before the game creates the objects.
- Randomizes object IDs within a small pool of basic block IDs (1–7), so eligible blocks can become different block types.
- Applies at runtime only; it does not rewrite or save the original level data.

## Not implemented yet
- Randomizing every object category.
- Converting blocks to triggers or triggers to blocks.
- Randomizing trigger settings, group IDs, portals, orbs, pads, and other special objects.

The current implementation is intentionally conservative because replacing special objects without matching their required data can break gameplay or crash the game. The Android 2.2081 build still needs to be compiled and tested in-game before it can be considered working.
