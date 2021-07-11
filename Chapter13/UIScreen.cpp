#include "UIScreen.h"
#include "Game.h"
#include "Texture.h"
#include "Font.h"
#include "Shader.h"
#include "Renderer.h"
#include "InputSystem.h"

Button::Button(const std::string& name, Font* font, std::function<void()> onClick, const Vector2& pos, const Vector2& dims) :
    mName{ "" },
    mFont{ font },
    mOnClick{ onClick },
    mPosition{ pos },
    mDimesions{ dims },
    mHighlighted{ false }
{
    SetName(name);
}

Button::~Button()
{
    if (mNameTex)
    {
        mNameTex->Unload();
        delete mNameTex;
    }
}

void Button::SetName(const std::string& name)
{
    mName = name;

    if (mNameTex)
    {
        mNameTex->Unload();
        delete mNameTex;
        mNameTex = nullptr;
    }
    mNameTex = mFont->RenderText(mName);
}

bool Button::Contains(const Vector2& pt) const
{
    bool no = pt.x < (mPosition.x - mDimesions.x / 2.f) ||
        pt.x >(mPosition.x + mDimesions.x / 2.f) ||
        pt.y < (mPosition.y - mDimesions.y / 2.f) ||
        pt.y >(mPosition.y + mDimesions.y / 2.f);
    return !no;
}

void Button::OnClick()
{
    if (mOnClick)
    {
        mOnClick();
    }
}

void Button::SetHighlighted(bool value)
{
    mHighlighted = value;
}

bool Button::GetHighlighted() const
{
    return mHighlighted;
}

const Vector2& Button::GetPosition() const
{
    return mPosition;
}

Texture* Button::GetNameTex() const
{
    return mNameTex;
}


void UIScreen::DrawTexture(Shader* shader, Texture* texture, const Vector2& offset, float scale)
{

    Matrix4 scaleMat = Matrix4::CreateScale(
        static_cast<float>(texture->GetWidth()),
        static_cast<float>(texture->GetHeight()),
        1.f
    );

    Matrix4 transMat = Matrix4::CreateTranslation(
        Vector3(offset.x, offset.y, 0.f));

    Matrix4 world = scaleMat * transMat;
    shader->SetMatrixUniform("uWorldTransform", world);
    texture->SetActive();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

UIScreen::UIScreen(Game* game)  :
    mGame{ game },
    mTitle{ nullptr },
    mBackground{ nullptr },
    mTitlePos{ 0.f, 300.f },
    mBGPos{ 0.f, 250.f },
    mNextButtonPos{ 0.f, 200.f },
    mState{ Active }
{
    game->PushUI(this);
    mFont = mGame->GetFont("Assets/Carlito-Regular.ttf");
    mButtonOn = mGame->GetRenderer()->GetTexture("Assets/test.jpg");
    mButtonOff = mGame->GetRenderer()->GetTexture("Assets/ButtonBlue.png");
}

UIScreen::~UIScreen()
{
    if (mTitle)
    {
        mTitle->Unload();
        delete mTitle;
    }
    for (auto b : mButtons)
    {
        delete b;
    }
    mButtons.clear();
}

void UIScreen::Update(float deltaTime)
{
}

void UIScreen::Draw(Shader* shader)
{
    if (mBackground)
    {
        DrawTexture(shader, mBackground, mBGPos);
    }
    if (mTitle)
    {
        DrawTexture(shader, mTitle, mTitlePos);
    }
    for (auto b : mButtons)
    {
        Texture* tex = b->GetHighlighted() ? mButtonOn : mButtonOff;
        DrawTexture(shader, tex, b->GetPosition());
        DrawTexture(shader, b->GetNameTex(), b->GetPosition());
    }
}

void UIScreen::ProcessInput(const InputState& state)
{
    if (!mButtons.empty())
    {
        Vector2 mousePos = state.Mouse.GetMousePos();
        mousePos.x -= mGame->GetRenderer()->GetScreenWidth() * 0.5f;
        mousePos.y = mGame->GetRenderer()->GetScreenHeight() * 0.5f -
            mousePos.y;

        for (auto b : mButtons)
        {
            if (b->Contains(mousePos))
            {
                b->SetHighlighted(true);

            }
            else
            {
                b->SetHighlighted(false);
            }
            if (state.Mouse.GetButtonState(SDL_BUTTON_LEFT) == Pressed)
            {
                if (b->GetHighlighted())
                {
                    b->OnClick();
                }
            }
        }

    }
}

void UIScreen::SetTitle(const std::string& text, const Vector3& color, int pointSize)
{
    if (mTitle)
    {
        mTitle->Unload();
        delete mTitle;
        mTitle = nullptr;
    }
    mTitle = mFont->RenderText(text, color, pointSize);
}

void UIScreen::Close()
{
    mState = Closing;
}

void UIScreen::AddButton(const std::string& name, std::function<void()> onClick)
{
    Vector2 dims(static_cast<float>(mButtonOn->GetWidth()),
        static_cast<float>(mButtonOn->GetHeight()));
    Button* b = new Button(name, mFont, onClick, mNextButtonPos, dims);
    mButtons.emplace_back(b);

    mNextButtonPos.y -= mButtonOff->GetHeight() + 20.f;
}

UIScreen::UIState UIScreen::GetState()
{
    return mState;
}
