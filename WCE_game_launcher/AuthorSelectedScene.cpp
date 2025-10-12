#include "stdafx.h"
#include "AuthorSelectedScene.h"
#include "IllustrationMenu.h"



AuthorSelectedScene::AuthorSelectedScene(const RectF& sceneRect, IllustrationMenu& illustrationMenu)
	: m_sceneRect(sceneRect)
{

	std::tie(m_upperAreaRect, m_authorAreaRect) = separateRect(m_sceneRect, Arg::top = upperAreaHeight);

	backToAuthorListButton = OnlyIconButton(layoutRect(m_upperAreaRect, Arg::left = 15, 50, 50), Texture{ 0xf053_icon, 50 });

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
			const double y = bottomYs[col] + paddingY + authorInfoAreaHeight;
			positionedIllustrations[authorId].push_back(PositionedIllust{id, Vec2{x + oneWidth / 2,y + height / 2} + m_authorAreaRect.pos, scale ,scaleWhenMouseOver});
			bottomYs[col] += paddingY + height;
		}

		double maxBottomY = *std::max_element(bottomYs.begin(), bottomYs.end());
		pageHeights[authorId] = maxBottomY + paddingY;
	}

	scrollBar = ScrollBar(layoutRect(m_authorAreaRect, Arg::right = 2, Arg::top = 5, Arg::bottom = 5, 10), m_authorAreaRect.h, 1000);
}

void AuthorSelectedScene::update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu)
{
	if (backToAuthorListButton.update(cursorPos)) {
		illustrationMenu.selectedAuthorID.reset();
		return;
	}

	if (cursorPos and m_upperAreaRect.mouseOver()) {
		cursorPos.use();
	}


	scrollBar.update(cursorPos);
	{
		//Transformer2D cursorPosTransformer(Mat3x2::Translate(area.pos), TransformCursor::Yes);
		{
			auto tf = scrollBar.createTransformer();

			mouseOveredIndex.reset();

			for (auto& illust : positionedIllustrations[*illustrationMenu.selectedAuthorID]) {
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
	m_authorAreaRect.draw(ColorF(1));
	{
		// Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		{
			auto tf = scrollBar.createTransformer();
			auto font = FontAsset(U"Regular");

			TextureAsset(U"Icon.user").resized(70).draw(Arg::leftCenter(m_authorAreaRect.pos + Vec2(70, authorInfoAreaHeight / 2)), ColorF(0.5));

			font(*illustrationMenu.selectedAuthorID).draw(70, Arg::leftCenter(m_authorAreaRect.pos + Vec2(150, authorInfoAreaHeight / 2)), ColorF(0.5));


			Line(m_authorAreaRect.pos + Vec2(50, authorInfoAreaHeight), m_authorAreaRect.tr() + Vec2(-50, authorInfoAreaHeight)).draw(ColorF(0.3));

			RectF viewRect = m_authorAreaRect;
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
	m_upperAreaRect.drawShadow({}, 5, 5, ColorF(0.7, 0.3));
	m_upperAreaRect.draw(ColorF(0.96, 0.99, 0.95));
	backToAuthorListButton.draw(Palette::Limegreen);
}
