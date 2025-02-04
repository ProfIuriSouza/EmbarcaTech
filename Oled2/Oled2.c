#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define I2C_PORT i2c1
#define PINO_SCL 14
#define PINO_SDA 15

ssd1306_t disp;

void inicializa(){
    stdio_init_all();
    i2c_init(I2C_PORT, 400*1000);// I2C Inicialização. Usando 400Khz.
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);

}
void print_texto(char *msg){
    ssd1306_clear(&disp);//Limpa a tela
    ssd1306_draw_string(&disp, 8, 24, 2, msg);//desenha o texto
    ssd1306_show(&disp);//apresenta no Oled
}
void print_linha(int x1, int y1, int x2, int y2){
    ssd1306_draw_line(&disp, x1, y1, x2, y2);
    ssd1306_show(&disp);    
}
void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp,x1,y1,x2,y2);
    ssd1306_show(&disp);
}


int main()
{
    inicializa();
    char *text = "";
   
    while(true){
       text = "IFRN";
       print_texto(text);
       print_linha(0, 10, 127, 10);
       sleep_ms(2000);
       text = "Embarcateh";
       print_texto(text);
       print_linha(0, 10, 127, 10);
       print_linha(0, 55, 127, 55);
       sleep_ms(2000);
       text = "BitDogLab";
       print_texto(text);
       print_retangulo(2,2,120,55);
       sleep_ms(3000);     
    }
    return 0;
}
