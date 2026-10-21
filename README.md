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

The patches fix two things in the pinned framework:

- `0002` makes the launcher's **PGO optimize** work on Windows. Without it, the
  training run fails with `[WinError 5] Access is denied` and writes no profile.
  `CMakeLists.txt` also wires up `PSX_PGO`, which this framework revision ignores.
- `0003` removes thin vertical lines on tiled 2D screens when **Texture
  filtering** is set to Bilinear.

`--force-emitters` is required. Without it, `generate` can pick up stale prebuilt
emitters that leave the build with no BIOS backend.

`game.toml` sets `discovery = "reachable"`. With the framework's default
whole-image sweep, about 60 blocks of data in this executable are read as
256 KB functions and `generate` writes 34 million lines of C. Reachable
discovery writes about 0.8 million; code it does not reach runs in the
interpreter instead.

## Status

Version 0.1.0 is an early release, tested on Windows x64 (Intel Iris Xe) with
the Redump dumps above.

- The disc check passes, and the game generates and builds
- Boots on OpenBIOS through the Crave Entertainment FMV to the title screen
- The attract-mode FMVs and in-engine scenes render correctly

**Not tested yet:** starting a New Game and playing, saving and loading, changing
discs mid-game, a full playthrough, PGO optimize, and the Linux and macOS
packages. Please open an issue if you hit a crash, a hang or a graphical bug,
and include the `psx_last_run_report.json` written next to the executable.

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
