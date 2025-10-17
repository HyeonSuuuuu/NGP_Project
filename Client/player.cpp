#include <Windows.h>
#include <vector>

#include "Player.h"
#include "constant.h"

void UNIT::Move(const std::vector<bool>& keyStates)
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
void Player::Move(const std::vector<bool>& keyStates)
{
    bool isHorizontal = false; // ����
    bool isVertical = false; // ����

    if (keyStates[VK_LEFT]) isHorizontal = !isHorizontal;
    if (keyStates[VK_RIGHT]) isHorizontal = !isHorizontal;
    if (keyStates[VK_UP])    isVertical = !isVertical;
    if (keyStates[VK_DOWN])  isVertical = !isVertical;

    if (isHorizontal == false && isVertical == false) return;

    // ���� ���� �̵�
    if (isHorizontal == true)
    {
        // ���� ���⵵ �̵�
        if (isVertical == true)
        {
            if (keyStates[VK_LEFT])   _position.x -= sqrt(_speed);
            if (keyStates[VK_RIGHT])  _position.x += sqrt(_speed);
        }
        // ���� �������θ� 
        else
        {
            if (keyStates[VK_LEFT])  _position.x -= _speed;
            if (keyStates[VK_RIGHT]) _position.x += _speed;
        }

        if (_position.x < -AGARIO_WIDTH) _position.x = -AGARIO_WIDTH;
        if (_position.x > AGARIO_WIDTH) _position.x = AGARIO_WIDTH;
    }

    // ���� ���� �̵�
    if (isVertical == true)
    {
        // ���� ���⵵ �̵�
        if (isHorizontal == true)
        {
            if (keyStates[VK_UP])    _position.y -= sqrt(_speed);
            if (keyStates[VK_DOWN])  _position.y += sqrt(_speed);
        }
        // ���� �������θ� 
        else
        {
            if (keyStates[VK_UP])    _position.y -= _speed;
            if (keyStates[VK_DOWN])  _position.y += _speed;
        }

        if (_position.y < -AGARIO_HEIGHT) _position.y = -AGARIO_HEIGHT;
        if (_position.y > AGARIO_HEIGHT) _position.y = AGARIO_HEIGHT;
    }
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
