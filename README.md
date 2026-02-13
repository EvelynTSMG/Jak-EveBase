> [!NOTE]
> You can read the original README for the OpenGOAL project [here](https://github.com/open-goal/jak-project/blob/master/README.md).\
> You can read the original README for the OpenGOAL Mod Base [here](https://github.com/OpenGOAL-Mods/OG-Mod-Base/blob/main/README.md).

# EveBase
EveBase is an opinionated base template for OpenGOAL mods,
and a fork of the offical [Mod Base](https://github.com/OpenGOAL-Mods/OG-Mod-Base) template.\
EveBase implements various changes and fixes to the vanilla game and the code backend
to make both playing and developing mods more pleasant.

## Known Issues
- The game may crash:
  - in Mountain Pass when trying to unload Rock Village
  - in Volcanic Crater when trying to unload Mountain Pass
  - These crashes do not seem to occur in Debug Mode.
    To restart in Debug Mode, press Alt, then navigate to Debugging > Reboot In Debug Mode!
- Skipping the Fisherman's cell cutscene causes the player to be spawned inside of the bridge.
  The player does *not* become stuck and is free to move around.

## Additions
- You can now press circle while swinging on a pole to change directions!
  (This requires timing the press for when Jak is horizontal, similar to a forward flip.)
- Options can now have descriptions associated with them
- Added **Mod Options**:
  - **Double-Jump Ability** determines when you can double-jump.
    - *Near Peak* behaves like vanilla,
      only allowing you to double-jump near the peak of your jump.
    - *Always* allows you to double-jump at any point in your jump.
  - **Always Max Jumps** can force jumps to always go to their maximum height.
  - **Skip Cell Cutscenes** allows you to collect Power Cells like you would any other collectable,
    without stopping for a victory animation.
  - **Skip Camera Pans** removes any short camera pans from the game.\
    (Examples: lightning moles jumping into their hole, jungle mirrors being connected.)
  - **Quick-Mount Vehicles** allows you to simply jump onto the zoomer or Flut-Flut to mount them.
- Added a **Warp To Checkpoint** button to the pause menu,
  which warps you to your last checkpoint.
- Added an **Exit Level** button to the pause menu,
  which warps you to the start of the level.
  - Attempting to exit Fire Canyon, Mountain Pass, or Lave Tube will warp you
    to either the beginning or the end of each level, depending on which is closer.
    It will always choose the beginning if the level has not been completed.

## Changes
- Shooting yellow eco in first person now allows **R2** to be held to shoot,
  instead of requiring separate button presses for each shot.
- Revamped first-person camera speed calculations to improve handling.

## Fixes
- Jak now properly uses the crouching hitbox when rolling and rolljumping.
- You can no longer fail the race in Mountain Pass by being too fast.

## Development
EveBase comes with many comments to guide you on your modding journey.\
Search for one of the terms below to begin!
- `[ADD-MOD-OPTION]` for adding settings to the Mod Options screen
  - `[ADD-MOD-OPTION-CAROUSEL]` for adding carousels
- `[ADD-EASTER-EGG]` for adding occasionally/seasonally occuring easter eggs
- `[ADD-FISH-TYPE]` for adding new fish to the fishing minigame
- `[ADD-FONT-COLOR]` for adding new font colors for text rendering
