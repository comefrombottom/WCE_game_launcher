# include <Siv3D.hpp> // OpenSiv3D v0.6.15
# include "SingleUseCursorPos.h"
# include "Master.h"

void Main()
{
	// フォント
	FontAsset::Register(U"Game.Title", 42, Typeface::Heavy);
	FontAsset::Register(U"Game.Desc", 26);
	FontAsset::Register(U"Game.Small", 16);
	FontAsset::Register(U"Game.Play", 30, Typeface::Heavy);

	FontAsset::Register(U"Game.End", 100, Typeface::Heavy);

	FontAsset::Register(U"Regular", FontMethod::MSDF, 40, Typeface::Regular);
	FontAsset::Register(U"Bold", FontMethod::MSDF, 40, Typeface::Bold);
	
	// 再生アイコン
	TextureAsset::Register(U"Icon.Play", 0xf144_icon, 48);
	
	//end icon
	TextureAsset::Register(U"Icon.End", 0xf2f5_icon, 48);

	TextureAsset::Register(U"Icon.user", 0xf007_icon, 100);
	
	// ウィンドウと背景色
	Window::SetTitle(U"WCE GAMES 2025");
	Scene::Resize(1920, 1080);
	Scene::SetResizeMode(ResizeMode::Keep);
	Window::SetStyle(WindowStyle::Sizable);
	//Window::Maximize();
	//Window::SetStyle(UI::Frameless ? WindowStyle::Frameless : WindowStyle::Fixed);
	Scene::SetBackground(UI::BackgroundColor);

	//Escキーを押しても終了しないようにする
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	Master master;

	Window::Resize(Scene::Size() / 2);
	Window::SetFullscreen(false);

	SingleUseCursorPos cursorPos;
	
	while (System::Update())
	{
		ClearPrint();
		
		// カーソル位置管理をローカル変数として作成
		cursorPos.init();

		// バグったとき用　
		if (cursorPos.isCaught() and MouseL.down()) {
			cursorPos.dragOff();
		}
		
		if (cursorPos and not Scene::Rect().mouseOver()) {
			cursorPos.use();
		}
		
		master.update(cursorPos);
		master.draw();

		//RectF(82, 72, 1370, 718).rounded(UI::ScreenAreaRound).draw(Alpha(128));
	}
}


