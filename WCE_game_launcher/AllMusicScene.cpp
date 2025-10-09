#include "stdafx.h"
#include "AllMusicScene.h"
#include "MusicMenu.h"


AllMusicScene::AllMusicScene(MusicMenu& musicMenu) {
	musicListScrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - upperAreaHeight - 5 * 2), Scene::Height() - UI::menuBarHeight - upperAreaHeight, musicMenu.musicJson[U"allMusic"].size() * musicListOneHeight);
}

void AllMusicScene::update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm) {
	Rect upperArea(0, 0, Scene::Width() - 430, upperAreaHeight);


	if (cursorPos and upperArea.mouseOver()) {
		cursorPos.use();
	}

	Rect musicListArea(00, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);

	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		musicListScrollBar.update(cursorPos);
		{
			auto tf = musicListScrollBar.createTransformer();

			mouseOveredIndex.reset();
			if (cursorPos) {
				for (auto i : step(musicMenu.musicJson[U"allMusic"].size())) {
					if (RectF(0, i * musicListOneHeight, musicListArea.w, musicListOneHeight).mouseOver()) {
						mouseOveredIndex = i;
						cursorPos.use();
						break;
					}
				}
			}

			if (mouseOveredIndex and MouseL.down()) {
				String id = musicMenu.musicJson[U"allMusic"][*mouseOveredIndex].getString();
				if (musicMenu.m_nowPlayingMusic and musicMenu.m_nowPlayingMusic->id != id) {
					bgm = Audio{ Audio::Stream, U"musics/" + musicMenu.musicJson[U"musicTable"][id][U"path"].getString() };
					bgm.play();
					musicMenu.m_nowPlayingMusic.reset();
					musicMenu.m_nowPlayingMusic = MusicMenu::NowPlayingMusic{ id,musicMenu.musicJson[U"allMusic"],*mouseOveredIndex };
				}
			}
		}

	}

}

void AllMusicScene::draw(MusicMenu& musicMenu, const Audio& bgm) {
	Rect musicListArea(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);
	musicListArea.draw(ColorF(1));
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		{
			auto tf = musicListScrollBar.createTransformer();

			size_t i = 0;
			for (auto item : musicMenu.musicJson[U"allMusic"]) {
				String id = item.value.getString();
				auto music = musicMenu.musicJson[U"musicTable"][id];
				if (mouseOveredIndex and *mouseOveredIndex == i) {
					RectF(40, i * musicListOneHeight, musicListArea.w, musicListOneHeight).rounded(10).draw(ColorF(0.7, 0.5));
				}

				if (musicMenu.m_nowPlayingMusic and musicMenu.m_nowPlayingMusic->id == id) {
					Circle(30, i * musicListOneHeight + musicListOneHeight / 2, 5).draw(Palette::Hotpink);
				}

				FontAsset(U"Game.Desc")(music[U"title"].getString()).draw(Arg::leftCenter(70, (i + 0.5) * musicListOneHeight), ColorF(0.3));
				FontAsset(U"Game.Small")(music[U"artist"].getString()).draw(Arg::leftCenter(500, (i + 0.5) * musicListOneHeight), ColorF(0.3));

				Line{ 50, (i + 1) * musicListOneHeight, musicListArea.w, (i + 1) * musicListOneHeight }.draw(1, ColorF(0.7));
				i++;
			}
		}


		musicListScrollBar.draw();
	}

	Rect upperArea(0, 0, Scene::Width() - 430, upperAreaHeight);
	upperArea.drawShadow({}, 5, 5, ColorF(0.7, 0.3));
	upperArea.draw(ColorF(0.96, 0.95, 0.99));

}
