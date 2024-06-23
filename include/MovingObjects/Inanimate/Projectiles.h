#pragma once

#include "Inanimate.h"

class Projectile : public Inanimate
{
public:
	Projectile(const sf::Texture&, const sf::Vector2f&,const sf::Vector2f&, const sf::Vector2f&);

	virtual void update(const sf::Time& deltaTime) = 0;
};