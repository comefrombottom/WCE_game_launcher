#pragma once
#include "Common.h"
#include "SingleUseCursorPos.h"


struct ScrollBar
{
	RectF rect{};
	Optional<double> dragOffset;

	double viewHeight = 600;
	double pageHeight = 1000;
	double viewTop = 0;
	double viewVelocity = 0;

	double accumulateTime = 0;
	static constexpr double stepTime = 1.0 / 200;
	static constexpr double resistance = 10;

	Transition sliderWidthTransition = Transition(0.1s, 0.1s);

	ScrollBar() = default;

	ScrollBar(const RectF& rect, double viewHeight, double pageHeight)
		: rect(rect)
		, viewHeight(viewHeight)
		, pageHeight(pageHeight)
	{

	}

	double sliderHeight() const
	{
		return Max(rect.h * viewHeight / pageHeight, 20.0);
	}

	double sliderYPerViewY() const
	{
		return (rect.h - sliderHeight()) / (pageHeight - viewHeight);
	}

	double sliderY() const
	{
		return viewTop * sliderYPerViewY();
	}

	RectF sliderRect() const
	{
		return RectF(rect.x, rect.y + sliderY(), rect.w, sliderHeight());
	}

	bool existSlider() const
	{
		return viewHeight < pageHeight;
	}

	bool isSliderMouseOver(SingleUseCursorPos& cursorPos) const
	{
		return cursorPos and sliderRect().stretched(5).mouseOver();
	}

	bool isSliderThick(SingleUseCursorPos& cursorPos) const
	{
		return isSliderMouseOver(cursorPos) || dragOffset;
	}

	Transformer2D createTransformer() const
	{
		return Transformer2D(Mat3x2::Translate(0, -viewTop), TransformCursor::Yes);
	}

	void scrollBy(double h) {
		viewVelocity += resistance * h;
	}

	void scrollTopTo(double y) {
		scrollBy(y - viewTop);
	}

	void scrollBottomTo(double y) {
		scrollBy(y - viewTop - viewHeight);
	}

	void scrollCenterTo(double y) {
		scrollBy(y - viewTop - viewHeight / 2);
	}

	void update(SingleUseCursorPos& cursorPos, double wheel = Mouse::Wheel(), double delta = Scene::DeltaTime())
	{
		if (not existSlider()) {
			viewTop = 0;
			viewVelocity = 0;
			dragOffset.reset();
			sliderWidthTransition.reset();
			return;
		}

		for (accumulateTime += delta; accumulateTime >= stepTime; accumulateTime -= stepTime)
		{
			if (not dragOffset) {
				viewTop += viewVelocity * stepTime;
			}

			if (viewVelocity != 0)
			{
				viewVelocity += -viewVelocity * stepTime * resistance;
			}
		}

		if (dragOffset)
		{
			const double prevTop = viewTop;
			viewTop = (Cursor::PosF().y - *dragOffset) / sliderYPerViewY();
			viewVelocity = (viewTop - prevTop) / delta;
		}


		if (isSliderMouseOver(cursorPos) and MouseL.down())
		{
			dragOffset = Cursor::PosF().y - sliderY();
			cursorPos.dragOn();
		}
		else if (dragOffset && MouseL.up())
		{
			dragOffset.reset();
			cursorPos.dragOff();
		}

		if (wheel) {
			viewVelocity = wheel * 2000;
		}

		if (viewTop < 0)
		{
			viewTop = 0;
			viewVelocity = 0;
		}
		else if (viewTop + viewHeight > pageHeight)
		{
			viewTop = pageHeight - viewHeight;
			viewVelocity = 0;
		}

		sliderWidthTransition.update(isSliderThick(cursorPos));

		if (isSliderMouseOver(cursorPos)) {
			cursorPos.use();
		}

	}

	void draw(const ColorF& color = Palette::Dimgray) const
	{
		if (not existSlider()) return;

		double w = rect.w * (sliderWidthTransition.value() * 0.5 + 0.5);

		RectF(rect.x - w + rect.w, rect.y + sliderY(), w, sliderHeight()).rounded(rect.w / 2).draw(color);
	}

