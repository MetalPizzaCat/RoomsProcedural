#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include <random>

class Random
{

private:
    std::random_device m_rd;
    std::mt19937 m_mt;
    std::uniform_int_distribution<std::mt19937::result_type> m_dist = std::uniform_int_distribution<std::mt19937::result_type>(0, 1);

public:
    Random()
    {
        m_mt = std::mt19937(m_rd());
    }

    bool nextBool() { return m_dist(m_mt); }

    int getNextInt(int min, int max)
    {
        std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
        return (int)dist(m_mt);
    }
};

enum RoomNodeDrawingFlags
{
    ERNDT_DrawBackground = 1,
    ERNDT_DrawRoom = 2
};

class Node
{
private:
    std::unique_ptr<Node> m_left = std::unique_ptr<Node>(nullptr);
    std::unique_ptr<Node> m_right = std::unique_ptr<Node>(nullptr);

    sf::Vector2f m_size;
    const sf::Vector2f m_minSize;
    sf::RectangleShape m_rect;
    sf::Vector2f m_position;

    sf::RectangleShape m_room;
    sf::Vector2f m_roomPosition;
    Random *m_random;

    std::unique_ptr<Node> generateSplitNode(bool move)
    {
        // bool splitDir = m_random->nextBool();
        // sf::Vector2f newSize = splitDir ? sf::Vector2f(m_size.x / 2.f, m_size.y) : sf::Vector2f(m_size.x, m_size.y / 2.f);
        // sf::Vector2f newPosition = m_position + (splitDir ? sf::Vector2f(m_size.x / 2.f, 0) : sf::Vector2f(0, m_size.y / 2.f));
        // return std::make_unique<Node>(newSize, m_minSize, move ? newPosition : m_position, m_random);

        bool vertSplit = rand() % 2 - 1; 
        sf::Vector2f newSize;
        sf::Vector2f newPosition;
        if (vertSplit)
        {
            newSize = sf::Vector2f(m_size.x / 2.f, m_size.y);
            newPosition = m_position + sf::Vector2f(m_size.x / 2.f, 0);
        }
        else
        {
            newSize = sf::Vector2f(m_size.x, m_size.y / 2.f);
            newPosition = m_position + sf::Vector2f(0, m_size.y / 2.f);
        }
        return std::make_unique<Node>(newSize, m_minSize, move ? newPosition : m_position, m_random);
    }

public:
    explicit Node(sf::Vector2f size,
                  sf::Vector2f minSize,
                  sf::Vector2f position,
                  Random *randManager) : m_size(size),
                                         m_minSize(minSize),
                                         m_rect(size),
                                         m_position(position),
                                         m_random(randManager)
    {
        m_rect.setFillColor(sf::Color((rand() & 0xFFFFFF00) | 0XFF));
        m_rect.setPosition(m_position);
    }

    void split()
    {
        // can't split further?
        // we reached the final cut and now have to make a room
        if (m_size.x <= m_minSize.x || m_size.y <= m_minSize.y)
        {
            m_roomPosition = sf::Vector2f(m_random->getNextInt(8, m_size.x * 0.2f), m_random->getNextInt(8, m_size.y * 0.2f));
            sf::Vector2f endPos = sf::Vector2f(m_random->getNextInt(m_size.x * 0.8f, m_size.x * 0.95f), m_random->getNextInt(m_size.y * 0.8f, m_size.y * 0.95f));
            m_room = sf::RectangleShape(endPos - m_roomPosition);
            m_room.setPosition(m_roomPosition + m_position);
            // m_room.setFillColor(sf::Color(rand()));
            return;
        }
        bool vertSplit = m_random->nextBool();
        std::cout << m_name << "-> " << vertSplit << std::endl;
        if (vertSplit)
        {
            sf::Vector2f nodeSize = sf::Vector2f(m_size.x / 2.f, m_size.y);

            
            m_left = std::make_unique<Node>(nodeSize, m_minSize, m_position, m_random);
            m_right = std::make_unique<Node>(nodeSize, m_minSize, m_position + sf::Vector2f(m_size.x / 2.f, 0), m_random);
        }
        else
        {
            sf::Vector2f nodeSize = sf::Vector2f(m_size.x, m_size.y / 2.f);

            
            m_left = std::make_unique<Node>(nodeSize, m_minSize, m_position, m_random);
            m_right = std::make_unique<Node>(nodeSize, m_minSize, m_position + sf::Vector2f(0, m_size.y / 2.f), m_random);
        }

        m_right->split();
        m_left->split();
    }

    void draw(sf::RenderWindow &window, int8_t flags)
    {

        // if node has children -> it's not the final node
        if (m_right && m_left)
        {
            m_right->draw(window, flags);
            m_left->draw(window, flags);
            return;
        }
        if (flags & RoomNodeDrawingFlags::ERNDT_DrawBackground)
        {
            window.draw(m_rect);
        }
        // if it doesn't that means that we generated room for it
        if (flags & RoomNodeDrawingFlags::ERNDT_DrawRoom)
        {
            window.draw(m_room);
        }
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");

    Random random;
    Node root = Node(sf::Vector2f(128 * 6, 128 * 6), sf::Vector2f(64, 64), sf::Vector2f(0, 0), &random);
    root.split();
    int8_t flags = RoomNodeDrawingFlags::ERNDT_DrawBackground | RoomNodeDrawingFlags::ERNDT_DrawRoom;
    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::W)
                {
                    flags ^= RoomNodeDrawingFlags::ERNDT_DrawBackground;
                }
                if (event.key.code == sf::Keyboard::E)
                {
                    flags ^= RoomNodeDrawingFlags::ERNDT_DrawRoom;
                }
            }
        }

        window.clear();
        root.draw(window, flags);
        window.display();
    }
}