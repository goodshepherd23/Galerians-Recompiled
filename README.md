# Galerians Recompiled

A native PC build of **Galerians** (PlayStation, 2000), made with the
[PSXRecomp](https://github.com/mstan/psxrecomp) static recompiler and the shared
[recomp-ui](https://github.com/mstan/recomp-ui) launcher.

You must supply your own **Galerians (USA)** discs as CUE/BIN dumps:

| Disc | Serial | Redump | Data track 01 MD5 |
|---|---|---|---|
| Disc 1 | SLUS-00986 | [5320](http://redump.org/disc/5320/) | `217885a22fbb0f3092f8405ce42419ea` |
| Disc 2 | SLUS-01098 | [5321](http://redump.org/disc/5321/) | `5bc6de661cb074cd5218ac0b9daecda2` |
| Disc 3 | SLUS-01099 | [5322](http://redump.org/disc/5322/) | `992e16e5164febd23c8242ede1fc0d8c` |

This repository and its release packages contain **no game disc, no BIOS dump, no
generated game code and no saves**. The game code is generated on your machine
from your Disc 1.

No retail BIOS is needed: the build uses the bundled
[OpenBIOS](https://github.com/grumpycoders/pcsx-redux) image.

## Setup

1. Download the release ZIP for your platform and extract all of it into a
   writable folder.
2. Start `Galerians_Recompiled` (`.exe` on Windows).
3. Select your `Galerians (USA) (Disc 1).cue` in the setup wizard. Keep each CUE
   next to its BIN, and keep all three discs in the same folder.
4. Run **Generate & rebuild** and wait. The game starts when it finishes.

On Windows the setup wizard can download portable build tools. On Linux and macOS,
install CMake, Ninja, Python 3 and a C/C++ compiler first.

## Three discs, one program

All three discs boot the same executable (SHA-256 `46987c4b…`), so one build
plays the whole game. The launcher has a **Disc Selection** list for starting
from a later disc. When the game asks for the next disc, open the in-game menu
and use **Change disc** to pick that disc's `.cue`.

## Controllers

Player 1 starts on the keyboard. To use a gamepad, pick it for Player 1 on the
launcher's **Controls** page, or set `p1_device = "auto"` under `[controller]`
in `settings.toml` next to the executable to use the first connected pad. The
game uses a digital pad; the left stick also works as the D-pad.

## Building from source

```bash
git clone --recursive https://github.com/goodshepherd23/Galerians-Recompiled
cd Galerians-Recompiled
cp bios/openbios.bin psxrecomp/bios/openbios.bin
for p in patches/*.patch; do git -C psxrecomp apply "../$p"; done
python psxrecomp/psxrecomp_cli.py generate --config game.toml --project-root . --disc "/path/to/Galerians (USA) (Disc 1).cue" --force-emitters
python psxrecomp/psxrecomp_cli.py rebuild --config game.toml --project-root . --build-dir build-release --target psx-runtime --exe-basename Galerians_Recompiled --no-pgo
```

The `cp` step is needed because the pinned framework revision does not include
the OpenBIOS image; `bios/openbios.bin` here is the MIT-licensed build pinned in
`psxrecomp/bios/OpenBIOS.toml` (SHA-256 `fabe498f…1c57`), with its notice in
`bios/OpenBIOS.LICENSE`.

The patches fix three things in the pinned framework:

- `0002` makes the launcher's **PGO optimize** work on Windows. Without it, the
  training run fails with `[WinError 5] Access is denied` and writes no profile.
  `CMakeLists.txt` also wires up `PSX_PGO`, which this framework revision ignores.
- `0003` removes thin vertical lines on tiled 2D screens when **Texture
  filtering** is set to Bilinear.
- `0004` fixes the opening-logo transitions on the OpenGL renderer (added in
  0.1.1). Galerians stays in 24-bit display mode from the Polygon Magic logo
  into the intro movies and clears each frame with a fill. The renderer
  presents 24-bit frames from its CPU copy of VRAM, but fills only reached the
  GPU copy, so the movies' letterbox bars showed the logo's gray background
  and its fade-out flickered. Fills now reach both copies while 24-bit mode is
  on. The software renderer was already correct.

`--force-emitters` is required. Without it, `generate` can pick up stale prebuilt
emitters that leave the build with no BIOS backend.

`game.toml` sets `discovery = "reachable"`. With the framework's default
whole-image sweep, about 60 blocks of data in this executable are read as
256 KB functions and `generate` writes 34 million lines of C. Reachable
discovery writes about 0.8 million; code it does not reach runs in the
interpreter instead.

## Status

Version 0.1.1 is an early release, tested on Windows x64 (Intel Iris Xe) with
the Redump dumps above.

- The disc check passes, and the game generates and builds
- Boots on OpenBIOS through the Crave Entertainment FMV to the title screen
- The attract-mode FMVs and in-engine scenes render correctly
- The opening logos (Crave, ASCII Soft, Polygon Magic) and the intro movie
  fade and letterbox cleanly on OpenGL as of 0.1.1, checked frame by frame
  through the debug server
- The Windows release package, built from a fresh extract, reaches the
  Load Game screen and early gameplay (Rion in the lab with the HP/AP/Nalcon
  display)

**Not tested yet:** play beyond the first room, saving and loading, changing
discs mid-game, a full playthrough, PGO optimize, and the Linux and macOS
packages. Please open an issue if you hit a crash, a hang or a graphical bug,
and include the `psx_last_run_report.json` written next to the executable.

## Cheats

The launcher's **Mods** page has a **Cheats** group (added in 0.1.1). Tick what
you want, then press PLAY. All are off by default, and they work on all three
discs.

| Cheat | Effect | Code |
|---|---|---|
| Infinite HP | Rion's health stays full | `801C2F9C 00C8` |
| Always Empty AP | The AP gauge stays empty | `801AF96C 0000` |
| Infinite Nalcon / Red / D-Felon PPEC | That PPEC gauge stays full | `801AF978` / `801AF97C` / `801AF980 00C8` |
| Psychic Power Level Maxed | Psychic power held at level 3 | `301AF994 0003` |
| Max Charge | The charge meter stays full | `301C2FA6 00C8` |
| Have D-Felon Ability | Unlocks D-Felon | `801AF9AE FFFF` |
| Never Get Knocked Down | Hits no longer knock Rion down or slow him | `D01C2FD4 000E/000C` |
| Quick Turn | Hold Down, press Square to spin round | `E01AC8D9 0040` … |
| Cause Shorting | Press L1 + L2 to fill AP and short out | `D01C2F94 0005` … |
| Instant Full Charge | Press R1 to charge to level 1, 2 or 3 | `D01C2F94 0008` … |
| Have Max Item Slots | All 40 item slots | `801AFAFE 0028` |
| Item Modifier | Any of 40 items in any of 40 slots | `801AFA0C`+ / `801AFAAC`+ |
| Medicine Modifier | Any medicine in any of 12 slots | `801AFB00`+ |
| Have All Movies | Unlocks Movie Preview | `801FD004 FFFFFFFF` |
| Overall Time Is 0:00:00 | Holds the play clock at zero | `801AFDEC 0000` |
| Number of Kills | Sets the kill count | `801AF99C ????` |
| Screen Display | Map always on, or no meters | `801AFDF2 0100/0800` |
| Actions Modifier (experimental) | Forces Rion's action, 0–22 | `801C2FA4 00??` |
| Room Modifier (experimental) | Writes the three room values | `801912DA`/`DC`/`DE` |
| Walk Through Walls | No wall collision | patch at `801212A4` |
| Invulnerable to Most Enemies | Most attacks do no harm | patches at `801486F0`, `801489C4` |
| Doctors Scared of Rion | Hospital doctors back away | patch at `801F86B8` (disc-loaded code) |
| Start New Game With Psychic Power Level 3 | Turn on before New Game | patch at `801267A0` |
| Shorting Without Any Drawbacks | Shorting costs nothing | four patches from `80121D38` |
| Infinite PPEC (code patch) | Powers never drain a gauge | patch at `801462FC` |
| No AP Gauge (code patch) | The AP gauge never fills | patch at `8014943C` |

The codes come from the
[DuckStation cheat database](https://github.com/duckstation/chtdb/blob/master/cheats/SLUS-00986.cht)
and the CodeBreaker and Action Replay pages on
[Almar's Guides](https://almarsguides.com/Retro/Walkthroughs/PS1/Games/Galerians/).
They are applied by a small plugin (`src/galerians_mods.c`) through the
framework's mod system (`mods/packages/galerians.cheats`). Memory cheats are
written every frame. The code patches each change an instruction in the game,
were checked against the USA executable, and stay on for the whole session, so
set them before pressing PLAY. Set `GA_CHEAT_LOG=<file>` to log each patch as it
applies.

Tested on Windows: the manifest loads, and every code patch except Doctors
Scared of Rion applies at boot. Doctors Scared of Rion changes code the game
loads from disc later, so it applies once that code is loaded; it was not seen
in play. The memory cheats were not tried in play. Actions Modifier and Room
Modifier use undocumented values and can lock up the game or put you somewhere
broken, so save first. Walk Through Walls can also leave you stuck.

## Credits and licenses

- Framework: [PSXRecomp](https://github.com/mstan/psxrecomp), PolyForm
  Noncommercial 1.0.0. This project is free and noncommercial.
- Launcher: [recomp-ui](https://github.com/mstan/recomp-ui), MIT.
- Framework revision and release pipeline: the
  [Alexbeav](https://github.com/Alexbeav/psxrecomp) fork used by
  [Alexbeav's PS1 ports](https://github.com/Alexbeav/psxrecomp-ports).
- Their licenses and dependency notices are in the `psxrecomp/` and `recomp-ui/`
  directories.

This project was made with AI assistance (Claude).

Galerians is © 1999 ASCII Corporation / Polygon Magic, Inc. The North American
release was published by Crave Entertainment. All trademarks belong to their
owners. This project is not affiliated with or endorsed by any of them.
