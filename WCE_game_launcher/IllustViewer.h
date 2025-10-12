#pragma once

#include "Common.h"
#include "UI.h"

class IllustrationMenu;
class IllustViewer {
	RectF m_sceneRect{};
	String illustrationID;
	Vec2 pos = {};
	double width = 0;
	double widthVelocity = 0;
	double targetWidth = 0;

	IconButton closeButton{ RectF(Arg::center(Scene::Width() - 90,90), 100, 100), Texture(0xF0156_icon, 60) };

public:
	IllustViewer() = default;
	IllustViewer(const RectF& sceneRect);

	void setIllustrationID(const String& id) {
		illustrationID = id;
	}

	void initWith(IllustrationMenu& illustrationMenu, const String& id);

	void update(SingleUseCursorPos& cursorPos, IllustrationMenu& illustrationMenu);
	void draw(IllustrationMenu& illustrationMenu);
};