	double progress0_1() {
		return viewTop / (pageHeight - viewHeight);
	}
};


class MenuSelector {
public:
	struct Menu {
		Texture icon;
		String name;
		Color color;

		bool mouseOvered = false;
		Transition mouseOverTransition = Transition(0.1s, 0.1s);

		bool pressed = false;
		Transition pressedTransition = Transition(0.1s, 0.1s);

		Menu(const Texture& icon, const String& name, const Color& color)
			:icon(icon)
			, name(name)
			, color(color)
		{
		}
	};
private:

	Array<Menu> menus;
	size_t selectMenuIndex = 0;

	double selectedLineCenterX = 0;
	double selectedLineVelocity = 0;

	double selectedTime = 10;

	Vec2 offset = Vec2(10, 5);
	Vec2 menuRectSize = { 160,45 };
	double gap = 10;
public:
	MenuSelector() = default;

	MenuSelector(const Array<Menu>& menus, const Vec2& offset = Vec2(10, 5), const Vec2 menuRectSize = { 160,45 }, double gap = 10)
		: menus(menus), offset(offset), menuRectSize(menuRectSize), gap(gap)
	{
	}

	RectF getRect(size_t i) const {
		return RectF(offset.x + i * (menuRectSize.x + gap), offset.y, menuRectSize);
	}

	size_t getSelectMenuIndex() const {
		return selectMenuIndex;
	}


	void update(SingleUseCursorPos& cursorPos) {

		for (auto [i, menu] : IndexedRef(menus)) {


			RectF rect = getRect(i);


			menu.mouseOvered = cursorPos and rect.mouseOver();
			if (menu.mouseOvered) {
				cursorPos.use();
			}
			menu.mouseOverTransition.update(menu.mouseOvered);


			menu.pressed = menu.mouseOvered and MouseL.pressed();
			menu.pressedTransition.update(menu.pressed);

			if (menu.mouseOvered and MouseL.down()) {
				if (i != selectMenuIndex) {
					selectedTime = 0;
					selectMenuIndex = i;
				}
			}
		}
		selectedTime += Scene::DeltaTime();

		selectedLineCenterX = Math::SmoothDamp(selectedLineCenterX, selectMenuIndex * 1.0, selectedLineVelocity, 0.1);
	}

	void draw(const Font& font, double fontSize) const {

		for (auto [i, menu] : Indexed(menus)) {

			RectF rect = getRect(i);
			auto dtext = font(menu.name);
			constexpr double buffer = 10;
			Vec2 iconPos = rect.center().movedBy(-dtext.region(fontSize).w / 2 - buffer / 2, 0);
			Vec2 textPos = rect.center().movedBy(menu.icon.width() / 2 + buffer / 2, 0);

			Color textColor = ColorF(0.6);

			if (menu.mouseOvered) {
				rect.stretched(menu.mouseOverTransition.value() - 1).rounded(5)
					.drawShadow(Vec2(0, 2), 5, 3, ColorF(0.7, menu.mouseOverTransition.value() * 0.5))
					.draw(ColorF(1, menu.mouseOverTransition.value()));
				textColor = ColorF(0.6).lerp(menu.color, menu.mouseOverTransition.value());
			}
			else {
				rect.stretched(menu.mouseOverTransition.value() - 1).rounded(5)
					.drawShadow(Vec2(0, 2), 5, 3, ColorF(0.7, menu.mouseOverTransition.value() * 0.5))
					.draw(ColorF(1, menu.mouseOverTransition.value()))
					.drawFrame(1, ColorF(0.7, menu.mouseOverTransition.value()));

			}

			if (selectMenuIndex == i) {
				Color rectColor = ColorF(1);
				rectColor = rectColor.lerp(ColorF(0.95), menu.pressedTransition.value());


				rect.rounded(5).draw(rectColor);
				textColor = menu.color;
			}
			if (selectedTime < 1.0 && selectMenuIndex == i) {
				menu.icon.rotated(exp(-7 * selectedTime) * sin(15 * selectedTime) * 0.9).drawAt(iconPos, textColor);
			}
			else {
				menu.icon.drawAt(iconPos, textColor);
			}
			dtext.drawAt(fontSize, textPos, textColor);
		}





		Vec2 selectedLineCenter = Vec2(offset.x + selectedLineCenterX * (menuRectSize.x + gap) + menuRectSize.x / 2, offset.y + menuRectSize.y + 5);

		Color color;
		{
			size_t left = static_cast<size_t>(Max(0.0, Floor(selectedLineCenterX)));
			double t = selectedLineCenterX - left;
			Color leftColor = left < menus.size() ? menus[left].color : Palette::Black;
			Color rightColor = left + 1 < menus.size() ? menus[left + 1].color : Palette::Black;

			color = leftColor.lerp(rightColor, t);
		}

		RectF(Arg::center = selectedLineCenter, menuRectSize.x * 0.9, 4).rounded(2).draw(color);
	}

