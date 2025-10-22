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

    // ���� ����
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
    // �÷��̾� �̵�
    m_player.Move(keyStates, deltaTime);
    
    // �浹 ó��
    auto playerPosition = m_player.GetPosition();
    auto playerSize = m_player.GetValue();
    float playerRadius = playerSize < 10 ? UNIT_SIZE_VALUE : playerSize;

    for (auto& feed : m_feeds)
    {
        if (feed.getActive() == false) continue;

        auto feedPosition = feed.GetPosition();
        auto feedSize = feed.GetValue();
        
        // �浹�̸�
        if (checkCollision(playerPosition, playerRadius, feedPosition, feedSize) == true)
        {
            feed.setActive(false);
            m_player.eatFeed(feedSize);
        }
    }

    // ���� ���� (�� �����Ӹ��� ��Ȱ��ȭ�� ���̸� ��� ����)
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

    // �� ���� ���
    if (playerSize < MAX_PLAYER_SCREEN_SIZE)
        m_zoomFactor = 1.f;
    else 
        m_zoomFactor = playerSize / MAX_PLAYER_SCREEN_SIZE;
}

void GameManager::Render(HWND hWnd)
{
    HDC hdc = GetDC(hWnd);

    // === 1. ����ۿ� �޸� DC �� ��Ʈ�� ���� ===
    HDC hMemDC = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // === 2. ��� �׸��� ===
    POSITION playerPosition = m_player.GetPosition();

    HBRUSH hBlackBrush = CreateSolidBrush(blackColor.getRGB());
    if (hBlackBrush)
    {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hMemDC, hBlackBrush);
        Rectangle(hMemDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        SelectObject(hMemDC, hOldBrush);
        DeleteObject(hBlackBrush);
    }

    // === 3. �ܰ���(�׵θ�) �׸��� ===
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

    // === 4. Feed �׸��� ===
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

    // === 5. Player �׸��� ===
    {
        // �÷��̾ ȭ�鿡 �׷��� ũ�� (���� ���� ����)
        float screenRadius =m_player.GetValue() / m_zoomFactor;
        if (screenRadius < UNIT_SIZE_VALUE)
            screenRadius = UNIT_SIZE_VALUE;


        // m_player.GetValue()�� m_zoomFactor�� ������� �ʽ��ϴ�.
        int left = static_cast<int>(WINDOW_HALF_WIDTH - screenRadius);
        int right = static_cast<int>(WINDOW_HALF_WIDTH + screenRadius);
        int top = static_cast<int>(WINDOW_HALF_HEIGHT - screenRadius);
        int bottom = static_cast<int>(WINDOW_HALF_HEIGHT + screenRadius);

        Ellipse(hMemDC, left, top, right, bottom);
    }

    // === 6. UI �׸��� ===
    SetTextColor(hMemDC, RGB(255, 255, 255));
    SetBkMode(hMemDC, TRANSPARENT);
    int score = static_cast<int>(m_player.GetValue());
    std::wstring scoreText = L"Score: " + std::to_wstring(score);

    RECT scoreRect;
    SetRect(&scoreRect, 10, 10, 300, 50);
    DrawText(hMemDC,scoreText.c_str(), -1,&scoreRect, DT_LEFT | DT_TOP | DT_SINGLELINE);

    // === 7. ����� �� ���� ȭ������ ���� ===
    BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);

    // === 8. ���ҽ� ���� ===
    SelectObject(hMemDC, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hMemDC);

    ReleaseDC(hWnd, hdc);
}

// ================== private function ==================

void GameManager::redrawAgarioLine(HDC hdc, const POSITION& playerPosition, float width, float height)
{
    // ����
    MoveToEx(hdc,
        WorldToScreenX(-AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y),
        NULL);
    LineTo(hdc,
        WorldToScreenX(-AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(AGARIO_HEIGHT, playerPosition.y));

    // ������
    MoveToEx(hdc,
        WorldToScreenX(AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y),
        NULL);
    LineTo(hdc,
        WorldToScreenX(AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(AGARIO_HEIGHT, playerPosition.y));

    // ����
    MoveToEx(hdc,
        WorldToScreenX(-AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y),
        NULL);
    LineTo(hdc,
        WorldToScreenX(AGARIO_WIDTH, playerPosition.x),
        WorldToScreenY(-AGARIO_HEIGHT, playerPosition.y));

    // �Ʒ���
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

    // 5. �Ÿ��� ������ ������ ���� �������� �۰ų� ������ �浹
    return distanceSquared <= sumRadiiSquared;
}

// ================== private function ==================