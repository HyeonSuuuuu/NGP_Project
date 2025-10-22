#include "pch.h"

#include "Player.h"
#include "constant.h"

void UNIT::Move(const std::vector<bool>& keyStates, float deltaTime)
{

}

void UNIT::Draw()
{
}

void UNIT::SetPosition(POSITION position)
{
	_position = position;
}

POSITION UNIT::GetPosition() const
{
	return _position;
}

void UNIT::SetValue(float value)
{
	_value = value;
}

float UNIT::GetValue() const
{
	return _value;
}

void UNIT::SetColor(COLOR color)
{
	_color = color;
}

COLOR UNIT::GetColor() const
{
	return _color;
}


// ------------- Player -------------
void Player::Move(const std::vector<bool>& keyStates, float deltaTime)
{

    if (deltaTime <= 0.f) return;

    float dx = 0.f;
    float dy = 0.f;

    // �̵� ���� ���
    if (keyStates[VK_LEFT] && !keyStates[VK_RIGHT])
        dx -= 1.f;
    else if (keyStates[VK_RIGHT] && !keyStates[VK_LEFT])
        dx += 1.f;
    if (keyStates[VK_UP] && !keyStates[VK_DOWN])
        dy -= 1.f;
    else if (keyStates[VK_DOWN] && !keyStates[VK_UP])
        dy += 1.f;

    if (dx == 0.f && dy == 0.f) return;

    // �밢�� �̵� ���� (Nomarization)
    if (dx != 0.f && dy != 0.f) {
        dx *= DIAGONAL_FACTOR;
        dy *= DIAGONAL_FACTOR;
    }

    float moveAmount = _speed * deltaTime;
    
    _position.x += dx * moveAmount;
    _position.y += dy * moveAmount;

    // ��谪 �˻�
    // X ���
    if (_position.x < -AGARIO_WIDTH)
        _position.x = -AGARIO_WIDTH;
    else if (_position.x > AGARIO_WIDTH)
        _position.x = AGARIO_WIDTH;
    // Y ���
    if (_position.y < -AGARIO_HEIGHT)
        _position.y = -AGARIO_HEIGHT;
    else if (_position.y > AGARIO_HEIGHT)
        _position.y = AGARIO_HEIGHT;
}

float Player::GetSpeed() const
{
	return _speed;
}

void Player::eatFeed(float value)
{
    _value += value;
}

void Feed::setActive(bool active)
{
    _isActive = active;
}

bool Feed::getActive() const
{
    return _isActive;
}
