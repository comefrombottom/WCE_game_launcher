#include "stdafx.h"
#include "AllIllustrationScene.h"
#include "IllustrationMenu.h"

AllIllustrationScene::AllIllustrationScene(IllustrationMenu& illustrationMenu)
{
	Array<String> allID;
	for (auto [id, illustration] : illustrationMenu.json[U"illustTable"]) {
		allID.push_back(id);
	}

	allID.shuffle();

	constexpr double paddingX = 50.0;
	constexpr double paddingY = 50.0;

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
		positionedIllusts.push_back(PositionedIllust{ id, Vec2{ x + oneWidth / 2,y + height / 2 }, scale ,scaleWhenMouseOver });
		bottomYs[col] += paddingY + height;
	}

	double maxBottomY = *std::max_element(bottomYs.begin(), bottomYs.end());
	scrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 5 * 2), Scene::Height() - UI::menuBarHeight, maxBottomY + paddingY);
}

void AllIllustrationScene::update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu)
{
	bool sceneMouseOvered = cursorPos and TScene::Rect().mouseOver();
	scrollBar.update(cursorPos, sceneMouseOvered * Mouse::Wheel());
	{
		auto tf = scrollBar.createTransformer();
		for (auto& illust : positionedIllusts) {
			RectF rect{ Arg::center = illust.center, illustrationMenu.illustrationImages[illust.id].size() * illust.scale };
			illust.mouseOvered = cursorPos and rect.mouseOver();
			if (illust.mouseOvered) {
				cursorPos.use();
			}
			if (illust.mouseOvered and MouseL.down()) {
				illust.pressed = true;
				cursorPos.dragOn();
			}

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
	}
}

void AllIllustrationScene::draw(IllustrationMenu& illustrationMenu)
{
	RectF backRect = TScene::Rect();
	backRect.draw(ColorF(1));
	{
		auto tf = scrollBar.createTransformer();

		RectF viewRect = backRect;
		viewRect.y += scrollBar.viewTop;

		for (const auto& illust : positionedIllusts) {
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
	scrollBar.draw();
}
