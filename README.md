# Lobby Ranks (Bakkesmod Rocket League plugin)

![Image of the Lobby Ranks table as displayed in Rocket League](https://i.imgur.com/E8XE0SA.png)

View and install this plugin on [BakkesMod Plugins: Lobby Ranks](https://bakkesplugins.com/plugins/view/348)

Alternatively, download the latest release from [GitHub - LobbyRanks Releases](https://github.com/GrantJL/rl-lobby-ranks/releases), then copy the `lobby-ranks.dll` file to `%appdata%\bakkesmod\bakkesmod\plugins\`

## About
This plugin displays a table of the players in your current match, and their rank, MMR and games played across a customizable number of playlists.

The plugin is configurable:
- Display user platform
- Display when scoreboard is open (table can be positioned)
- Colours
  - Table rows and header
  - Teams
  - Ranks
  - Platform (Optional via plugin settings)
- Displayed Playlists
  - Unranked (Casual)
  - Ranked Duel
  - Ranked Twos
  - Ranked Threes
  - Tournament
  - Dropshot
  - Hoops
  - Rumble
  - Snowday
  - Heatseeker

Plugin configuration is automatically saved to file, and restore on load.

## Usage
**Important:** Make sure you have `Show Player MMR on scoreboard` check in the Bakkesmod settings, under the Ranked tab. Also check `Show MMR in casual playlists` for casual mode support.

You will want to adjust the plugin's settings in Bakkesmod. You can do this by pressing F2 and navigating to the settings tab. Selecting "Lobby Ranks" will display the settings.

In here you can:
- Enable the plugin
- Set it so the Lobby Ranks table displays when the scoreboard is open
- Adjust the table's anchor and position on screen
- More customization

Additionally, you can bind the `jlg_lobby_rank_toggle` command in the BakkesMod Bindings tab if you wish to toggle the table on key press.

>Inspired by [Yoni's Playlist Rank Viewer](https://bakkesplugins.com/plugins/view/125)([Github](https://github.com/yonilerner/bakkesmod-playlist-rank-viewer/))

---

## Change Log

### 1.2.2
- Adds Ranked Heatseeker to playlist selection

### 1.2.1
- Adds Ranked 4v4 (Chaos) to playlist selection

### 1.2.0
- Added Platform display to the table (optional)
- Added ability to adjust which Rank Playlists are displayed in the table (and their order)
- Added option to customize the colours used:
  - Table rows and header colours
  - Team colours
  - Rank colours
  - Platform colours
- Added persistent storage (save config to file)

### 1.1.4
- Removed default `P` keybind (was bound to toggle the lobby table)

### 1.1.2
- Added plugin settings window
- No longer hides table when closing scoreboard (if not set to do so)

### 1.1.1
- Improved performance
- Corrected rank division displayed

### 1.0.0
Initial release
