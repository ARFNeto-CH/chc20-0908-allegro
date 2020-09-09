#include "part.h"

int main(int argc, char** argv)
{
    ALLEGRO_EVENT   ev;
    Config cfg;
    power_on(&cfg); // inicia tudo
    Tela* tela = inicia_tela(&cfg);
    atualiza_tela(tela, &cfg);
    do
    {
        //mostra_nuvem(tela->nuvem);
        tela = proxima_nuvem(tela, &cfg);
        //mostra_nuvem(tela->nuvem);
        atualiza_tela(tela, &cfg);
        //al_flip_display();
        al_get_keyboard_state(&cfg.teclado);

        if (!al_is_event_queue_empty(cfg.eventos))
        {
            al_wait_for_event(
                cfg.eventos,
                &ev
            );
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                break;
            else
                if (
                    (ev.type == ALLEGRO_EVENT_KEY_DOWN) &&
                    (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    ) break;
        };  // if()
    } while (1);

    al_destroy_display(cfg.display);
    al_destroy_event_queue(cfg.eventos);

    return 0;
};  // main()


void        atualiza_tela(Tela* T, Config* cfg)
{
    // desenha as particulas no bitmap e
    // depois copia pra tela
    al_set_target_bitmap(T->buffer);
    for (int i = 0; i < T->nuvem->nPart; i += 1)
    {
        // desenha os circulos correspondentes 
        // aas particulas
        al_draw_filled_circle(
            T->nuvem->p[i]->x,
            T->nuvem->p[i]->y,
            T->nuvem->p[i]->raio,
            T->nuvem->p[i]->cor);
    };
    al_set_target_bitmap(
        al_get_backbuffer(cfg->display)
    );
    al_draw_bitmap(T->buffer, 0, 0, 0);
    al_flip_display();
    return;
};  // atualiza_tela()

int         colide_com_nuvem(const int pos, const Nuvem* N)
{   // retorna -1 se particula em pos 
    // nao colide com alguma
    // particula na nuvem N
    for (int a = 0; a < N->nPart; a += 1)
        if (a != pos)
        {
            float qx = (N->p[pos]->x - N->p[a]->x) *
                (N->p[pos]->x - N->p[a]->x);

            float qy = (N->p[pos]->y - N->p[a]->y) *
                (N->p[pos]->y - N->p[a]->y);

            float qr = (N->p[pos]->raio + N->p[a]->raio) *
                (N->p[pos]->raio + N->p[a]->raio);

            if ((qx + qy) <= qr) return a;
        }
    return -1;
};  // colide_com_nuvem()

Particula* gera_particula(Config* cfg)
{
    Particula* p = (Particula*)malloc(sizeof(Particula));
    p->raio = 2. + (rand() % 8);
    p->massa = p->raio;
    // as velocidades
    p->vx = -5 + (rand() % 11);
    p->vy = -5 + (rand() % 11);
    // as coordenadas
    p->x = p->raio + (rand() % cfg->largura - p->raio);
    p->y = p->raio + (rand() % cfg->altura - p->raio);

    p->cor = nova_cor(cfg);
    return p;
};

int         impacto(
    const uint16_t a,
    const uint16_t b,
    const Nuvem* N, const Config* cfg)
{
    const double PI = 3.1415926536;
    // logica mais simples: a bate em b 
    // a menor para e a maior se divide
    //
    float qx = (N->p[a]->x - N->p[b]->x) *
        (N->p[a]->x - N->p[b]->x);
    float qy = (N->p[a]->y - N->p[b]->y) *
        (N->p[a]->y - N->p[b]->y);
    float qr = (N->p[a]->raio + N->p[b]->raio) *
        (N->p[a]->raio + N->p[b]->raio);
    if ((qx + qy) >= qr) return 0; // sem colisao

    uint16_t maior = a;
    uint16_t menor = b;
    if (N->p[a]->massa < N->p[b]->massa)
    {
        maior = b;
        menor = a;
    };
    // calcula os novos parametros 
    N->p[menor]->vx = N->p[menor]->vx * sin(PI / 4);
    if (N->p[menor]->vx == 0) N->p[menor]->vx = -5 + (rand() % 11);
    N->p[menor]->vy = N->p[menor]->vy * cos(PI / 4);
    if (N->p[menor]->vy == 0) N->p[menor]->vy = 5 - (rand() % 11);

    N->p[menor]->x = N->p[menor]->x - N->p[menor]->raio;
    if (N->p[menor]->x <= 0) N->p[menor]->x = 0;
    N->p[menor]->y = N->p[menor]->y - N->p[menor]->raio;
    if (N->p[menor]->y <= 0) N->p[menor]->y = 0;

    N->p[maior]->vx = N->p[maior]->vx * sin(PI / 4);
    if (N->p[maior]->vx == 0) N->p[maior]->vx = ((unsigned)rand() % 3 + 1) / 10.;
    N->p[maior]->vy = N->p[maior]->vy * cos(PI / 4);
    if (N->p[maior]->vy == 0) N->p[maior]->vy = ((unsigned)rand() % 3 + 1) / 10.;

    N->p[maior]->x = N->p[maior]->x + N->p[maior]->raio;
    if (N->p[maior]->x <= 0) N->p[maior]->x = 0;
    N->p[maior]->y = N->p[maior]->y + N->p[maior]->raio;
    if (N->p[maior]->y <= 0) N->p[maior]->y = 0;

    N->p[menor]->cor = nova_cor(cfg);
    return 1;
};  // impacto()

Tela* inicia_tela(Config* cfg)
{
    Tela* tela = (Tela*)malloc(sizeof(Tela));
    tela->H = cfg->altura;
    tela->W = cfg->largura;
    tela->buffer = al_create_bitmap(tela->W, tela->H);
    tela->nuvem = primeira_nuvem(cfg);
    return tela;
};

void        mostra_nuvem(Nuvem* nv)
{
    printf("Nuvem com %d particulas\n\n", nv->nPart);
    for (int i = 0; i < nv->nPart; i += 1)
    {
        printf("(%6.2f,%6.2f) M: %3.0f R: %3.0f Vel (x: %6.2f y: %6.2f) cor:(%3.3f, %3.3f, %3.3f)\n",
            nv->p[i]->x,
            nv->p[i]->y,
            nv->p[i]->raio,
            nv->p[i]->massa,
            nv->p[i]->vx,
            nv->p[i]->vy,
            nv->p[i]->cor.r,
            nv->p[i]->cor.g,
            nv->p[i]->cor.b
        );
    };
    printf("Tecle algo => \n");
    char ch = fgetc(stdin);
    return;
};  // mostra_nuvem()

ALLEGRO_COLOR   nova_cor(const Config* cfg)
{
    char not_ok = 1;
    unsigned char r, g, b;
    do
    {
        r = 10 + rand() % 246;
        g = 10 + rand() % 246;
        b = 10 + rand() % 246;
        not_ok =
            ((r / cfg->fundo.r) > 0.8) &&
            ((g / cfg->fundo.g) > 0.8) &&
            ((b / cfg->fundo.b) > 0.8);
    } while (not_ok);
    return al_map_rgb(r, g, b);
};  // nova_cor()

int         power_on(Config* cfg)
{    // basico para o teste
    Config padrao = {
        .n_Part = 40,
        .altura = 900,
        .largura = 1600,
        .min_velocidade = .05, // abaixo disso considera parado
        .fundo = { 127.F, 127.F, 127.F, 0 }, // cinza
        .semente = 2009090901,
        .time_out = 0.1, // em segundos
        .titulo = "Um teste"
    };
    *cfg = padrao;

    srand(cfg->semente);
    al_init(); // Allegro
    al_install_keyboard();
    al_install_mouse();
    if (al_init_primitives_addon())
    {
        //printf("Versao de primitives: '%ld'\n",
        //    al_get_allegro_primitives_version());
    }
    else
    {
        printf("Algo errado com os add_on()\n");
        exit(0);
    };
    cfg->display =
        al_create_display(cfg->largura, cfg->altura);
    al_clear_to_color(cfg->fundo);
    al_set_window_title(
        cfg->display,
        cfg->titulo);
    al_flip_display();
    cfg->eventos = al_create_event_queue();
    // para encerrar vai aceitar ESCAPE
    // ou fechar a janela 
    al_register_event_source(
        cfg->eventos,
        al_get_display_event_source(cfg->display));

    al_register_event_source(
        cfg->eventos,
        al_get_keyboard_event_source());

    al_init_timeout(&cfg->timeout, cfg->time_out);

    return 0;
};  // power_on()

Nuvem* primeira_nuvem(Config* cfg)
{
    Nuvem* const nv = (Nuvem*)malloc(sizeof(Nuvem));
    nv->p = (Particula**)malloc(cfg->n_Part * sizeof(Particula*));
    // primeira
    nv->p[0] = gera_particula(cfg);
    nv->nPart = 1;
    // as outras
    while (nv->nPart < cfg->n_Part)
    {   // cria um vetor de particulas, a nuvem
        // coloca a nova na proxima posicao livre
        nv->p[nv->nPart] = gera_particula(cfg);
        nv->nPart += 1; // conta a nova
        while (colide_com_nuvem(nv->nPart - 1, nv) >= 0)
        {   // enquanto colidir tenta outra posicao
            nv->p[nv->nPart]->x = nv->p[nv->nPart]->raio +
                (rand() % cfg->largura - nv->p[nv->nPart]->raio);
            nv->p[nv->nPart]->y = nv->p[nv->nPart]->raio +
                (rand() % cfg->altura - nv->p[nv->nPart]->raio);
        };  // while()
        // ok: sem colisao
    };  // while()
    //
    // ajuste
    //
    Particula p0 = {
        .x = cfg->largura / 2, .y = cfg->altura - 100,
        .raio = 6, .massa = 6,
        .vx = 0., .vy = -1.,
        .cor = { 0, 0, 255 }
    };
    Particula p1 = {
        .x = cfg->largura / 2, .y = 100,
        .raio = 5, .massa = 5,
        .vx = 0., .vy = 1.,
        .cor = { 255, 0, 0 }
    };
    *(nv->p[0]) = p0;
    *(nv->p[1]) = p1;
    return nv;
};  // primeira_nuvem()

Tela* proxima_nuvem(const Tela* prev, const Config* cfg)
{   char n_colisoes = 0;
    do
    {   // move uma particula pA
        for (int a = 0; a < prev->nuvem->nPart; a += 1)
        {
            Particula* pA = prev->nuvem->p[a];
            if ((pA->vx == 0) && (pA->vy == 0)) continue;
            for (int b = a + 1; b < prev->nuvem->nPart; b += 1)
            {
                Particula* pB = prev->nuvem->p[b]; // a outra
                n_colisoes = impacto(a, b, prev->nuvem, cfg);
            };  // for()

            pA->x += pA->vx;
            pA->y += pA->vy; // pA andou
            // se esta nas bordas inverte o sentido
            if ((pA->x >= prev->W - pA->raio) ||
                (pA->x <= pA->raio)) pA->vx *= -1;
            if ((pA->y >= prev->H - pA->raio) ||
                (pA->y <= pA->raio)) pA->vy *= -1;
            if (n_colisoes > 0) break;
        };  // for()
    } while (n_colisoes != 0);
    return (Tela*)prev; // retorna a nova Tela
};  // proxima_nuvem()

float       vfinal(int m1, int m2, float v1, float v2)
{
    return (v1 * (m1 - m2) + 2 * v2 * m2) / (m1 + m2);
}
