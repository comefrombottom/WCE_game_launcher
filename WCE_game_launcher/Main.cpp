# include <Siv3D.hpp> // OpenSiv3D v0.6.15

// 起動設定
namespace Config
{
	// Web アプリを起動する際に使用する Web ブラウザのパス
# if SIV3D_PLATFORM(WINDOWS)

	const FilePath BrowserPath = U"C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe";

# elif SIV3D_PLATFORM(MACOS)

	const FilePath BrowserPath = U"/Applications/Google Chrome.app/Contents/MacOS/Google Chrome";

# endif
}

// UI オプション
namespace UI
{
	// ウィンドウのサイズ
	//constexpr Size WindowSize{ 1280, 640 };

	// ウィンドウのフレームを表示するか
	//constexpr bool Frameless = true;

	// タイルの基本サイズ
	//constexpr double TileSize = 250;
	//constexpr double TileSizeX = 370;
	constexpr double TileIntervalY = 236;

	constexpr Vec2 TileSize = Vec2(350, TileIntervalY-20);

	// 背景色
	constexpr ColorF BackgroundColor{ 0.85, 0.9, 0.95 };

	// タイル選択の色
	constexpr ColorF TileFrmaeColor{ 1.0, 0.7, 0.3 };

	constexpr Vec2 BaseTilePos{ 1705, 130 };

	constexpr RectF InfoArea{ 180, 780, 915, 155 };

	constexpr RectF StaffArea{ 180, 940, 915, 70 };

	constexpr RectF PlayButton{ 1100, 780, 220, 85 };

	constexpr ColorF PlayButtonColor{ 0.0, 0.67, 1.0 };

	constexpr ColorF EndButtonColor{ 1.0, 0.1, 0.1 };

	constexpr RectF ControlArea{ 1100, 870, 220, 140 };

	constexpr ColorF InfoAreaMouseOverColor{ 1.0, 0.95, 0.9 };

	constexpr ColorF TextColor{ 0.2 };

	constexpr double InfoAreaRound = 8.0;

	constexpr double ScreenAreaRound = 35.0;

	constexpr RectF ScreenArea{ 82, 12, 1370, 718 };

	constexpr int EndMinute = 30;

	constexpr RectF EndArea{ 100, 40, 1720, 880 };

	constexpr RectF CommentArea{ 300, 240, 1320, 480 };

	constexpr double menuBarHeight = 60;
}

// ゲームの情報
struct Game
{
	// ゲームのタイトル
	String title;

	// ゲーム実行ファイル または URL
	FilePath path;

	// Web ブラウザで起動
	bool isWebApp = false;

	// Web ブラウザで起動するHTMLゲーム
	bool isHTMLApp = false;

	//起動しないただの枠
	bool isPlaceHolder = false;

	//やめるとき用の枠
	bool isPlaceHolderEnd = false;

	// ゲームの画像
	Texture texture;

	//画像のスケール(スクリーン)
	//double scale_screen;

	//スクリーン用のファイル名
	FilePath screen_file;

	//スクリーン用動画
	//VideoTexture screen_video;

	//スクリーン用画像
	//Texture screen_image;

	//動画かどうか
	bool isVideo = false;

	// ゲームの説明文
	String desc;

	// ゲームの開発スタッフ
	String staff;

	// ゲームの開発ツール
	String tools;

	// マウスを使用するか
	bool useMouse = false;

	// キーボードを使用するか
	bool useKeyboard = false;

	// ゲームパッドを使用するか
	bool useGamepad = false;

	// ランチャー表示優先度（大きいほど優先）
	int32 priority = 0;

	double mouseOverScale = 1;
	double mouseOverScaleVel = 0;
	Vec2 tileMovePos{};
	Vec2 tileMovePosVel{};

};

// ゲームのパスが Web ページかどうかを調べる関数
bool IsURL(StringView path)
{
	return (path.starts_with(U"http://") || path.starts_with(U"https://"));
}

// ゲームが ブラウザ用かどうかを調べる関数(PICO-8用)
bool IsHTML(StringView path)
{
	return path.ends_with(U".html");
}

// ゲームが プレースホルダーかどうかを調べる関数
bool IsPlaceHolder(StringView path)
{
	return path.starts_with(U"plc:");
}

// ゲームが やめるプレースホルダーかどうかを調べる関数
bool IsPlaceHolderEnd(StringView path)
{
	return path.starts_with(U"plc:end");
}

// リソースが動画かどうかを調べる関数
bool IsVideo(StringView path)
{
	return path.ends_with(U".mp4");
}

// ゲームの情報をロードする関数
Array<Game> LoadGames()
{
	// ゲームのリスト
	Array<Game> games;

	// ホームディレクトリ
	const FilePath homeDirectory = FileSystem::CurrentDirectory() + U"/games";

	// ホームディレクトリにあるアイテムを検索
	for (const FilePath& gameDirectory : FileSystem::DirectoryContents(homeDirectory, Recursive::No))
	{
		// フォルダでない場合はスキップ
		if (not FileSystem::IsDirectory(gameDirectory))
		{
			continue;
		}

		// launcher_info.ini を読み込む
		const FilePath iniPath = (gameDirectory + U"launcher_info.ini");
		const INI ini{ iniPath };

		// 読み込みに失敗
		if (not ini)
		{
			continue;
		}

		// ゲームの情報を読み込む
		Game game;
		game.title = ini[U"Game.title"];
		game.texture = Texture{ Image{ gameDirectory + ini[U"Game.image"] }, TextureDesc::Mipped };
		
		{
			String str = ini[U"Game.screen"];
			game.screen_file = gameDirectory + str;
			game.isVideo = IsVideo(str);
		}

		game.desc = ini[U"Game.desc"].replaced(U"\\n", U"\n");
		game.staff = ini[U"Game.staff"];
		game.tools = ini[U"Game.tools"];
		game.useMouse = ini.get<bool>(U"Game.mouse");
		game.useKeyboard = ini.get<bool>(U"Game.keyboard");
		game.useGamepad = ini.get<bool>(U"Game.gamepad");
		game.priority = ini.get<int32>(U"Game.priority");

		const String path = game.path = ini[U"Game.path"];
		game.isWebApp = IsURL(path);
		game.isHTMLApp = IsHTML(path);
		game.isPlaceHolder = IsPlaceHolder(path);
		game.isPlaceHolderEnd = IsPlaceHolderEnd(path);
		game.path = (game.isWebApp ? path : (gameDirectory + path));

		// ゲームのリストに追加
		games << game;
	}

	// プライオリティに基づいてゲームをソート
	return games.sort_by([](const Game& a, const Game& b) { return a.priority > b.priority; });
}

class ScopedRenderTarget2DWithTransformReset :Uncopyable {
public:
	[[nodiscard]]
	ScopedRenderTarget2DWithTransformReset() = default;
	[[nodiscard]]
	explicit ScopedRenderTarget2DWithTransformReset(const Optional<RenderTexture>& rt)
		: m_renderTarget(rt)
		, m_setter(Mat3x2::Identity(), Transformer2D::Target::SetLocal)
		, m_cameraSetter(Mat3x2::Identity(), Transformer2D::Target::SetCamera) {}

private:
	ScopedRenderTarget2D m_renderTarget;
	Transformer2D m_setter;
	Transformer2D m_cameraSetter;
};

void GaussianBlur(const TextureRegion& from, const RenderTexture& internalBuffer, const RenderTexture& internalBuffer2, const RenderTexture& to,
	double t = 1.0, BoxFilterSize boxFilterSize = BoxFilterSize::BoxFilter9x9)
{
	//t=0,1,2,4,8,16,32...ごとに区切り、ミックスする。tは何倍縮小してぼかしをかけるかを表す。0の場合はそのまま描画する。
	double floor_t = 0;
	double ceil_t = 1;

	ScopedRenderTarget2D target(to);
	Transformer2D setter{ Mat3x2::Identity(),Transformer2D::Target::SetLocal };
	Transformer2D camsetter{ Mat3x2::Identity(),Transformer2D::Target::SetCamera };
	if (t < 1) {
		from.draw();
	}
	else {
		floor_t = pow(2, floor(log2(t)));
		ceil_t = 2 * floor_t;
		{
			ScopedRenderTarget2D target(internalBuffer);
			auto texQuad = RectF(from.size)(from);
			texQuad.uvRect.right = floor_t * (texQuad.uvRect.right - texQuad.uvRect.left) + texQuad.uvRect.left;
			texQuad.uvRect.bottom = floor_t * (texQuad.uvRect.bottom - texQuad.uvRect.top) + texQuad.uvRect.top;
			texQuad.draw();
		}
		Shader::GaussianBlur(internalBuffer, internalBuffer2, internalBuffer, boxFilterSize);
		internalBuffer.scaled(floor_t).draw();
	}

	if (t != floor_t) {
		{
			ScopedRenderTarget2D target(internalBuffer);
			auto texQuad = RectF(from.size)(from);
			texQuad.uvRect.right = ceil_t * (texQuad.uvRect.right - texQuad.uvRect.left) + texQuad.uvRect.left;
			texQuad.uvRect.bottom = ceil_t * (texQuad.uvRect.bottom - texQuad.uvRect.top) + texQuad.uvRect.top;
			texQuad.draw();
		}
		Shader::GaussianBlur(internalBuffer, internalBuffer2, internalBuffer, boxFilterSize);
		internalBuffer.scaled(ceil_t).draw(ColorF(1, (t - floor_t) / (ceil_t - floor_t)));
	}
}

