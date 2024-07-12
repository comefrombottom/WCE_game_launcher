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
	constexpr double TileSizeX = 376;
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
	if (KeySpace.down() || KeyEnter.down()) return true;
	if (XInput(0).isConnected()) {
		if (XInput(0).buttonA.down()) return true;
	}
	if (Gamepad(0).isConnected()) {
		if (Gamepad(0).buttons[1].down()) return true;
	}
	return false;
}

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

	// ゲーム情報
	const Array<Game> games = LoadGames();
	if (not games)
	{
		System::MessageBoxOK(U"ゲームがありません。");
		return;
	}

	// 実行中のゲームのプロセス
	Optional<ChildProcess> process;

	// 選択しているタイルのインデックス [0, games.size()-1]
	size_t selectGameIndex = 0;

	size_t selectGameIndex_last = 0;

	// タイルのスクロール用の変数
	double tileOffsetY = 0.0, targetTileOffsetY = 0.0, tileOffsetYVelocity = 0.0;

	Texture background(Resource(U"resource/WCE_L.jpg"));

	double wait = 0;

	//Stopwatch timer;

	bool isPlayed = false;

	Audio bgm{ Resource(U"resource/bgm.mp3"), Loop::Yes };

	bgm.setLoopPoint(0.03s);

	bgm.play();

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

	// ウィンドウと背景色
	Window::SetTitle(U"WCE GAMES 2023");
	Scene::Resize(1920, 1080);
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetStyle(WindowStyle::Sizable);
	Window::Maximize();
	//Window::Resize(UI::WindowSize);
	//Window::SetStyle(UI::Frameless ? WindowStyle::Frameless : WindowStyle::Fixed);
	Scene::SetBackground(UI::BackgroundColor);

	//Escキーを押しても終了しないようにする
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	while (System::Update())
	{
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
				continue;
			}
			else // プロセスが終了したら
			{
				// ウィンドウを復帰
				Window::Restore();
				Window::Maximize();
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

		for (auto i : step(games.size()))
		{
			const Vec2 center = UI::BaseTilePos.movedBy(0, tileOffsetY + i * UI::TileSizeY);
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
		}

		///////////////////////////////////////////////
		//
		//	タイル表示のスクロール更新
		//
		{
			const Vec2 center = UI::BaseTilePos.movedBy(0, targetTileOffsetY + selectGameIndex * UI::TileSizeY);
			const RectF tile{ Arg::center = center, UI::TileSizeX - 20, UI::TileSizeY - 20 };

			// 左端、右端のタイルが画面外ならスクロール
			if (tile.y <= 0)
			{
				targetTileOffsetY += UI::TileSizeY;
			}
			else if (Scene::Height() <= tile.br().y)
			{
				targetTileOffsetY -= UI::TileSizeY;
			}

			// スムーズスクロール
			tileOffsetY = Math::SmoothDamp(tileOffsetY, targetTileOffsetY, tileOffsetYVelocity, 0.1);
		}

		///////////////////////////////////////////////
		//
		//	描画
		//
		background.scaled(0.91).draw();

		for (auto [i, g] : Indexed(games))
		{
			const Vec2 center = UI::BaseTilePos.movedBy(0, tileOffsetY + i * UI::TileSizeY);
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

		//RectF(82, 72, 1370, 718).rounded(UI::ScreenAreaRound).draw(Alpha(128));
	}
}
