#include "lobby-ranks.h"

#include "imgui/imgui.h"

#include "util.h"

using namespace jlg;

std::string LobbyRanks::GetPluginName()
{
	return "Lobby Ranks";
}
void LobbyRanks::SetImGuiContext( uintptr_t ctx )
{
	ImGui::SetCurrentContext( reinterpret_cast<ImGuiContext*>(ctx) );
}
void LobbyRanks::RenderSettings()
{
	auto cvEnabled = cvarManager->getCvar( Var::enabled );
	if( !cvEnabled ) return;
	bool enabled = cvEnabled.getBoolValue();
	if( ImGui::Checkbox("Enabled", &enabled) )
		cvEnabled.setValue( enabled );

	auto cvShowWSB = cvarManager->getCvar( Var::showWithScoreboard );
	if( !cvShowWSB ) return;
	bool showWSB = cvShowWSB.getBoolValue();
	if( ImGui::Checkbox("Show ranks when scoreboard is open", &showWSB) )
		cvShowWSB.setValue( showWSB );

	auto ha = cvarManager->getCvar( Var::xAnchor );
	auto va = cvarManager->getCvar( Var::yAnchor );
	if( !ha || !va ) return;
	int iv = va.getIntValue();
	int ih = ha.getIntValue();
	if( ImGui::Combo("Table Anchor X", &ih, "Left\0Middle\0Right\0\0") )
		ha.setValue( ih );
	if( ImGui::Combo("Table Anchor Y", &iv, "Top\0Middle\0Bottom\0\0") )
		va.setValue( iv );

	CVarWrapper xLocCvar = cvarManager->getCvar(Var::xPosition);
	if (!xLocCvar) return;
	float xLoc = xLocCvar.getFloatValue();
	if (ImGui::SliderFloat("Table Position X", &xLoc, 0.0, 1.0)) {
		xLocCvar.setValue(xLoc);
	}
	CVarWrapper yLocCvar = cvarManager->getCvar(Var::yPosition);
	if (!yLocCvar) return;
	float yLoc = yLocCvar.getFloatValue();
	if (ImGui::SliderFloat("Table Position Y", &yLoc, 0.0, 1.0)) {
		yLocCvar.setValue(yLoc);
	}

	// JLG TODO Select and Re-order displayed playlists
	//if (ImGui::TreeNode("Playlist order"))
	//{
	//	// Simple reordering
	//	/*HelpMarker(
	//		"We don't use the drag and drop api at all here! "
	//		"Instead we query when the item is held but not hovered, and order items accordingly.");*/
	//	std::vector<std::pair<Playlist, std::string>> playlistItems;
	//	for( auto playlist : PlaylistValues )
	//		playlistItems.push_back( {playlist, util::toString<Playlist>(playlist)} );

	//	for (int n = 0; n < playlistItems.size(); n++)
	//	{
	//		ImGui::Selectable( playlistItems[n].second.c_str() );

	//		if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
	//		{
	//			int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
	//			if (n_next >= 0 && n_next < playlistItems.size())
	//			{
	//				std::swap( playlistItems[n], playlistItems[n_next] );
	//				ImGui::ResetMouseDragDelta();
	//			}
	//		}
	//	}
	//	ImGui::TreePop();
	//}
}
