#include "stdafx.h"
#include "AuthorListScene.h"
#include "IllustrationMenu.h"

AuthorListScene::AuthorListScene()
{
	scrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 5 * 2), Scene::Height() - UI::menuBarHeight, 1000);
}

void AuthorListScene::update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu)
{
	RectF area(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);

	scrollBar.update(cursorPos);
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(area.pos), TransformCursor::Yes);
		{
			auto tf = scrollBar.createTransformer();

			mouseOveredIndex.reset();
			String mouseOveredAuthorID;

			if (cursorPos) {
				size_t i = 0;
				for (auto [authorID, author] : illustrationMenu.json[U"authorTable"]) {
					if (RectF(0, i * authorOneHeight, Scene::Width() - 430, authorOneHeight).mouseOver()) {
						mouseOveredIndex = i;
						mouseOveredAuthorID = authorID;
						cursorPos.use();
						break;
					}

					i++;
				}
			}
			if (mouseOveredAuthorID and MouseL.down()) {
				illustrationMenu.selectedAuthorID = mouseOveredAuthorID;
			}

		}
	}
}

void AuthorListScene::draw(IllustrationMenu& illustrationMenu)
{
	RectF area(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);
	area.draw(ColorF(1));
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(area.pos), TransformCursor::Yes);
		{
			auto tf = scrollBar.createTransformer();
			size_t i = 0;
			for (auto [authorID, author] : illustrationMenu.json[U"authorTable"]) {
				if (mouseOveredIndex and *mouseOveredIndex == i) {
					RectF(40, i * authorOneHeight, area.w, authorOneHeight).rounded(10).draw(ColorF(0.7, 0.5));
				}
				FontAsset(U"Game.Desc")(authorID).draw(Arg::leftCenter(70, (i + 0.5) * authorOneHeight), ColorF(0.3));
				Line{ 50, (i + 1) * authorOneHeight, area.w, (i + 1) * authorOneHeight }.draw(1, ColorF(0.7));
				i++;
			}
		}
	}
	scrollBar.draw();
	Rect upperArea(0, 0, Scene::Width() - 430, upperAreaHeight);
	upperArea.drawShadow({}, 5, 5, ColorF(0.7, 0.3));
	upperArea.draw(ColorF(0.96, 0.99, 0.95));
}
