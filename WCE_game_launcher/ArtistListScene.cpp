#include "stdafx.h"
#include "ArtistListScene.h"
#include "MusicMenu.h"

ArtistListScene::ArtistListScene(MusicMenu& musicMenu) {
	artistListScrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - upperAreaHeight - 5 * 2), Scene::Height() - UI::menuBarHeight - upperAreaHeight, musicMenu.musicJson[U"artistTable"].size() * artistListOneHeight);
}

void ArtistListScene::update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm) {
	Rect musicListArea(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);

	artistListScrollBar.update(cursorPos);
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);

		auto tf = artistListScrollBar.createTransformer();


		mouseOveredIndex.reset();
		mouseOveredArtistID.clear();
		if (cursorPos) {
			size_t i = 0;
			for (auto [artistID, artist] : musicMenu.musicJson[U"artistTable"]) {
				if (RectF(0, i * artistListOneHeight, Scene::Width() - 430, artistListOneHeight).mouseOver()) {
					mouseOveredIndex = i;
					mouseOveredArtistID = artistID;
					cursorPos.use();
					break;
				}

				i++;
			}
		}
		if (mouseOveredArtistID and MouseL.down()) {
			musicMenu.selectedArtistID = mouseOveredArtistID;
		}
	}
}

void ArtistListScene::draw(MusicMenu& musicMenu, const Audio& bgm) {
	Rect musicListArea(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);
	musicListArea.draw(ColorF(1));
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		{
			auto tf = artistListScrollBar.createTransformer();

			size_t i = 0;
			for (auto [artistID, artist] : musicMenu.musicJson[U"artistTable"]) {

				if (mouseOveredIndex and *mouseOveredIndex == i) {
					RectF(40, i * artistListOneHeight, musicListArea.w, artistListOneHeight).rounded(10).draw(ColorF(0.7, 0.5));
				}


				FontAsset(U"Game.Desc")(artistID).draw(Arg::leftCenter(70, (i + 0.5) * artistListOneHeight), ColorF(0.3));
				Line{ 50, (i + 1) * artistListOneHeight, musicListArea.w, (i + 1) * artistListOneHeight }.draw(1, ColorF(0.7));
				i++;
			}
		}
	}
	artistListScrollBar.draw();

	Rect upperArea(0, 0, Scene::Width() - 430, upperAreaHeight);

	upperArea.drawShadow({}, 5, 5, ColorF(0.7, 0.3));
	upperArea.draw(ColorF(0.96, 0.95, 0.99));
}