	void draw(const Font& font) const {
		draw(font, font.fontSize());
	}
};

class VolumeSlider {
	double m_value = 0;
	double min = 0;
	double max = 1;
	Vec2 pos{};
	double width = 200;
	static constexpr double height = 10;
	static constexpr double knobRadius = 15;

	bool mouseOverKnob = false;
	Transition knobFloatTransition = Transition(0.1s, 0.1s);
	Transition knobPressedTransition = Transition(0.1s, 0.1s);

	Optional<double> dragOffset;

	Optional<double> warpPos;

public:
	VolumeSlider() = default;
	VolumeSlider(double firstValue, double min, double max, Vec2 pos, double width)
		:m_value(firstValue), min(min), max(max), pos(pos), width(width)
	{
	}

	double value() const {
		return m_value;
	}

	double value0_1() const {
		return (m_value - min) / (max - min);
	}

	bool isDragging() const {
		return dragOffset.has_value();
	}

	double valueTo0_1(double value) const {
		return (value - min) / (max - min);
	}

	Circle knob() const {
		return Circle(pos.x + width * value0_1(), pos.y, knobRadius);
	}

	double xToClampValue(double x) const {
		double value01 = (x - pos.x) / width;
		double v = value01 * (max - min) + min;
		return Clamp(v, min, max);
	}

	double valueToX(double value) const {
		return value0_1() * width + pos.x;
	}

	bool update(SingleUseCursorPos& cursorPos) {
		double prevValue = m_value;

		mouseOverKnob = cursorPos and knob().stretched(5).mouseOver();
		if (mouseOverKnob) {
			cursorPos.use();
		}

		if (mouseOverKnob and MouseL.down()) {
			dragOffset = Cursor::PosF().x - knob().x;
			cursorPos.dragOn();
		}
		if (dragOffset and MouseL.up()) {
			dragOffset.reset();
			cursorPos.dragOff();
		}

		if (dragOffset) {
			double knobX = Cursor::PosF().x - *dragOffset;
			m_value = xToClampValue(knobX);
		}


		if (cursorPos and not mouseOverKnob and not dragOffset and RectF(pos.x, pos.y - height / 2, width, height).stretched(height).mouseOver()) {
			warpPos = xToClampValue(Cursor::PosF().x);
		}
		else {
			warpPos.reset();
		}

		if (warpPos and MouseL.down()) {
			m_value = *warpPos;
			dragOffset = 0;
			cursorPos.dragOn();
		}

		knobFloatTransition.update(mouseOverKnob or dragOffset);
		knobPressedTransition.update(bool(dragOffset));

		return prevValue != m_value;
	}

	void draw(const Color& color = Palette::Hotpink) {
		RectF(pos.x, pos.y - height / 2, width, height).rounded(height / 2).draw(ColorF(0.8));
		RectF(pos.x, pos.y - height / 2, width * value0_1(), height).rounded(height / 2).draw(color);
		//constexpr double buf = 5;
		//RectF brightRect(pos.x + buf, pos.y - height * 0.4, width * value0_1() - buf * 2, height * 0.4);
		//brightRect.rounded(brightRect.h / 2).draw(ColorF(1, 0.1));

		if (warpPos) {
			if (*warpPos < m_value) {
				RectF(pos.x, pos.y - height / 2, width * (valueTo0_1(*warpPos)), height).rounded(height / 2).draw(ColorF(0.5, 0.2));
			}
			else {
				RectF(pos.x + value0_1() * width, pos.y - height / 2, width * (valueTo0_1(*warpPos) - value0_1()), height).rounded(height / 2).draw(ColorF(0.5, 0.2));
			}
		}

		knob().stretched(knobFloatTransition.value()).drawShadow({ 0,2 }, 5, 2, ColorF(0.7, knobFloatTransition.value() * 0.3)).draw(ColorF(1).lerp(ColorF(0.95), knobPressedTransition.value()));
	}
};

