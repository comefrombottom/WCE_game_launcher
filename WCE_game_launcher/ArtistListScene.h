#pragma once
#include "UI.h"
class MusicMenu;

class ArtistListScene {
	ScrollBar artistListScrollBar;
	static constexpr double artistListOneHeight = 80;
	static constexpr double upperAreaHeight = 70;

	Optional<size_t> mouseOveredIndex;
	String mouseOveredArtistID;
public:
	ArtistListScene() = default;
	ArtistListScene(MusicMenu& musicMenu);

	void update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm);

	void draw(MusicMenu& musicMenu, const Audio& bgm);

};

