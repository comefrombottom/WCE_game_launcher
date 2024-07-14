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
	constexpr double TileSizeX = 370;
	constexpr double TileSizeY = 235;

	// 背景色
	constexpr ColorF BackgroundColor{ 0.85, 0.9, 0.95 };

	// タイル選択の色
	constexpr ColorF TileFrmaeColor{ 1.0, 0.7, 0.3 };

	constexpr Vec2 BaseTilePos{ 1725, 180 };

	constexpr RectF InfoArea{ 180, 840, 915, 155 };

	constexpr RectF StaffArea{ 180, 1000, 915, 70 };

	constexpr RectF PlayButton{ 1100, 840, 220, 85 };

	constexpr ColorF PlayButtonColor{ 0.0, 0.67, 1.0 };

	constexpr ColorF EndButtonColor{ 1.0, 0.1, 0.1 };

	constexpr RectF ControlArea{ 1100, 930, 220, 140 };

	constexpr ColorF InfoAreaMouseOverColor{ 1.0, 0.95, 0.9 };

	constexpr ColorF TextColor{ 0.2 };

	constexpr double InfoAreaRound = 8.0;

	constexpr double ScreenAreaRound = 25.0;

	constexpr RectF ScreenArea{ 82, 72, 1370, 718 };

	constexpr int EndMinute = 30;

	constexpr RectF EndArea{ 100, 100, 1720, 880 };

	constexpr RectF CommentArea{ 300, 300, 1320, 480 };
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

	//画像のスケール(アイコン)
	double scale;

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
			double ratio = (UI::TileSizeX - 20) / (UI::TileSizeY - 20);
			double ratioT = (double)(game.texture.size().x) / (double)(game.texture.size().y);


			if (ratioT >= ratio) {
				game.scale = (UI::TileSizeX - 20) / (double)game.texture.size().x;
			}
			else {
				game.scale = (UI::TileSizeY - 20) / (double)game.texture.size().y;
			}
		}
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
	RectF rect;
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
		return Transformer2D(Mat3x2::Translate(0, -viewTop));
	}

	void scrollBy(double h) {
		viewVelocity = resistance * h;
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

		RectF(Arg::center = selectedLineCenter, menuRectSize.x * 0.9, 2).rounded(1).draw(color);
	}

	void draw(const Font& font) const {
		draw(font, font.fontSize());
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

	double scale_screen;

	ScrollBar scrollBar;

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

		scrollBar = ScrollBar(RectF(Scene::Width() - 12, 5 + 60, 10, Scene::Height() - 10 - 60), Scene::Height(), UI::TileSizeY * games.size() + UI::BaseTilePos.y - UI::TileSizeY / 2 + 30);
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

		for (auto i : step(games.size()))
		{
			const Vec2 center = UI::BaseTilePos.movedBy(0, -scrollBar.viewTop + i * UI::TileSizeY);
			const RectF tile{ Arg::center = center, UI::TileSizeX - 20, UI::TileSizeY - 20 };

			// タイルがクリックされたら選択
			if (tile.leftClicked())
			{
				selectGameIndex = i;
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
				const Vec2 center = UI::BaseTilePos.movedBy(0, -scrollBar.viewTop + selectGameIndex * UI::TileSizeY);
				const RectF tile{ Arg::center = center, UI::TileSizeX - 20, UI::TileSizeY - 20 };

				// 左端、右端のタイルが画面外ならスクロール
				if (tile.y <= 0)
				{
					scrollBar.scrollTopTo(UI::BaseTilePos.y - UI::TileSizeY / 2 + selectGameIndex * UI::TileSizeY - 50);
				}
				else if (Scene::Height() <= tile.br().y)
				{
					scrollBar.scrollBottomTo(UI::BaseTilePos.y - UI::TileSizeY / 2 + (selectGameIndex + 1) * UI::TileSizeY + 50);
				}

				// スムーズスクロール
				//tileOffsetY = Math::SmoothDamp(tileOffsetY, targetTileOffsetY, tileOffsetYVelocity, 0.1);
			}
		}
	}

	void draw() {

		const Game& game = games[selectGameIndex];

		///////////////////////////////////////////////
		//
		//	描画
		//
		//background.scaled(0.91).draw();




		for (auto [i, g] : Indexed(games))
		{
			const Vec2 center = UI::BaseTilePos.movedBy(0, -scrollBar.viewTop + i * UI::TileSizeY);
			const RectF tile{ Arg::center = center, UI::TileSizeX - 20, UI::TileSizeY - 20 };

			// 選択されていたら、タイルの枠を描画
			if (selectGameIndex == i)
			{
				tile.stretched(6)
					.drawShadow(Vec2{ 0, 3 }, 8, 0)
					.draw(UI::BackgroundColor)
					.drawFrame(4, 0, ColorF(UI::TileFrmaeColor, 0.6 + Periodic::Sine0_1(1s) * 0.4));
			}

			// ゲーム画像を描画
			tile.draw(Palette::Black);
			g.texture.scaled(g.scale).drawAt(center);

			if (tile.mouseOver())
			{
				Cursor::RequestStyle(CursorStyle::Hand);
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
			//if (!game.isPlaceHolder) {
			UI::ScreenArea.rounded(UI::ScreenAreaRound).draw(Palette::Black);
			if (game.isVideo) {
				screen_video.advance();
				screen_video.scaled(scale_screen).drawAt(UI::ScreenArea.center());
				//game.screen_video.advance();
				//game.screen_video.scaled(game.scale_screen).drawAt(UI::ScreenArea.center());
			}
			else screen_image.scaled(scale_screen).drawAt(UI::ScreenArea.center());
			//}
		}

		//
		{
			//FontAsset(U"Game.Title")(U"{:0>2}:{:0>2}"_fmt(timer.min(), timer.s()%60)).draw(Vec2(30, 1020), Palette::White);
		}
	}
};

void Main()
{
	// フォント
	FontAsset::Register(U"Game.Title", 42, Typeface::Heavy);
	FontAsset::Register(U"Game.Desc", 26);
	FontAsset::Register(U"Game.Small", 16);
	FontAsset::Register(U"Game.Play", 30, Typeface::Heavy);

	FontAsset::Register(U"Game.End", 100, Typeface::Heavy);

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

	Texture closeIcon(0xf2f5_icon, 40);

	RectF menuBar(0, 0, Scene::Width(), 60);

	GameMenu gameMenu;


	Window::Resize(Scene::Size() / 2);
	Window::SetFullscreen(true);

	while (System::Update())
	{

		menuSelector.update();


		{
			RectF exitRect(Scene::Width() - 80, 5, 60, 45);

			if (exitRect.leftClicked()) {
				MessageBoxResult result = System::MessageBoxOKCancel(U"終了しますか？", MessageBoxStyle::Question);
				if (result == MessageBoxResult::OK) {
					System::Exit();
				}
			}
		}

		if (menuBar.leftClicked()) {
			MouseL.clearInput();
		}

		switch (menuSelector.getSelectMenuIndex())
		{
		case 0:
			gameMenu.update();
			gameMenu.draw();
			break;
		default:
			break;
		}

		menuBar.drawShadow(Vec2(0, 3), 8);
		menuBar.draw(ColorF(0.9));

		menuSelector.draw(FontAsset(U"Game.Desc"));


		{
			RectF exitRect(Scene::Width() - 80, 5, 60, 45);
			exitRect.rounded(5).draw(Palette::Tomato);

			closeIcon.drawAt(exitRect.center(), Palette::White);
			

		}

		//RectF(82, 72, 1370, 718).rounded(UI::ScreenAreaRound).draw(Alpha(128));
	}
}
