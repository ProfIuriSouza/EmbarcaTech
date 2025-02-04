#include "pico/stdlib.h"

#define LED_R_PIN 12
#define LED_G_PIN 13
#define LED_B_PIN 11

#define BTN_A_PIN 5
#define BTN_B_PIN 6


void set_leds(bool red, bool green, bool blue){
    gpio_put(LED_R_PIN, red);
    gpio_put(LED_G_PIN, green);
    gpio_put(LED_B_PIN, blue);
}

int main(){
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);   

    while(true){
        int A_state = !gpio_get(BTN_A_PIN);
        int B_state = !gpio_get(BTN_B_PIN);
        if(A_state && B_state){
            set_leds(1,1,0);
        }
        else if(A_state){
            set_leds(1,0,0);
        }
        else if(B_state){
            set_leds(0,1,0);
        }
        else{
            set_leds(0,0,0);
        }

        sleep_ms(100);
    }

    return 0;
}