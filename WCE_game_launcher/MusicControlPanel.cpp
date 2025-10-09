#include "stdafx.h"
#include "MusicControlPanel.h"
#include "Master.h"



void MusicControlPanel::update(SingleUseCursorPos& cursorPos, Master& master)
{
	auto roundRectWhenClose = RoundRect(RectF{ m_pos,sizeWhenClose }, sizeWhenClose.y / 5);
	auto roundRectWhenOpen = RoundRect(RectF{ Arg::topCenter = roundRectWhenClose.rect.topCenter(),sizeWhenOpen }, sizeWhenOpen.y / 5);

	auto roundRect = m_open ? roundRectWhenOpen : roundRectWhenClose;

	m_mouseOvered = cursorPos and roundRect.stretched(5).mouseOver() or seekBar.isDragging();

	m_open = m_mouseOvered;
	m_openTransition = Math::SmoothDamp(m_openTransition, m_open ? 1.0 : 0.0, m_openTransitionVel, 0.1);


	if (master.musicMenu.m_nowPlayingMusic) {

		if (m_open) {
			Transformer2D tf(Mat3x2::Translate(roundRectWhenOpen.topCenter()), TransformCursor::Yes);

			playButton.update(cursorPos, master.bgm);

			if (forwardButton.update(cursorPos)) {
				master.musicMenu.goForwardMusic(master.bgm);
			}
			if (backButton.update(cursorPos)) {
				master.bgm.seekSamples(0);
			}

			seekBar.update(cursorPos, master.bgm);

			if (shuffleSwitch.update(cursorPos)) {
				if (shuffleSwitch.isOn()) {
					master.musicMenu.m_nextMusicMode = NextMusicMode::random;
				}
				else {
					master.musicMenu.m_nextMusicMode = NextMusicMode::next;
				}
			}
		}
		else {

		}
	}

	if (cursorPos and m_mouseOvered) {
		cursorPos.use();
	}
}

void MusicControlPanel::draw(const Master& master)
{

	auto roundRectWhenClose = RoundRect(RectF{ m_pos,sizeWhenClose }, sizeWhenClose.y / 5);
	auto roundRectWhenOpen = RoundRect(RectF{ Arg::topCenter = roundRectWhenClose.rect.topCenter(),sizeWhenOpen }, sizeWhenOpen.y / 5);

	auto roundRect = roundRectWhenClose.lerp(roundRectWhenOpen, m_openTransition);


	roundRect.drawShadow({ 0,5 }, 10, 10, ColorF(0, 0.1));
	makeSmoothShape(roundRect).draw(ColorF(1));

	constexpr RoundRect musicHugeIconWhenClose = RectF(-280, 5, 40, 40).rounded(40.0 / 5);
	constexpr RoundRect musicHugeIconWhenOpen = RectF(-320, 40, 130, 130).rounded(20);

	if (master.musicMenu.m_nowPlayingMusic) {

		const Texture* texture = nullptr;
		const auto& music = master.musicMenu.musicJson[U"musicTable"][master.musicMenu.m_nowPlayingMusic->id];
		if (music.hasElement(U"albums") and music[U"albums"].size() > 0) {
			texture = &master.musicMenu.albumImages.at(music[U"albums"][0].getString());
		}

		if (m_open) {
			Transformer2D tf(Mat3x2::Translate(roundRectWhenOpen.topCenter()), TransformCursor::Yes);

			auto musicHugeIcon = musicHugeIconWhenClose.lerp(musicHugeIconWhenOpen, m_openTransition);

			musicHugeIcon.drawShadow({ 0,2 }, 5, 3, ColorF(0.7, 0.3));
			if (texture) {
				// musicHugeIcon(*texture).draw();
				makeSmoothShape(musicHugeIcon).toBuffer2D(musicHugeIcon.rect.pos, texture->size() * (musicHugeIcon.rect.size / texture->size()).maxComponent()).draw(*texture);
			}
			else
			{
				musicHugeIcon.draw(ColorF(1));
			}

			FontAsset(U"GenreList")(music[U"title"].getString()).draw(Math::Lerp(25, 30, m_openTransition), Arg::leftCenter(Vec2{ -230, 17 }.lerp(Vec2{ -170, 70 }, m_openTransition)), ColorF(0.2));
			FontAsset(U"Regular")(music[U"artist"].getString()).draw(Math::Lerp(15, 20, m_openTransition), Arg::leftCenter(Vec2{ -230, 40 }.lerp(Vec2{ -170, 120 }, m_openTransition)), ColorF(0.2));

			//FontAsset(U"GenreList")(music[U"title"].getString()).draw(30, Arg::leftCenter(-170, 70), ColorF(0.2));
			//FontAsset(U"Regular")(music[U"artist"].getString()).draw(20, Arg::leftCenter(-170, 120), ColorF(0.2));

			Transformer2D tf2(Mat3x2::Translate(-Vec2(0, 300) * (1 - m_openTransition)), TransformCursor::Yes);

			ScopedColorMul2D colorMul(ColorF(1, m_openTransition));


			playButton.draw(ColorF(0.2, 0.5));
			forwardButton.draw(ColorF(0.2, 0.5));
			backButton.draw(ColorF(0.2, 0.5));
			seekBar.draw(ColorF(0.5, 0.5), ColorF(0.2, 0.5));
			double seconds = seekBar.value() * master.bgm.lengthSec();
			String time_tex = U"{:0>2}:{:0>2}"_fmt((int)seconds / 60, (int)seconds % 60);
			FontAsset(U"Regular")(time_tex).draw(30, Arg::rightCenter(0 - 220, 450), ColorF(0.2));
			double seconds_all = master.bgm.lengthSec();
			String time_tex_all = U"{:0>2}:{:0>2}"_fmt((int)seconds_all / 60, (int)seconds_all % 60);
			FontAsset(U"Regular")(time_tex_all).draw(30, Arg::leftCenter(0 + 220, 450), ColorF(0.2));

			shuffleSwitch.draw();


		}
		else {
			Transformer2D tf(Mat3x2::Translate(roundRectWhenClose.topCenter()), TransformCursor::Yes);

			auto musicHugeIcon = musicHugeIconWhenClose.lerp(musicHugeIconWhenOpen, m_openTransition);

			musicHugeIcon.drawShadow({ 0,2 }, 5, 3, ColorF(0.7, 0.3));
			if (texture) {
				// musicHugeIcon(*texture).draw();
				makeSmoothShape(musicHugeIcon).toBuffer2D(musicHugeIcon.rect.pos, texture->size() * (musicHugeIcon.rect.size / texture->size()).maxComponent()).draw(*texture);
			}
			else
			{
				musicHugeIcon.draw(ColorF(1));
			}

			FontAsset(U"GenreList")(music[U"title"].getString()).draw(Math::Lerp(25, 30, m_openTransition), Arg::leftCenter(Vec2{ -230, 17 }.lerp(Vec2{ -170, 70 }, m_openTransition)), ColorF(0.2));
			FontAsset(U"Regular")(music[U"artist"].getString()).draw(Math::Lerp(15, 20, m_openTransition), Arg::leftCenter(Vec2{ -230, 40 }.lerp(Vec2{ -170, 120 }, m_openTransition)), ColorF(0.2));


			//FontAsset(U"GenreList")(music[U"title"].getString()).draw(25, Arg::leftCenter(-230, 17), ColorF(0.2));
			//FontAsset(U"Regular")(music[U"artist"].getString()).draw(15, Arg::leftCenter(-230, 40), ColorF(0.2));

			ScopedColorMul2D colorMul(ColorF(1, 1 - m_openTransition));


		}

	}
}

