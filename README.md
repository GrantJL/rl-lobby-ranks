# Lobby Ranks (Bakkesmod Rocket League plugin)
This is a bakkesmod plugin which can display a table of players in the current match and their ranks across various playlists.

![Image of the Lobby Ranks table as displayed in Rocket League](https://i.imgur.com/E8XE0SA.png)

This plugin was inspired by [Yoni's Playlist Rank Viewer](https://bakkesplugins.com/plugins/view/125)([Github](https://github.com/yonilerner/bakkesmod-playlist-rank-viewer/)) plugin, which this was initially based off of.

You can view and install the plugin on bakkesmod plugins here: [Lobby Ranks](https://bakkesplugins.com/plugins/view/348)

Optionally, download the latest release from [GitHub - LobbyRanks Releases](https://github.com/GrantJL/rl-lobby-ranks/releases), then copy the `lobby-ranks.dll` file to `%appdata%\bakkesmod\bakkesmod\plugins\`

## About
The plugin is a table of players in the current match, it displays each players rank, mmr and games player in various playlists. 

## Usage
**Important:** Make sure you have `Show Player MMR on scoreboard` check in the Bakkesmod settings, under the Ranked tab. Also check `Show MMR in casual playlists` for casual mode support.

You will want to adjust the plugin's settings in Bakkesmod. You can do this by pressing F2 and navigating to the settings tab. Selecting "Lobby Ranks" will display the settings.

In here you can:
- Enable the plugin
- Set it so the Lobby Ranks table displays when the scoreboard is open
- Adjust the table's anchor and position on screen

Additionally, you can bind the `jlg_lobby_rank_toggle` command in the BakkesMod Bindings tab if you wish to toggle the table on key press.

## Future Updates
Currently the plugin only displays the 1s, 2s, 3s and Tournament playlist ranks. Plans are in place to allow user's to choose which playlists to display and in what order.
