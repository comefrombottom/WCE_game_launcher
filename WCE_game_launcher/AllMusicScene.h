#pragma once
#include "UI.h"

class MusicMenu;
class AllMusicScene {

	ScrollBar musicListScrollBar;
	static constexpr double musicListOneHeight = 80;
	static constexpr double upperAreaHeight = 70;

	Optional<size_t> mouseOveredIndex;
public:
	AllMusicScene() = default;
	AllMusicScene(MusicMenu& musicMenu);

	void update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm);

	void draw(MusicMenu& musicMenu, const Audio& bgm);
};


