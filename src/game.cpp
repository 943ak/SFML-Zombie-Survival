#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include <string>

using namespace sf;
using namespace std;

class Bullet{
  public:
    CircleShape shape;
    Vector2f velocity; // Remember's its own Speed/Direction

    Bullet(Vector2f startposition, Vector2f direction){
      shape.setRadius(5.0f);
      shape.setFillColor(Color::Yellow);
      shape.setPosition(startposition);
      velocity = direction;
    }

    void update(){
      shape.move(velocity);
    }
};

int main(){
  const int length = 800; // Length of the Window
  const int breadth = 600; // Height of the Window

  RenderWindow window(VideoMode(length, breadth), "Zombie Survival"); // Renders the Window
  window.setFramerateLimit(60); // Maximum Frame Limit per Second

  //FONT LOAD----------
  Font font;
  if(!font.loadFromFile("assets/OldbitzDemo-8OmwJ.ttf")){
  	return 1;
  }

  //SCORE SETUP--------
  int score = 0;
  Text ScoreText;
  ScoreText.setFont(font);
  ScoreText.setCharacterSize(24);
  ScoreText.setFillColor(Color::White);
  ScoreText.setPosition(10.f, 10.f);
  ScoreText.setString("Score: 0");

  // Random number Generator
  random_device rd;
  mt19937 rng(rd());

  // PLAYER----------------
  ConvexShape player; // Creates a Convex Shape
  player.setPointCount(3);
  player.setPoint(0, Vector2f(25.f, 0.f)); // Top Point of the Triangle
  player.setPoint(1, Vector2f(0.f, 75.f)); // Bottom-Left Point of the Triangle
  player.setPoint(2, Vector2f(50.f, 75.f)); // Bottom-Right Point of the Triangle 
  player.setFillColor(Color::Green); // Color of the Player
  Vector2f p0 = player.getPoint(0); // Nose of the Triangle
  Vector2f p1 = player.getPoint(1); // Left Vertex 
  Vector2f p2 = player.getPoint(2); // Right Vertex
  float centroidX = (p0.x + p1.x + p2.x) / 3.f;
  float centroidY = (p0.y + p1.y + p2.y) / 3.f;
  player.setOrigin(centroidX, centroidY); // Center of the Triangle(Player) Bounding Box
  player.setPosition(length - 400.0f, breadth -  300.0f); // Starting Position of the Player

  float playerangle = player.getRotation(); //Degrees
  float rotationspeed = 5.f; // Degrees per frame

  // ENEMY-----------------
  CircleShape enemy(25.0f); // Radius of the Circular Enemy
  enemy.setFillColor(Color::Red); // Color of the enemy
  enemy.setPosition(length - 600.0f, breadth - 400.0f); // Starting Position of the enemy

  // Random Distributions
  float enemyradius = enemy.getRadius();
  uniform_real_distribution<float> distx(0.f, length - enemyradius * 4.f);
  uniform_real_distribution<float> disty( 0.f, breadth - enemyradius * 4.f);

  vector<Bullet> bullets; //Bullets stored in a vector

  while(window.isOpen()){
    Event event;
   	while(window.pollEvent(event)){
   	  if(event.type == Event::Closed) window.close(); // Closes the Window upon close button
   	  if(event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) window.close(); // Closes the Window upon Esc button

      //-----SHOOTING 
   	  if(event.type == Event::KeyPressed && event.key.code == Keyboard::Space){ // Shoots bullets upon Spacebar
        float radians = playerangle * 3.14159265359 / 180.f; // Converts Degrees to Radians

        // Calculates Direction vector based on rotation
        Vector2f direction(sin(radians) * 10.f, -cos(radians) * 10.f); // sin(a) is X Component, -cos(a) is Y Component(Y is inverted here)

        Vector2f bulletPos = player.getPosition();
        bullets.emplace_back(bulletPos, direction);
   	  }
   	}

    //------PLAYER MOVEMENT & SPEED
   	if(Keyboard::isKeyPressed(Keyboard::W)) player.move(0.0f, -5.0f); // Upward Movement Speed
   	if(Keyboard::isKeyPressed(Keyboard::A)) player.move(-5.0f, 0.0f); // Left Movement Speed
   	if(Keyboard::isKeyPressed(Keyboard::S)) player.move(0.0f, 5.0f); // Downward Movement Speed
   	if(Keyboard::isKeyPressed(Keyboard::D)) player.move(5.0f, 0.0f); // Right Movement Speed

    //------WALL COLLISION
   	Vector2f pos = player.getPosition();
    if (pos.x < 25) player.setPosition(25, pos.y);         // Left
    if (pos.x > length - 25) player.setPosition(length - 25, pos.y); // Right
    if (pos.y < 25) player.setPosition(pos.x, 25);         // Top
    if (pos.y > breadth - 25) player.setPosition(pos.x, breadth - 25); // Bottom

    //------PLAYER ROTATION
    bool rotate = false;
    float targetangle = playerangle;

    if(Keyboard::isKeyPressed(Keyboard::Up)){
      targetangle = 0.f;
      rotate = true;
    }
    if(Keyboard::isKeyPressed(Keyboard::Right)){
      targetangle = 90.f;
      rotate = true;
    }
    if(Keyboard::isKeyPressed(Keyboard::Down)){
      targetangle = 180.f;
      rotate = true;
    }
    if(Keyboard::isKeyPressed(Keyboard::Left)){
      targetangle = 270.f;
      rotate = true;
    }

    //Rotate to the Closet Key Pressed
    if(rotate){
      float diff = targetangle - playerangle;

      // Keeping Angle difference between -180 and 180
      while(diff > 180.f) diff -= 360.f;
      while(diff < -180.f) diff += 360.f;

      if(abs(diff) < rotationspeed) playerangle  = targetangle;
      else playerangle += (diff > 0 ? rotationspeed : -rotationspeed);

      player.setRotation(playerangle); 
      
    }
   
    //-----SHOOTING BULLETS & ERASING
    for (size_t i = 0; i < bullets.size(); i++) {
      bullets[i].update();

      if (bullets[i].shape.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
        enemy.setPosition(distx(rng), disty(rng)); // Respawn
        bullets.erase(bullets.begin() + i);
        i--;

        score++;
        ScoreText.setString("Score: " + to_string(score)); // Update Text
        continue;
      }
   
      if (bullets[i].shape.getPosition().y + bullets[i].shape.getRadius() < 0 || 
          bullets[i].shape.getPosition().y - bullets[i].shape.getRadius() > breadth ||
          bullets[i].shape.getPosition().x + bullets[i].shape.getRadius() < 0 ||
          bullets[i].shape.getPosition().x - bullets[i].shape.getRadius() > length
      ) {
        bullets.erase(bullets.begin() + i);
        i--;
      }
    }
   
    //-------PLAYER–ENEMY COLLISION
    if (player.getGlobalBounds().intersects(enemy.getGlobalBounds())) {
      player.setFillColor(Color::Red);
      window.clear();
      window.draw(player);
      window.draw(enemy);
      window.display();
      sleep(seconds(1.f));
      player.setPosition(400.f, 300.f);
    } else {
        player.setFillColor(Color::Green);
    }
   
    //-----ENEMY CHASE
    Vector2f playerPos = player.getPosition();
    Vector2f enemyPos = enemy.getPosition();
    float enemyspeed = 2.f;
   
    if (playerPos.x > enemyPos.x) enemy.move(enemyspeed, 0.f);
    if (playerPos.x < enemyPos.x) enemy.move(-enemyspeed, 0.f);
    if (playerPos.y > enemyPos.y) enemy.move(0.f, enemyspeed);   // down
    if (playerPos.y < enemyPos.y) enemy.move(0.f, -enemyspeed);  // up

    window.clear();
    window.draw(player);
    window.draw(enemy);

    //-----DRAW BULLETS
    for(size_t i = 0; i < bullets.size(); i++){ 
      window.draw(bullets[i].shape);
    }
    window.draw(ScoreText);
    window.display();
  }
  return 0;
}

