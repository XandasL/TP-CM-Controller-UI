# Changelog

## v1.3.2

- Fixed Linux and Android crashes caused by game-structure ABI offsets being compiled into the mod.
- Updated the pinned Dusklight SDK so game-state accessors such as `dComIfGp_getPlayer()` use the exported game ABI instead of direct structure offsets.
- Public CI builds now use `Release` explicitly on every platform.
- Added CI checks to reject Linux/Android release libraries that contain `.debug_info` or inline the player lookup instead of importing the game ABI function.
- No controller layout or calibration changes are included in this release.


## v1.0.0 - Initial Release

- Initial release of **TP Classic Modern Controller UI**.
- Added PlayStation and Xbox controller prompts across the HUD and menus.
- Uses the original GameCube controls as the reference mapping for modern controller layouts.
- Added custom Twilight Princess-inspired UI elements, animations and controller customization options.
- Improved prompt scaling, positioning and visual consistency throughout the interface.
- Added official multi-platform build workflow using the Dusklight mod template.
