#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/valorR.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define ADC_PIN 28 
#define Botao_A 5 

int R_conhecido = 47000;   // Resistor de 47k ohm
float R_x = 0.0;           // Resistor desconhecido
float ADC_VREF = 3.31;     // Tensão de referência do ADC
int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)
float R_x_original = 0.0;  // Valor que irá passar por mudanças de ajuste para entregar o valor real ao R_x
bool fora_da_escala = false; // Booleano responsável por verificar se esta dentro da escala ou não
float media = 0;
float tensao;
char str_x[5]; // Buffers para armazenar a string
char str_y[5]; 
bool cor = true;

ssd1306_t ssd;              // Inicializa a estrutura do display
Resistor* encontra_resistor(float Rx);
void i2cinit();
void disconfig();
void adcinit();
float media_adc();
float calculo_Rx_original(float m);
bool ajuste_Rx(float Rx_orig, float *pRx_corrigido);
void moldura(ssd1306_t* ssd, bool cor);
void display_fixo(ssd1306_t* ssd);
void display_alternando(ssd1306_t* ssd, Resistor* sel);
void display_escala(ssd1306_t* ssd);
void display_val(ssd1306_t* ssd, const char* str_x, const char* str_y);


// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

int main()
{
  // Para ser utilizado o modo BOOTSEL com botão B
  gpio_init(botaoB);
  gpio_set_dir(botaoB, GPIO_IN);
  gpio_pull_up(botaoB);
  gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  // Aqui termina o trecho para modo BOOTSEL com botão B

  i2cinit();
  disconfig();    // Configura o display
  adcinit();
  
  while (true){

    media = media_adc();
    R_x_original = calculo_Rx_original(media);
    fora_da_escala = ajuste_Rx(R_x_original, &R_x);

    Resistor* sel = NULL;
    if (!fora_da_escala) {
        sel = encontra_resistor(R_x);
    }

    sprintf(str_x, "%1.0f", media);
    sprintf(str_y, "%1.0f", R_x);

    moldura(&ssd, cor);
    display_fixo(&ssd);
    if(fora_da_escala)display_escala(&ssd);
    else if(sel) display_alternando(&ssd, sel);
    display_val(&ssd, str_x, str_y);
    ssd1306_send_data(&ssd);
    sleep_ms(700);

  }
}

Resistor* encontra_resistor(float Rx) {
  Resistor* melhor = NULL;
  float dif_min = 1e6f;
  int n = sizeof(resistores)/sizeof(resistores[0]);
  for (int i = 0; i < n; i++) {
      float dif = fabsf(resistores[i].valor - Rx);
      if (dif < dif_min) {
          dif_min = dif;
          melhor = &resistores[i];
      }
  }
  return melhor;
}

void i2cinit(){
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA);                                        // Pull up the data line
  gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
}

void disconfig(){
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd);     
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);
}

void adcinit(){
  adc_init();
  adc_gpio_init(ADC_PIN);
  adc_select_input(2);
}

float media_adc(){
  float soma = 0.0f;
  for (int i = 0; i < 500; i++) {
      soma += adc_read();
      sleep_ms(1);
  }
  return media = soma / 500.0f;
}

float calculo_Rx_original(float m){
  return (R_conhecido * m) / (ADC_RESOLUTION - m);
}

bool ajuste_Rx(float Rx_orig, float *pRx_corrigido){
  if (Rx_orig < 485.0f || Rx_orig > 105000.0f) return true;
  if (Rx_orig <  800.0f) *pRx_corrigido = Rx_orig * 0.70f;
  else if (Rx_orig <  900.0f) *pRx_corrigido = Rx_orig * 0.75f;
  else if (Rx_orig < 1500.0f) *pRx_corrigido = Rx_orig * 0.82f;
  else if (Rx_orig < 2500.0f) *pRx_corrigido = Rx_orig * 0.88f;
  else if (Rx_orig < 8000.0f) *pRx_corrigido = Rx_orig * 0.93f;
  else if (Rx_orig < 50000.0f)*pRx_corrigido = Rx_orig * 1.02f;
  else *pRx_corrigido = Rx_orig * 1.10f;
  return false;
}

void moldura(ssd1306_t* ssd, bool cor){
  ssd1306_fill(   ssd, !cor);
  ssd1306_rect(   ssd, 3,  3, 122, 60, cor, !cor);
  ssd1306_line(   ssd, 3, 25, 123, 25, cor);
  ssd1306_line(   ssd, 3, 37, 123, 37, cor);
  ssd1306_line(   ssd, 44, 37, 44, 60, cor);
}

void display_fixo(ssd1306_t* ssd){
  ssd1306_draw_string(ssd, "ADC",       13, 41);
  ssd1306_draw_string(ssd, "Resisten.", 50, 41);
}

void display_alternando(ssd1306_t* ssd, Resistor* sel){
  ssd1306_draw_string(ssd, sel->cor1,       8,  6);
  ssd1306_draw_string(ssd, sel->cor2,       8, 16);
  ssd1306_draw_string(ssd, sel->multiplicador, 8, 28);
}

void display_escala(ssd1306_t* ssd){
  ssd1306_draw_string(ssd, "Fora da escala", 8,  6);
  ssd1306_draw_string(ssd, "Tolerancia",     23, 16);
  ssd1306_draw_string(ssd, "Ouro",           45, 28);
}

void display_val(ssd1306_t* ssd, const char* str_x, const char* str_y){
  ssd1306_draw_string(ssd, str_x,  8, 52);
  ssd1306_draw_string(ssd, str_y, 59, 52);
}