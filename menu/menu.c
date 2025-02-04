#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "inc/ssd1306.h"
#include "hardware/adc.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

ssd1306_t disp;

void print_texto(char *msg){
    ssd1306_clear(&disp);//Limpa a tela
    ssd1306_draw_string(&disp, 8, 24, 2, msg);//desenha o texto
    ssd1306_show(&disp);//apresenta no Oled
}
/*void print_linha(int x1, int y1, int x2, int y2){
    ssd1306_draw_line(&disp, x1, y1, x2, y2);
    ssd1306_show(&disp);    
}*/
void print_retangulo(int x1, int y1, int x2, int y2){
    ssd1306_draw_empty_square(&disp,x1,y1,x2,y2);
    ssd1306_show(&disp);
}

int main()
{
    stdio_init_all();
    adc_init();
    char *text = "";

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);

    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(26);
    adc_gpio_init(27);

    while (true) {
        //leitura da posição do Joystick
        adc_select_input(0);
        uint adc_y_raw = adc_read();
        adc_select_input(1);
        uint adc_x_raw = adc_read();

        text = "Menu";
        print_texto(text);

        // Display the joystick position something like this:
        // X: [            o             ]  Y: [              o         ]
        const uint bar_width = 40;
        const uint adc_max = (1 << 12) - 1;
        uint bar_x_pos = adc_x_raw * bar_width / adc_max;
        uint bar_y_pos = adc_y_raw * bar_width / adc_max;
        printf("\rX: [");
        for (uint i = 0; i < bar_width; ++i)
            putchar( i == bar_x_pos ? 'o' : ' ');
        printf("]  Y: [");
        for (uint i = 0; i < bar_width; ++i)
            putchar( i == bar_y_pos ? 'o' : ' ');
        printf("]");
        sleep_ms(50);
    }
    return 0;
}
