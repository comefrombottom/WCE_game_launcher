#include "stdafx.h"
#include "IllustViewer.h"
#include "IllustrationMenu.h"

IllustViewer::IllustViewer()
{
}

void IllustViewer::initWith(IllustrationMenu& illustrationMenu, const String& id)
{
	const Vec2 sceneSize = { Scene::Width(), Scene::Height() - UI::menuBarHeight };
	illustrationID = id;
	Texture& texture = illustrationMenu.illustrationImages[illustrationID];
	double scale = (sceneSize / texture.size()).minComponent() * 0.9;
	Vec2 scaledSize = texture.size() * scale;
	pos = sceneSize / 2 - scaledSize / 2;
	width = scaledSize.x;
	targetWidth = scaledSize.x;
	widthVelocity = 0;
}

void IllustViewer::update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu)
{
	if (double wheel = Mouse::Wheel()) {
		double s = Pow(1.1, -wheel);
		Vec2 cp = Cursor::Pos();

		constexpr double minWidth = 300;
		constexpr double maxWidth = 20000;
		double new_width = Clamp(width * s, minWidth, maxWidth);
		s = new_width / width;

		pos = (pos - cp) * s + cp;
		width = new_width;
	}

	if (MouseL.pressed()) {
		pos += Cursor::DeltaF();
	}

	Vec2 center = TScene::Center();
	pos.x = Clamp(pos.x, center.x - width, center.x);
	pos.y = Clamp(pos.y, center.y - width / illustrationMenu.illustrationImages[illustrationID].horizontalAspectRatio(), center.y);


	if (closeButton.update(cursorPos)) {
		illustrationMenu.illustViewerOpen = false;
	}

	if (cursorPos and MouseL.down()) {
		cursorPos.dragOn();
	}

	if (MouseL.up()) {
		cursorPos.dragOff();
	}

	if (cursorPos) {
		cursorPos.use();
	}
}

void IllustViewer::draw(IllustrationMenu& illustrationMenu)
{
	TScene::Rect().draw(ColorF(0, 0.8));
	Texture& texture = illustrationMenu.illustrationImages[illustrationID];
	texture.scaled(width / texture.width()).draw(pos);

	closeButton.draw(ColorF(0.5), Palette::White);
}
