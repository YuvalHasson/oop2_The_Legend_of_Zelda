#include "OctorokProjectile.h"

#include <iostream> //debugging

bool OctorokProjectile::m_registerit = Factory::registerit("OctorokProjectile",
    [](const sf::Vector2f& position) -> std::unique_ptr<GameObject> {
        return std::make_unique<OctorokProjectile>(*Resources::getResource().getTexture(TEXTURE::Enemies), position);
    });

OctorokProjectile::OctorokProjectile(const sf::Texture& Texture, const sf::Vector2f& position)
    : Projectile(Texture, position)
{
    setGraphics({ 265, 143 }, 1);
	std::cout << "OctorokProjectile" << std::endl;
}