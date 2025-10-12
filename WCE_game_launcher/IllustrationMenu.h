#pragma once
#include "AllIllustrationScene.h"
#include "AuthorListScene.h"
#include "AuthorSelectedScene.h"
#include "IllustViewer.h"
#include "layoutRect.h"

class IllustrationMenu {
	RectF m_sceneRect{};

	SideGenreList sideGenreList;
	JSON json;
	Optional<String> selectedillustrationID;

	HashTable<String, Texture> illustrationImages;

	AllIllustrationScene allIllustrationScene;
	AuthorListScene authorListScene;
	AuthorSelectedScene authorSelectedScene;

	Optional<String> selectedAuthorID;

	IllustViewer illustViewer;
	bool illustViewerOpen = false;

	RectF m_sideRect{};

	void loadIllustrationImages() {
		for (auto [id, illustration] : json[U"illustTable"]) {
			illustrationImages[id] = Texture{ U"illustrations/" + illustration[U"path"].getString() ,TextureDesc::Mipped };
		}
	}
public:
	IllustrationMenu() = default;
	IllustrationMenu(const RectF& sceneRect) :m_sceneRect(sceneRect)
	{
		sideGenreList = SideGenreList{
			{
				{Texture{0Xf03e_icon,40}, U"全イラスト"},
				{Texture{0Xf0c0_icon,40}, U"作者"},
			},
			{Vec2(20,20) + m_sceneRect.pos},
			{400,80}
		};
		json = JSON::Load(U"illustrations/settings.json");
		if (not json) {
			throw Error(U"illustrations/settings.jsonが読み込めませんでした。");
		}

		loadIllustrationImages();

		// 作者にイラストを紐づける
		for (auto [id, illustration] : json[U"illustTable"]) {
			if (auto author = illustration[U"author"].getOpt<String>()) {
				json[U"authorTable"][author.value()][U"illusts"].push_back(id);
			}
		}

		auto [sideRect, otherRect] = separateRect(m_sceneRect, Arg::left = 430);

		m_sideRect = sideRect;

		allIllustrationScene = AllIllustrationScene(otherRect, *this);
		authorListScene = AuthorListScene(otherRect, *this);
		authorSelectedScene = AuthorSelectedScene(otherRect, *this);
		illustViewer = IllustViewer(m_sceneRect);

	}
	void update(SingleUseCursorPos& cursorPos) {

		if (illustViewerOpen) {
			illustViewer.update(cursorPos, *this);
		}

		sideGenreList.update(cursorPos);
		if (cursorPos and m_sideRect.mouseOver()) {
			cursorPos.use();
		}

		// Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);
		switch (sideGenreList.index())
		{
		case 0:
			allIllustrationScene.update(cursorPos, *this);
			break;
		case 1:
			if (selectedAuthorID) {
				authorSelectedScene.update(cursorPos, *this);
			}
			else {
				authorListScene.update(cursorPos, *this);
			}

			break;
		default:
			break;
		}
	}
	void draw() {
		{
			// Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);
			switch (sideGenreList.index())
			{
			case 0:
				allIllustrationScene.draw(*this);
				break;
			case 1:
				if (selectedAuthorID) {
					authorSelectedScene.draw(*this);
				}
				else {
					authorListScene.draw(*this);
				}
				break;
			default:
				break;
			}
		}

		m_sideRect.drawShadow({}, 5, 3, ColorF(0.5, 0.3)).draw(ColorF(0.96, 0.99, 0.95));
		sideGenreList.draw(FontAsset(U"Bold"), Palette::Limegreen);

		if (illustViewerOpen) {
			illustViewer.draw(*this);
		}
	}

	friend class AllIllustrationScene;
	friend class AuthorListScene;
	friend class AuthorSelectedScene;
	friend class IllustViewer;
};


