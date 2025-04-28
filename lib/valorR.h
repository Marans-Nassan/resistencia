#include <stdio.h>

typedef struct {
    int valor;
    const char* cor1;
    const char* cor2;
    const char* multiplicador;
} Resistor;

Resistor resistores[] = {
    {510,   "Faixa 1 - Verde",    "Faixa 2 - Marrom", "Mult - Marrom"},
    {560,   "Faixa 1 - Verde",    "Faixa 2 - Azul",   "Mult - Marrom"},
    {600,   "Faixa 1 - Azul",     "Faixa 2 - Preto",  "Mult - Marrom"},
    {680,   "Faixa 1 - Azul",     "Faixa 2 - Cinza",  "Mult - Marrom"},
    {750,   "Faixa 1 - Violeta",  "Faixa 2 - Verde",  "Mult - Marrom"},
    {820,   "Faixa 1 - Cinza",    "Faixa 2 - Vermelho", "Mult - Marrom"},
    {910,   "Faixa 1 - Branco",   "Faixa 2 - Marrom", "Mult - Marrom"},
    {1000,  "Faixa 1 - Marrom",   "Faixa 2 - Preto",  "Mult - Vermelho"},
    {1100,  "Faixa 1 - Marrom",   "Faixa 2 - Marrom", "Mult - Vermelho"},
    {1200,  "Faixa 1 - Marrom",   "Faixa 2 - Vermelho", "Mult - Vermelho"},
    {1500,  "Faixa 1 - Marrom",   "Faixa 2 - Verde",  "Mult - Vermelho"},
    {1800,  "Faixa 1 - Marrom",   "Faixa 2 - Cinza",  "Mult - Vermelho"},
    {2200,  "Faixa 1 - Vermelho", "Faixa 2 - Vermelho", "Mult - Vermelho"},
    {2700,  "Faixa 1 - Vermelho", "Faixa 2 - Violeta", "Mult - Vermelho"},
    {3300,  "Faixa 1 - Laranja",  "Faixa 2 - Laranja", "Mult - Vermelho"},
    {3900,  "Faixa 1 - Laranja",  "Faixa 2 - Branco", "Mult - Vermelho"},
    {4700,  "Faixa 1 - Amarelo",  "Faixa 2 - Violeta", "Mult - Vermelho"},
    {5600,  "Faixa 1 - Verde",    "Faixa 2 - Azul",   "Mult - Vermelho"},
    {6800,  "Faixa 1 - Azul",     "Faixa 2 - Cinza",  "Mult - Vermelho"},
    {8200,  "Faixa 1 - Cinza",    "Faixa 2 - Vermelho", "Mult - Vermelho"},
    {10000, "Faixa 1 - Marrom",   "Faixa 2 - Preto",  "Mult - Laranja"},
    {12000, "Faixa 1 - Marrom",   "Faixa 2 - Vermelho", "Mult - Laranja"},
    {15000, "Faixa 1 - Marrom",   "Faixa 2 - Verde",  "Mult - Laranja"},
    {18000, "Faixa 1 - Marrom",   "Faixa 2 - Cinza",  "Mult - Laranja"},
    {22000, "Faixa 1 - Vermelho", "Faixa 2 - Vermelho", "Mult - Laranja"},
    {27000, "Faixa 1 - Vermelho", "Faixa 2 - Violeta", "Mult - Laranja"},
    {33000, "Faixa 1 - Laranja",  "Faixa 2 - Laranja", "Mult - Laranja"},
    {39000, "Faixa 1 - Laranja",  "Faixa 2 - Branco", "Mult - Laranja"},
    {47000, "Faixa 1 - Amarelo",  "Faixa 2 - Violeta", "Mult - Laranja"},
    {56000, "Faixa 1 - Verde",    "Faixa 2 - Azul",   "Mult - Laranja"},
    {68000, "Faixa 1 - Azul",     "Faixa 2 - Cinza",  "Mult - Laranja"},
    {82000, "Faixa 1 - Cinza",    "Faixa 2 - Vermelho", "Mult - Laranja"},
    {91000, "Faixa 1 - Branco",   "Faixa 2 - Marrom", "Mult - Laranja"},
    {100000,"Faixa 1 - Marrom",   "Faixa 2 - Preto",  "Mult - Amarelo"}
};