#pragma once
#include <Siv3D.hpp>

/// @brief カーソル位置の使用状態を管理するクラス
/// @remark UIコンポーネント間でカーソルの使用状態を共有し、重複した反応を防ぎます
class SingleUseCursorPos {
	bool m_notHovered = true;
	bool m_isCaught = false; // カーソル位置がキャッチされているかどうか
public:
	SingleUseCursorPos() = default;

	/// @brief カーソルが使用可能かどうかを返します
	/// @return カーソルが使用可能な場合 true
	[[nodiscard]]
	bool has_value() const { return m_notHovered; }
	
	/// @brief カーソルが使用可能かどうかを返します
	[[nodiscard]]
	operator bool() const { return has_value(); }

	/// @brief カーソル位置がドラッグ操作でキャッチされているかを返します
	/// @return キャッチされている場合 true
	[[nodiscard]]
	bool isCaught() const { return m_isCaught; }

	/// @brief カーソル位置を取得します（使用済みフラグは変更しません）
	/// @return カーソル位置
	/// @throws std::runtime_error カーソルが使用不可能な場合
	[[nodiscard]]
	Vec2 operator*() const {
		if (m_notHovered) {
			return Cursor::PosF();
		}
		throw std::runtime_error("Cursor position is not set.");
	}

	/// @brief カーソル位置を取得します（使用済みフラグは変更しません）
	/// @return カーソル位置
	/// @throws std::runtime_error カーソルが使用不可能な場合
	[[nodiscard]]
	Vec2 value() const {
		if (m_notHovered) {
			return Cursor::PosF();
		}
		throw std::runtime_error("Cursor position is not set or has already been used.");
	}

	/// @brief カーソルの使用状態を初期化します
	void init() {
		if (m_isCaught) {
			return;
		}
		m_notHovered = true;
	}

	/// @brief カーソルを使用不可にします
	void reset() { 
		m_notHovered = false; 
	}

	/// @brief カーソル位置を取得し、使用済みにします
	/// @return カーソル位置
	/// @throws std::runtime_error カーソルが既に使用済みの場合
	Vec2 use() {
		if (m_notHovered) {
			m_notHovered = false;
			return Cursor::PosF();
		}
		throw std::runtime_error("Cursor position has already been used or is not set.");
	}

	/// @brief ドラッグ操作を開始します
	void dragOn() {
		m_notHovered = false;
		m_isCaught = true;
	}

	/// @brief ドラッグ操作を終了します
	void dragOff() { 
		m_isCaught = false; 
	}
};
