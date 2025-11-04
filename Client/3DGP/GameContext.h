#pragma once
#include "stdafx.h"
#include "Timer.h"
#include "SceneManager.h"

class CGameContext {
public:

    CGameContext() {
        m_sceneManager.Init(*this);
    }
    ~CGameContext() {}

    HINSTANCE                   m_hInstance = nullptr;
    HWND                        m_hWnd = nullptr;
    HDC                         m_hDCFrameBuffer = nullptr;
    HBITMAP                     m_hBitmapFrameBuffer = nullptr;
    RECT                        m_rcClient = {};
    CGameTimer                  m_gameTimer;
    CSceneManager               m_sceneManager;

    bool                        m_bWin = false;
};