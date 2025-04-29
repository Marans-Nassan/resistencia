#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/valorR.h"

#define I2C_PORT      i2c1
#define I2C_SDA       14
#define I2C_SCL       15
#define endereco      0x3C
#define ADC_PIN       28

// --- Parâmetros de medição ---
int   R_conhecido       = 47000;   // Resistor de referência, 47 kΩ
float R_x               = 0.0f;    // Valor calculado do resistor desconhecido
int   ADC_RESOLUTION    = 4095;    // 12 bits
float R_x_original      = 0.0f;    // Valor bruto antes de ajuste
bool  fora_da_escala    = false;   // Flag se R_x_original está fora da faixa 510–100 kΩ
float media             = 0.0f;    // Média das leituras do ADC
char  str_x[6];                    // Buffer para string da média
char  str_y[6];                    // Buffer para string de R_x
bool  cor                = true;   // Controle de inversão de cor pra animação

ssd1306_t ssd;                     // Struct do display OLED

// Protótipos
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

int main() {

    i2cinit(); 
    disconfig();
    adcinit();

    while (true) {
        // 1) Mede e calcula média do ADC
        media = media_adc();

        // 2) Calcula valor bruto do resistor (antes de correções)
        R_x_original = calculo_Rx_original(media);

        // 3) Aplica fator de correção ou sinaliza fora da faixa
        fora_da_escala = ajuste_Rx(R_x_original, &R_x);

        // 4) Se estiver na faixa, encontra as cores correspondentes
        Resistor* sel = NULL;
        if (!fora_da_escala) {
            sel = encontra_resistor(R_x);
        }

        // 5) Formata valores para exibição
        sprintf(str_x, "%1.0f", media);
        sprintf(str_y, "%1.0f", R_x);

        // 6) Desenha tela
        moldura(&ssd, cor);  // bordas e linhas
        display_fixo(&ssd);  // textos estáticos
        if (fora_da_escala) {
            display_escala(&ssd); // mensagem "Fora da escala"
        } else if (sel) {
            display_alternando(&ssd, sel);  // cores do resistor
        }
        display_val(&ssd, str_x, str_y);  // valores numéricos

        // 7) Envia tudo de uma vez e espera
        ssd1306_send_data(&ssd);
        sleep_ms(700);
    }
}

//Funções

// Busca no array 'resistores[]' o elemento com valor mais próximo de Rx
Resistor* encontra_resistor(float Rx) {
    Resistor* melhor = NULL;
    float dif_min   = 1e6f;
    int   n         = sizeof(resistores) / sizeof(resistores[0]);
    for (int i = 0; i < n; i++) {
        float dif = fabsf(resistores[i].valor - Rx);
        if (dif < dif_min) {
            dif_min = dif;
            melhor  = &resistores[i];
        }
    }
    return melhor;
}

// Inicializa I2C e configura GPIOs SDA/SCL
void i2cinit() {
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Inicializa o display SSD1306, limpa tela e configura I2C internamente
void disconfig() {
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Inicializa ADC e seleciona o canal do pino ADC_PIN
void adcinit() {
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(2);
}

// Lê 500 amostras, calcula e retorna a média
float media_adc() {
    float soma = 0.0f;
    for (int i = 0; i < 500; i++) {
        soma += adc_read();
        sleep_ms(1);
    }
    return soma / 500.0f;
}

// Calcula valor bruto de R_x a partir da média do ADC
float calculo_Rx_original(float m) {
    return (R_conhecido * m) / (ADC_RESOLUTION - m);
}

// Corrige R_x pelo fator de tolerância e indica se está fora da faixa 485–105000
bool ajuste_Rx(float Rx_orig, float *pRx_corrigido) {
    if (Rx_orig < 485.0f || Rx_orig > 105000.0f) return true;  // fora da escala
    if (Rx_orig <  800.0f) *pRx_corrigido = Rx_orig * 0.70f;
    else if (Rx_orig <  900.0f) *pRx_corrigido = Rx_orig * 0.75f;
    else if (Rx_orig < 1500.0f) *pRx_corrigido = Rx_orig * 0.82f;
    else if (Rx_orig < 2500.0f) *pRx_corrigido = Rx_orig * 0.88f;
    else if (Rx_orig < 8000.0f) *pRx_corrigido = Rx_orig * 0.93f;
    else if (Rx_orig < 50000.0f) *pRx_corrigido = Rx_orig * 1.02f;
    else *pRx_corrigido = Rx_orig * 1.10f;
    return false;  // dentro da escala
}

// Desenha moldura (retângulo e linhas de separação)
void moldura(ssd1306_t* ssd, bool cor) {
    ssd1306_fill(   ssd, !cor);
    ssd1306_rect(   ssd, 3,  3, 122, 60, cor, !cor);
    ssd1306_line(   ssd, 3, 25, 123, 25, cor);
    ssd1306_line(   ssd, 3, 37, 123, 37, cor);
    ssd1306_line(   ssd, 44,37,  44, 60, cor);
}

// Desenha textos fixos "ADC" e "Resisten."
void display_fixo(ssd1306_t* ssd) {
    ssd1306_draw_string(ssd, "ADC",       13, 41);
    ssd1306_draw_string(ssd, "Resisten.", 50, 41);
}

// Desenha as três faixas de cor do resistor encontrado
void display_alternando(ssd1306_t* ssd, Resistor* sel) {
    ssd1306_draw_string(ssd, sel->cor1,       8,  6);
    ssd1306_draw_string(ssd, sel->cor2,       8, 16);
    ssd1306_draw_string(ssd, sel->multiplicador, 8, 28);
}

// Desenha mensagem de erro quando fora da escala
void display_escala(ssd1306_t* ssd) {
    ssd1306_draw_string(ssd, "Fora da escala", 8,  6);
    ssd1306_draw_string(ssd, "Tolerancia",     23, 16);
    ssd1306_draw_string(ssd, "Ouro",           45, 28);
}

// Desenha os valores numéricos de média e R_x
void display_val(ssd1306_t* ssd, const char* str_x, const char* str_y) {
    ssd1306_draw_string(ssd, str_x,  8, 52);
    ssd1306_draw_string(ssd, str_y, 59, 52);
}