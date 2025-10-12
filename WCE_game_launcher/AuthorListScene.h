#pragma once

#include "UI.h"
class IllustrationMenu;
class AuthorListScene {
	RectF m_sceneRect{};
	RectF m_upperAreaRect{};
	RectF m_authorAreaRect{};
	ScrollBar scrollBar;
	Optional<size_t> mouseOveredIndex;
	static constexpr double authorOneHeight = 100;
	static constexpr double upperAreaHeight = 70;

public:
	AuthorListScene() = default;
	AuthorListScene(const RectF& sceneRect, IllustrationMenu& illustrationMenu);
	void update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu);
	void draw(IllustrationMenu& illustrationMenu);
};
