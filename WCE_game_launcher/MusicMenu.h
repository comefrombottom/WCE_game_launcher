#pragma once
#include "Common.h"
#include "AllMusicScene.h"
#include "AlbumListScene.h"
#include "AlbumSelectedScene.h"
#include "ArtistListScene.h"
#include "ArtistSelectedScene.h"

class MusicMenu {

	SideGenreList sideGenreList;

	//Array<Music> musics;

	JSON musicJson;

	struct NowPlayingMusic {
		String id;
		JSON album;
		size_t index;
	};

	Optional<NowPlayingMusic> m_nowPlayingMusic;

	NextMusicMode m_nextMusicMode = NextMusicMode::random;


	HashTable<String, Texture> albumImages;

	void loadAlbumImages() {
		for (auto [id, album] : musicJson[U"albumTable"]) {
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
	Optional<String> selectedArtistID;

	AllMusicScene allMusicScene;
	AlbumListScene albumListScene;
	AlbumSelectedScene albumSelectedScene;
	ArtistListScene artistListScene;
	ArtistSelectedScene artistSelectedScene;


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

		musicJson = JSON::Load(U"musics/settings.json");

		if (not musicJson) {
			throw Error(U"settings.jsonが読み込めませんでした。");
		}

		size_t musics_size = musicJson[U"allMusic"].size();

		loadAlbumImages();

		//曲にアルバムを紐づける
		for (auto [id, album] : musicJson[U"albumTable"]) {
			for (auto [i, musicId] : album[U"musics"]) {
				musicJson[U"musicTable"][musicId.getString()][U"albums"].push_back(id);
			}
		}

		//アーティストに曲を紐づける
		for (auto [id, music] : musicJson[U"musicTable"]) {
			musicJson[U"artistTable"][music[U"artist"].getString()][U"musics"].push_back(id);
		}


		if (musics_size > 0) {
			String id = musicJson[U"allMusic"][0].getString();
			bgm = Audio{ Audio::Stream,U"musics/" + musicJson[U"musicTable"][id][U"path"].getString() };
			//bgm.play();
			m_nowPlayingMusic = NowPlayingMusic{ id,musicJson[U"allMusic"],0 };

			goForwardMusic(bgm);
			bgm.pause();// 最初は再生しない
		}

		allMusicScene = AllMusicScene(*this);
		albumListScene = AlbumListScene(*this);
		albumSelectedScene = AlbumSelectedScene(*this);
		artistListScene = ArtistListScene(*this);
		artistSelectedScene = ArtistSelectedScene(*this);



	}

	Vec2 getOffset() const {
		return { 430, 0 };
	}

	bool hasNowPlayingMusic() const {
		return m_nowPlayingMusic.has_value();
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
			bgm = Audio{ Audio::Stream,U"musics/" + musicJson[U"musicTable"][id][U"path"].getString() };
			bgm.play();
			m_nowPlayingMusic->id = id;
			m_nowPlayingMusic->index = nextIndex;
		}
	}

	void update(SingleUseCursorPos& cursorPos, Audio& bgm) {
		Transformer2D t(Mat3x2::Translate(0, UI::menuBarHeight), TransformCursor::Yes);

		//Print << bgm.posSample() << U" / " << bgm.samples();
		//Print <<U"empty: {}, "_fmt(bgm.isEmpty()) <<U"active :{}, "_fmt(bgm.isActive()) << U"paused: {}, "_fmt(bgm.isPaused()) << U"playing: {}, "_fmt(bgm.isPlaying());

		sideGenreList.update(cursorPos);

		Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);

		switch (sideGenreList.index())
		{
		case MusicMenuState::AllMusic:
		{
			allMusicScene.update(cursorPos, *this, bgm);
		}
		break;
		case MusicMenuState::Album:
		{
			if (selectedAlbumID) {
				albumSelectedScene.update(cursorPos, *this, bgm);
			}
			else {
				albumListScene.update(cursorPos, *this, bgm);
			}
		}
		break;
		case MusicMenuState::Composer:
		{
			if (selectedArtistID) {
				artistSelectedScene.update(cursorPos, *this, bgm);
			}
			else {
				artistListScene.update(cursorPos, *this, bgm);
			}
		}
		break;
		default:
			break;
		}
	}

	void draw(const Audio& bgm) {
		Transformer2D t(Mat3x2::Translate(0, UI::menuBarHeight), TransformCursor::Yes);
		{
			Transformer2D genleTf(Mat3x2::Translate(430, 0), TransformCursor::Yes);

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
			case MusicMenuState::Composer:
			{
				if (selectedArtistID) {
					artistSelectedScene.draw(*this, bgm);
				}
				else {
					artistListScene.draw(*this, bgm);
				}
			}
			break;
			default:
				break;
			}
		}

		RectF(0, 0, 430, Scene::Height() - UI::menuBarHeight).drawShadow({}, 5, 3, ColorF(0.5, 0.3)).draw(ColorF(0.96, 0.95, 0.99));
		sideGenreList.draw(FontAsset(U"Bold"), Palette::Hotpink);
	}

	friend class AllMusicScene;
	friend class AlbumListScene;
	friend class AlbumSelectedScene;
	friend class MusicControlPanel;
	friend class ArtistListScene;
	friend class ArtistSelectedScene;
};



