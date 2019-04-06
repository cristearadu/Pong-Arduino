#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Joystick Voltage Pin must be connected to 5V !!
//Define JOYSTICK Pins
const int SW_pin = 2; // digital pin connected to switch output
const int X_pin = 0; // analog pin connected to X output
const int Y_pin = 1; // analog pin connected to Y output


const unsigned long PADDLE_RATE = 40;
const unsigned long BALL_RATE = 20;
const uint8_t PADDLE_HEIGHT = 24;

//Define DISPLAY Pins
// On MEGA, MOSI is 51, CLK is 52
#define OLED_CS    12
#define OLED_DC    11
#define OLED_RESET 13
#define OLED_CLK 10 //D0
#define OLED_MOSI 9 //D1

// MOSI is Data pin on display breakout

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

//int threshold = range / 4;

void drawCourt();

uint8_t playerScore = 0, aiScore = 0;
uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;

unsigned long paddle_update;
const uint8_t CPU_X = 12;
uint8_t cpu_y = 16;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC);

  // Display the splash screen (we're legally required to do so)
  display.display();
  
  //fct millis afiseaza cate secunde au trecut de cand a pornit placa
  unsigned long start = millis();

  display.clearDisplay();
  drawCourt();


  while (millis() - start < 2000);
  pinMode(SW_pin, INPUT);
  digitalWrite(SW_pin, HIGH);
  display.display();

  ball_update = millis();
  paddle_update = ball_update;
}

void loop() {

  bool update = false;
  unsigned long time = millis();

  static bool up_state = false;
  static bool down_state = false;

  uint16_t yReading = readAxis(Y_pin);

  Serial.println(analogRead(yReading));
  if (time > ball_update) {
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;


    // check if ball hits vertical walls
    if (new_x == 0) {
      ++playerScore;

      display.display();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(85, 15);
      display.println(playerScore);

      display.setCursor(35, 15);
      display.println(aiScore);

      new_x = 64;
      new_y = 32;
      display.display();
      delay(3000);
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.setCursor(85, 15);
      display.println(playerScore);

      display.setCursor(35, 15);
      display.println(aiScore);
    }
    //check if ball hits vertical walls
    if (new_x == 127) {
      ++aiScore;
      display.display();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(85, 15);
      display.println(playerScore);

      display.setCursor(35, 15);
      display.println(aiScore);

      new_x = 64;
      new_y = 32;
      display.display();
      delay(3000);
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.setCursor(85, 15);
      display.println(playerScore);

      display.setCursor(35, 15);
      display.println(aiScore);
    }
    //if AI wins
    if (aiScore == 3) {
      display.display();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(25, 25);
      display.println("AI  WINS");

      new_x = 64;
      new_y = 32;
      display.display();
      delay(3000);
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.setCursor(25, 25);
      display.println("AI  WINS");

      aiScore = 0;
    }
    //if Player wins
    if (playerScore == 3) {
      display.display();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(25, 25);
      display.println("PLAYER");
      display.setCursor(25, 43);
      display.println("WINS");
      new_x = 64;
      new_y = 32;
      display.display();
      delay(3000);
      display.setTextSize(2);
      display.setTextColor(BLACK);
      display.setCursor(25, 25);
      display.println("PLAYER WINS");
      display.setCursor(25, 43);
      display.println("WINS");

      playerScore = 0;
    }
    // check if the ball hits the horizontal walls.
    if (new_y == 0 || new_y == 63) {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    // check if the ball hits the CPU paddle
    if (new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // check if the ball hits the player paddle
    if (new_x == PLAYER_X
        && new_y >= player_y
        && new_y <= player_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    display.drawPixel(ball_x, ball_y, BLACK);
    display.drawPixel(new_x, new_y, WHITE);
    ball_x = new_x;
    ball_y = new_y;

    ball_update += BALL_RATE;

    update = true;
  }

  if (time > paddle_update) {

    // Draw Middle Net
    display.drawLine(64, 0, 64, 5, WHITE);
    display.drawLine(64, 10, 64, 15, WHITE);
    display.drawLine(64, 20, 64, 25, WHITE);
    display.drawLine(64, 30, 64, 35, WHITE);
    display.drawLine(64, 40, 64, 45, WHITE);
    display.drawLine(64, 50, 64, 55, WHITE);
    display.drawLine(64, 60, 64, 65, WHITE);
    display.drawLine(64, 70, 64, 75, WHITE);

    paddle_update += PADDLE_RATE;

    // CPU paddle
    display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);
    const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
    if (cpu_y + half_paddle > ball_y) {
      cpu_y -= 1;
    }
    if (cpu_y + half_paddle < ball_y) {
      cpu_y += 1;
    }
    if (cpu_y < 1) cpu_y = 1;
    if (cpu_y + PADDLE_HEIGHT > 63) cpu_y = 63 - PADDLE_HEIGHT;
    display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, WHITE);

    // Player paddle
    display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
    if (analogRead(yReading) < 400) {
      player_y -= 1;
    }

    if (analogRead(yReading) > 600) {
      player_y += 1;
    }
    if (player_y < 1) player_y = 1;
    if (player_y + PADDLE_HEIGHT > 63) player_y = 63 - PADDLE_HEIGHT;
    display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

    update = true;
  }

  if (update)
    display.display();


}

void drawCourt() {
  display.setTextColor(BLACK);
  display.drawRect(0, 0, 128, 64, BLACK);
}

uint16_t readAxis(uint16_t thisAxis) {
  // read the analog input:
  uint16_t reading = analogRead(thisAxis);
}
