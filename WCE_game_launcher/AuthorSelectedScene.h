#pragma once
#include "UI.h"
class IllustrationMenu;

class AuthorSelectedScene {
	OnlyIconButton backToAuthorListButton{ RectF(Arg::center(40,70 / 2), 50, 50), Texture{0xf053_icon, 50} };
	ScrollBar scrollBar;
	Optional<size_t> mouseOveredIndex;
	static constexpr double upperAreaHeight = 70;
	static constexpr double authorInfoAreaHeight = 150;

	static constexpr size_t columns = 3;

	HashTable<String, Array<PositionedIllust>> positionedIllustrations;
public:
	AuthorSelectedScene() = default;
	AuthorSelectedScene(IllustrationMenu& illustrationMenu);
	void update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu);
	void draw(IllustrationMenu& illustrationMenu);
};


