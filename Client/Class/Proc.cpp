#include <vector>
#include <string>
#include <random>
#include <functional>

#include "NGP_Client_Agario.h"
#include "Player.h"
#include "debugUtil.h"


extern HINSTANCE g_hInst;

// ================== private function ==================
void redrawAgarioLine(HDC hdc, const POSITION& playerPosition, float width, float height, float cameraZoom)
{
    // ����
    MoveToEx(hdc, -AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, -AGARIO_WIDTH - playerPosition.x + width
        , AGARIO_HEIGHT - playerPosition.y + height);

    // ������
    MoveToEx(hdc, AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, AGARIO_WIDTH - playerPosition.x + width,
        AGARIO_HEIGHT - playerPosition.y + height);

    // ����
    MoveToEx(hdc, -AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, AGARIO_WIDTH - playerPosition.x + width,
        -AGARIO_HEIGHT - playerPosition.y + height);

    // �Ʒ���
    MoveToEx(hdc, -AGARIO_WIDTH - playerPosition.x + width,
        AGARIO_HEIGHT - playerPosition.y + height, NULL);
    LineTo(hdc, AGARIO_WIDTH - playerPosition.x + width,
        AGARIO_HEIGHT - playerPosition.y + height);
}

bool checkCollision(const POSITION& playerPosition, float playerSize, 
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

    // player x��� ��ü x�� ��

    // player ������ ��ü �����ʺ��� ũ��
    if (otherRight < playerLeft) return false;
    // player �������� ��ü ���ʺ��� ������
    if (playerRight < otherLeft) return false;

    // player ���� ��ü�� �Ʒ����� ũ��
    if (otherBottom < playerTop) return false;
    // player �Ʒ��� ��ü�� ������ ������
    if (playerBottom < otherTop) return false;

    return true;
}
// ================== private function ==================


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc, hMemdc;
    static Player player;
    static std::vector<Feed> feeds;

    static std::vector<bool> keyStates(256, false); // Ű ���� �迭

    // ���� ����
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> positionDis(-2000, 2000);
    static std::uniform_int_distribution<int> colorDis(100, 255);

    static float WINDOW_HALF_WIDTH;
    static float WINDOW_HALF_HEIGHT;

    switch (message)
    {
    case WM_CREATE:
    {
        WINDOW_HALF_WIDTH = WINDOW_WIDTH / 2.0f;
        WINDOW_HALF_HEIGHT = WINDOW_HEIGHT / 2.0f;
        
        auto positionGenerator = bind(positionDis, gen);
        auto colorGenerator = bind(colorDis, gen);

        // ���� ����
        feeds.reserve(100);

        for (int index = 0; index < 100; ++index)
        {
            Feed feed ({ positionGenerator(), positionGenerator() },
                { static_cast<BYTE>(colorGenerator()), static_cast<BYTE>(colorGenerator()),
                static_cast<BYTE>(colorGenerator()) });
            feeds.emplace_back(feed);
        }

        SetTimer(hWnd, MOVE_TIMER, FRAME_PER_MILLISECOND, NULL);
        SetTimer(hWnd, COLLISION_TIMER, FRAME_PER_MILLISECOND, NULL);
        SetTimer(hWnd, FEEDS_REGEN, FRAME_PER_MILLISECOND * 60.f, NULL);

        break;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }
    case WM_KEYDOWN:
        keyStates[wParam] = true;
        break;

    case WM_KEYUP:
        keyStates[wParam] = false;
        break;

    case WM_TIMER:
    {
        switch (wParam)
        {
        case MOVE_TIMER:
            player.Move(keyStates);

            InvalidateRect(hWnd, NULL, false);
            break;
        case COLLISION_TIMER:
        {
            // ����ȭ ���� �׳� Ÿ�̸ӷ� �浹ó��
            // �������� �浹ó���ϴ� �ɷ� �����ؾ���.
            auto playerPosition = player.GetPosition();
            auto playerSize = player.GetValue();

            for (auto& feed : feeds)
            {
                if (feed.getActive() == false) continue;

                auto feedPosition = feed.GetPosition();
                auto feedSize = feed.GetValue();

                // �浹�̸�
                if (checkCollision(playerPosition, playerSize, feedPosition, feedSize) == true)
                {
                    feed.setActive(false);
                    player.eatFeed(feedSize);
                }
            }
            break;
        }
        case FEEDS_REGEN:
        {
            auto positionGenerator = bind(positionDis, gen);
            auto colorGenerator = bind(colorDis, gen);
            // ���� ������
            for (auto& feed : feeds)
            {
                if (feed.getActive() == true) continue;

                feed.setActive(true);

                feed.SetPosition({ positionGenerator(), positionGenerator() });
                feed.SetColor({ static_cast<BYTE>(colorGenerator()), static_cast<BYTE>(colorGenerator()), static_cast<BYTE>(colorGenerator()) });
            }
            break;
        }
        }
        break;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // === 1. ����ۿ� �޸� DC �� ��Ʈ�� ���� ===
        HDC hMemDC = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

        // === 2. ��� �׸��� ===
        POSITION playerPosition = player.GetPosition();
        auto cameraZoom = static_cast<int>(player.GetValue()) / 2;

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
                redrawAgarioLine(hMemDC, playerPosition, WINDOW_HALF_WIDTH, WINDOW_HALF_HEIGHT, cameraZoom);
                SelectObject(hMemDC, hOldPen);
                DeleteObject(hPen);
            }
        }

        // === 4. Feed �׸��� ===
        for (const auto& feed : feeds)
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

        // === 5. Player �׸��� ===
        {
            float radius = player.GetValue() * UNIT_SIZE_VALUE;
            float left = WINDOW_HALF_WIDTH - radius + cameraZoom;
            float right = WINDOW_HALF_WIDTH + radius - cameraZoom;
            float top = WINDOW_HALF_HEIGHT - radius + cameraZoom;
            float bottom = WINDOW_HALF_HEIGHT + radius - cameraZoom;

            Ellipse(hMemDC, static_cast<int>(left), static_cast<int>(top), static_cast<int>(right), static_cast<int>(bottom));
        }

        // === 6. ����� �� ���� ȭ������ ���� ===
        BitBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, SRCCOPY);

        // === 7. ���ҽ� ���� ===
        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hMemDC);

        EndPaint(hWnd, &ps);
        break;
    }


    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}