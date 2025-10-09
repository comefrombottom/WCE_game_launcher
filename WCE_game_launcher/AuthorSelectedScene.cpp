#include "stdafx.h"
#include "AuthorSelectedScene.h"
#include "IllustrationMenu.h"



AuthorSelectedScene::AuthorSelectedScene(IllustrationMenu& illustrationMenu)
{
	scrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 5 * 2), Scene::Height() - UI::menuBarHeight, 1000);

	constexpr double paddingX = 50.0;
	constexpr double paddingY = 50.0;

	for (auto [authorId, author] : illustrationMenu.json[U"authorTable"]) {

		Array<String> allID;
		for (const auto& [i, id] : author[U"illusts"]) {
			allID.push_back(id.getString());
		}
		

		Array<double> bottomYs(columns);

		auto getMinColmn = [&bottomYs]() {
			return static_cast<size_t>(std::distance(
				bottomYs.begin(),
				std::min_element(bottomYs.begin(), bottomYs.end())
			));
			};

		const double oneWidth = (Scene::Width() - 430 - paddingX * (columns + 1)) / columns;

		for (const auto& id : allID) {
			Texture& texture = illustrationMenu.illustrationImages[id];
			double scale = oneWidth / texture.width();
			double scaleWhenMouseOver = (oneWidth + 30) / texture.width();
			double height = texture.height() * scale;
			const size_t col = getMinColmn();
			const double x = paddingX + col * (oneWidth + paddingX);
			const double y = bottomYs[col] + paddingY;
			positionedIllustrations[authorId].push_back(PositionedIllust{id, Vec2{x + oneWidth / 2,y + height / 2}, scale ,scaleWhenMouseOver});
			bottomYs[col] += paddingY + height;
		}

		//double maxBottomY = *std::max_element(bottomYs.begin(), bottomYs.end());
		//scrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 5 * 2), Scene::Height() - UI::menuBarHeight, maxBottomY + paddingY);
	}
}

void AuthorSelectedScene::update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu)
{
	if (backToAuthorListButton.update(cursorPos)) {
		illustrationMenu.selectedAuthorID.reset();
		return;
	}

	RectF area(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);

	scrollBar.update(cursorPos);
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(area.pos), TransformCursor::Yes);
		{
			auto tf = scrollBar.createTransformer();

			mouseOveredIndex.reset();

			PrintDebug(Cursor::PosF());
			for (auto& illust : positionedIllustrations[*illustrationMenu.selectedAuthorID]) {
				RectF rect{ Arg::center = illust.center, illustrationMenu.illustrationImages[illust.id].size() * illust.scale };
				PrintDebug(rect);
				illust.mouseOvered = cursorPos and rect.mouseOver();
				if (illust.mouseOvered) {
					cursorPos.use();
				}
				if (illust.mouseOvered and MouseL.down()) {
					illust.pressed = true;
					cursorPos.dragOn();
				}
				PrintDebug(illust.pressed);

				if (illust.pressed and (MouseL.up() or not rect.mouseOver())) {
					illust.pressed = false;
					cursorPos.dragOff();

					if (rect.mouseOver()) {
						illustrationMenu.illustViewerOpen = true;
						illustrationMenu.illustViewer.initWith(illustrationMenu, illust.id);
					}
				}

				illust.mouseOverTransition = Math::SmoothDamp(illust.mouseOverTransition, illust.mouseOvered or illust.pressed ? 1.0 : 0.0, illust.mouseOverTransitionVel, 0.1);
			}

			if (mouseOveredIndex and MouseL.down()) {
				String selectedillustrationID = illustrationMenu.json[U"authorTable"][*illustrationMenu.selectedAuthorID][U"illusts"][*mouseOveredIndex].getString();
				illustrationMenu.illustViewerOpen = true;
				illustrationMenu.illustViewer.initWith(illustrationMenu, selectedillustrationID);
			}


		}
	}
}

void AuthorSelectedScene::draw(IllustrationMenu& illustrationMenu)
{
	Rect musicListArea(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);
	musicListArea.draw(ColorF(1));
	const RectF backRect = TScene::Rect();
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		{
			auto tf = scrollBar.createTransformer();

			RectF viewRect = backRect;
			viewRect.y += scrollBar.viewTop;

			for (const auto& illust : positionedIllustrations[*illustrationMenu.selectedAuthorID]) {
				Texture& texture = illustrationMenu.illustrationImages[illust.id];
				RectF rect{ Arg::center = illust.center, texture.size() * illust.scale };
				if (viewRect.intersects(rect)) {
					RectF sr = texture.scaled(Math::Lerp(illust.scale, illust.scaleWhenMouseOver, illust.mouseOverTransition)).drawAt(illust.center);
					if (illust.pressed) {
						sr.draw(ColorF(0, 0.2));
					}
				}
			}
		}
	}
	scrollBar.draw();
	Rect upperArea(0, 0, Scene::Width() - 430, upperAreaHeight);
	upperArea.drawShadow({}, 5, 5, ColorF(0.7, 0.3));
	upperArea.draw(ColorF(0.96, 0.99, 0.95));
	backToAuthorListButton.draw(Palette::Limegreen);
}