class SpeakerButton {
	RectF m_rect;
	bool mute = false;

	bool mouseOvered = false;
	Transition mouseOverTransition = Transition(0.1s, 0.1s);

	bool pressed = false;
	Transition pressedTransition = Transition(0.1s, 0.1s);

	double timeAfterChange = 10;

	Texture speakerTexture;
	Texture muteTexture;

public:
	SpeakerButton(const RectF& rect, bool mute = false) :m_rect(rect), mute(mute)
	{
		speakerTexture = Texture(0xf028_icon, rect.h * 0.75);
		muteTexture = Texture(0xf6a9_icon, rect.h * 0.75);
	}

	bool isMute() const {
		return mute;
	}

	bool setMute(bool m) {
		bool prevMute = mute;
		mute = m;
		if (prevMute != mute) {
			timeAfterChange = 0;
		}
		return prevMute != mute;
	}

	bool update(SingleUseCursorPos& cursorPos) {
		bool prevMute = mute;
		mouseOvered = cursorPos and m_rect.mouseOver();
		if (mouseOvered) {
			cursorPos.use();
		}

		mouseOverTransition.update(mouseOvered);

		pressed = mouseOvered and MouseL.pressed();
		pressedTransition.update(pressed);

		timeAfterChange += Scene::DeltaTime();

		if (mouseOvered and MouseL.down()) {
			mute = not mute;
			timeAfterChange = 0;
		}

		return prevMute != mute;
	}

	void draw() {

		RoundRect rrect = m_rect.stretched(mouseOverTransition.value() - 1).rounded(5);

		rrect.drawShadow(Vec2(0, 2), 5, 3, ColorF(0.7, mouseOverTransition.value() * 0.5));
		rrect.draw(ColorF(1 - pressedTransition.value() * 0.05, mouseOverTransition.value()));

		if (not mouseOvered) {
			rrect.drawFrame(1, ColorF(0.7, mouseOverTransition.value()));
		}

		if (mute) {
			muteTexture.rotated(exp(-7 * timeAfterChange) * sin(30 * timeAfterChange) * 0.5)
				.drawAt(m_rect.center(), ColorF(0.6).lerp(Palette::Hotpink, mouseOverTransition.value()));
		}
		else {
			speakerTexture.rotated(exp(-7 * timeAfterChange) * sin(30 * timeAfterChange) * 0.5)
				.drawAt(m_rect.center(), ColorF(0.6).lerp(Palette::Hotpink, mouseOverTransition.value()));
		}

	}
};

class TextButton {
	RoundRect m_roundRect;
	String m_text;

	bool mouseOvered = false;
	double mouseOverTransition = 0;
	double mouseOverTransitionVelocity = 0;

	bool pressed = false;
	double pressedTransition = 0;
	double pressedTransitionVelocity = 0;

	bool clicked = false;

public:

	TextButton(const RectF& rect, const String& text)
		:m_roundRect(rect.rounded(rect.h / 4))
		, m_text(text)
	{
	}

	bool update(SingleUseCursorPos& cursorPos) {
		mouseOvered = cursorPos and m_roundRect.mouseOver();
		if (mouseOvered) {
			cursorPos.use();
		}
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = mouseOvered and MouseL.pressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		clicked = mouseOvered and MouseL.up();

		return clicked;
	}


