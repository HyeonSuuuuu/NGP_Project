#include "pch.h"
#include "GameManager.h"


GameManager::GameManager() :
    m_gen(m_rd()),
    m_positionDis(-AGARIO_WIDTH, AGARIO_WIDTH),
    m_colorDis(100, 255),
    m_zoomFactor(1.f)
{
}


void GameManager::Init(HWND hWnd)
{
    WINDOW_HALF_WIDTH = WINDOW_WIDTH / 2.0f;
    WINDOW_HALF_HEIGHT = WINDOW_HEIGHT / 2.0f;

    auto positionGenerator = bind(m_positionDis, m_gen);
    auto colorGenerator = bind(m_colorDis, m_gen);

    // 먹이 생성
    m_feeds.reserve(FEED_COUNT);

    for (int index = 0; index < FEED_COUNT; ++index)
    {
        Feed feed({ positionGenerator(), positionGenerator() },
            { static_cast<BYTE>(colorGenerator()), static_cast<BYTE>(colorGenerator()),
            static_cast<BYTE>(colorGenerator()) });
        m_feeds.emplace_back(feed);
    }
}

void GameManager::Update(float deltaTime, const std::vector<bool>& keyStates)
{
    // 플레이어 이동
    m_player.Move(keyStates, deltaTime);
    
    // 충돌 처리
    auto playerPosition = m_player.GetPosition();
    auto playerSize = m_player.GetValue();
    float playerRadius = playerSize < 10 ? UNIT_SIZE_VALUE : playerSize;

    for (auto& feed : m_feeds)
    {
        if (feed.getActive() == false) continue;

        auto feedPosition = feed.GetPosition();
        auto feedSize = feed.GetValue();
        
        // 충돌이면
        if (checkCollision(playerPosition, playerRadius, feedPosition, feedSize) == true)
        {
            feed.setActive(false);
            m_player.eatFeed(feedSize);
        }
    }

    // 먹이 리젠 (매 프레임마다 비활성화된 먹이를 즉시 리젠)
    auto positionGenerator = std::bind(m_positionDis, m_gen);
    auto colorGenerator = std::bind(m_colorDis, m_gen);

    for (auto& feed : m_feeds)
    {
        if (feed.getActive() == false)
        {
            feed.setActive(true);
            feed.SetPosition({ positionGenerator(), positionGenerator() });
            feed.SetColor({ static_cast<BYTE>(colorGenerator()), static_cast<BYTE>(colorGenerator()), static_cast<BYTE>(colorGenerator()) });
        }
    }

    // 줌 팩터 계산
    if (playerSize < MAX_PLAYER_SCREEN_SIZE)
        m_zoomFactor = 1.f;
    else 
        m_zoomFactor = playerSize / MAX_PLAYER_SCREEN_SIZE;
}

