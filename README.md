# QUERY NOIR
### A Cyber-Detective SQL Investigation Game

```
╔══════════════════════════════════════════════════════╗
║  FORENSICS TERMINAL v4.7  |  CASE: ORION MURDER      ║
║  STATUS: ACTIVE           |  CLEARANCE: LEVEL 4      ║
╚══════════════════════════════════════════════════════╝
```

Marcus Orion, Senior Data Architect at NovaCorp, was found dead in Server Room B-7 at 02:14. You have access to the company's internal database. Five colleagues were in the building that night. The data doesn't lie — people do.

Write SQL queries to uncover the truth.

---

## Requirements

| Dependency | Version |
|---|---|
| C++ Compiler | GCC 13+ or Clang 16+ |
| CMake | 3.16+ |
| SDL2 | 2.0.18+ |
| SQLite3 | 3.x |
| Git | any |
| curl + unzip | used by setup script |

SDL2_ttf and SDL2_mixer are listed as dependencies in CMake but audio is synthesized procedurally — no SDL2_mixer is strictly required at runtime. If it fails to find SDL2_mixer, remove it from `CMakeLists.txt` link targets.

---

## Quick Start

```bash
cd query_noir
chmod +x setup.sh build.sh run.sh

# Install dependencies, download Dear ImGui and JetBrains Mono font
./setup.sh

# Compile
./build.sh

# Play
./run.sh
```

The setup script handles everything: system package installation, cloning Dear ImGui from GitHub, downloading the JetBrains Mono font, and falling back to a SQLite3 amalgamation if the system library is not found.

---

## Platform Notes

**macOS** — Homebrew is required. The build system automatically detects both Apple Silicon (`/opt/homebrew`) and Intel (`/usr/local`) prefixes.

**Linux** — Works on Ubuntu/Debian, Fedora, and Arch. The setup script detects your package manager.

**Windows** — Use WSL2 with Ubuntu 22.04+. Install an X server (VcXsrv, or WSLg on Windows 11), then run the setup and build scripts normally inside WSL2. Native MinGW/MSYS2 builds are also possible; see the documentation.

If you get a build error about a stale CMake cache after updating the project, delete the build directory and rebuild clean:

```bash
rm -rf build/
./build.sh
```

---

## Project Structure

```
query_noir/
├── CMakeLists.txt          Build configuration
├── setup.sh                Dependency installer
├── build.sh                CMake build wrapper
├── run.sh                  Launch the built binary
│
├── src/
│   ├── main.cpp            SDL2 init, ImGui setup, fonts, main loop
│   ├── Renderer.cpp        Top bar, left panel, all modals and overlays
│   ├── QueryConsole.cpp    SQL input widget and results table
│   ├── NarrativeFeed.cpp   Right-panel detective feed with typewriter
│   ├── CaseManager.cpp     Case lifecycle — init and future case loading
│   ├── AudioManager.cpp    Audio init and game-event callback wiring
│   ├── GameState.cpp       Puzzle engine, clue system, unlock logic
│   ├── Database.cpp        SQLite3 wrapper and case data seeding
│   ├── Audio.cpp           Procedural synthesizer and ambient music
│   ├── Intro.cpp           14-second animated intro sequence
│   └── UITheme.cpp         Dear ImGui cyber-noir style system
│
├── include/
│   ├── RenderCommon.h      Shared layout, colors, extern state, draw_* decls
│   ├── Types.h             All shared structs and enums
│   ├── GameState.h         Game logic interface
│   ├── Database.h          SQLite3 wrapper interface
│   ├── Audio.h             Synthesizer interface and SFX enum
│   ├── AudioManager.h      Audio manager interface
│   ├── CaseManager.h       Case manager interface
│   ├── Intro.h             Intro sequence state machine
│   └── UITheme.h           Styling helpers
│
├── vendor/
│   ├── imgui/              Dear ImGui (cloned by setup.sh)
│   └── sqlite3/            SQLite3 amalgamation fallback
│
└── assets/
    └── fonts/
        ├── JetBrainsMono-Regular.ttf
        └── JetBrainsMono-Bold.ttf
```

---

## How to Play

### The Interface

