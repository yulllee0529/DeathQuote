# Random Everything (prototype)

A Geode mod prototype for Geometry Dash. This branch targets Geometry Dash 2.208 on Android64 and Windows.

## Current prototype behavior
- Rerolls common visual properties when a PlayLayer is initialized.
- Randomizes rotation, scale, opacity, and visibility of loaded objects.
- Uses a new random sequence on each level load/restart.

## Not implemented yet
- Converting normal blocks into triggers and triggers into normal objects.
- Replacing object IDs/types (blocks, orbs, portals, Start Pos, triggers, etc.).
- Randomizing group IDs and target group IDs.
- Randomizing all type-specific trigger/object settings.

Those require transforming the serialized level object data before the game creates the objects. This prototype intentionally does not claim those features are complete.
