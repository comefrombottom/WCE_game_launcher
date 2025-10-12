#pragma once
#include "UI.h"
class IllustrationMenu;

class AuthorSelectedScene {
	RectF m_sceneRect{};
	RectF m_upperAreaRect{};
	RectF m_authorAreaRect{};
	OnlyIconButton backToAuthorListButton;
	ScrollBar scrollBar;
	Optional<size_t> mouseOveredIndex;
	static constexpr double upperAreaHeight = 70;
	static constexpr double authorInfoAreaHeight = 250;

	static constexpr size_t columns = 3;

	HashTable<String, Array<PositionedIllust>> positionedIllustrations;

	HashTable<String, double> pageHeights;
public:
	AuthorSelectedScene() = default;
	AuthorSelectedScene(const RectF& sceneRect, IllustrationMenu& illustrationMenu);

	void init(const String& authorID) {
		mouseOveredIndex.reset();

		scrollBar.pageHeight = pageHeights[authorID];
		scrollBar.viewTop = 0;
	}

	void update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu);
	void draw(IllustrationMenu& illustrationMenu);
};


