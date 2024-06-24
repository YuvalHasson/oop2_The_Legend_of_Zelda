#include "Link.h"

#include <iostream> //debugging

bool m_registerit = Factory<Link>::instance()->registerit("Link",
    [](const sf::Vector2f& position) -> std::unique_ptr<Link>
	{
		return std::make_unique<Link>(*Resources::getResource().getTexture(TEXTURE::Link), position);
	});

Link::Link(const sf::Texture& texture, const sf::Vector2f& position)
	: Animate(texture, position, sf::Vector2f(7,7),
    sf::Vector2f(tileSize/5, tileSize / 10)),
    m_state(std::make_unique<LinkStandingState>()),
    m_sword(Factory<Sword>::instance()->create("Sword", { 0,0 })),
    m_shield(Factory<Shield>::instance()->create("Shield", { 0,0 })),
    m_isPushing(false), m_wasTabPressed(false),
    m_isShooting(false), m_arrow(nullptr)
{
    setGraphics(ANIMATIONS_POSITIONS::LinkDown, 2);
    updateSprite();
    setHp(4);
}

Link::~Link()
{
    NotifyObserversLinkOut();
}

void Link::update(const sf::Time& deltaTime){

    Input input;
    bool up     = sf::Keyboard::isKeyPressed(sf::Keyboard::Up)      || sf::Keyboard::isKeyPressed(sf::Keyboard::W);
    bool down   = sf::Keyboard::isKeyPressed(sf::Keyboard::Down)    || sf::Keyboard::isKeyPressed(sf::Keyboard::S);
    bool right  = sf::Keyboard::isKeyPressed(sf::Keyboard::Right)   || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    bool left   = sf::Keyboard::isKeyPressed(sf::Keyboard::Left)    || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool space  = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    bool tab    = sf::Keyboard::isKeyPressed(sf::Keyboard::Tab);
    bool b      = sf::Keyboard::isKeyPressed(sf::Keyboard::B);
    
    //check for shield
    if(b){
        m_isShielding = true;
        m_shield->activate(getPosition(), getDirection());
    }
    else{
        m_isShielding = false;
        m_shield->deActivate();
    }
    if(m_shield->getCollided()){
        pushBack(m_shield->getCollisionDirection());
    }

    if(space)
    {
        input = PRESS_SPACE;
    }
    else if(down && right)
    {
        input = PRESS_DOWN_RIGHT;
    }
    else if(down && left)
    {
        input = PRESS_DOWN_LEFT;
    }
    else if(up && right)
    {
        input = PRESS_UP_RIGHT;
    }
    else if(up && left)
    {
        input = PRESS_UP_LEFT;
    }
    else if(up)
    {
        input = PRESS_UP;
    }
    else if(down)
    {
        input = PRESS_DOWN;
    }
    else if(right)
    {
        input = PRESS_RIGHT;
    }
    else if(left)
    {
        input = PRESS_LEFT;
    }
    else
    {
        input = NONE;
    }
    if(tab && !m_wasTabPressed){
        std::cout<<"tab!!!!!\n";
        m_isShooting = !m_isShooting;
    }
    m_wasTabPressed = tab;
    std::unique_ptr<LinkState> state = m_state->handleInput(input, m_isShielding, isPush());

    if(state)
    {
        m_state = std::move(state);
        m_state->enter(*this);
    }   
    if(!dynamic_cast<LinkStandingState*>(m_state.get()) && !dynamic_cast<LinkShieldStandingState*>(m_state.get()))
    {
        updateGraphics(deltaTime);
    }
    m_sword->update(deltaTime);
    updateSprite();
}

void Link::move()
{
    Animate::move();
    NotifyObservers();
}

void Link::swipeSword()
{
    if(m_sword)
    {
        m_sword->activate(getSprite().getPosition(), getDirection());
    }
}
void Link::stopSwordSwipe()
{
    if(m_sword)
    {
        m_sword->deActivate();
    }
    setAttacking(false);
}

bool Link::getInvincible() const
{
    return m_invincibleTimer.getElapsedTime().asSeconds() - invincibilityTime.asSeconds() <= 0;
}

void Link::initializeInvincible()
{
    m_invincibleTimer.restart();
}

Sword* Link::getSword(){
    if(m_attacking){
        return m_sword.get();
    }
    return nullptr;
}

Shield* Link::getShield(){
    if(m_isShielding){
        return m_shield.get();
    }
    return nullptr;
}

void Link::draw(sf::RenderTarget& target){
    GameObject::draw(target);
    m_sword->draw(target);
    m_shield->draw(target);
}
void Link::setPush(bool isPushing)
{
	m_isPushing = isPushing;
}

bool Link::isPush() const
{
    return m_isPushing;
}

bool Link::getShooting() const
{
    return m_isShooting;
}

void Link::stopShooting()
{
    setAttacking(false);
}

void Link::shoot()
{
    setAttacking(true);
}

std::unique_ptr<Inanimate> Link::getAttack()
{
    if(m_attacking && m_isShooting){
        if(auto p = Factory<LinkArrow>::instance()->create("LinkArrow", getPosition())){
            m_arrow = std::move(p);
            m_arrow->setPosition(getPosition());
            m_arrow->getSprite().setPosition(getPosition());
            m_arrow->initArrow(getDirection());
        }
        stopShooting();
        return std::move(m_arrow);
    }
    return nullptr;
}

//-------------observer list functions--------------
void Link::RegisterObserver(LinkObserver* observer){
    m_observers.emplace_back(observer);
}

void Link::RemoveObserver(LinkObserver* observer){
    m_observers.erase(remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}

void Link::NotifyObservers(){
    for(const auto& observer: m_observers){
        observer->updateLinkPosition(getPosition());
    }
}

void Link::NotifyObserversLinkOut()
{
    for (const auto& observer : m_observers) {
        observer->removeLink();
    }
}

