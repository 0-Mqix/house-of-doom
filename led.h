#ifndef LED_H
#define LED_H

#include <Arduino.h>

const int color_matrix[][3] = {
    {1, 0, 0}, //red 
    {1, 1, 0}, //yellow
    {0, 1, 0}, //green
    {0, 1, 1},  //cyan
    {0, 0, 1}, //blue
    {1, 0, 1} //purple
};

enum Color {
    red,
    yellow,
    green,
    cyan,
    blue,
    purple
};

class RGB {
    int color_position = 0;
    bool active = true;

   int red_pin;
   int green_pin;
   int blue_pin;

public:   
    RGB(int r_pin, int g_pin, int b_pin) {
        red_pin = r_pin;
        green_pin = g_pin;
        blue_pin = b_pin;
    }
    
    void show() {
        if (!active) {
            digitalWrite(red_pin, 0);
            digitalWrite(green_pin, 0);
            digitalWrite(blue_pin, 0);
            return;
        }

        digitalWrite(red_pin, color_matrix[color_position][0]);
        digitalWrite(green_pin, color_matrix[color_position][1]);
        digitalWrite(blue_pin, color_matrix[color_position][2]);
    } 


    void begin() {
        pinMode(red_pin, OUTPUT);        
        pinMode(green_pin, OUTPUT);        
        pinMode(blue_pin, OUTPUT);        
        
        show();
    }

    void shift() {
        color_position = color_position < 5 ? color_position + 1 : 0;
    }

    void set_color(Color color) {
        color_position = color;
    }

    void random_color() {
        int random_number = random(0, 6);

        if (random_number == color_position) {
            random_color();
            return;
        }

        Serial.println(random_number);

        set_color(random_number);
    }

    void toggle() {
        active = !active;
    }

    bool is_active() {
        return active;
    }
};


#endif