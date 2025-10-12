#pragma once
#include "Common.h"
#include "UI.h"

class Master;
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

	//Audio bgm;

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

	Transition screenMouseOverTransition = Transition(0.5s, 0.5s);

	Buffer2D tileRoundRectBuffer = makeSmoothShape(RoundRect({ 0,0 }, UI::TileSize, 25)).toBuffer2D({ 0,0 }, UI::TileSize);

	bool bgmPausedWhenStartGame = false;

public:
	GameMenu() {
		games = LoadGames();
		if (not games)
		{
			System::MessageBoxOK(U"ゲームがありません。");
			return;
		}

		background = Texture(Resource(U"resource/WCE_L.jpg"));

		//bgm = Audio{ Resource(U"resource/bgm.mp3"), Loop::Yes };

		//bgm.setLoopPoint(0.03s);

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

		scrollBar = ScrollBar(RectF(Scene::Width() - 12, 5, 10, Scene::Height() - 10 - UI::menuBarHeight), Scene::Height() - UI::menuBarHeight, UI::TileIntervalY * games.size() + UI::BaseTilePos.y - UI::TileIntervalY / 2 + 30);

		gameListRenderer = MSRenderTexture(UI::TileSize.asPoint());

		screenRenderer = BlurRenderTextureSet(UI::ScreenArea.size.asPoint());
	}

	void update(SingleUseCursorPos& cursorPos, Master& master);

	void draw() {

		Transformer2D t(Mat3x2::Translate(0, UI::menuBarHeight), TransformCursor::Yes);

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
				const Vec2 center = UI::BaseTilePos.movedBy(0, i * UI::TileIntervalY);
				const RectF tile{ Arg::center = center, UI::TileSize };

				if (tile.bottomY() + 20 < scrollBar.viewTop or scrollBar.viewTop + Scene::Height() - UI::menuBarHeight < tile.topY() - 20) continue;

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
		else if (game.isPlaceHolder) {

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

			if (true) {
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

			if (true) {
				Transformer2D setter{ Mat3x2::Identity(),Transformer2D::Target::SetLocal };
				Transformer2D camsetter{ Mat3x2::Identity(),Transformer2D::Target::SetCamera };
				ScopedRenderTarget2D target{ screenRenderer.to };

				Rect(screenRenderer.to.size()).draw(ColorF(0.0, 0.5));

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
				makeSmoothShape(RoundRect(UI::ScreenArea, UI::ScreenAreaRound)).toBuffer2D(UI::ScreenArea.pos, UI::ScreenArea.size).draw(screenRenderer.to);
			}
			//UI::ScreenArea.scaled(screenMosueOverTransition.easeOut()*0.02+1).rounded(UI::ScreenAreaRound)(screenRenderer.to).draw();

		}

		//
		{
			//FontAsset(U"Game.Title")(U"{:0>2}:{:0>2}"_fmt(timer.min(), timer.s()%60)).draw(Vec2(30, 1020), Palette::White);
		}
	}
};


