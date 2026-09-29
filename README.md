# TP Classic Modern Controller UI

**Modern controller prompts, reimagined in the style of Twilight Princess.**

TP Classic Modern Controller UI modernizes the controller interface of **The Legend of Zelda: Twilight Princess** by combining the original game's visual language with custom UI elements designed for modern controllers.

The mod does not aim to reproduce the original HUD exactly. It keeps the recognizable Twilight Princess aesthetic while introducing its own visual identity where appropriate, including custom ornamentation, redesigned controller prompts, and additional presentation options.

## Features

- PlayStation and Xbox controller mappings.
- Modern controller prompts across the HUD, menus, dialogues, maps, Item Wheel, shops, skills and save screens.
- Custom Twilight Princess-inspired ornamentation and UI elements.
- Animated analog-stick prompts using the game's native animation system.
- Configurable positioning, scaling, glow and animation behavior for multiple UI elements.
- D-Pad customization and related ITEM / MAP presentation controls.

## Controller Mapping

| GameCube | PlayStation | Xbox |
| --- | --- | --- |
| A | Cross | A |
| B | Circle | B |
| X | Triangle | Y |
| Y | Square | X |
| Z | R1 | RB |
| R | R2 | RT |
| L | L2 | LT |
| Start | Options | Menu |

The mapping follows the **function of the original GameCube controls**, rather than matching button letters between controllers.

## Texture Replacement Compatibility

TP Classic Modern Controller UI is compatible with texture replacement packs. However, button textures, HUD elements, and interface modifications handled directly by this mod take priority over external texture replacements. Other game textures can still be replaced normally.

## Platform Support

This repository uses the official Dusklight native mod template workflow. CI builds the mod for all platforms supported by that template and merges them into one distributable `.dusk` bundle:

- Windows AMD64 and ARM64
- Linux x86_64 and ARM64
- macOS Apple Silicon and Intel
- iOS ARM64
- Android ARM64

Local builds are platform-specific and are intended for testing only. For distribution, use the **`mod-combined`** artifact produced by GitHub Actions.

## Building

A local test build can be created with:

```sh
cmake -B build
cmake --build build
```

The local bundle is written to `build/mods/tp_classic_modern_controller_ui.dusk`.

For a distributable build, push the repository to GitHub and let the included GitHub Actions workflow build every supported platform and create the combined bundle.

## Credits

**Created by XandasLegend.**

Special thanks to the developers and contributors of **Dusklight** and the wider Twilight Princess modding community.

## AI Disclaimer

AI tools were used during parts of development to assist with code analysis, debugging, documentation, naming, and workflow support. Final design decisions, testing, integration, asset selection, configuration, and release decisions were reviewed and carried out by the project author.

AI-assisted output does not represent Nintendo, the Dusklight developers, or any other third party.

## License

This project is licensed under the **MIT License**. See [LICENSE](LICENSE).