	void draw(const ColorF& color, const ColorF& textColor, const Font& font, double fontSize) const {
		{
			Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition) * 0.05, m_roundRect.center()));

			m_roundRect.drawShadow(Vec2{ 0,2 }, m_roundRect.h / 4, 0, ColorF(0.5, mouseOverTransition - pressedTransition));
			makeSmoothShape(m_roundRect).draw(color).draw(ColorF(0.9, pressedTransition * 0.5));
		}

		font(m_text).drawAt(fontSize, m_roundRect.center(), textColor);
	}

	void draw(const ColorF& color, const ColorF& textColor, const Font& font) const {
		draw(color, textColor, font, font.fontSize());
	}
};

class IconButton {
	RoundRect m_roundRect;
	Texture m_texture;

	bool mouseOvered = false;
	double mouseOverTransition = 0;
	double mouseOverTransitionVelocity = 0;

	bool pressed = false;
	double pressedTransition = 0;
	double pressedTransitionVelocity = 0;

	bool clicked = false;

public:
	IconButton(const RectF& rect, const Texture& texture)
		:m_roundRect(rect.rounded(rect.h / 4))
		, m_texture(texture)
	{
	}

	bool update(SingleUseCursorPos& cursorPos) {
		mouseOvered = cursorPos and m_roundRect.mouseOver();
		if (mouseOvered) {
			cursorPos.use();
		}
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = mouseOvered and MouseL.pressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		clicked = mouseOvered and MouseL.up();

		return clicked;
	}

	void draw(const ColorF& color, const ColorF& texColor) {
		{
			Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition) * 0.05, m_roundRect.center()));

			m_roundRect.drawShadow(Vec2{ 0,2 }, m_roundRect.h / 4, 0, ColorF(0.5, mouseOverTransition - pressedTransition));
			makeSmoothShape(m_roundRect).draw(color).draw(ColorF(0.9, pressedTransition * 0.5));
		}

		m_texture.drawAt(m_roundRect.center(), texColor);
	}
};

class ColorIconButton {
	RectF m_rect;

	bool mouseOvered = false;
	Transition mouseOverTransition = Transition(0.1s, 0.1s);

	bool pressed = false;
	Transition pressedTransition = Transition(0.1s, 0.1s);

	double timeAfterChange = 10;
	Texture m_texture;

public:
	ColorIconButton(const RectF& rect, const Texture& texture) :m_rect(rect), m_texture(texture)
	{
	}

	bool update(SingleUseCursorPos& cursorPos) {
		mouseOvered = cursorPos and m_rect.mouseOver();
		if (mouseOvered) {
			cursorPos.use();
		}

		mouseOverTransition.update(mouseOvered);

		pressed = mouseOvered and MouseL.pressed();
		pressedTransition.update(pressed);

		timeAfterChange += Scene::DeltaTime();

		bool clicked = mouseOvered and MouseL.up();
		if (clicked) {
			timeAfterChange = 0;
		}

		return clicked;
	}

	void draw(const ColorF& color) {

		RoundRect rrect = m_rect.stretched(mouseOverTransition.value() - 1).rounded(5);

		rrect.drawShadow(Vec2(0, 2), 5, 3, ColorF(0.7, mouseOverTransition.value() * 0.5));
		rrect.draw(ColorF(1 - pressedTransition.value() * 0.05, mouseOverTransition.value()));

		if (not mouseOvered) {
			rrect.drawFrame(1, ColorF(0.7, mouseOverTransition.value()));
		}

		m_texture.drawAt(m_rect.center(), ColorF(0.6).lerp(color, mouseOverTransition.value()));

	}
};

class SideGenreList {
	struct Genre {
		Texture icon;
		String name;

		Genre(const Texture& icon, const String& name)
			:icon(icon)
			, name(name)
		{
		}

	};

	Array<Genre> genres;
	size_t selectGenreIndex = 0;
	size_t prevSelectGenreIndex = 0;

	Vec2 offset{};
	Vec2 oneGenreSize{};

	Optional<size_t> mouseOveredIndex = 0;

	double selectedLinePos = 0;
	double selectedLineVelocity = 0;

	constexpr static double selectedLineWidth = 6;
	constexpr static double selectedLineHeightRatio = 0.5;

	double timeAfterChangeIndex = 0;

public:

	SideGenreList() = default;

