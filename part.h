#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

typedef struct
{
    float           x;
    float           y;
    float           raio;
    float           massa;
    float           vx;
    float           vy;
    ALLEGRO_COLOR   cor;

}   Particula;

typedef struct
{
    uint16_t    nPart; // contagem
    Particula** p;  // as proprias

}   Nuvem;

typedef struct
{
    ALLEGRO_BITMAP* buffer;
    uint16_t        H; // altura em pixels
    Nuvem* nuvem;
    uint16_t        W; // largura

}   Tela;

typedef struct
{
    uint16_t        altura;
    uint16_t        largura;
    double          min_velocidade;
    uint16_t        n_Part;
    unsigned        semente;
    double          time_out;
    const char* titulo;

    ALLEGRO_COLOR           fundo;
    ALLEGRO_DISPLAY* display;
    ALLEGRO_EVENT_QUEUE* eventos;
    ALLEGRO_KEYBOARD_STATE  teclado;
    ALLEGRO_TIMEOUT         timeout;

}   Config;

void        atualiza_tela(Tela*, Config*);
int         colide_com_nuvem(const int, const Nuvem*);
Particula* gera_particula(Config*);
int         impacto(
    const uint16_t a,
    const uint16_t b,
    const Nuvem* N,
    const Config*);
Tela* inicia_tela(Config*);
void        mostra_nuvem(Nuvem*);
ALLEGRO_COLOR   nova_cor(const Config*);
int         power_on(Config*);
Nuvem* primeira_nuvem(Config*);
Tela* proxima_nuvem(const Tela*, const Config*);
float       vfinal(int, int, float, float);
