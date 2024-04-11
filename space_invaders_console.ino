/*********
  Complete project details at https://randomnerdtutorials.com

  This is an example for our Monochrome OLEDs based on SSD1306 drivers. Pick one up today in the adafruit shop! ------> http://www.adafruit.com/category/63_98
  This example is for a 128x32 pixel display using I2C to communicate 3 pins are required to interface (two I2C and one reset).
  Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!
  Written by Limor Fried/Ladyada for Adafruit Industries, with contributions from the open source community. BSD license, check license.txt for more information All text above, and the splash screen below must be included in any redistribution.
*********/

#include <stdlib.h>
#include <stdint.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16

uint8_t paddleX = 0;
uint8_t paddleY = 62;
const int right = 6;
const int left = 7;
const int fire = 8;

typedef struct
{
    uint8_t posX;
    uint8_t posY;
    uint8_t width;
    uint8_t height;
    uint8_t alienType;
    uint8_t alive;
} alien_t;

typedef struct
{
    uint8_t posX;
    uint8_t posY;
    uint8_t width;
    uint8_t height;
    uint8_t alive;
    uint8_t speed;
} player_t;

typedef struct
{
    uint8_t posX;
    uint8_t posY;
    uint8_t speed;
    uint8_t alive;
} bullet_t;

struct bulletNode
{
    bulletNode *next;
    bullet_t bullet;
};

typedef struct bulletNode bulletNode_t;

const uint8_t ALIEN_WIDTH = 8;
const uint8_t ALIEN_HEIGHT = 4;
const uint8_t NUM_ALIEN_ROWS = 4;
const uint8_t ALIENT_SPEED = 1;
const uint8_t NUM_ALIENS = 28;

const uint8_t TANK_WIDTH = 16;
const uint8_t TANK_HEIGHT = 2;
const uint8_t TANK_SPEED = 2;

const uint8_t BULLET_HEIGHT = 8;
const uint8_t BULLET_WIDTH = 2;

player_t tank = {
    .posX = 0,
    .posY = 62,
    .width = 16,
    .height = 2,
    .alive = 1,
    .speed = 2};

uint8_t ALIEN_DIRECTION = -1;

alien_t aliens[28];
alien_t spawnAlien(uint8_t, uint8_t, uint8_t alienType);

bulletNode_t *tankBulletList;

void loadAliens();
void update();
void render();
void spawnTankBullet(uint8_t, uint8_t);

void spawnTankBullet(uint8_t posX, uint8_t posY)
{
    bullet_t bullet = {
        .posX = posX,
        .posY = posY,
        .speed = 1,
        .alive = 1
    };
    bulletNode_t *bNode = (bulletNode_t*)malloc(sizeof(bulletNode_t));
    bNode->bullet = bullet;
    bNode->next = NULL;
    if (tankBulletList == NULL)
    {
        tankBulletList = bNode;
        return;
    }
    bulletNode_t *tHead = tankBulletList;
    while(tHead->next != NULL) {
        tHead = tHead->next;
    }
    tHead->next = bNode;
    Serial.println("asasas");
}

void loadAliens()
{
    uint8_t posX = 24;
    uint8_t posY = 8;

    uint8_t alientIndex = 0;
    for (size_t row = 1; row <= NUM_ALIEN_ROWS; row++)
    {
        while (posX < SCREEN_WIDTH)
        {
            aliens[alientIndex] = spawnAlien(posX, posY, 1);
            posX += 16;
            alientIndex += 1;
        }
        posX = 24;
        posY += 8;
    }
}

alien_t spawnAlien(uint8_t posX, uint8_t posY, uint8_t alienType)
{
    return (alien_t){
        .posX = posX,
        .posY = posY,
        .width = ALIEN_WIDTH,
        .height = ALIEN_HEIGHT,
        .alienType = alienType,
        .alive = 1};
}

void render()
{
    display.clearDisplay();
    for (size_t alienIndex = 0; alienIndex < 28; alienIndex++)
    {
        if (aliens[alienIndex].alive == 1)
        {
            display.fillRect(aliens[alienIndex].posX, aliens[alienIndex].posY, aliens[alienIndex].width, aliens[alienIndex].height, SSD1306_INVERSE);
        }
    }
    if (tank.alive == 1)
    {
        display.fillRect(tank.posX, tank.posY, tank.width, tank.height, SSD1306_INVERSE);
    }
    bulletNode_t *tHead = tankBulletList;
    while (tHead != NULL) {
        if (tHead->bullet.alive == 1) {
            display.fillRect(tHead->bullet.posX, tHead->bullet.posY, BULLET_WIDTH, BULLET_HEIGHT, SSD1306_INVERSE);
        }
        tHead = tHead->next;
    }
    display.display();
}

void update()
{
    int rightVal = digitalRead(right);
    int leftVal = digitalRead(left);
    int fireVal = digitalRead(fire);
    Serial.printf("%d  %d   %d\n", leftVal, rightVal, fireVal);
    if (rightVal == 0 && (tank.posX + tank.width < SCREEN_WIDTH))
    {
        tank.posX += tank.speed;
    }
    if (leftVal == 0 && tank.posX > 0)
    {
        tank.posX -= tank.speed;
    }
    if (fireVal == 0) {
        spawnTankBullet(tank.posX + tank.width / 2, tank.posY - 4);
    }


    int updateAlienDirection = 0;

    for (size_t alienIndex = 0; alienIndex < NUM_ALIENS; alienIndex++)
    {
        if (aliens[alienIndex].alive == 1)
        {
            aliens[alienIndex].posX += (ALIENT_SPEED * ALIEN_DIRECTION);
            if (aliens[alienIndex].posX == 0 || (aliens[alienIndex].posX + ALIEN_WIDTH == SCREEN_WIDTH))
            {
                updateAlienDirection = 1;
            }
        }
    }

    ALIEN_DIRECTION = updateAlienDirection == 1 ? ALIEN_DIRECTION * -1 : ALIEN_DIRECTION;

    uint8_t moveDown = 0;
    if (moveDown == 1)
    {
        if (updateAlienDirection == 1)
        {
            for (size_t i = 0; i < NUM_ALIENS; i++)
            {
                if (aliens[i].alive == 1)
                {
                    aliens[i].posY += 1;
                }
            }
        }
    }

    bulletNode_t *tHead = tankBulletList;
    while (tHead != NULL) {
        if (tHead->bullet.alive == 1) {
            tHead->bullet.posY -= tHead->bullet.speed;
            if (tHead->bullet.posY < 0) {
                tHead->bullet.alive = 0;
            }
        }
        tHead = tHead->next;
    }

    free(tHead);
    bulletNode_t *prev = tankBulletList;
    if (prev != NULL) {
        bulletNode_t *tHead = prev->next;
        while(tHead != NULL) {
            if (tHead->bullet.alive == 0) {
                
            }
        }
    }
    
}

void setup()
{
    Serial.begin(115200);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    // Clear the buffer
    display.clearDisplay();
    pinMode(right, INPUT_PULLUP);
    pinMode(left, INPUT_PULLUP);
    pinMode(fire, INPUT_PULLUP);
    loadAliens();
}

void loop()
{
    update();
    render();
    delay((int)(1000 / 60));
}
