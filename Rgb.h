#ifndef LED_H
#define LED_H

#include <Arduino.h>

const int color_matrix[][3] = {
    {1, 0, 0}, //red 
    {1, 1, 0}, //yellow
    {0, 1, 0}, //green
    {0, 1, 1}, //cyan
    {0, 0, 1}, //blue
    {1, 0, 1}, //purple
    {1, 1, 1} //white
};

enum Color {
    RED,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    PUPLE,
    WHITE
};

class RGB {
private:
    int color_position = 0;
    bool active = false;
    bool is_analog = false;

   int red_pin;
   int green_pin;
   int blue_pin;

public:   
    RGB(int r_pin, int g_pin, int b_pin, bool is_analog = false) {
        red_pin = r_pin;
        green_pin = g_pin;
        blue_pin = b_pin;
        
        this->is_analog = is_analog;
    }
    
    void update() {
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


    void init() {
        pinMode(red_pin, OUTPUT);        
        pinMode(green_pin, OUTPUT);        
        pinMode(blue_pin, OUTPUT);        
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

        set_color((Color)random_number);
    }

    void toggle() {
        active = !active;
    }

    bool is_active() {
        return active;
    }

    void set_active(bool value) {
        active = value;
    }
};


#endif