	SideGenreList(const Array<Genre>& genres, const Vec2& offset, const Vec2& oneGenreSize)
		:genres(genres)
		, offset(offset)
		, oneGenreSize(oneGenreSize)
	{
	}

	size_t index() const {
		return selectGenreIndex;
	}

	bool update(SingleUseCursorPos& cursorPos) {
		size_t prevSelect = selectGenreIndex;

		mouseOveredIndex.reset();
		for (auto [i, genre] : IndexedRef(genres)) {
			RectF rect = RectF(offset, oneGenreSize).movedBy(0, i * oneGenreSize.y);

			bool mouseOvered = false;
			if (cursorPos and rect.mouseOver()) {
				mouseOvered = true;
				mouseOveredIndex = i;
				cursorPos.use();
			}

			if (mouseOvered and MouseL.down()) {
				prevSelectGenreIndex = selectGenreIndex;
				selectGenreIndex = i;
			}

		}

		timeAfterChangeIndex += Scene::DeltaTime();
		if (prevSelect != selectGenreIndex) {
			timeAfterChangeIndex = 0;
		}
		selectedLinePos = Math::SmoothDamp(selectedLinePos, selectGenreIndex * 1.0, selectedLineVelocity, 0.1);
		return prevSelect != selectGenreIndex;
	}

	void draw(const Font& font, double fontSize, const Color& color) const {
		for (auto [i, genre] : Indexed(genres)) {
			auto rect = RectF(offset, oneGenreSize).stretched(0, -2).movedBy(0, i * oneGenreSize.y).rounded(5);


			if (selectGenreIndex == i) {
				rect.draw(ColorF(0.5, 0.1));
			}

			if (mouseOveredIndex && *mouseOveredIndex == i) {
				rect.draw(ColorF(0.5, 0.1));
			}

			constexpr double gap1 = 30;
			double iconSizeX = genre.icon.width();
			genre.icon.draw(Arg::leftCenter(offset.x + gap1, offset.y + (0.5 + i) * oneGenreSize.y), ColorF(0.4));

			constexpr double gap2 = 20;
			font(genre.name).draw(fontSize, Arg::leftCenter(offset.x + gap1 + iconSizeX + gap2, offset.y + (0.5 + i) * oneGenreSize.y), ColorF(0.4));

			//Line{ 20, (i + 1) * oneGenreSize.y, oneGenreSize.x, (i + 1) * oneGenreSize.y }.movedBy(offset).draw(1, ColorF(0.7));
		}


		double selectedLineBegin01 = Math::Lerp<double, double, double>(static_cast<double>(prevSelectGenreIndex), static_cast<double>(selectGenreIndex), EaseInOutQuart(Min(timeAfterChangeIndex * 3, 1.0)));
		double selectedLineEnd01 = Math::Lerp<double, double, double>(static_cast<double>(prevSelectGenreIndex), static_cast<double>(selectGenreIndex), EaseOutQuart(Min(timeAfterChangeIndex * 3, 1.0)));

		if (prevSelectGenreIndex > selectGenreIndex) {
			std::swap(selectedLineBegin01, selectedLineEnd01);
		}

		RectF(offset.x + 5, offset.y + (selectedLineBegin01 + (1 - selectedLineHeightRatio) / 2) * oneGenreSize.y, selectedLineWidth, (selectedLineEnd01 + selectedLineHeightRatio - selectedLineBegin01) * oneGenreSize.y).rounded(selectedLineWidth / 2).draw(color);

		//RectF(Arg::leftCenter(offset.x + 5, offset.y+(selectedLinePos+0.5) * oneGenreSize.y), selectedLineWidth, oneGenreSize.y*0.8).rounded(selectedLineWidth/2).draw(Palette::Hotpink);
	}

	void draw(const Font& font, const Color& color) const {
		draw(font, font.fontSize(), color);
	}

};

class ExitWindow {
	RoundRect m_roundRect;
	TextButton m_yesButton;
	TextButton m_noButton;
	String m_text;

