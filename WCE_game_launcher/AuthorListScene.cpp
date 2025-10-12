#include "stdafx.h"
#include "AuthorListScene.h"
#include "IllustrationMenu.h"

AuthorListScene::AuthorListScene(const RectF& sceneRect, IllustrationMenu& illustrationMenu)
	: m_sceneRect(sceneRect)
{

	std::tie(m_upperAreaRect, m_authorAreaRect) = separateRect(m_sceneRect, Arg::top = upperAreaHeight);

	double pageHeight = authorOneHeight * illustrationMenu.json[U"authorTable"].size();

	scrollBar = ScrollBar(layoutRect(m_authorAreaRect, Arg::right = 2, Arg::top = 5, Arg::bottom = 5, 10), m_authorAreaRect.h, pageHeight);
}

void AuthorListScene::update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu)
{
	//RectF area(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);

	if (cursorPos and m_upperAreaRect.mouseOver()) {
		cursorPos.use();
	}

	scrollBar.update(cursorPos);
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(m_authorAreaRect.pos), TransformCursor::Yes);
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
				illustrationMenu.authorSelectedScene.init(mouseOveredAuthorID);
			}

		}
	}
}

void AuthorListScene::draw(IllustrationMenu& illustrationMenu)
{
	// RectF area(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);
	m_authorAreaRect.draw(ColorF(1));
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(m_authorAreaRect.pos), TransformCursor::Yes);
		{
			auto tf = scrollBar.createTransformer();
			size_t i = 0;
			for (auto [authorID, author] : illustrationMenu.json[U"authorTable"]) {
				if (mouseOveredIndex and *mouseOveredIndex == i) {
					RectF(40, i * authorOneHeight, m_authorAreaRect.w, authorOneHeight).rounded(10).draw(ColorF(0.7, 0.5));
				}
				FontAsset(U"Game.Desc")(authorID).draw(Arg::leftCenter(70, (i + 0.5) * authorOneHeight), ColorF(0.3));
				Line{ 50, (i + 1) * authorOneHeight, m_authorAreaRect.w, (i + 1) * authorOneHeight }.draw(1, ColorF(0.7));
				i++;
			}
		}
	}
	scrollBar.draw();
	m_upperAreaRect.drawShadow({}, 5, 5, ColorF(0.7, 0.3));
	m_upperAreaRect.draw(ColorF(0.96, 0.99, 0.95));
}