void GameManager::Render(HWND hWnd)
{
    HDC hdc = GetDC(hWnd);

    // === 1. 백버퍼용 메모리 DC 및 비트맵 생성 ===
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // === 2. 배경 그리기 ===
    POSITION playerPosition = m_player.GetPosition();

    HBRUSH hBlackBrush = CreateSolidBrush(blackColor.getRGB());
    if (hBlackBrush)
    {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, hBlackBrush);
        Rectangle(hMemDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        SelectObject(hMemDC, hOldBrush);
        DeleteObject(hBlackBrush);
    }

    // === 3. 외곽선(테두리) 그리기 ===
    {
        HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 0));
        if (hPen)
        {
            HPEN hOldPen = (HPEN)SelectObject(hMemDC, hPen);
            redrawAgarioLine(hMemDC, playerPosition, WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT);
            SelectObject(hMemDC, hOldPen);
            DeleteObject(hPen);
        }
    }

    // === 4. Feed 그리기 ===
    for (const auto& feed : m_feeds)
    {
        if (feed.getActive() == false) continue;

        POSITION worldPos = feed.GetPosition();
        float worldRadius = feed.GetValue() * UNIT_SIZE_VALUE;

        int screenCenterX = WorldToScreenX(worldPos.x, playerPosition.x);
        int screenCenterY = WorldToScreenY(worldPos.y, playerPosition.y);
        float screenRadius = worldRadius / m_zoomFactor;

        HBRUSH hBrush = CreateSolidBrush(feed.GetColor().getRGB());
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

        int left = static_cast<int>(screenCenterX - screenRadius);
        int right = static_cast<int>(screenCenterX + screenRadius);
        int top = static_cast<int>(screenCenterY - screenRadius);
        int bottom = static_cast<int>(screenCenterY + screenRadius);

        //if (right - left <= 4)
        //{
        //    left -= 2;
        //    right += 2;
        //}
        //if (bottom - top <= 4)
        //{
        //    top -= 2;
        //    bottom += 2;
        //}

        Ellipse(hMemDC, left, top, right, bottom);

        SelectObject(hMemDC, hOldBrush);
        DeleteObject(hBrush);
    }

    // === 5. Player 그리기 ===
    {
        // 플레이어가 화면에 그려질 크기 (게임 내내 고정)
        float screenRadius =m_player.GetValue() / m_zoomFactor;
        if (screenRadius < UNIT_SIZE_VALUE)
            screenRadius = UNIT_SIZE_VALUE;


        // m_player.GetValue()나 m_zoomFactor를 사용하지 않습니다.
        int left = static_cast<int>(WINDOW_HALF_WIDTH - screenRadius);
        int right = static_cast<int>(WINDOW_HALF_WIDTH + screenRadius);
        int top = static_cast<int>(WINDOW_HALF_HEIGHT - screenRadius);
        int bottom = static_cast<int>(WINDOW_HALF_HEIGHT + screenRadius);

        Ellipse(hMemDC, left, top, right, bottom);
    }

    // === 6. UI 그리기 ===
    SetTextColor(hMemDC, RGB(255, 255, 255));
    SetBkMode(hMemDC, TRANSPARENT);
    int score = static_cast<int>(m_player.GetValue());
    std::wstring scoreText = L"Score: " + std::to_wstring(score);

    RECT scoreRect;
    SetRect(&scoreRect, 10, 10, 300, 50);
    DrawText(hMemDC,scoreText.c_str(), -1,&scoreRect, DT_LEFT | DT_TOP | DT_SINGLELINE);

    // === 7. 백버퍼 → 실제 화면으로 복사 ===
    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);

    // === 8. 리소스 해제 ===
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);

    ReleaseDC(hWnd, hdc);
}

// ================== private function ==================

void GameManager::redrawAgarioLine(HDC hdc, const POSITION& playerPosition, float width, float height)
{
    // 왼쪽
    MoveToEx(hdc,
        WorldToScreenX(-AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y),
        NULL);
    LineTo(hdc,
        WorldToScreenX(-AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(AGARIO_HEIGHT, playerPosition.y));

    // 오른쪽
    MoveToEx(hdc,
        WorldToScreenX(AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y),
        NULL);
    LineTo(hdc,
        WorldToScreenX(AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(AGARIO_HEIGHT, playerPosition.y));

    // 위쪽
    MoveToEx(hdc,
        WorldToScreenX(-AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y),
        NULL);
    LineTo(hdc,
        WorldToScreenX(AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y));

    // 아래쪽
    MoveToEx(hdc,
        WorldToScreenX(-AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(AGARIO_HEIGHT, playerPosition.y),
        NULL);
    LineTo(hdc,
        WorldToScreenX(AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(AGARIO_HEIGHT, playerPosition.y));
}

int GameManager::WorldToScreenX(float worldX, float playerX) const
{
    float screenX = ((worldX - playerX) / m_zoomFactor) + WINDOW_HALF_WIDTH;
    return static_cast<int>(screenX);
}

int GameManager::WorldToScreenY(float worldY, float playerY) const
{
    float screenY = ((worldY - playerY) / m_zoomFactor) + WINDOW_HALF_HEIGHT;
    return static_cast<int>(screenY);
}



bool GameManager::checkCollision(const POSITION& playerPosition, float playerRadius,
    const POSITION& otherPosition, float otherRadius)
{
    float dx = playerPosition.x - otherPosition.x;
    float dy = playerPosition.y - otherPosition.y;

    float distanceSquared = (dx * dx) + (dy * dy);

    float sumRadii = playerRadius + otherRadius;

    float sumRadiiSquared = sumRadii * sumRadii;

    // 5. 거리의 제곱이 반지름 합의 제곱보다 작거나 같으면 충돌
    return distanceSquared <= sumRadiiSquared;
}

// ================== private function ==================