	bool m_yesClicked = false;
	bool m_noClicked = false;



public:
	ExitWindow(const Vec2& center)
		:m_roundRect(RectF(Arg::center = center, 400, 250), 50)
		, m_yesButton(RectF(Arg::center = center.movedBy(90, 70), 140, 60), U"終了")
		, m_noButton(RectF(Arg::center = center.movedBy(-90, 70), 140, 60), U"キャンセル")
		, m_text(U"終了しますか？")
	{
	}

	bool update(SingleUseCursorPos& cursorPos) {

		m_yesClicked = m_yesButton.update(cursorPos);
		m_noClicked = m_noButton.update(cursorPos);

		if (cursorPos and m_roundRect.mouseOver()) {
			cursorPos.use();
		}

		return m_yesClicked or m_noClicked;
	}

	bool yesClicked() const {
		return m_yesClicked;
	}

	bool noClicked() const {
		return m_noClicked;
	}

	void draw() const {
		makeSmoothShape(m_roundRect).draw(ColorF(1, 0.8));
		m_yesButton.draw(Palette::Tomato, Palette::White, FontAsset(U"Game.Desc"));
		m_noButton.draw(Palette::White, Palette::Dimgray, FontAsset(U"Game.Desc"));

		FontAsset(U"Game.Desc")(m_text).drawAt(m_roundRect.center().movedBy(0, -20), Palette::Dimgray);
	}
};

class PlayButton {
	RectF m_rect;
	Texture m_playTexture;
	Texture m_pauseTexture;

	bool mouseOvered = false;
	double mouseOverTransition = 0;
	double mouseOverTransitionVelocity = 0;

	bool pressed = false;
	double pressedTransition = 0;
	double pressedTransitionVelocity = 0;

	bool clicked = false;

	bool m_isPlaying = false;

public:

	PlayButton(const RectF& rect)
		:m_rect(rect)
		, m_playTexture(0xf04b_icon, 100)
		, m_pauseTexture(0xf04c_icon, 100)
	{
	}

	bool update(SingleUseCursorPos& cursorPos, Audio& bgm) {
		m_isPlaying = bgm.isPlaying();
		mouseOvered = cursorPos and m_rect.mouseOver();
		if (mouseOvered) {
			cursorPos.use();
		}
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.05);

		pressed = m_rect.leftPressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);


		clicked = mouseOvered and MouseL.up();

		if (clicked) {
			m_isPlaying = not m_isPlaying;
			if (m_isPlaying) {
				bgm.play();
			}
			else {
				bgm.pause();
			}
		}

		return clicked;
	}

	bool isPlaying() const {
		return m_isPlaying;
	}

	void draw(const ColorF& color = Palette::White) {

		//m_rect.drawFrame(1, ColorF(0.7, mouseOverTransition));

		Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition) * 0.07, m_rect.center()));

		if (m_isPlaying) {
			m_pauseTexture.drawAt(m_rect.center(), color);
		}
		else {
			m_playTexture.drawAt(m_rect.center().movedBy(8, 0), color);
		}
	}
};

class OnlyIconButton {
	RectF m_rect;
	Texture m_texture;

	bool mouseOvered = false;
	double mouseOverTransition = 0;
	double mouseOverTransitionVelocity = 0;

	bool pressed = false;
	double pressedTransition = 0;
	double pressedTransitionVelocity = 0;

	bool clicked = false;

public:
	OnlyIconButton() = default;
	OnlyIconButton(const RectF& rect, const Texture& texture)
		:m_rect(rect)
		, m_texture(texture)
	{
	}

	bool update(SingleUseCursorPos& cursorPos) {
		mouseOvered = cursorPos and m_rect.mouseOver();
		if (mouseOvered) {
			cursorPos.use();
		}
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = mouseOvered and MouseL.pressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);



		clicked = mouseOvered and MouseL.up();


		return clicked;
	}

	void draw(const ColorF& color = Palette::White) {
		Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition) * 0.07, m_rect.center()));

		m_texture.drawAt(m_rect.center(), color);
	}
};

class SeekBar {
	RectF m_rect;
	double m_value = 0;//0-1

	bool mouseOvered = false;
	double mouseOverTransition = 0;
	double mouseOverTransitionVelocity = 0;

	bool pressed = false;
	double pressedTransition = 0;
	double pressedTransitionVelocity = 0;

