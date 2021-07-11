#pragma once
#include "Math.h"
#include <string>
#include <functional>
#include <vector>

class Button
{
	std::function<void()> mOnClick;
	std::string mName;
	class Texture* mNameTex;
	class Font* mFont;
	Vector2 mPosition;
	Vector2 mDimesions;
	bool mHighlighted;

public:
	Button(const std::string& name, class Font* font,
		std::function<void()> onClick, const Vector2& pos,
		const Vector2& dims);
	~Button();

	void SetName(const std::string& name);

	bool Contains(const Vector2& pt) const;

	void OnClick();

	void SetHighlighted(bool value);
	bool GetHighlighted() const;
	const Vector2& GetPosition() const;
	class Texture* GetNameTex() const;
};


class UIScreen
{
public:
	enum UIState { Active, Closing };

protected:
	class Game* mGame;
	class Font* mFont;
	class Texture* mTitle;
	class Texture* mBackground;
	class Texture* mButtonOn;
	class Texture* mButtonOff;
	Vector2 mTitlePos;
	Vector2 mNextButtonPos;
	Vector2 mBGPos;
	std::vector<class Button*> mButtons;
	UIState mState;

	void DrawTexture(class Shader* shader, class Texture* texture, const Vector2& offset = Vector2::Zero, float scale = 1.f);

public:
	UIScreen(class Game* game);
	virtual ~UIScreen();

	virtual void Update(float deltaTime);
	virtual void Draw(class Shader* shader);
	virtual void ProcessInput(const struct InputState& state);

	void SetTitle(const std::string& text, const Vector3& color = Color::White, int pointSize = 40);
	void Close();

	void AddButton(const std::string& name, std::function<void()> onClick);

	UIState GetState();

};

