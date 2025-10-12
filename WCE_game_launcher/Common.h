#pragma once
#define PrintDebug(x) Print << Unicode::Widen(#x) << U": " << x;
#define ConsoleDebug(x) Console << Unicode::Widen(#x) << U": " << x;

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

	constexpr Vec2 TileSize = Vec2(350, TileIntervalY - 20);

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

	constexpr RectF ScreenArea{ 82, 12, 16 * 80, 9 * 80 };

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
inline bool IsURL(StringView path)
{
	return (path.starts_with(U"http://") || path.starts_with(U"https://"));
}

// ゲームが ブラウザ用かどうかを調べる関数(PICO-8用)
inline bool IsHTML(StringView path)
{
	return path.ends_with(U".html");
}

// ゲームが プレースホルダーかどうかを調べる関数
inline bool IsPlaceHolder(StringView path)
{
	return path.starts_with(U"plc:");
}

// ゲームが やめるプレースホルダーかどうかを調べる関数
inline bool IsPlaceHolderEnd(StringView path)
{
	return path.starts_with(U"plc:end");
}

// リソースが動画かどうかを調べる関数
inline bool IsVideo(StringView path)
{
	return path.ends_with(U".mp4");
}

// ゲームの情報をロードする関数
inline Array<Game> LoadGames()
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
		, m_cameraSetter(Mat3x2::Identity(), Transformer2D::Target::SetCamera) {
	}

private:
	ScopedRenderTarget2D m_renderTarget;
	Transformer2D m_setter;
	Transformer2D m_cameraSetter;
};

inline void GaussianBlur(const TextureRegion& from, const RenderTexture& internalBuffer, const RenderTexture& internalBuffer2, const RenderTexture& to,
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

inline Shape2D makeSmoothShape(const RoundRect& roundRect) {

	//頂点と三角形インデックスのサンプルをキャッシュ
	static Array<Float2> sampleVertices;
	static Array<TriangleIndex> sampleIndices;

	constexpr Vec2 sampleSize{ 4,4 };
	constexpr double sampleRadius = 1.0;

	constexpr int32 quality = 24; // 曲線の品質

	//曲線の角が roundRect の角の座標と一致するように調整
	constexpr double t = 0.8;
	constexpr double by = Bezier3(Vec2{ 0,1 }, Vec2{ t,1 }, Vec2{ 1,t }, Vec2{ 1,0 }).getPos(0.5).y;
	constexpr double s = (1 - Math::InvSqrt2) / (1 - by);

	constexpr Vec2 localTR = { sampleSize.x / 2, -sampleSize.y / 2 };
	constexpr Vec2 localTop = localTR + Vec2{ -sampleRadius * s,0 };
	constexpr Vec2 localRight = localTR + Vec2{ 0,sampleRadius * s };

	if (not sampleVertices) {
		//topRightの丸角を作成する
		LineString quadLine = Bezier3(localTop, localTop.lerp(localTR, t), localRight.lerp(localTR, t), localRight).getLineString(quality);

		size_t quarter = quadLine.size();

		//複製・反転・結合を2度行い完全な roundRect を作成
		quadLine.append(quadLine.scaled(1, -1).reverse());
		quadLine.append(quadLine.scaled(-1, 1).reverse());

		sampleVertices = Array<Float2>(quadLine.size());
		for (size_t i = 0; i < quadLine.size(); ++i) {
			sampleVertices[i] = Float2{ quadLine[i] };

			//丸角を(0,0)基準に移動
			if (i / quarter == 0) {
				sampleVertices[i].moveBy(-Vec2{ sampleSize.x / 2, -sampleSize.y / 2 });
			}
			else if (i / quarter == 1) {
				sampleVertices[i].moveBy(-Vec2{ sampleSize.x / 2, sampleSize.y / 2 });
			}
			else if (i / quarter == 2) {
				sampleVertices[i].moveBy(-Vec2{ -sampleSize.x / 2, sampleSize.y / 2 });
			}
			else if (i / quarter == 3) {
				sampleVertices[i].moveBy(-Vec2{ -sampleSize.x / 2, -sampleSize.y / 2 });
			}
		}

		//三角形インデックスを生成
		sampleIndices = Array<TriangleIndex>(quadLine.size() - 2);
		for (size_t i = 0; i < quadLine.size() - 2; ++i) {
			sampleIndices[i] = TriangleIndex{ 0, static_cast<uint16>(i + 1), static_cast<uint16>(i + 2) };
		}
	}

	Array<Float2> vertices(sampleVertices.size());
	const size_t quarter = vertices.size() / 4;

	double r = Min(roundRect.r, Min(roundRect.w / 2, roundRect.h / 2) / s);

	//与えられた roundRect に合わせて頂点を拡大・移動
	for (size_t i = 0; i < quarter; ++i) {
		vertices[i] = sampleVertices[i] * r + roundRect.rect.tr();
		vertices[i + quarter] = sampleVertices[i + quarter] * r + roundRect.rect.br();
		vertices[i + 2 * quarter] = sampleVertices[i + 2 * quarter] * r + roundRect.rect.bl();
		vertices[i + 3 * quarter] = sampleVertices[i + 3 * quarter] * r + roundRect.rect.tl();
	}

	return Shape2D(vertices, sampleIndices);
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

inline bool ContUp() {
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

inline bool ContDown() {
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

inline bool ContA() {
	if (KeySpace.down() || (not KeyAlt.pressed() and KeyEnter.down())) return true;
	if (XInput(0).isConnected()) {
		if (XInput(0).buttonA.down()) return true;
	}
	if (Gamepad(0).isConnected()) {
		if (Gamepad(0).buttons[1].down()) return true;
	}
	return false;
}



struct Music {
	String title;
	String artist;
	String path;
};

enum class NextMusicMode {
	next,
	random,
};

struct PositionedIllust {
	String id;
	Vec2 center;
	double scale;
	double scaleWhenMouseOver;
	bool mouseOvered = false;
	double mouseOverTransition = 0;
	double mouseOverTransitionVel = 0;
	bool pressed = false;
};
