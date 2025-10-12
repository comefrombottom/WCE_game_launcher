#include "stdafx.h"
#include "AllIllustrationScene.h"
#include "IllustrationMenu.h"

AllIllustrationScene::AllIllustrationScene(const RectF& sceneRect, IllustrationMenu& illustrationMenu)
	: m_sceneRect(sceneRect)
{
	Array<String> allID;
	for (auto [id, illustration] : illustrationMenu.json[U"illustTable"]) {
		allID.push_back(id);
	}

	allID.shuffle();

	constexpr double paddingX = 50.0;
	constexpr double paddingY = 80.0;

	Array<double> bottomYs(columns);

	auto getMinColmn = [&bottomYs]() {
		return static_cast<size_t>(std::distance(
			bottomYs.begin(),
			std::min_element(bottomYs.begin(), bottomYs.end())
		));
		};

	const double oneWidth = (m_sceneRect.w - paddingX * (columns + 1)) / columns;

	for (const auto& id : allID) {
		Texture& texture = illustrationMenu.illustrationImages[id];
		double scale = oneWidth / texture.width();
		double scaleWhenMouseOver = (oneWidth + 30) / texture.width();
		double height = texture.height() * scale;
		const size_t col = getMinColmn();
		const double x = paddingX + col * (oneWidth + paddingX);
		double y = bottomYs[col] + paddingY;

		positionedIllusts.push_back(PositionedIllust{ id, Vec2{ x + oneWidth / 2,y + height / 2 } + m_sceneRect.pos, scale ,scaleWhenMouseOver });

		auto illustJSON = illustrationMenu.json[U"illustTable"][id];

		y += height;

		String title = illustJSON[U"title"].getOr<String>(U"");
		if (title) {
			y += 30 + 15;
		}

		String authorName = illustJSON[U"author"].getOr<String>(U"");
		if (authorName) {
			y += 30 + 5;
		}

		// y += paddingY;

		bottomYs[col] = y;
	}

	double maxBottomY = *std::max_element(bottomYs.begin(), bottomYs.end());
	scrollBar = ScrollBar(layoutRect(m_sceneRect, Arg::right = 2, Arg::top = 5, Arg::bottom = 5, 10), m_sceneRect.h, maxBottomY + paddingY);
}

void AllIllustrationScene::update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu)
{
	bool sceneMouseOvered = cursorPos and m_sceneRect.mouseOver();
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
	m_sceneRect.draw(ColorF(1));
	{
		auto tf = scrollBar.createTransformer();

		RectF viewRect = m_sceneRect;
		viewRect.y += scrollBar.viewTop;

		for (const auto& illust : positionedIllusts) {
			Texture& texture = illustrationMenu.illustrationImages[illust.id];
			RectF rect{ Arg::center = illust.center, texture.size() * illust.scale };
			double realScale = Math::Lerp(illust.scale, illust.scaleWhenMouseOver, illust.mouseOverTransition);
			RectF scaledRect{ Arg::center = illust.center, texture.size() * realScale };
			if (viewRect.intersects(rect)) {
				RectF sr = texture.scaled(realScale).drawAt(illust.center);
				if (illust.pressed) {
					sr.draw(ColorF(0, 0.2));
				}
			}

			auto illustJSON = illustrationMenu.json[U"illustTable"][illust.id];

			String title = illustJSON[U"title"].getOr<String>(U"");
			double y = scaledRect.bottomY() + 15;
			if (title) {
				const auto& font = FontAsset(U"Bold");
				RectF titleRect = font(title).region(30,scaledRect.x, y);
				if (viewRect.intersects(titleRect)) {
					if (titleRect.w > rect.w) { // 収まらない場合
						font(title).draw(30, RectF{ scaledRect.x, y, rect.w, titleRect.h }, Palette::Black);
					}
					else {
						font(title).draw(30, Arg::topLeft(scaledRect.x, y), Palette::Black);
					}
				}
				y += titleRect.h + 5;
			}

			String authorName = illustJSON[U"author"].getOr<String>(U"");
			if (authorName) {
				const auto& font = FontAsset(U"Regular");
				const auto& usericon = TextureAsset(U"Icon.user");

				RectF usericonRect = usericon.resized(30).draw(Arg::topLeft(scaledRect.x, y), ColorF(0.3));
				font(authorName).draw(30, Arg::leftCenter(usericonRect.rightCenter() + Vec2{10, 0}), Palette::Dimgray);


			}
			
		}
	}
	scrollBar.draw();
}
