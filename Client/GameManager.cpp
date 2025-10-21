#include "pch.h"
#include "GameManager.h"


GameManager::GameManager() :
    m_gen(m_rd()),
    m_positionDis(-AGARIO_WIDTH, AGARIO_WIDTH),
    m_colorDis(100, 255)
{
}


void GameManager::Init(HWND hWnd)
{
    WINDOW_HALF_WIDTH = WINDOW_WIDTH / 2.0f;
    WINDOW_HALF_HEIGHT = WINDOW_HEIGHT / 2.0f;

    auto positionGenerator = bind(m_positionDis, m_gen);
    auto colorGenerator = bind(m_colorDis, m_gen);

    // 먹이 생성
    m_feeds.reserve(100);

    for (int index = 0; index < 100; ++index)
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

    for (auto& feed : m_feeds)
    {
        if (feed.getActive() == false) continue;

        auto feedPosition = feed.GetPosition();
        auto feedSize = feed.GetValue();

        // 충돌이면
        if (checkCollision(playerPosition, playerSize, feedPosition, feedSize) == true)
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
}

void GameManager::Render(HWND hWnd)
{
    PAINTSTRUCT ps;

    HDC hdc = GetDC(hWnd);

    // === 1. 백버퍼용 메모리 DC 및 비트맵 생성 ===
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // === 2. 배경 그리기 ===
    POSITION playerPosition = m_player.GetPosition();
    auto cameraZoom = static_cast<int>(m_player.GetValue()) / 2;

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
            redrawAgarioLine(hMemDC, playerPosition, WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, cameraZoom);
            SelectObject(hMemDC, hOldPen);
            DeleteObject(hPen);
        }
    }

    // === 4. Feed 그리기 ===
    for (const auto& feed : m_feeds)
    {
        if (feed.getActive() == false) continue;

        POSITION pos = feed.GetPosition();
        float radius = feed.GetValue() * UNIT_SIZE_VALUE;

        HBRUSH hBrush = CreateSolidBrush(feed.GetColor().getRGB());
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

        auto left = pos.x - radius + WINDOW_HALF_WIDTH - playerPosition.x;
        auto right = pos.x + radius + WINDOW_HALF_WIDTH - playerPosition.x;
        auto top = pos.y - radius + WINDOW_HALF_HEIGHT - playerPosition.y;
        auto bottom = pos.y + radius + WINDOW_HALF_HEIGHT - playerPosition.y;

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
        float radius = m_player.GetValue() * UNIT_SIZE_VALUE;
        float left = WINDOW_HALF_WIDTH - radius + cameraZoom;
        float right = WINDOW_HALF_WIDTH + radius - cameraZoom;
        float top = WINDOW_HALF_HEIGHT - radius + cameraZoom;
        float bottom = WINDOW_HALF_HEIGHT + radius - cameraZoom;

        Ellipse(hMemDC, static_cast<int>(left), static_cast<int>(top), static_cast<int>(right), static_cast<int>(bottom));
    }

    // === 6. 백버퍼 → 실제 화면으로 복사 ===
    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);

    // === 7. 리소스 해제 ===
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);

    EndPaint(hWnd, &ps);
}

// ================== private function ==================

void GameManager::redrawAgarioLine(HDC hdc, const POSITION& playerPosition, float width, float height, float cameraZoom)
{
    // 왼쪽
    MoveToEx(hdc, -AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, -AGARIO_WIDTH - playerPosition.x + width
        , AGARIO_HEIGHT - playerPosition.y + height);

    // 오른쪽
    MoveToEx(hdc, AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, AGARIO_WIDTH - playerPosition.x + width,
        AGARIO_HEIGHT - playerPosition.y + height);

    // 위쪽
    MoveToEx(hdc, -AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height);

    // 아래쪽
    MoveToEx(hdc, -AGARIO_WIDTH - playerPosition.x + width,
        AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, AGARIO_WIDTH - playerPosition.x + width,
        AGARIO_HEIGHT - playerPosition.y + height);
}

bool GameManager::checkCollision(const POSITION& playerPosition, float playerSize,
    const POSITION& otherPosition, float otherSize)
{
    auto playerLeft = playerPosition.x - playerSize;
    auto playerRight = playerPosition.x + playerSize;
    auto playerTop = playerPosition.y - playerSize;
    auto playerBottom = playerPosition.y + playerSize;

    auto otherLeft = otherPosition.x - otherSize;
    auto otherRight = otherPosition.x + otherSize;
    auto otherTop = otherPosition.y - otherSize;
    auto otherBottom = otherPosition.y + otherSize;

    // player x축과 객체 x축 비교

    // player 왼쪽이 객체 오른쪽보다 크면
    if (otherRight < playerLeft) return false;
    // player 오른쪽이 객체 왼쪽보다 작으면
    if (playerRight < otherLeft) return false;

    // player 위가 객체의 아래보다 크면
    if (otherBottom < playerTop) return false;
    // player 아래가 객체의 위보다 작으면
    if (playerBottom < otherTop) return false;

    return true;
}

// ================== private function ==================