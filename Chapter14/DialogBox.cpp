#include "DialogBox.h"
#include "Game.h"
#include "Renderer.h"
#include "Texture.h"

DialogBox::DialogBox(Game* game, const std::string& text, std::function<void()> onOK)	:
	UIScreen(game)
{
	mBGPos = Vector2(0.f, 0.f);
	mTitlePos = Vector2(0.f, 100.f);
	mNextButtonPos = Vector2(0.f, 0.f);
	mBackground = mGame->GetRenderer()->GetTexture("Assets/DialogBG.png");
	SetTitle(text, Vector3::Zero, 30);
	
	AddButton("OKButton", [onOK]() {
		onOK();
	});
	AddButton("CancelButton", [this]() {
		Close();
	});
}
