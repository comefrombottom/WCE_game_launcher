#pragma once
#include "UI.h"

class MusicMenu;
class AlbumListScene {
	ScrollBar albumsScrollBar;
	static constexpr RoundRect albumRoundRect = RoundRect(RectF(Arg::center(0, 0), { 400, 400 }), 50);

	struct AlubmIcon {
		String id;
		Vec2 pos{};
		Vec2 posVel{};
		double scale = 1;
		double scaleVel = 0;
	};

	Array<AlubmIcon> albumIcons;
public:
	AlbumListScene() = default;
	AlbumListScene(MusicMenu& musicMenu);

	void update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm);

	void draw(MusicMenu& musicMenu, const Audio& bgm);
};