Polygon makeSmoothRoundRect(const RoundRect& roundRect, int32 quality = 24) {
	constexpr double t = 0.8;
	constexpr double by = Bezier3(Vec2{ 0,1 }, Vec2{ t,1 }, Vec2{ 1,t }, Vec2{ 1,0 }).getPos(0.5).y;
	constexpr double s = (1 - Math::InvSqrt2) / (1 - by);

	const Vec2 localTR = { roundRect.w / 2, -roundRect.h / 2 };
	const Vec2 localTop = localTR + Vec2{ -roundRect.r * s,0 };
	const Vec2 localRight = localTR + Vec2{ 0,roundRect.r * s };
	LineString quadLine = Bezier3(localTop, localTop.lerp(localTR, t), localRight.lerp(localTR, t), localRight).getLineString(quality);

	quadLine.append(quadLine.scaled(1, -1).reverse());
	quadLine.append(quadLine.scaled(-1, 1).reverse());
	quadLine.moveBy(roundRect.center());
	return Polygon(quadLine);
}

struct BlurRenderTextureSet {
	RenderTexture from;
	RenderTexture internalBuffer;
	RenderTexture internalBuffer2;
	RenderTexture to;

	BlurRenderTextureSet() = default;

	BlurRenderTextureSet(const Size& size)
		: from(size)
		, internalBuffer(size)
		, internalBuffer2(size)
		, to(size)
	{
	};

	void blur(double t = 1.0, BoxFilterSize boxFilterSize = BoxFilterSize::BoxFilter9x9) const {
		GaussianBlur(from, internalBuffer, internalBuffer2, to, t, boxFilterSize);
	}
};

struct BlurMSRenderTextureSet {
	MSRenderTexture from;
	RenderTexture internalBuffer;
	RenderTexture internalBuffer2;
	RenderTexture to;

	BlurMSRenderTextureSet() = default;

	BlurMSRenderTextureSet(const Size& size)
		: from(size)
		, internalBuffer(size)
		, internalBuffer2(size)
		, to(size)
	{
	};

	void blur(double t = 1.0, BoxFilterSize boxFilterSize = BoxFilterSize::BoxFilter9x9) const {
		GaussianBlur(from, internalBuffer, internalBuffer2, to, t, boxFilterSize);
	}
};

//コントローラー対応

bool ContUp() {
	if (KeyUp.pressed()) return true;
	if (XInput(0).isConnected()) {
		if (XInput(0).leftThumbY > 0.75) return true;
		else if (XInput(0).buttonUp.pressed()) return true;
	}
	if (Gamepad(0).isConnected()) {
		if (Gamepad(0).axes[1] < -0.75) return true;
	}
	return false;
}

bool ContDown() {
	if (KeyDown.pressed()) return true;
	if (XInput(0).isConnected()) {
		if (XInput(0).leftThumbY < -0.75) return true;
		else if (XInput(0).buttonDown.pressed()) return true;
	}
	if (Gamepad(0).isConnected()) {
		if (Gamepad(0).axes[1] > 0.75) return true;
	}
	return false;
}

