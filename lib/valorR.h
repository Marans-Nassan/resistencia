#include <stdio.h>

typedef struct {
    int valor;
    const char* cor1;
    const char* cor2;
    const char* multiplicador;
} Resistor;

Resistor resistores[] = {
    {  510, "Verde",  "Marrom",   "Marrom"},   
    {  680, "Azul",   "Cinza",    "Marrom"},   
    { 1000, "Marrom", "Preto",    "Vermelho"}, 
    { 2200, "Vermelho","Vermelho","Vermelho"}, 
    { 3300, "Laranja","Laranja",  "Vermelho"},  
    { 4700, "Amarelo","Violeta",  "Vermelho"}, 
    { 6800, "Azul",   "Cinza",    "Vermelho"},  
    {10000, "Marrom", "Preto",    "Laranja"},  
    {47000, "Amarelo","Violeta",  "Laranja"}, 
    {68000, "Azul",   "Cinza",    "Laranja"},  
    {100000,"Marrom", "Preto",    "Amarelo"}  
};