#include "Projectiles.h"

Projectile::Projectile(const sf::Texture& Texture, const sf::Vector2f& position)
	: MovingObjects(Texture, position, sf::Vector2f(8,8), sf::Vector2f(tileSize/2, tileSize/2))
{
}