bool ContA() {
	if (KeySpace.down() || (not KeyAlt.pressed() and KeyEnter.down())) return true;
	if (XInput(0).isConnected()) {
		if (XInput(0).buttonA.down()) return true;
	}
	if (Gamepad(0).isConnected()) {
		if (Gamepad(0).buttons[1].down()) return true;
	}
	return false;
}

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

	bool isSliderMouseOver() const
	{
		return sliderRect().stretched(5).mouseOver();
	}

	bool isSliderThick() const
	{
		return isSliderMouseOver() || dragOffset;
	}

	Transformer2D createTransformer() const
	{
		return Transformer2D(Mat3x2::Translate(0, -viewTop),TransformCursor::Yes);
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

	void update(double wheel = Mouse::Wheel(), double delta = Scene::DeltaTime())
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


		if (isSliderMouseOver() and MouseL.down())
		{
			dragOffset = Cursor::PosF().y - sliderY();
		}
		else if (dragOffset && MouseL.up())
		{
			dragOffset.reset();
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

		sliderWidthTransition.update(isSliderThick());

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

struct Menu {
	Texture icon;
	String name;
	Color color;
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
		{}
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


	void update() {

		for (auto [i, menu] : IndexedRef(menus)) {


			RectF rect = getRect(i);


			menu.mouseOvered = rect.mouseOver();
			menu.mouseOverTransition.update(menu.mouseOvered);


			menu.pressed = rect.leftPressed();
			menu.pressedTransition.update(menu.pressed);

			if (rect.leftClicked()) {
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
	{}

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

	bool update() {
		double prevValue = m_value;

		mouseOverKnob = knob().stretched(5).mouseOver();

		if (mouseOverKnob and MouseL.down()) {
			dragOffset = Cursor::PosF().x - knob().x;
		}
		if (MouseL.up()) {
			dragOffset.reset();
		}

		if (dragOffset) {
			double knobX = Cursor::PosF().x - *dragOffset;
			m_value = xToClampValue(knobX);
		}


		if (not mouseOverKnob and not dragOffset and RectF(pos.x, pos.y - height / 2, width, height).stretched(height).mouseOver()) {
			warpPos = xToClampValue(Cursor::PosF().x);
		}
		else {
			warpPos.reset();
		}

		if (warpPos and MouseL.down()) {
			m_value = *warpPos;
			dragOffset = 0;
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
	SpeakerButton(const RectF& rect,bool mute = false) :m_rect(rect),mute(mute)
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

	bool update() {
		bool prevMute = mute;
		mouseOvered = m_rect.mouseOver();
		mouseOverTransition.update(mouseOvered);

		pressed = m_rect.leftPressed();
		pressedTransition.update(pressed);

		timeAfterChange += Scene::DeltaTime();

		if (m_rect.leftClicked()) {
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
	Polygon m_polygon;
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
		, m_polygon(makeSmoothRoundRect(m_roundRect))
		, m_text(text)
	{}

	bool update() {
		mouseOvered = m_roundRect.mouseOver();
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = m_roundRect.leftPressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		if (pressed) {
			MouseL.clearInput();
		}

		clicked = mouseOvered and MouseL.up();

		return clicked;
	}


	void draw(const ColorF& color, const ColorF& textColor, const Font& font, double fontSize) const{
		{
			Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition) * 0.05, m_roundRect.center()));

			m_roundRect.drawShadow(Vec2{ 0,2 }, m_roundRect.h / 4, 0, ColorF(0.5, mouseOverTransition - pressedTransition));
			m_polygon.draw(color).draw(ColorF(0.9, pressedTransition * 0.5));
		}

		font(m_text).drawAt(fontSize, m_roundRect.center(), textColor);
	}

	void draw(const ColorF& color, const ColorF& textColor, const Font& font) const{
		draw(color, textColor, font, font.fontSize());
	}
};

class IconButton {
	RoundRect m_roundRect;
	Polygon m_polygon;
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
		, m_polygon(makeSmoothRoundRect(m_roundRect))
		, m_texture(texture)
	{}

	bool update() {
		mouseOvered = m_roundRect.mouseOver();
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = m_roundRect.leftPressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		if (pressed) {
			MouseL.clearInput();
		}

		clicked = mouseOvered and MouseL.up();

		return clicked;
	}

	void draw(const ColorF& color, const ColorF& texColor) {
		{
			Transformer2D scaler(Mat3x2::Scale(1 + (mouseOverTransition - pressedTransition) * 0.05, m_roundRect.center()));

			m_roundRect.drawShadow(Vec2{ 0,2 }, m_roundRect.h / 4, 0, ColorF(0.5, mouseOverTransition - pressedTransition));
			m_polygon.draw(color).draw(ColorF(0.9, pressedTransition * 0.5));
		}

		m_texture.drawAt(m_roundRect.center(), texColor);
	}
};

class SideGenreList {
	struct Genre {
		Texture icon;
		String name;

		Genre(const Texture& icon, const String& name)
			:icon(icon)
			, name(name)
		{}

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
	{}

	size_t index() const {
		return selectGenreIndex;
	}

	bool update() {
		size_t prevSelect = selectGenreIndex;

		bool mouseOvered = false;
		for (auto [i, genre] : IndexedRef(genres)) {
			RectF rect = RectF(offset, oneGenreSize).movedBy(0, i * oneGenreSize.y);

			if (rect.leftClicked()) {
				prevSelectGenreIndex = selectGenreIndex;
				selectGenreIndex = i;
			}

			if (rect.mouseOver()) {
				mouseOvered = true;
				mouseOveredIndex = i;
			}
		}

		if (not mouseOvered) {
			mouseOveredIndex.reset();
		}

		timeAfterChangeIndex += Scene::DeltaTime();
		if (prevSelect != selectGenreIndex) {
			timeAfterChangeIndex = 0;
		}
		selectedLinePos = Math::SmoothDamp(selectedLinePos, selectGenreIndex * 1.0, selectedLineVelocity, 0.1);
		return prevSelect != selectGenreIndex;
	}

	void draw(const Font& font, double fontSize) const {
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


		double selectedLineBegin01 = Math::Lerp<double, double, double>(prevSelectGenreIndex, selectGenreIndex, EaseInOutQuart(Min(timeAfterChangeIndex * 3, 1.0)));
		double selectedLineEnd01 = Math::Lerp<double, double, double>(prevSelectGenreIndex, selectGenreIndex, EaseOutQuart(Min(timeAfterChangeIndex * 3, 1.0)));

		if (prevSelectGenreIndex > selectGenreIndex) {
			std::swap(selectedLineBegin01, selectedLineEnd01);
		}

		RectF(offset.x + 5, offset.y + (selectedLineBegin01 + (1 - selectedLineHeightRatio) / 2) * oneGenreSize.y, selectedLineWidth, (selectedLineEnd01 + selectedLineHeightRatio - selectedLineBegin01) * oneGenreSize.y).rounded(selectedLineWidth / 2).draw(Palette::Hotpink);

		//RectF(Arg::leftCenter(offset.x + 5, offset.y+(selectedLinePos+0.5) * oneGenreSize.y), selectedLineWidth, oneGenreSize.y*0.8).rounded(selectedLineWidth/2).draw(Palette::Hotpink);
	}

	void draw(const Font& font) const {
		draw(font, font.fontSize());
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
		:m_roundRect(RectF(Arg::center=center,400,250),50)
		, m_yesButton(RectF(Arg::center = center.movedBy(-90, 70), 140, 60), U"終了")
		, m_noButton(RectF(Arg::center = center.movedBy(90, 70), 140, 60), U"キャンセル")
		, m_text(U"終了しますか？")
	{}

	bool update() {

		m_yesClicked = m_yesButton.update();
		m_noClicked = m_noButton.update();

		if (m_roundRect.leftClicked()) {
			MouseL.clearInput();
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
		m_roundRect.draw(ColorF(1,0.8));
		m_yesButton.draw(Palette::Tomato, Palette::White, FontAsset(U"Game.Desc"));
		m_noButton.draw(Palette::White, Palette::Dimgray, FontAsset(U"Game.Desc"));

		FontAsset(U"Game.Desc")(m_text).drawAt(m_roundRect.center().movedBy(0,-20), Palette::Dimgray);
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
	{}

	bool update(Audio& bgm) {
		m_isPlaying = bgm.isPlaying();
		mouseOvered = m_rect.mouseOver();
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.05);

		pressed = m_rect.leftPressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		if (pressed) {
			MouseL.clearInput();
		}

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
	OnlyIconButton(const RectF& rect, const Texture& texture)
		:m_rect(rect)
		, m_texture(texture)
	{}

	bool update() {
		mouseOvered = m_rect.mouseOver();
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = m_rect.leftPressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);



		clicked = mouseOvered and MouseL.up();

		if (pressed) {
			MouseL.clearInput();
		}

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
	{}

	bool update(Audio& bgm) {
		m_value = double(bgm.posSample()) / bgm.samples();

		mouseOvered = m_rect.stretched(20).mouseOver() or dragOffset.has_value();
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = dragOffset.has_value();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		if (dragOffset) {
			m_value = Clamp((Cursor::PosF().x - m_rect.x - *dragOffset) / (m_rect.w - m_rect.h), 0.0, 1.0);
		}

		if (mouseOvered and MouseL.down()) {
			dragOffset = Cursor::PosF().x - m_rect.x - (m_rect.w - m_rect.h) * m_value;
		}

		m_dragReleased = false;
		if (dragOffset and MouseL.up()) {
			m_dragReleased = true;
			dragOffset.reset();
		}


		if (m_dragReleased) {
			bgm.seekSamples(m_value * bgm.samples());
		}

		if (pressed) {
			MouseL.clearInput();
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
	IconSwitch(const RectF& rect, const Texture& texture)
		:m_rect(rect)
		, m_texture(texture)
	{}

	bool update() {
		mouseOvered = m_rect.mouseOver();
		mouseOverTransition = Math::SmoothDamp(mouseOverTransition, mouseOvered, mouseOverTransitionVelocity, 0.03);

		pressed = m_rect.leftPressed();
		pressedTransition = Math::SmoothDamp(pressedTransition, pressed, pressedTransitionVelocity, 0.03);

		if (pressed) {
			MouseL.clearInput();
		}

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

		body.drawFrame(1,ColorF(0.8));

		body.draw(ColorF(1, m_onTransition));

		m_texture.drawAt(m_rect.center().movedBy(0, 0), ColorF(0.6).lerp(Palette::Hotpink, m_onTransition));

	}
};

class GameMenu {
	
	// ゲーム情報
	Array<Game> games;

	// 実行中のゲームのプロセス
	Optional<ChildProcess> process;

	// 選択しているタイルのインデックス [0, games.size()-1]
	size_t selectGameIndex = 0;

	size_t selectGameIndex_last = 0;

	// タイルのスクロール用の変数
	//double tileOffsetY = 0.0, targetTileOffsetY = 0.0, tileOffsetYVelocity = 0.0;

	Texture background;

	double wait = 0;

	//Stopwatch timer;

	bool isPlayed = false;

	Audio bgm;

	//TextEditState com;

	//bool end = false;

	bool commented = false;

	//TextWriter writer(U"comment.txt", OpenMode::Append);

	//スクリーン用動画
	VideoTexture screen_video;

	//スクリーン用画像
	Texture screen_image;

	double timer = 0;

	const double changeTime = 8;

	bool isMove = false;

	double scale_screen = 1;

	ScrollBar scrollBar;

	MSRenderTexture gameListRenderer;

	BlurRenderTextureSet screenRenderer;

	Transition screenMosueOverTransition = Transition(0.5s, 0.5s);

	Polygon screenRoundRect = makeSmoothRoundRect(RoundRect(UI::ScreenArea, UI::ScreenAreaRound));

	Buffer2D tileRoundRectBuffer = makeSmoothRoundRect(RoundRect({ 0,0 }, UI::TileSize, 25)).toBuffer2D({ 0,0 }, UI::TileSize);

public:
	GameMenu(){
		games = LoadGames();
		if (not games)
		{
			System::MessageBoxOK(U"ゲームがありません。");
			return;
		}

		background = Texture(Resource(U"resource/WCE_L.jpg"));

		bgm = Audio{ Resource(U"resource/bgm.mp3"), Loop::Yes };

		bgm.setLoopPoint(0.03s);

		//bgm.play();

		{
			double ratioS = UI::ScreenArea.w / UI::ScreenArea.h;
			double ratio;
			double x;
			double y;
			if (games[0].isVideo) {
				screen_video = VideoTexture{ games[0].screen_file, Loop::Yes };
				x = screen_video.size().x;
				y = screen_video.size().y;
			}
			else {
				screen_image = Texture{ games[0].screen_file, TextureDesc::Mipped };
				x = screen_image.size().x;
				y = screen_image.size().y;
			}
			ratio = x / y;
			if (ratio >= ratioS) {
				scale_screen = UI::ScreenArea.w / x;
			}
			else {
				scale_screen = UI::ScreenArea.h / y;
			}
		}

		scrollBar = ScrollBar(RectF(Scene::Width() - 12, 5, 10, Scene::Height() - 10 - UI::menuBarHeight), Scene::Height()-UI::menuBarHeight, UI::TileIntervalY * games.size() + UI::BaseTilePos.y - UI::TileIntervalY / 2 + 30);

		gameListRenderer = MSRenderTexture(UI::TileSize.asPoint());

		screenRenderer = BlurRenderTextureSet(UI::ScreenArea.size.asPoint());
	}

	void update() {
		// 現在選択されているゲーム
		const Game& game = games[selectGameIndex];
		///////////////////////////////////////////////
		//
		//	ウィンドウの最小化・復帰
		//
		if (process)
		{
			// プロセスが実行中なら
			if (process->isRunning())
			{
				// ウィンドウを最小化
				Window::Minimize();
				bgm.stop();
				return;
			}
			else // プロセスが終了したら
			{
				// ウィンドウを復帰
				Window::Restore();
				//Window::Maximize();
				Window::SetFullscreen(true);
				process.reset();
				bgm.play();
			}
		}

		///////////////////////////////////////////////
		//
		//  タイマー時間経過後
		//
		/*
		if (end) {
			if (!commented) {
				FontAsset(U"Game.Title")(U"よろしければコメントをお書きください").drawAt(Vec2(960, 480), Palette::Black);
				SimpleGUI::TextBoxAt(com, Scene::Center(), 800);
				if (SimpleGUI::Button(U"決定", Vec2(1440, 720))) {
					writer.write(U"\n---------\n");
					writer.write(com.text);
					commented = true;
				}
			}
			else {
				UI::EndArea.rounded(UI::ScreenAreaRound).draw(ColorF(1.0));
				FontAsset(U"Game.End")(U"今回はここまで！\nプレイしてくださり\nありがとうございました！\n次の人に代わってください。").drawAt(Scene::Center(), Palette::Black);
			}


			continue;
		}
		else {
			if (timer.min() >= UI::EndMinute) end = true;
		}
		*/

		///////////////////////////////////////////////
		//
		//	ゲームの起動
		//
		if (UI::PlayButton.leftClicked() || ContA())
		{
			if (!isPlayed) {
				isPlayed = true;
				//timer.start();
			}
			if (game.isWebApp || game.isHTMLApp)
			{
				// Web ブラウザを起動
				process = ChildProcess{ Config::BrowserPath, game.path };
			}
			else if (game.isPlaceHolderEnd) {
				//end = true;
				System::Exit();
			}
			else if (game.isPlaceHolder) {
				selectGameIndex = (selectGameIndex + 1) % games.size();
			}
			else
			{
				// 実行ファイルを起動
				process = ChildProcess{ game.path };
			}
		}

		if (KeyP.down()) isMove = !isMove;

		///////////////////////////////////////////////
		//
		//	選択しているタイルの変更
		//
		selectGameIndex_last = selectGameIndex;

		scrollBar.update();


		{
			auto scrollTf = scrollBar.createTransformer();

			for (auto [i,game] : IndexedRef(games))
			{
				const Vec2 center = UI::BaseTilePos.movedBy(0, i * UI::TileIntervalY);
				const RectF tile{ Arg::center = center, UI::TileSize };

				// タイルがクリックされたら選択
				if (tile.movedBy(game.tileMovePos).leftClicked())
				{
					game.mouseOverScaleVel = -1;
					selectGameIndex = i;
				}

				

				{
					double target = 1.0;
					Vec2 moveTarget = Vec2(0,0);
					if (tile.movedBy(game.tileMovePos).mouseOver()) {
						target = 1.05;
						moveTarget = Tanh((Cursor::PosF() - center) / 100) * 8;
					}
					if(i == selectGameIndex) {
						moveTarget += Vec2{-50,0};
					}

					game.mouseOverScale = Math::SmoothDamp(game.mouseOverScale, target, game.mouseOverScaleVel, 0.1);
					game.tileMovePos = Math::SmoothDamp(game.tileMovePos, moveTarget, game.tileMovePosVel, 0.1);
				}

			}
		}
		

		// [↑][↓] キーを押して選択の移動
		/*
		if (KeyUp.down())
		{
			//selectGameIndex = (selectGameIndex > 0) ? (selectGameIndex - 1) : 0;
			selectGameIndex = (selectGameIndex + games.size() - 1) % games.size();
		}
		else if (KeyDown.down())
		{
			//selectGameIndex = Min(selectGameIndex + 1, games.size() - 1);
			selectGameIndex = (selectGameIndex + 1) % games.size();
		}*/

		if (ContDown()) {
			if (wait <= 0) {
				//move.stop();
				//move.play();
				wait = 20;
				//selectGameIndex = Min(selectGameIndex + 1, games.size() - 1);
				selectGameIndex = (selectGameIndex + 1) % games.size();
			}
		}
		else if (ContUp()) {
			if (wait <= 0) {
				//move.stop();
				//move.play();
				wait = 20;
				//selectGameIndex = (selectGameIndex > 0) ? (selectGameIndex - 1) : 0;
				selectGameIndex = (selectGameIndex + games.size() - 1) % games.size();
			}
		}
		else wait = 0;
		if (wait > 0)wait -= Scene::DeltaTime() * 60;

		if (isMove) {
			timer += Scene::DeltaTime();
			if (timer > changeTime) {
				timer -= changeTime;
				selectGameIndex = (selectGameIndex + 1) % games.size();
			}
		}

		if (selectGameIndex != selectGameIndex_last) {
			double ratioS = UI::ScreenArea.w / UI::ScreenArea.h;
			double ratio;
			double x;
			double y;
			if (games[selectGameIndex].isVideo) {
				screen_video = VideoTexture{ games[selectGameIndex].screen_file, Loop::Yes };
				x = screen_video.size().x;
				y = screen_video.size().y;
			}
			else {
				screen_image = Texture{ games[selectGameIndex].screen_file, TextureDesc::Mipped };
				x = screen_image.size().x;
				y = screen_image.size().y;
			}
			ratio = x / y;
			if (ratio >= ratioS) {
				scale_screen = UI::ScreenArea.w / x;
			}
			else {
				scale_screen = UI::ScreenArea.h / y;
			}

			///////////////////////////////////////////////
			//
			//	タイル表示のスクロール更新
			//
			{
				const Vec2 center = UI::BaseTilePos.movedBy(0, -scrollBar.viewTop + selectGameIndex * UI::TileIntervalY);
				const RectF tile{ Arg::center = center, UI::TileSize };

				// 左端、右端のタイルが画面外ならスクロール
				if (tile.y <= 0)
				{
					scrollBar.scrollTopTo(UI::BaseTilePos.y - UI::TileIntervalY / 2 + selectGameIndex * UI::TileIntervalY - 50);
				}
				else if (Scene::Height() <= tile.br().y)
				{
					scrollBar.scrollBottomTo(UI::BaseTilePos.y - UI::TileIntervalY / 2 + (selectGameIndex + 1) * UI::TileIntervalY + 50);
				}
			}

		}


		//screen

		screenMosueOverTransition.update(UI::ScreenArea.mouseOver());
	}

	void draw() {

		const Game& game = games[selectGameIndex];

		///////////////////////////////////////////////
		//
		//	描画
		//
		//background.scaled(0.91).draw();



		{
			auto scrollTf = scrollBar.createTransformer();

			for (auto [i, g] : Indexed(games))
			{
				const Vec2 center = UI::BaseTilePos.movedBy(0,i * UI::TileIntervalY);
				const RectF tile{ Arg::center = center, UI::TileSize };

				if (tile.bottomY()+20 < scrollBar.viewTop or scrollBar.viewTop + Scene::Height() - UI::menuBarHeight < tile.topY()-20) continue;

				// 選択されていたら、タイルの枠を描画
				//if (selectGameIndex == i)
				//{
				//	tile.stretched(6)
				//		.rounded(28)
				//		.drawShadow(Vec2{ 0, 3 }, 8, 0);
				//		//.draw(Arg::top(Palette::Cyan.withA(40)), Arg::bottom(Palette::Blue.withA(40)));
				//		//.drawFrame(4, 0, ColorF(UI::TileFrmaeColor, 0.6 + Periodic::Sine0_1(1s) * 0.4));
				//}

				if (g.mouseOverScale > 1.1) {
					tile.scaled(g.mouseOverScale)
						.movedBy(g.tileMovePos)
						.rounded(28)
						.drawShadow(Vec2{ 0, 3 }, 8, 0, ColorF(0, (g.mouseOverScale - 1) * 5));
				}


				// ゲーム画像を描画
				{
					ScopedRenderTarget2DWithTransformReset target{ gameListRenderer.clear(Palette::Black) };

					double scale = (Vec2(gameListRenderer.size()) / g.texture.size()).maxComponent();

					g.texture.scaled(scale).drawAt(gameListRenderer.size() / 2.0);
				}
				Graphics2D::Flush();
				gameListRenderer.resolve();

				{
					Transformer2D mover{ Mat3x2::Scale(g.mouseOverScale,tile.size / 2).translated(center - tile.size / 2 + g.tileMovePos) };
					tileRoundRectBuffer.draw(gameListRenderer);
				}

				//tile.rounded(15)(gameListRenderer).draw();
				//g.texture.scaled(g.scale).drawAt(center);

				if (tile.mouseOver())
				{
					Cursor::RequestStyle(CursorStyle::Hand);
				}
			}
		}
		

		scrollBar.draw();

		// タイトルと説明
		{
			UI::InfoArea.rounded(UI::InfoAreaRound).draw((UI::InfoArea.mouseOver() ? UI::InfoAreaMouseOverColor : ColorF(1.0)).withAlpha(0.8));
			FontAsset(U"Game.Title")(game.title).draw(UI::InfoArea.pos.movedBy(20, 10), UI::TextColor);
			FontAsset(U"Game.Desc")(game.desc).draw(UI::InfoArea.stretched(-65, -20, -10, -20), UI::TextColor);
		}

		// スタッフと開発ツール
		{
			UI::StaffArea.rounded(UI::InfoAreaRound).draw((UI::StaffArea.mouseOver() ? UI::InfoAreaMouseOverColor : ColorF(1.0)).withAlpha(0.8));
			FontAsset(U"Game.Small")(game.staff).draw(UI::StaffArea.pos.movedBy(30, 10), UI::TextColor);
			FontAsset(U"Game.Small")(U"開発ツール: {}"_fmt(game.tools)).draw(UI::StaffArea.pos.movedBy(30, 35), UI::TextColor);
		}

		// プレイボタン
		if (game.isPlaceHolderEnd) {
			UI::PlayButton.rounded(UI::InfoAreaRound).draw((UI::PlayButton.mouseOver() ? ColorF(HSV(UI::EndButtonColor) + HSV(10.0, -0.1, 0.0)) : UI::EndButtonColor).withAlpha(0.8));
			Transformer2D t(Mat3x2::Scale(0.95 + Periodic::Sine0_1(1.2s) * 0.05, UI::PlayButton.center()));
			TextureAsset(U"Icon.End").drawAt(UI::PlayButton.center().movedBy(-60, 0));
			FontAsset(U"Game.Play")(U"やめる").draw(Arg::leftCenter = UI::PlayButton.center().movedBy(-25, 0));

			if (UI::PlayButton.mouseOver())
			{
				Cursor::RequestStyle(CursorStyle::Hand);
			}
		}
		else {
			UI::PlayButton.rounded(UI::InfoAreaRound).draw((UI::PlayButton.mouseOver() ? ColorF(HSV(UI::PlayButtonColor) + HSV(10.0, -0.1, 0.0)) : UI::PlayButtonColor).withAlpha(0.8));
			Transformer2D t(Mat3x2::Scale(0.95 + Periodic::Sine0_1(1.2s) * 0.05, UI::PlayButton.center()));
			TextureAsset(U"Icon.Play").drawAt(UI::PlayButton.center().movedBy(-60, 0));
			FontAsset(U"Game.Play")(U"あそぶ").draw(Arg::leftCenter = UI::PlayButton.center().movedBy(-25, 0));

			if (UI::PlayButton.mouseOver())
			{
				Cursor::RequestStyle(CursorStyle::Hand);
			}
		}

		// 操作方法
		{
			UI::ControlArea.rounded(UI::InfoAreaRound).draw((UI::ControlArea.mouseOver() ? UI::InfoAreaMouseOverColor : ColorF(1.0)).withAlpha(0.8));
			String control = U"操作\n";
			control += game.useMouse ? U"・マウス\n" : U"";
			control += game.useKeyboard ? U"・キーボード\n" : U"";
			control += game.useGamepad ? U"・ゲームパッド\n" : U"";
			FontAsset(U"Game.Small")(control).draw(UI::ControlArea.pos.movedBy(30, 20), UI::TextColor);
		}

		//スクリーン
		{

			if(true){
				Transformer2D setter{ Mat3x2::Identity(),Transformer2D::Target::SetLocal };
				Transformer2D camsetter{ Mat3x2::Identity(),Transformer2D::Target::SetCamera };
				ScopedRenderTarget2D target{ screenRenderer.from };
				if (game.isVideo) {
					double scale = (Vec2(screenRenderer.from.size()) / screen_video.size()).maxComponent();
					screen_video.advance();
					screen_video.scaled(scale).drawAt(screenRenderer.from.size() / 2);
				}
				else {
					double scale = (Vec2(screenRenderer.from.size()) / screen_image.size()).maxComponent();
					screen_image.scaled(scale).drawAt(screenRenderer.from.size() / 2);
				}
			}

			screenRenderer.blur(16);

			if(true){
				Transformer2D setter{ Mat3x2::Identity(),Transformer2D::Target::SetLocal };
				Transformer2D camsetter{ Mat3x2::Identity(),Transformer2D::Target::SetCamera };
				ScopedRenderTarget2D target{ screenRenderer.to };

				Rect(screenRenderer.to.size()).draw(ColorF(0.0,0.5));

				if (game.isVideo) {
					double scale = (Vec2(screenRenderer.to.size()) / screen_video.size()).minComponent();
					screen_video.scaled(scale).drawAt(screenRenderer.to.size() / 2);
				}
				else {
					double scale = (Vec2(screenRenderer.to.size()) / screen_image.size()).minComponent();
					screen_image.scaled(scale).drawAt(screenRenderer.to.size() / 2);
				}
			}

			{
				screenRoundRect.toBuffer2D(UI::ScreenArea.pos, UI::ScreenArea.size).draw(screenRenderer.to);
			}
			//UI::ScreenArea.scaled(screenMosueOverTransition.easeOut()*0.02+1).rounded(UI::ScreenAreaRound)(screenRenderer.to).draw();
			
		}

		//
		{
			//FontAsset(U"Game.Title")(U"{:0>2}:{:0>2}"_fmt(timer.min(), timer.s()%60)).draw(Vec2(30, 1020), Palette::White);
		}
	}
};

struct Music {
	String title;
	String artist;
	String path;
};

enum class NextMusicMode {
	next,
	random,
};

class MusicMenu;
class AllMusicScene {
	PlayButton playButton{ RectF(1000,300, 100, 100) };
	OnlyIconButton forwardButton{ RectF(1200, 300, 100, 100), Texture{ 0xf04e_icon, 100 } };
	OnlyIconButton backButton{ RectF(800, 300, 100, 100), Texture{ 0xf04a_icon, 100 } };

	SeekBar musicBar{ RectF(Arg::center(1050, 450), 450, 20) };

	IconSwitch shuffleSwitch{ RectF(120, 410, 100, 60), Texture{ 0xf074_icon, 50 } };


	ScrollBar musicListScrollBar;
	static constexpr double musicListOneHeight = 80;

	Optional<size_t> mouseOveredIndex;
public:
	AllMusicScene() = default;
	AllMusicScene(MusicMenu& musicMenu);

	void update(MusicMenu& musicMenu, Audio& bgm);

	void draw(MusicMenu& musicMenu, const Audio& bgm);
};

class AlbumListScene {
	ScrollBar albumsScrollBar;
	RoundRect albumRoundRect = RoundRect(RectF(Arg::center(0, 0), { 400, 400 }), 50);
	Polygon albumSmoothRoundRect = makeSmoothRoundRect(albumRoundRect);

	struct AlubmIcon {
		String id;
		Vec2 pos{};
		Vec2 posVel{};
		double scale = 1;
		double scaleVel = 0;
	};

	Array<AlubmIcon> albumIcons;
public:
	AlbumListScene() = default;
	AlbumListScene(MusicMenu& musicMenu);

	void update(MusicMenu& musicMenu, Audio& bgm);

	void draw(MusicMenu& musicMenu, const Audio& bgm);
};

class AlbumSelectedScene {
	PlayButton playButton{ RectF(1000,300, 100, 100) };
	OnlyIconButton forwardButton{ RectF(1200, 300, 100, 100), Texture{ 0xf04e_icon, 100 } };
	OnlyIconButton backButton{ RectF(800, 300, 100, 100), Texture{ 0xf04a_icon, 100 } };

	SeekBar musicBar{ RectF(Arg::center(1050, 450), 450, 20) };

	IconSwitch shuffleSwitch{ RectF(120, 410, 100, 60), Texture{ 0xf074_icon, 50 } };

	OnlyIconButton backToAlbumListButton{ RectF(50, 50, 50, 50), Texture{ 0xf04a_icon, 50 } };

	ScrollBar musicListScrollBar;
	static constexpr double musicListOneHeight = 80;

	Optional<size_t> mouseOveredIndex;
public:
	AlbumSelectedScene() = default;
	AlbumSelectedScene(MusicMenu& musicMenu);

	void update(MusicMenu& musicMenu, Audio& bgm);

	void draw(MusicMenu& musicMenu, const Audio& bgm);
};

class MusicMenu {

	SideGenreList sideGenreList;

	//Array<Music> musics;

	JSON json;

	struct NowPlayingMusic {
		String id;
		JSON album;
		size_t index;
	};

	Optional<NowPlayingMusic> m_nowPlayingMusic;

	NextMusicMode m_nextMusicMode = NextMusicMode::next;


	HashTable<String, Texture> albumImages;

	void loadAlbumImages() {
		for (auto [id, album] : json[U"albumTable"]) {
			albumImages[id] = Texture{ U"musics/" + album[U"imagePath"].getString() ,TextureDesc::Mipped };
		}
	}

	struct MusicMenuState {
		enum {
			AllMusic,
			Album,
			Composer,
		};
	};

	Optional<String> selectedAlbumID;

	AllMusicScene allMusicScene;
	AlbumListScene albumListScene;
	AlbumSelectedScene albumSelectedScene;

public:
	MusicMenu(Audio& bgm)
	{
		sideGenreList = SideGenreList{
			{
				{Texture{0Xf001_icon,40}, U"すべての曲"},
				{Texture{0Xf009_icon,40}, U"アルバム"},
				{Texture{0Xf0c0_icon,40}, U"作曲者"},
			},
			{20,20},
			{400,80}
		};

		json = JSON::Load(U"musics/settings.json");

		if(not json) {
			throw Error(U"settings.jsonが読み込めませんでした。");
		}

		size_t musics_size = json[U"allMusic"].size();

		loadAlbumImages();

		//曲にアルバムを紐づける
		for (auto [id, album] : json[U"albumTable"]) {
			for (auto [i,musicId] : album[U"musics"]) {
				json[U"musicTable"][musicId.getString()][U"albums"].push_back(id);
			}
		}

		//musics = Array<Music>{
		//	{
		//		U"曲1",
		//		U"アーティスト1"
		//	},
		//	{
		//		U"曲2",
		//		U"アーティスト2"
		//	},
		//	{
		//		U"曲3",
		//		U"アーティスト3"
		//	},
		//	{
		//		U"曲4",
		//		U"アーティスト4"
		//	},
		//	{
		//		U"曲5",
		//		U"アーティスト5"
		//	},
		//	{
		//		U"曲6",
		//		U"アーティスト6"
		//	},
		//	{
		//		U"曲7",
		//		U"アーティスト7"
		//	},
		//	{
		//		U"曲8",
		//		U"アーティスト8"
		//	},
		//	{
		//		U"曲9",
		//		U"アーティスト9"
		//	},
		//	{
		//		U"曲10",
		//		U"アーティスト10"
		//	},
		//};


		if(musics_size > 0) {
			String id = json[U"allMusic"][0].getString();
			bgm = Audio{ Audio::Stream,U"musics/" + json[U"musicTable"][id][U"path"].getString() };
			bgm.play();
			m_nowPlayingMusic = NowPlayingMusic{ id,json[U"allMusic"],0 };
		}

		allMusicScene = AllMusicScene(*this);
		albumListScene = AlbumListScene(*this);
		albumSelectedScene = AlbumSelectedScene(*this);



	}

	Vec2 getOffset() const {
		return { 430, 0 };
	}

	const Optional<String>& nowPlayingMusicID() const {

		if (not m_nowPlayingMusic) {
			return none;
		}

		return m_nowPlayingMusic->id;
	}

	void goForwardMusic(Audio& bgm) {
		if (m_nowPlayingMusic) {
			size_t nextIndex = 0;
			JSON& album = m_nowPlayingMusic->album;
			if (m_nextMusicMode == NextMusicMode::next) {
				nextIndex = (m_nowPlayingMusic->index + 1) % album.size();
			}
			else if (m_nextMusicMode == NextMusicMode::random) {
				nextIndex = Random(album.size() - 1);
			}

			String id = album[nextIndex].getString();
			bgm = Audio{ Audio::Stream,U"musics/" + json[U"musicTable"][id][U"path"].getString() };
			bgm.play();
			m_nowPlayingMusic->id = id;
			m_nowPlayingMusic->index = nextIndex;
		}
	}

	void update(Audio& bgm) {

		//音楽が終わったら次の曲を再生
		if (m_nowPlayingMusic and not bgm.isActive()) {
			goForwardMusic(bgm);
		}
		//Print << bgm.posSample() << U" / " << bgm.samples();
		//Print <<U"empty: {}, "_fmt(bgm.isEmpty()) <<U"active :{}, "_fmt(bgm.isActive()) << U"paused: {}, "_fmt(bgm.isPaused()) << U"playing: {}, "_fmt(bgm.isPlaying());

		sideGenreList.update();
		switch (sideGenreList.index())
		{
		case MusicMenuState::AllMusic:
		{
			allMusicScene.update(*this, bgm);
		}
			break;
		case MusicMenuState::Album:
		{
			if (selectedAlbumID) {
				albumSelectedScene.update(*this, bgm);
			}
			else {
				albumListScene.update(*this, bgm);
			}
		}
		break;
		default:
			break;
		}
	}

	void draw(const Audio& bgm) {

		switch (sideGenreList.index())
		{
		case MusicMenuState::AllMusic:
		{
			allMusicScene.draw(*this, bgm);
		}
			break;
		case MusicMenuState::Album:
		{

			if (selectedAlbumID) {
				albumSelectedScene.draw(*this, bgm);
			}
			else {
				albumListScene.draw(*this, bgm);
			}
		}
			break;
		default:
			break;
		}

		
		RectF(0,0,430,Scene::Height()-UI::menuBarHeight).drawShadow({},5,3,ColorF(0.5,0.3)).draw(ColorF(0.96,0.95,0.99));
		sideGenreList.draw(FontAsset(U"GenreList"));
	}

	friend class AllMusicScene;
	friend class AlbumListScene;
	friend class AlbumSelectedScene;
};


AllMusicScene::AllMusicScene(MusicMenu& musicMenu){
	musicListScrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 500 - 5 * 2), Scene::Height() - UI::menuBarHeight - 500, musicMenu.json[U"allMusic"].size() * musicListOneHeight);
}

void AllMusicScene::update(MusicMenu& musicMenu, Audio& bgm) {
	Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);

	if (musicMenu.m_nowPlayingMusic) {
		playButton.update(bgm);

		if (forwardButton.update()) {
			musicMenu.goForwardMusic(bgm);
		}
		if (backButton.update()) {
			bgm.seekSamples(0);
		}

		musicBar.update(bgm);

		if (shuffleSwitch.update()) {
			if (shuffleSwitch.isOn()) {
				musicMenu.m_nextMusicMode = NextMusicMode::random;
			}
			else {
				musicMenu.m_nextMusicMode = NextMusicMode::next;
			}
		}
	}

	Rect upperArea(0, 0, Scene::Width() - 430, 500);

	if (upperArea.leftClicked()) {
		MouseL.clearInput();
	}

	Rect musicListArea(0, 500, Scene::Width() - 430, Scene::Height() - 500);

	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		musicListScrollBar.update();
		if (musicListScrollBar.dragOffset) {
			MouseL.clearInput();
		}
		{
			auto tf = musicListScrollBar.createTransformer();

			mouseOveredIndex.reset();
			for (auto i : step(musicMenu.json[U"allMusic"].size())) {
				if (RectF(0, i * musicListOneHeight, musicListArea.w, musicListOneHeight).mouseOver()) {
					mouseOveredIndex = i;
				}
			}

			if (mouseOveredIndex and MouseL.down()) {
				String id = musicMenu.json[U"allMusic"][*mouseOveredIndex].getString();
				if (musicMenu.m_nowPlayingMusic and musicMenu.m_nowPlayingMusic->id != id) {
					bgm = Audio{ Audio::Stream, U"musics/" + musicMenu.json[U"musicTable"][id][U"path"].getString() };
					bgm.play();
					musicMenu.m_nowPlayingMusic.reset();
					musicMenu.m_nowPlayingMusic = MusicMenu::NowPlayingMusic{ id,musicMenu.json[U"allMusic"],*mouseOveredIndex };
				}
			}
		}

	}

}

void AllMusicScene::draw(MusicMenu& musicMenu, const Audio& bgm) {
	Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);

	Rect musicListArea(0, 500, Scene::Width() - 430, Scene::Height() - 500);
	musicListArea.draw(ColorF(1));
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		{
			auto tf = musicListScrollBar.createTransformer();

			size_t i = 0;
			for (auto item : musicMenu.json[U"allMusic"]) {
				String id = item.value.getString();
				auto music = musicMenu.json[U"musicTable"][id];
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

	Rect upperArea(0, 0, Scene::Width() - 430, 500);

	if (musicMenu.m_nowPlayingMusic) {
		Texture* texture = nullptr;
		const auto& music = musicMenu.json[U"musicTable"][musicMenu.m_nowPlayingMusic->id];
		if (music.hasElement(U"albums") and music[U"albums"].size() > 0) {
			texture = &musicMenu.albumImages[music[U"albums"][0].getString()];
		}


		upperArea.drawShadow({}, 5, 3, ColorF(0.7, 0.3)).draw(ColorF(1));

		auto musicHugeIcon = RectF(130, 40, 300, 300).rounded(50);

		musicHugeIcon.drawShadow({ 0,2 }, 5, 3, ColorF(0.7, 0.3));
		if (texture) {
			musicHugeIcon(*texture).draw();
		}
		else
		{
			musicHugeIcon.draw(ColorF(1));
		}

		FontAsset(U"GenreList")(music[U"title"].getString()).draw(50, Arg::leftCenter(500, 100), ColorF(0.2));
		FontAsset(U"Regular")(music[U"artist"].getString()).draw(30, Arg::leftCenter(500, 160), ColorF(0.2));


		playButton.draw(ColorF(0.2, 0.5));
		forwardButton.draw(ColorF(0.2, 0.5));
		backButton.draw(ColorF(0.2, 0.5));
		musicBar.draw(ColorF(0.5, 0.5), ColorF(0.2, 0.5));
		double seconds = musicBar.value() * bgm.lengthSec();
		String time_tex = U"{:0>2}:{:0>2}"_fmt((int)seconds / 60, (int)seconds % 60);
		FontAsset(U"Regular")(time_tex).draw(30, Arg::rightCenter(1050 - 250, 450), ColorF(0.2));
		double seconds_all = bgm.lengthSec();
		String time_tex_all = U"{:0>2}:{:0>2}"_fmt((int)seconds_all / 60, (int)seconds_all % 60);
		FontAsset(U"Regular")(time_tex_all).draw(30, Arg::leftCenter(1050 + 250, 450), ColorF(0.2));

		shuffleSwitch.draw();

	}
}

AlbumListScene::AlbumListScene(MusicMenu& musicMenu){
	albumsScrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 5 * 2), Scene::Height() - UI::menuBarHeight, 2000);

	albumIcons.clear();
	for (auto [id, album] : musicMenu.json[U"albumTable"]) {
		albumIcons.push_back(AlubmIcon{ id });
	}
}

void AlbumListScene::update(MusicMenu& musicMenu, Audio& bgm) {
	Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);
	albumsScrollBar.update();
	{
		auto tf = albumsScrollBar.createTransformer();

		size_t i = 0;
		for (auto& albumIcon : albumIcons) {
			auto album = musicMenu.json[U"albumTable"][albumIcon.id];
			size_t x_i = i % 3;
			size_t y_i = i / 3;

			const Vec2 iconCenter = Vec2(250 + x_i * 450, 250 + y_i * 450);
			Transformer2D albumTf(Mat3x2::Translate(iconCenter), TransformCursor::Yes);
			Vec2 cp = Cursor::PosF();
			const Vec2 target = 0.2 * cp / (1 + cp.length() / 150);

			Transformer2D albumTf2(Mat3x2::Scale(albumIcon.scale).translated(albumIcon.pos), TransformCursor::Yes);

			bool mouseOver = albumRoundRect.mouseOver();

			albumIcon.pos = Math::SmoothDamp(albumIcon.pos, mouseOver ? target : Vec2{ 0, 0 }, albumIcon.posVel, 0.2);
			albumIcon.scale = Math::SmoothDamp(albumIcon.scale, mouseOver ? 1.05 : 1.0, albumIcon.scaleVel, 0.2);

			if (mouseOver and MouseL.down()) {
				albumIcon.scaleVel = -1;
				musicMenu.selectedAlbumID = albumIcon.id;
			}

			i++;
		}
	}
}

void AlbumListScene::draw(MusicMenu& musicMenu, const Audio& bgm) {
	Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);

	Rect rect(0, 0, Scene::Width(), Scene::Height());
	rect.draw(ColorF(1));
	{
		auto tf = albumsScrollBar.createTransformer();

		size_t i = 0;
		for (const auto& albumIcon : albumIcons) {
			auto album = musicMenu.json[U"albumTable"][albumIcon.id];
			size_t x_i = i % 3;
			size_t y_i = i / 3;

			const Texture& texture = musicMenu.albumImages[albumIcon.id];

			const Vec2 iconCenter = Vec2(250 + x_i * 450, 250 + y_i * 450);
			Transformer2D albumTf(Mat3x2::Translate(iconCenter), TransformCursor::Yes);

			FontAsset(U"GenreList")(album[U"title"].getString()).draw(30, Arg::center(0, 250), ColorF(0.3));

			Transformer2D albumTf2(Mat3x2::Scale(albumIcon.scale).translated(albumIcon.pos), TransformCursor::Yes);
			albumRoundRect.drawShadow({ 0, 3 }, 5, 5, ColorF(0.7, 0.3));
			albumSmoothRoundRect.toBuffer2D(Arg::center(0, 0), texture.size() * (albumRoundRect.rect.size / texture.size()).maxComponent()).draw(texture);

			i++;
		}
	}
	albumsScrollBar.draw();
}

AlbumSelectedScene::AlbumSelectedScene(MusicMenu& musicMenu)
{
	musicListScrollBar = ScrollBar(RectF(Scene::Width() - 430 - 12, 5, 10, Scene::Height() - UI::menuBarHeight - 500 - 5 * 2), Scene::Height() - UI::menuBarHeight - 500, musicMenu.json[U"allMusic"].size() * musicListOneHeight);
}

void AlbumSelectedScene::update(MusicMenu& musicMenu, Audio& bgm)
{
	Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);

	if (musicMenu.m_nowPlayingMusic) {
		playButton.update(bgm);

		if (forwardButton.update()) {
			musicMenu.goForwardMusic(bgm);
		}
		if (backButton.update()) {
			bgm.seekSamples(0);
		}

		musicBar.update(bgm);

		if (shuffleSwitch.update()) {
			if (shuffleSwitch.isOn()) {
				musicMenu.m_nextMusicMode = NextMusicMode::random;
			}
			else {
				musicMenu.m_nextMusicMode = NextMusicMode::next;
			}
		}
	}

	Rect upperArea(0, 0, Scene::Width() - 430, 500);

	if (upperArea.leftClicked()) {
		MouseL.clearInput();
	}

	if (backToAlbumListButton.update()) {
		musicMenu.selectedAlbumID.reset();
		return;
	}

	Rect musicListArea(0, 500, Scene::Width() - 430, Scene::Height() - 500);

	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		musicListScrollBar.pageHeight = musicMenu.json[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"].size() * musicListOneHeight;
		musicListScrollBar.update();
		if (musicListScrollBar.dragOffset) {
			MouseL.clearInput();
		}
		{
			auto tf = musicListScrollBar.createTransformer();

			mouseOveredIndex.reset();
			for (auto i : step(musicMenu.json[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"].size())) {
				if (RectF(0, i * musicListOneHeight, musicListArea.w, musicListOneHeight).mouseOver()) {
					mouseOveredIndex = i;
				}
			}

			if (mouseOveredIndex and MouseL.down()) {
				String id = musicMenu.json[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"][*mouseOveredIndex].getString();
				if (musicMenu.m_nowPlayingMusic and musicMenu.m_nowPlayingMusic->id != id) {
					bgm = Audio{ Audio::Stream, U"musics/" + musicMenu.json[U"musicTable"][id][U"path"].getString() };
					bgm.play();
					musicMenu.m_nowPlayingMusic.reset();
					musicMenu.m_nowPlayingMusic = MusicMenu::NowPlayingMusic{ id,musicMenu.json[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"],*mouseOveredIndex };
				}
			}
		}

	}
}

void AlbumSelectedScene::draw(MusicMenu& musicMenu, const Audio& bgm)
{
	Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);

	Rect musicListArea(0, 500, Scene::Width() - 430, Scene::Height() - 500);
	musicListArea.draw(ColorF(1));
	{
		Transformer2D cursorPosTransformer(Mat3x2::Translate(musicListArea.pos), TransformCursor::Yes);
		{
			auto tf = musicListScrollBar.createTransformer();

			auto musics = musicMenu.json[U"albumTable"][*musicMenu.selectedAlbumID][U"musics"];

			size_t i = 0;
			for (auto item : musics) {
				String id = item.value.getString();
				auto music = musicMenu.json[U"musicTable"][id];
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

	Rect upperArea(0, 0, Scene::Width() - 430, 500);

	

	if (musicMenu.m_nowPlayingMusic) {
		Texture* texture = nullptr;
		const auto& music = musicMenu.json[U"musicTable"][musicMenu.m_nowPlayingMusic->id];
		if (music.hasElement(U"albums") and music[U"albums"].size() > 0) {
			texture = &musicMenu.albumImages[music[U"albums"][0].getString()];
		}



		upperArea.drawShadow({}, 5, 3, ColorF(0.7, 0.3)).draw(ColorF(1));

		auto musicHugeIcon = RectF(130, 40, 300, 300).rounded(50);

		musicHugeIcon.drawShadow({ 0,2 }, 5, 3, ColorF(0.7, 0.3));
		if (texture) {
			musicHugeIcon(*texture).draw();
		}
		else
		{
			musicHugeIcon.draw(ColorF(1));
		}

		FontAsset(U"GenreList")(music[U"title"].getString()).draw(50, Arg::leftCenter(500, 100), ColorF(0.2));
		FontAsset(U"Regular")(music[U"artist"].getString()).draw(30, Arg::leftCenter(500, 160), ColorF(0.2));


		playButton.draw(ColorF(0.2, 0.5));
		forwardButton.draw(ColorF(0.2, 0.5));
		backButton.draw(ColorF(0.2, 0.5));
		musicBar.draw(ColorF(0.5, 0.5), ColorF(0.2, 0.5));
		double seconds = musicBar.value() * bgm.lengthSec();
		String time_tex = U"{:0>2}:{:0>2}"_fmt((int)seconds / 60, (int)seconds % 60);
		FontAsset(U"Regular")(time_tex).draw(30, Arg::rightCenter(1050 - 250, 450), ColorF(0.2));
		double seconds_all = bgm.lengthSec();
		String time_tex_all = U"{:0>2}:{:0>2}"_fmt((int)seconds_all / 60, (int)seconds_all % 60);
		FontAsset(U"Regular")(time_tex_all).draw(30, Arg::leftCenter(1050 + 250, 450), ColorF(0.2));

		shuffleSwitch.draw();

	}

	backToAlbumListButton.draw(ColorF(0.2, 0.5));
}


void Main()
{
	// フォント
	FontAsset::Register(U"Game.Title", 42, Typeface::Heavy);
	FontAsset::Register(U"Game.Desc", 26);
	FontAsset::Register(U"Game.Small", 16);
	FontAsset::Register(U"Game.Play", 30, Typeface::Heavy);

	FontAsset::Register(U"Game.End", 100, Typeface::Heavy);

	FontAsset::Register(U"GenreList", FontMethod::MSDF, 40, Typeface::Bold);
	FontAsset::Register(U"Regular", FontMethod::MSDF, 40, Typeface::Regular);

	// 再生アイコン
	TextureAsset::Register(U"Icon.Play", 0xf144_icon, 48);

	//end icon
	TextureAsset::Register(U"Icon.End", 0xf2f5_icon, 48);


	// ウィンドウと背景色
	Window::SetTitle(U"WCE GAMES 2024");
	Scene::Resize(1920, 1080);
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetStyle(WindowStyle::Sizable);
	//Window::Maximize();
	//Window::SetStyle(UI::Frameless ? WindowStyle::Frameless : WindowStyle::Fixed);
	Scene::SetBackground(UI::BackgroundColor);

	//Escキーを押しても終了しないようにする
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	
	MenuSelector menuSelector{
		{
			{Texture{0Xf11b_icon,40}, U"ゲーム", Palette::Royalblue},
			{Texture{0Xf001_icon,40}, U"音楽", Palette::Hotpink},
			{Texture{0Xf53f_icon,40}, U"イラスト", Palette::Limegreen}
		}
	};

	RectF menuBar(0, 0, Scene::Width(), UI::menuBarHeight);

	SpeakerButton speakerButton(RectF(Arg::center(600, UI::menuBarHeight / 2), 50, 50), true);
	
	VolumeSlider volumeSlider(0.5, 0, 1, Vec2(650, UI::menuBarHeight / 2), 300);

	Audio bgm;

	GameMenu gameMenu;
	MusicMenu musicMenu(bgm);

	GlobalAudio::SetVolume(speakerButton.isMute() ? 0 : volumeSlider.value());

	Window::Resize(Scene::Size() / 2);
	//Window::SetFullscreen(true);

	BlurMSRenderTextureSet wholeSceneRenderTexture(Scene::Size());

	Stopwatch exitTimer;

	IconButton exitButton{ RectF(Scene::Width() - 80, UI::menuBarHeight/2-45.0/2, 60, 45),Texture(0xf2f5_icon, 40) };

	ExitWindow exitWindow{Scene::CenterF()};

	while (System::Update())
	{
		ClearPrint();
		
		if (not exitTimer.isStarted()) {

			menuSelector.update();

			if (volumeSlider.update()) {
				GlobalAudio::SetVolume(speakerButton.isMute() ? 0 : volumeSlider.value());
			}
			if (speakerButton.update()) {
				GlobalAudio::SetVolume(speakerButton.isMute() ? 0 : volumeSlider.value());
			}

			if (exitButton.update()) {
				exitTimer.restart();
			}


			if (menuBar.leftClicked()) {
				MouseL.clearInput();
			}


			{
				ScopedRenderTarget2D target{ wholeSceneRenderTexture.from.clear(UI::BackgroundColor) };


				{
					Transformer2D t(Mat3x2::Translate(0, UI::menuBarHeight), TransformCursor::Yes);
					switch (menuSelector.getSelectMenuIndex())
					{
					case 0:
						gameMenu.update();
						gameMenu.draw();
						break;
					case 1:
						musicMenu.update(bgm);
						musicMenu.draw(bgm);
						break;
					default:
						break;
					}
				}

				menuBar.drawShadow(Vec2(0, 3), 8);
				menuBar.draw(ColorF(0.9));

				menuSelector.draw(FontAsset(U"Game.Desc"));

				speakerButton.draw();
				volumeSlider.draw();

				exitButton.draw(Palette::Salmon,Palette::White);
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

			Scene::Rect().draw(ColorF(0, Min(exitTimer.sF(),0.2)));

			exitWindow.update();

			if (exitWindow.yesClicked()) {
				System::Exit();
			}
			else if (exitWindow.noClicked()) {
				exitTimer.reset();
			}

			if (MouseL.down()) {
				exitTimer.reset();
			}

			exitWindow.draw();
		}

		

		//RectF(82, 72, 1370, 718).rounded(UI::ScreenAreaRound).draw(Alpha(128));
	}
}

