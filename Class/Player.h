#include "ColorKeyCode.h"

#define UNIT_SIZE_VALUE 5

enum class direction {
	RIGHT,
	LEFT,
	UP,
	DOWN,
};

struct POSITION {
	float x, y;
};

class UNIT {
protected:
	POSITION _position;
	COLOR _color;
	float _value;
public:
	UNIT(float value) : _value(value) {};
	UNIT(POSITION position = { 0, 0 }, COLOR color = { 255, 255, 255 }, float value = 0.5f)
		: _position(position), _color(color), _value(value)	{};


	virtual void Move(const std::vector<bool>& keyStates);

	void SetPosition(POSITION position);
	POSITION GetPosition() const;

	void SetValue(float value);
	float GetValue() const;

	void SetColor(COLOR color);
	COLOR GetColor() const;
};

class Feed : public UNIT {
private:
	bool _isActive;
public:
	Feed() : _isActive(false) {};
	Feed(POSITION position) : UNIT(position), _isActive(true) {};

	Feed(POSITION position = { 0, 0 }, COLOR color = { 255, 255, 255 }, 
		float value = 1.0f, bool active = true)
		: UNIT(position, color, value), _isActive(active) {};

	void setActive(bool active);
	bool getActive() const;
};

class Player : public UNIT {
	float _speed;
public:
	Player() : UNIT(1.0f), _speed(3.0f) {};
	Player(float speed) : UNIT(1.0f), _speed(speed) {};

	virtual void Move(const std::vector<bool>& keyStates) override;

	float GetSpeed() const;

	void eatFeed(float value);
};