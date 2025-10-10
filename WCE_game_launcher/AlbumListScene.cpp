#include "stdafx.h"
#include "AlbumListScene.h"
#include "MusicMenu.h"

AlbumListScene::AlbumListScene(MusicMenu& musicMenu) {
	albumsScrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 5 * 2), Scene::Height() - UI::menuBarHeight, 1000);

	albumIcons.clear();
	for (auto [id, album] : musicMenu.musicJson[U"albumTable"]) {
		albumIcons.push_back(AlubmIcon{ id });
	}
}

void AlbumListScene::update(SingleUseCursorPos& cursorPos, MusicMenu& musicMenu, Audio& bgm) {
	albumsScrollBar.update(cursorPos);
	{
		auto tf = albumsScrollBar.createTransformer();

		for (auto [i, albumIcon] : IndexedRef(albumIcons)) {
			auto album = musicMenu.musicJson[U"albumTable"][albumIcon.id];
			size_t x_i = i % 3;
			size_t y_i = i / 3;

			const Vec2 iconCenter = Vec2(250 + x_i * 450, 250 + y_i * 450);
			Transformer2D albumTf(Mat3x2::Translate(iconCenter), TransformCursor::Yes);
			Vec2 cp = Cursor::PosF();
			const Vec2 target = 0.2 * cp / (1 + cp.length() / 150);

			Transformer2D albumTf2(Mat3x2::Scale(albumIcon.scale).translated(albumIcon.pos), TransformCursor::Yes);

			bool mouseOver = cursorPos and albumRoundRect.mouseOver();
			if (mouseOver) {
				cursorPos.use();
			}

			albumIcon.pos = Math::SmoothDamp(albumIcon.pos, mouseOver ? target : Vec2{ 0, 0 }, albumIcon.posVel, 0.2);
			albumIcon.scale = Math::SmoothDamp(albumIcon.scale, mouseOver ? 1.05 : 1.0, albumIcon.scaleVel, 0.2);

			if (mouseOver and MouseL.down()) {
				albumIcon.scaleVel = -1;
				musicMenu.selectedAlbumID = albumIcon.id;
			}
		}
	}
}

void AlbumListScene::draw(MusicMenu& musicMenu, const Audio& bgm) {
	Rect rect(0, 0, Scene::Width(), Scene::Height());
	rect.draw(ColorF(1));
	{
		auto tf = albumsScrollBar.createTransformer();

		size_t i = 0;
		for (const auto& albumIcon : albumIcons) {
			auto album = musicMenu.musicJson[U"albumTable"][albumIcon.id];
			size_t x_i = i % 3;
			size_t y_i = i / 3;

			const Texture& texture = musicMenu.albumImages[albumIcon.id];

			const Vec2 iconCenter = Vec2(250 + x_i * 450, 250 + y_i * 450);
			Transformer2D albumTf(Mat3x2::Translate(iconCenter), TransformCursor::Yes);

			FontAsset(U"Bold")(album[U"title"].getString()).draw(30, Arg::center(0, 250), ColorF(0.3));

			Transformer2D albumTf2(Mat3x2::Scale(albumIcon.scale).translated(albumIcon.pos), TransformCursor::Yes);
			albumRoundRect.drawShadow({ 0, 3 }, 5, 5, ColorF(0.7, 0.3));
			makeSmoothShape(albumRoundRect).toBuffer2D(Arg::center(0, 0), texture.size() * (albumRoundRect.rect.size / texture.size()).maxComponent()).draw(texture);

			i++;
		}
	}
	albumsScrollBar.draw();
}

