#pragma once

#include "UI.h"
#include "Common.h"

class IllustrationMenu;
class AllIllustrationScene {
	ScrollBar scrollBar;
	Optional<size_t> mouseOveredIndex;

	static constexpr int32 columns = 3;

	Array<PositionedIllust> positionedIllusts;

public:
	AllIllustrationScene() = default;
	AllIllustrationScene(IllustrationMenu& illustrationMenu);
	void update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu);
	void draw(IllustrationMenu& illustrationMenu);
};


