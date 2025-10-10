#include "stdafx.h"
#include "AlbumSelectedScene.h"
#include "MusicMenu.h"

AlbumSelectedScene::AlbumSelectedScene(MusicMenu& musicMenu)
{
	musicListScrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - upperAreaHeight - 5 * 2), Scene::Height() - UI::menuBarHeight - upperAreaHeight, musicMenu.musicJson[U"allMusic"].size() * musicListOneHeight + albumIconAreaHeight);
}

void AlbumSelectedScene::update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm)
{

	Rect upperArea(0, 0, Scene::Width() - 430, upperAreaHeight);

	if (backToAlbumListButton.update(cursorPos)) {
		musicMenu.selectedAlbumID.reset();
		return;
	}

	if (cursorPos and upperArea.mouseOver()) {
		cursorPos.use();
	}


	Rect musicListArea(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);

	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		musicListScrollBar.pageHeight = musicMenu.musicJson[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"].size() * musicListOneHeight + albumIconAreaHeight;
		musicListScrollBar.update(cursorPos);
		{
			auto tf = musicListScrollBar.createTransformer();
			Transformer2D downForAlbumIcon{ Mat3x2::Translate(0, albumIconAreaHeight) ,TransformCursor::Yes };

			mouseOveredIndex.reset();
			if (cursorPos) {
				for (auto i : step(musicMenu.musicJson[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"].size())) {
					if (RectF(0, i * musicListOneHeight, musicListArea.w, musicListOneHeight).mouseOver()) {
						mouseOveredIndex = i;
						cursorPos.use();
						break;
					}
				}
			}

			if (mouseOveredIndex and MouseL.down()) {
				String id = musicMenu.musicJson[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"][*mouseOveredIndex].getString();
				if (musicMenu.m_nowPlayingMusic and musicMenu.m_nowPlayingMusic->id != id) {
					bgm = Audio{ Audio::Stream, U"musics/" + musicMenu.musicJson[U"musicTable"][id][U"path"].getString() };
					bgm.play();
					musicMenu.m_nowPlayingMusic.reset();
					musicMenu.m_nowPlayingMusic = MusicMenu::NowPlayingMusic{ id,musicMenu.musicJson[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"],*mouseOveredIndex };
				}
			}
		}

	}
}

void AlbumSelectedScene::draw(MusicMenu& musicMenu, const Audio& bgm)
{
	Rect musicListArea(0, upperAreaHeight, Scene::Width() - 430, Scene::Height() - upperAreaHeight - UI::menuBarHeight);
	musicListArea.draw(ColorF(1));
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		{
			auto tf = musicListScrollBar.createTransformer();


			//album image icon
			{
				auto album = musicMenu.musicJson[U"albumTable"][*musicMenu.selectedAlbumID];
				const Texture& texture = musicMenu.albumImages[*musicMenu.selectedAlbumID];
				{
					Transformer2D albumTf(Mat3x2::Translate(250, 250), TransformCursor::Yes);
					albumRoundRect.drawShadow({ 0, 3 }, 5, 5, ColorF(0.7, 0.3));
					makeSmoothShape(albumRoundRect).toBuffer2D(Arg::center(0, 0), texture.size() * (albumRoundRect.rect.size / texture.size()).maxComponent()).draw(texture);
				}

				FontAsset(U"Bold")(album[U"title"].getString()).draw(60, Arg::leftCenter(500, 250), ColorF(0.3));
			}

			Transformer2D downForAlbumIcon{ Mat3x2::Translate(0, albumIconAreaHeight) ,TransformCursor::Yes };

			auto musics = musicMenu.musicJson[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"];

			size_t i = 0;
			for (auto item : musics) {
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

	backToAlbumListButton.draw(Palette::Hotpink);
}
