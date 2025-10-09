#include "stdafx.h"
#include "GameMenu.h"
#include "SingleUseCursorPos.h"
#include "Master.h"

void GameMenu::update(SingleUseCursorPos& cursorPos, Master& master)
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

			if (master.bgm.isPlaying()) {
				master.bgm.pause();
				bgmPausedWhenStartGame = true;
			}
			return;
		}
		else // プロセスが終了したら
		{
			// ウィンドウを復帰
			Window::Restore();
			//Window::Maximize();
			Window::SetFullscreen(true);
			process.reset();

			if (bgmPausedWhenStartGame) {
				master.bgm.play();
			}
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
	if ((cursorPos and UI::PlayButton.leftClicked()) || ContA())
	{
		if (cursorPos) cursorPos.use();

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
			//selectGameIndex = (selectGameIndex + 1) % games.size();
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

	scrollBar.update(cursorPos);


	{
		auto scrollTf = scrollBar.createTransformer();

		for (auto [i, game] : IndexedRef(games))
		{
			const Vec2 center = UI::BaseTilePos.movedBy(0, i * UI::TileIntervalY);
			const RectF tile{ Arg::center = center + game.tileMovePos, UI::TileSize };

			bool tileMouseOver = cursorPos and tile.mouseOver();
			if (tileMouseOver) {
				cursorPos.use();
			}

			{
				double target = 1.0;
				Vec2 moveTarget = Vec2(0, 0);
				if (tileMouseOver) {
					target = 1.05;
					moveTarget = Tanh((Cursor::PosF() - center) / 100) * 8;
				}
				if (i == selectGameIndex) {
					moveTarget += Vec2{ -50,0 };
				}

				game.mouseOverScale = Math::SmoothDamp(game.mouseOverScale, target, game.mouseOverScaleVel, 0.1);
				game.tileMovePos = Math::SmoothDamp(game.tileMovePos, moveTarget, game.tileMovePosVel, 0.1);
			}

			// タイルがクリックされたら選択
			if (tileMouseOver and MouseL.down())
			{
				game.mouseOverScaleVel = -1;
				selectGameIndex = i;
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
		Vec2 contentSize{};
		if (games[selectGameIndex].isVideo) {
			screen_video = VideoTexture{ games[selectGameIndex].screen_file, Loop::Yes };
			contentSize = screen_video.size();
		}
		else {
			screen_image = Texture{ games[selectGameIndex].screen_file, TextureDesc::Mipped };
			contentSize = screen_image.size();
		}
		scale_screen = (UI::ScreenArea.size / contentSize).minComponent();

		

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

	screenMouseOverTransition.update(UI::ScreenArea.mouseOver());
}