```
┌────────────────────────────────────────────────────────────────────────┐
│  ◈ QUERY NOIR  |  CASE: ORION MURDER  |  ● ACTIVE  |  EVIDENCE: 0/8    │
├──────────────────┬─────────────────────────────┬───────────────────────┤
│                  │                             │                       │
│  TABLES          │  // QUERY CONSOLE           │  ◈ DETECTIVE FEED     │
│                  │                             │                       │
│  ◈ victims       │  noir@forensics:~/orion$    │  > Marcus Orion.      │
│  ? suspects      │  SELECT * FROM ...          │    42. Found dead.    │
│  ⊡ access_logs   │                             │                       │
│  ⊕ [LOCKED]      │  OUTPUT  —  N rows          │  ? Start with the     │
│  ✉ [LOCKED]      │  ┌─────────────────────┐    │    access logs.       │
│  $ [LOCKED]      │  │  results table      │    │                       │
│  ⚑ [LOCKED]      │  └─────────────────────┘    │  # New file unlocked  │
│  ⚿ [LOCKED]      │                             │                       │
│                  │                             │                       │
│  EVIDENCE        │                             │                       │
│  ✓ Clue found    │                             │                       │
│  ○ [UNSOLVED]    │                             │                       │
└──────────────────┴─────────────────────────────┴───────────────────────┘
```

### Controls

| Input | Action |
|---|---|
| `Enter` or `EXECUTE` button | Run the current query |
| `↑` / `↓` arrow keys | Navigate query history |
| `Escape` | Close active modal, or quit |
| Click a table name | Auto-fill a `SELECT * LIMIT 5` preview |
| Hover a table name | Preview column schema |
| Hover a clue `○` | See the hint for that clue |
| `SCHEMA` button (top bar) | Toggle the full schema viewer |
| `ACCUSE` button (top bar) | Appears when you have enough evidence |
| `SCHEMA` typed as a query | Same as the button |

### The Tables

Eight database tables. Three are open from the start. The rest unlock progressively as you find the right data in the previous table — not just by querying it, but by finding something meaningful.

| Table | Starts | Contains |
|---|---|---|
| `victims` | Unlocked | Marcus Orion's file |
| `suspects` | Unlocked | Five suspects with alibis |
| `access_logs` | Unlocked | 24 building entry/exit records |
| `badge_registry` | Locked | Who has access to what — and who modified the MASTER badge |
| `messages` | Locked | Internal and external communications |
| `transactions` | Locked | NovaCorp financial records |
| `incident_reports` | Locked | Internal complaints and flags |
| `decrypted_messages` | Locked | Encrypted messages — requires finding a key |

### Evidence

There are 8 pieces of evidence to find. Each one requires a specific, targeted query — not just `SELECT * FROM table`. Hover over any unsolved clue `○` in the left panel to see its hint.

You need at least 5 clues, including the badge tampering clue and Lena's pre-murder message, before the `ACCUSE` button appears.

### Winning

When you have gathered enough evidence, the `ACCUSE` button appears in the top bar. Click it, type the full name of the person you believe murdered Marcus Orion, and press `CHARGE`. A wrong accusation gives you specific counter-evidence feedback. You can try again.

---

## Sound

All audio is synthesized procedurally at runtime using SDL2's audio API. No external sound files are required. The game produces:

- Mechanical key click on every keystroke
- Rising digital sweep when a query executes
- Ascending chime when a new table unlocks
- Crystalline ping when evidence is found
- Descending buzzer on SQL errors
- Dark ambient drone that increases in tension as evidence accumulates
- A dramatic three-note sting when you make your accusation

---

## Troubleshooting

**SDL2 not found** — Run `./setup.sh`, or on macOS run `brew install sdl2`.

**ImGui not found** — Run `./setup.sh`. It clones Dear ImGui from GitHub into `vendor/imgui/`.

**Font not loading** — The game falls back to ImGui's built-in bitmap font automatically. To get JetBrains Mono, run `./setup.sh`.

**Wayland issues on Linux** — Run `export SDL_VIDEODRIVER=x11` before launching.

**Black screen / renderer error** — Make sure your system supports hardware-accelerated rendering (`SDL_RENDERER_ACCELERATED`). On virtual machines, try `export SDL_RENDERER=software`.

**Stale build errors after updating** — Delete `build/` and rebuild: `rm -rf build/ && ./build.sh`.

**incident_reports shows 0 rows** — This was a known bug caused by Unicode em-dashes in SQL string literals aborting the seed. It is fixed in the current version. If you are seeing it, make sure you have the latest source.

---

## Extending the Game

To add a new case, add a new seed method to `Database.cpp`, define tables and clues in a new `GameState::init_case_*()` method, add the unlock logic to `check_unlocks()` and `check_puzzles()`, and call it from `CaseManager::init()`. The architecture is designed to support multiple cases through `CaseManager`.

---

*"The data doesn't lie. People do."*
