#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "hardware/adc.h"

//pinos e módulos controlador i2c selecionado
#define I2C_PORT i2c1
#define PINO_SCL 14
#define PINO_SDA 15

//botão do Joystick
const int SW = 22;  

//definição dos LEDs RGB
const uint BLUE_LED_PIN= 12;   // LED azul no GPIO 12
const uint RED_LED_PIN  = 13; // LED vermelho no GPIO 13
const uint GREEN_LED_PIN = 11;  // LED verde no GPIO 11

//variável para armazenar a posição do seletor do display
uint pos_y=12;

ssd1306_t disp;

//função para inicialização de todos os recursos do sistema
void inicializa(){
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);
    i2c_init(I2C_PORT, 400*1000);// I2C Inicialização. Usando 400Khz.
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);

//inicialização dos LEDs
    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    // Inicialmente, desligar o LED RGB
    gpio_put(RED_LED_PIN, 0);
    gpio_put(GREEN_LED_PIN, 0);
    gpio_put(BLUE_LED_PIN, 0);

    //botão do Joystick
    gpio_init(SW);             // Inicializa o pino do botão
    gpio_set_dir(SW, GPIO_IN); // Configura o pino do botão como entrada
    gpio_pull_up(SW);
    
}

//função escrita no display.
void print_texto(char *msg, uint pos_x, uint pos_y, uint scale){
    ssd1306_draw_string(&disp, pos_x, pos_y, scale, msg);//desenha texto
    ssd1306_show(&disp);//apresenta no Oled
}

//o desenho do retangulo fará o papel de seletor
void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp,x1,y1,x2,y2);
    ssd1306_show(&disp);
}


int main()
{
    inicializa();
     char *text = ""; //texto do menu
     uint countdown = 0; //verificar seleções para baixo do joystick
     uint countup = 2; //verificar seleções para cima do joystick
        
   
    while(true){
        //trecho de código aproveitado de https://github.com/BitDogLab/BitDogLab-C/blob/main/joystick/joystick.c
        adc_select_input(0);
        uint adc_y_raw = adc_read();
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max; //bar_y_pos determinará se o Joystick foi pressionado para cima ou para baixo

        //texto do Menu
        ssd1306_clear(&disp);//Limpa a tela
        print_texto(text="Menu", 52, 2, 1);
        print_retangulo(2,pos_y,120,18);
        print_texto(text="Programa 01", 6, 18, 1.5);
        print_texto(text="Programa 02", 6, 30, 1.5);
        print_texto(text="Programa 03", 6, 42, 1.5);

        //printf("Valor de y e: %d", bar_y_pos);
        //o valor de 20 é o estado de repouso do Joystick
        if(bar_y_pos < 20 && countdown <2){
            pos_y+=12;
            countdown+=1;
            countup-=1;
        }else
            if(bar_y_pos > 20 && countup <2){
                pos_y-=12;
                countup+=1;
                countdown-=1;
            }
      
       sleep_ms(250);

    //verifica se botão foi pressionado. Se sim, entra no switch case para verificar posição do seletor e chama acionamento dos leds.
        if(gpio_get(SW) == 0){
       switch (pos_y){
       case 12:
            gpio_put(BLUE_LED_PIN, 0);   
            gpio_put(RED_LED_PIN, 0);   
            gpio_put(GREEN_LED_PIN, 1);
        break;
        case 24:
            gpio_put(BLUE_LED_PIN, 0);   
            gpio_put(RED_LED_PIN, 1);   
            gpio_put(GREEN_LED_PIN, 0);
        break;
         case 36:
            gpio_put(BLUE_LED_PIN, 1);   
            gpio_put(RED_LED_PIN, 0);   
            gpio_put(GREEN_LED_PIN, 0);
        break;
       default:
            gpio_put(BLUE_LED_PIN, 0);   
            gpio_put(RED_LED_PIN, 0);   
            gpio_put(GREEN_LED_PIN, 0);
        break;
        }
       }
    }
    return 0;
}
