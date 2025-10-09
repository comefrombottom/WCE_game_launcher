#pragma once
#include "Common.h"
#include "UI.h"

class Master;
class MusicControlPanel {
	bool m_open = false;
	double m_openTransition = 0;
	double m_openTransitionVel = 0;

	Vec2 m_pos{};

	static constexpr Vec2 sizeWhenClose = { 600,50 };
	static constexpr Vec2 sizeWhenOpen = { 700,500 };

	bool m_mouseOvered = false;


	PlayButton playButton{ RectF(Arg::center(0,350), 100, 100) };
	OnlyIconButton forwardButton{ RectF(Arg::center(150,350), 100, 100), Texture{ 0xf04e_icon, 100 } };
	OnlyIconButton backButton{ RectF(Arg::center(-150,350), 100, 100), Texture{ 0xf04a_icon, 100 } };

	SeekBar seekBar{ RectF(Arg::center(0, 450), 400, 20) };

	IconSwitch shuffleSwitch{ RectF(220, 210, 100, 60), Texture{ 0xf074_icon, 50 },true };

public:
	MusicControlPanel() = default;
	MusicControlPanel(const Vec2& pos) : m_pos(pos) {}

	bool mouseOvered() const {
		return m_mouseOvered;
	}

	void update(SingleUseCursorPos& cursorPos, Master& master);

	void draw(const Master& master);

};
