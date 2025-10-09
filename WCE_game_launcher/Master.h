#pragma once
#include "Common.h"
#include "UI.h"
#include "GameMenu.h"
#include "MusicMenu.h"
#include "IllustrationMenu.h"
#include "MusicControlPanel.h"

class Master {
public:

	MenuSelector menuSelector{
		{
			{Texture{0Xf11b_icon,40}, U"ゲーム", Palette::Royalblue},
			{Texture{0Xf001_icon,40}, U"音楽", Palette::Hotpink},
			{Texture{0Xf53f_icon,40}, U"イラスト", Palette::Limegreen}
		}
	};

	RectF menuBar{ 0, 0, Scene::Width(), UI::menuBarHeight };

	SpeakerButton speakerButton{ RectF(Arg::center(600, UI::menuBarHeight / 2), 50, 50), false };

	VolumeSlider volumeSlider{ 0.5, 0, 1, Vec2(650, UI::menuBarHeight / 2), 300 };

	MusicControlPanel musicControlPanel{ Vec2{1000,5} };

	Audio bgm;

	GameMenu gameMenu;
	MusicMenu musicMenu{ bgm };
	IllustrationMenu illustrationMenu;

	BlurMSRenderTextureSet wholeSceneRenderTexture{ Scene::Size() };

	Stopwatch exitTimer;

	ColorIconButton exitButton{ RectF(Scene::Width() - 70, UI::menuBarHeight / 2 - 45.0 / 2, 60, 45),Texture(0xf2f5_icon, 40) };

	ExitWindow exitWindow{ Scene::CenterF() };



public:
	Master() {
		GlobalAudio::SetVolume(speakerButton.isMute() ? 0 : volumeSlider.value());
	}

	void update(SingleUseCursorPos& cursorPos) {

		//音楽が終わったら次の曲を再生
		if (musicMenu.hasNowPlayingMusic() and not bgm.isActive()) {
			musicMenu.goForwardMusic(bgm);
		}

		if (not exitTimer.isStarted()) {

			menuSelector.update(cursorPos);

			if (volumeSlider.update(cursorPos)) {
				GlobalAudio::SetVolume(speakerButton.isMute() ? 0 : volumeSlider.value());
			}
			if (speakerButton.update(cursorPos)) {
				GlobalAudio::SetVolume(speakerButton.isMute() ? 0 : volumeSlider.value());
			}

			musicControlPanel.update(cursorPos, *this);

			if (exitButton.update(cursorPos)) {
				exitTimer.restart();
			}


			if (cursorPos and menuBar.mouseOver()) {
				cursorPos.use();
			}




			{
				{
					Transformer2D t(Mat3x2::Translate(0, UI::menuBarHeight), TransformCursor::Yes);
					switch (menuSelector.getSelectMenuIndex())
					{
					case 0:
						gameMenu.update(cursorPos, *this);
						break;
					case 1:
						musicMenu.update(cursorPos, bgm);
						break;
					case 2:
						illustrationMenu.update(cursorPos);
						break;
					default:
						break;
					}
				}

			}
		}
		else {
			exitWindow.update(cursorPos);

			if (exitWindow.yesClicked()) {
				System::Exit();
			}
			else if (exitWindow.noClicked()) {
				exitTimer.reset();
			}

			if (cursorPos and MouseL.down()) {
				exitTimer.reset();
				cursorPos.use();
			}

		}
	}

	void draw() {
		if (not exitTimer.isStarted()) {

			{
				ScopedRenderTarget2D target{ wholeSceneRenderTexture.from.clear(UI::BackgroundColor) };


				{
					Transformer2D t(Mat3x2::Translate(0, UI::menuBarHeight), TransformCursor::Yes);
					switch (menuSelector.getSelectMenuIndex())
					{
					case 0:
						gameMenu.draw();
						break;
					case 1:
						musicMenu.draw(bgm);
						break;
					case 2:
						illustrationMenu.draw();
						break;
					default:
					{
						FontAsset(U"GenreList")(U"Commig soon...").drawAt(50, Scene::Center(), Palette::Black);
					}
					break;
					}
				}

				menuBar.drawShadow(Vec2(0, 3), 8);
				menuBar.draw(ColorF(0.9));

				menuSelector.draw(FontAsset(U"Game.Desc"));

				musicControlPanel.draw(*this);
				speakerButton.draw();
				volumeSlider.draw();


				exitButton.draw(Palette::Salmon);
			}
			Graphics2D::Flush();
			wholeSceneRenderTexture.from.resolve();
			wholeSceneRenderTexture.from.draw();
		}
		else {
			double t = exitTimer.sF() * 10;
			double blurStrength = (t < 1 ? EaseOutQuad(t) : 1) * 16;

			wholeSceneRenderTexture.blur(blurStrength);
			wholeSceneRenderTexture.to.draw();

			Scene::Rect().draw(ColorF(0, Min(exitTimer.sF(), 0.2)));

			exitWindow.draw();
		}
	}
};


