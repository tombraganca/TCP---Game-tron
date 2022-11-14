#include <iostream>
#include "./canvas.cpp"
#include "./safe_queue.cpp"
#include "./socket_client.cpp"
#include <time.h>

using namespace std;
using namespace std::placeholders;
class game
{
    private:
        int speed = 4;
        bool field[W][H] = {0};
        bool running = false;
        int win = -1;
        string lastSend = "";
        socket_client connection;
        player p1, p2;
        canvas screen;
        safe_queue events_queue; 
    public:
        game() {
            setup();
        }

        ~game() {
            connection.close_conn();
        }

        void setup() {
            srand(time(0));
            p1.setup(Color::Red, 1, 30, 3);
            p2.setup(Color::Green, 50, 30, 4); 

            screen.setup("game");

            auto io_thread = thread([&] {
                while (screen.isOpen()) events_queue.add(connection.read_msg());
            });
            while(loop());
            connection.close_conn();
        }

        void action(string s) {
            if(win != -1)  return;
            if (s.compare("R") == 0)  connection.send_msg("r");
            if (s.compare("Left") == 0 && lastSend.compare(s) != 0)  if (p1.dir != 2)  connection.send_msg("Left");
            if (s.compare("Right") == 0 && lastSend.compare(s) != 0)  if (p1.dir != 1)  connection.send_msg("Right");
            if (s.compare("Up") == 0 && lastSend.compare(s) != 0)   if (p1.dir != 0) connection.send_msg("Up");
            if (s.compare("B") == 0 && lastSend.compare(s) != 0)  if (p1.dir != 3) connection.send_msg("B");
            if (s.compare("A") == 0 && lastSend.compare(s) != 0)  if (p2.dir != 2) connection.send_msg("A");
            if (s.compare("D") == 0 && lastSend.compare(s) != 0)  if (p2.dir != 1) connection.send_msg("D");
            if (s.compare("W") == 0 && lastSend.compare(s) != 0) if (p2.dir != 0) connection.send_msg("W");
            if (s.compare("S") == 0 && lastSend.compare(s) != 0) if (p2.dir != 3) connection.send_msg("S");
            lastSend = s;
        }
        
        bool loop() {
            if (!screen.isOpen()) false;
            screen.eventHandler(std::bind(&game::action,  this, _1));
        
            move();
            if(running) {
                screen.drawText(sf::Color::Black, "Press R to Start");
                screen.update(p1, p2); 
                applySpeed();
                screen.draw();
            } else {
                if(win == -1)
                    screen.drawText(sf::Color::Red, "Press R to Start");
                else
                    screen.drawText(sf::Color::Red, "Player "  + std::to_string(win) + " Wins!" );
                screen.draw();
            }
            return true;
        }

        void updatePosition() { 
            p1.tick();
            p2.tick();
            if ( field[p1.x][p1.y] == 1 ) {
                running = 0;
                win = 1;
            }
            if ( field[p2.x][p2.y] == 1 ) {
                running = 0;
                win = 2;
            }
            field[p1.x][p1.y] = 1;
            field[p2.x][p2.y] = 1;
        }

        void move() {
            string mssg;
            if (events_queue.pop(mssg))
            {
                if(running) {
                    if (mssg.compare("Left") == 0) if (p1.dir != 2) p1.dir = 1;
                    if (mssg.compare("Right") == 0) if (p1.dir != 1) p1.dir = 2;
                    if (mssg.compare("Up") == 0) if (p1.dir != 0) p1.dir = 3;
                    if (mssg.compare("B") == 0) if (p1.dir != 3) p1.dir = 0;
                    if (mssg.compare("A") == 0) if (p2.dir != 2) p2.dir = 1;
                    if (mssg.compare("D") == 0) if (p2.dir != 1)  p2.dir = 2;
                    if (mssg.compare("W") == 0) if (p2.dir != 0) p2.dir = 3;
                    if (mssg.compare("S") == 0) if (p2.dir != 3) p2.dir = 0;
                }

                if (mssg.compare("start") == 0) {
                    running = true;
                    cout << "started" << endl;
                };
            }
        }

        void applySpeed() {
            for (int i = 0; i < speed; i++) updatePosition();
        }
};

