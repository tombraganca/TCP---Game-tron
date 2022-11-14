#include <SFML/Graphics.hpp>
#include <cmath>
#include <functional>
const int W = 1200;
const int H = 700;
#include "../entities/player.cpp"

using namespace std;
class canvas
{
    private:
        Sprite sprite;
        sf::Font MyFont;
        RenderWindow window {VideoMode(W, H), "Game"};
        RenderTexture canva;
    public:
        void setup(string title) {
            window.setFramerateLimit(60);
            canva.create(W, H);
            canva.setSmooth(true);
            sprite.setTexture(canva.getTexture());
            canva.clear();

            if (!MyFont.loadFromFile("font.ttf"))
                sf::err() << "ERROR Loading font"<< endl;
        }

        void update(player p1, player p2) {
            CircleShape shape(3);
            shape.setPosition(p1.x, p1.y);
            shape.setFillColor(p1.color);
            canva.draw(shape);
            shape.setPosition(p2.x, p2.y);
            shape.setFillColor(p2.color);
            canva.draw(shape);
            canva.display();
        }

        void drawText(sf::Color color, std::string _text) {
            auto text = sf::Text{ _text, MyFont };
            text.setPosition(H / 2 , W / 2);
            text.setFillColor(color);
            canva.draw(text);
            canva.display();
        }

        void eventHandler(std::function<void(string)> action) {
            Event _event;
            while (window.pollEvent(_event))
                onClose(_event);
            keyboardEvents(action);
        }
        
        void onClose(Event _event) {
            if (_event.type == Event::Closed)
                window.close();
        }

        void keyboardEvents(std::function<void(string)> action) {
            if (Keyboard::isKeyPressed(Keyboard::R)) action("R");
            if (Keyboard::isKeyPressed(Keyboard::Left)) action("Left");
            if (Keyboard::isKeyPressed(Keyboard::Right)) action("Right");
            if (Keyboard::isKeyPressed(Keyboard::Up)) action("Up");
            if (Keyboard::isKeyPressed(Keyboard::Down)) action("B");
            if (Keyboard::isKeyPressed(Keyboard::A)) action("A");
            if (Keyboard::isKeyPressed(Keyboard::D)) action("D");
            if (Keyboard::isKeyPressed(Keyboard::W)) action("W");
            if (Keyboard::isKeyPressed(Keyboard::S)) action("S");
            if (Keyboard::isKeyPressed(Keyboard::X)) window.close();
        }

        bool isOpen() {
            return window.isOpen();
        }

        void draw() {
        window.clear();
        window.draw(sprite);
        window.display();
    }
};

