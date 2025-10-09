#pragma once
#include "UI.h"

class MusicMenu;
class AlbumSelectedScene {
	OnlyIconButton backToAlbumListButton{ RectF(Arg::center(40,upperAreaHeight / 2), 50, 50), Texture{0xf053_icon, 50} };
	static constexpr RoundRect albumRoundRect = RoundRect(RectF(Arg::center(0, 0), { 400, 400 }), 50);

	ScrollBar musicListScrollBar;
	static constexpr double musicListOneHeight = 80;
	static constexpr double upperAreaHeight = 70;
	static constexpr double albumIconAreaHeight = 500;

	Optional<size_t> mouseOveredIndex;
public:
	AlbumSelectedScene() = default;
	AlbumSelectedScene(MusicMenu& musicMenu);

	void update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm);

	void draw(MusicMenu& musicMenu, const Audio& bgm);
};


