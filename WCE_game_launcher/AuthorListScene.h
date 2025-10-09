#pragma once

#include "UI.h"
class IllustrationMenu;
class AuthorListScene {
	ScrollBar scrollBar;
	Optional<size_t> mouseOveredIndex;
	static constexpr double authorOneHeight = 100;
	static constexpr double upperAreaHeight = 70;

public:

	AuthorListScene();
	void update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu);
	void draw(IllustrationMenu& illustrationMenu);
};