	Optional<double> dragOffset;

	bool m_dragReleased = false;

public:
	SeekBar(const RectF& rect)
		:m_rect(rect)
	{
	}

	bool update(SingleUseCursorPos& cursorPos, Audio& bgm) {
		m_value = double(bgm.posSample()) / bgm.samples();

		mouseOvered = (cursorPos and m_rect.stretched(20).mouseOver());
		if (mouseOvered) {
			cursorPos.use();
		}

		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered or dragOffset.has_value(), mouseOverTransitionVelocity, 0.03);

		pressed = dragOffset.has_value();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		if (dragOffset) {
			m_value = Clamp((Cursor::PosF().x - m_rect.x - *dragOffset) / (m_rect.w - m_rect.h), 0.0, 1.0);
		}

		if (mouseOvered and MouseL.down()) {
			dragOffset = Cursor::PosF().x - m_rect.x - (m_rect.w - m_rect.h) * m_value;
			cursorPos.dragOn();
		}

		m_dragReleased = false;
		if (dragOffset and MouseL.up()) {
			m_dragReleased = true;
			dragOffset.reset();
			cursorPos.dragOff();
		}


		if (m_dragReleased) {
			bgm.seekSamples(m_value * bgm.samples());
		}


		return false;
	}

	bool isDragging() const {
		return dragOffset.has_value();
	}

	bool dragReleased() const {
		return m_dragReleased;
	}

	double value() const {
		return m_value;
	}



	void draw(const ColorF& color = Palette::White, const ColorF& color2 = Palette::Dimgray) {
		Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition) * 0.03, m_rect.center()));

		m_rect.rounded(m_rect.h / 2).draw(color);
		RectF(m_rect.x, m_rect.y, (m_rect.w - m_rect.h) * m_value + m_rect.h, m_rect.h).rounded(m_rect.h / 2).draw(color2);
	}

};

class IconSwitch {
	RectF m_rect;
	Texture m_texture;

	bool m_on = false;
	double m_onTransition = 0;
	double m_onTransitionVelocity = 0;
	double m_onTransitionSlow = 0;
	double m_onTransitionSlowVelocity = 0;


	bool mouseOvered = false;
	double mouseOverTransition = 0;
	double mouseOverTransitionVelocity = 0;

	bool pressed = false;
	double pressedTransition = 0;
	double pressedTransitionVelocity = 0;

	bool clicked = false;

public:
	IconSwitch(const RectF& rect, const Texture& texture, bool on = false)
		:m_rect(rect)
		, m_texture(texture)
		, m_on(on)
	{
	}

	bool update(SingleUseCursorPos& cursorPos) {
		mouseOvered = cursorPos and m_rect.mouseOver();
		if (mouseOvered) {
			cursorPos.use();
		}
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = mouseOvered and MouseL.pressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);


		clicked = mouseOvered and MouseL.up();

		if (clicked) {
			m_on = not m_on;
		}

		m_onTransition = Math::SmoothDamp(m_onTransition, m_on, m_onTransitionVelocity, 0.1);
		m_onTransitionSlow = Math::SmoothDamp(m_onTransitionSlow, m_on, m_onTransitionSlowVelocity, 0.25);

		return clicked;
	}

	bool isOn() const {
		return m_on;
	}

	void draw() {

		RoundRect body = m_rect.rounded(m_rect.h / 5);

		auto updownFunc = [](double t) {
			return Max(0.0, (1 - t) * t * (t + 1));
			};

		double updown = updownFunc(m_onTransitionSlow * m_on);

		Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition + updown * 3) * 0.07, m_rect.center()));
		body.drawShadow(Vec2(0, 2), 5, 3, ColorF(0.7, m_onTransition * 0.5));
		Transformer2D scaler2(Mat3x2::Scale(1 + updown * 3 * 0.07, m_rect.center()).translated(0, -m_rect.h * 0.2 * updown));

		body.drawFrame(1, ColorF(0.8));

		body.draw(ColorF(1, m_onTransition));

		m_texture.drawAt(m_rect.center().movedBy(0, 0), ColorF(0.6).lerp(Palette::Hotpink, m_onTransition));

	}
};
