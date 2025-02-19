#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdlib.h>

// Constantes de Colores
const Uint32 COLOR_SALMON = 0xf29175;
const Uint32 COLOR_WHITE = 0xffffff;
const Uint32 COLOR_BLACK = 0x000000;
const Uint32 COLOR_LIGHT_BLACK = 0x2d2d2d;
const Uint32 COLOR_SHEET = 0x1f1f1f;

// Colores lápiz
const Uint32 COLOR_RED = 0xff0000;
const Uint32 COLOR_BLUE = 0x0000ff;
const Uint32 COLOR_GREEN = 0x00ff00;
const Uint32 COLOR_YELLOW = 0xffff00;

// Tamaño pantalla
#define WIDTH 1800
#define HEIGHT 980

// Folio como tal
#define SHEET_X 50
#define SHEET_Y 200
#define SHEET_WIDTH (WIDTH - 100)
#define SHEET_HEIGHT (HEIGHT - 250)

// Slider para grosor del lápiz
#define SLIDER_X 10
#define SLIDER_Y 200
#define SLIDER_WIDTH 30
#define SLIDER_HEIGHT 400
#define SLIDER_MIN 4
#define SLIDER_MAX 16

// Botones
#define BACK_BUTTON_X 50
#define BACK_BUTTON_Y 50
#define FORWARD_BUTTON_X 110
#define FORWARD_BUTTON_Y 50
#define COLOR_BUTTON_X 170
#define COLOR_BUTTON_Y 50
#define BUTTON_SIZE 50

// Estados
int brushSize = 4;
Uint32 currentColor = 0xffffffff;
int isDraggingSlider = 0;
int showColorMenu = 0;
SDL_Surface* canvas = NULL;

typedef struct {
    SDL_Surface **surfaces;
    int size;
    int capacity;
} Stack;

Stack undoStack = {0};
Stack redoStack = {0};

void pushStack(Stack *stack, SDL_Surface *surface) {
    if (stack->size >= stack->capacity) {
        int newCapacity = stack->capacity == 0 ? 4 : stack->capacity * 2;
        SDL_Surface **newSurfaces = realloc(stack->surfaces, newCapacity * sizeof(SDL_Surface*));
        if (!newSurfaces) return;
        stack->surfaces = newSurfaces;
        stack->capacity = newCapacity;
    }
    stack->surfaces[stack->size++] = surface;
}

SDL_Surface* popStack(Stack *stack) {
    if (stack->size == 0) return NULL;
    return stack->surfaces[--stack->size];
}

void screen(SDL_Surface* surface, SDL_Surface* backImage, SDL_Surface* forwardImage, SDL_Surface* colorImage) {
    SDL_FillRect(surface, NULL, COLOR_LIGHT_BLACK);
    SDL_Rect sheet = {SHEET_X, SHEET_Y, SHEET_WIDTH, SHEET_HEIGHT};
    SDL_BlitSurface(canvas, NULL, surface, &sheet);

    // Slider
    SDL_Rect slider = {SLIDER_X, SLIDER_Y, SLIDER_WIDTH, SLIDER_HEIGHT};
    SDL_FillRect(surface, &slider, COLOR_SHEET);
    int sliderKnobY = SLIDER_Y + (SLIDER_HEIGHT * (brushSize - SLIDER_MIN)) / (SLIDER_MAX - SLIDER_MIN);
    SDL_Rect knob = {SLIDER_X + 5, sliderKnobY, SLIDER_WIDTH - 10, 20};
    SDL_FillRect(surface, &knob, COLOR_WHITE);

    // Botones
    SDL_Rect backButton = {BACK_BUTTON_X, BACK_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE};
    SDL_BlitSurface(backImage, NULL, surface, &backButton);
    
    SDL_Rect forwardButton = {FORWARD_BUTTON_X, FORWARD_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE};
    SDL_BlitSurface(forwardImage, NULL, surface, &forwardButton);

    SDL_Rect colorButton = {COLOR_BUTTON_X, COLOR_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE};
    SDL_FillRect(surface, &colorButton, COLOR_WHITE);
    if (colorImage) SDL_BlitSurface(colorImage, NULL, surface, &colorButton);

    // Menú de colores
    if (showColorMenu) {
        Uint32 colors[] = {COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};
        for (int i = 0; i < 6; i++) {
            SDL_Rect colorRect = {COLOR_BUTTON_X + BUTTON_SIZE + 10 + (i * 60), COLOR_BUTTON_Y, 50, 50};
            SDL_FillRect(surface, &colorRect, colors[i]);
        }
    }
}

int isInsideSheet(int x, int y) {
    return (x >= SHEET_X && x < SHEET_X + SHEET_WIDTH && y >= SHEET_Y && y < SHEET_Y + SHEET_HEIGHT);
}

int isInsideSlider(int x, int y) {
    return (x >= SLIDER_X && x < SLIDER_X + SLIDER_WIDTH && y >= SLIDER_Y && y < SLIDER_Y + SLIDER_HEIGHT);
}

int isInsideColorButton(int x, int y) {
    return (x >= COLOR_BUTTON_X && x < COLOR_BUTTON_X + BUTTON_SIZE && 
            y >= COLOR_BUTTON_Y && y < COLOR_BUTTON_Y + BUTTON_SIZE);
}

int isInsideBackButton(int x, int y) {
    return (x >= BACK_BUTTON_X && x < BACK_BUTTON_X + BUTTON_SIZE && 
            y >= BACK_BUTTON_Y && y < BACK_BUTTON_Y + BUTTON_SIZE);
}

int isInsideForwardButton(int x, int y) {
    return (x >= FORWARD_BUTTON_X && x < FORWARD_BUTTON_X + BUTTON_SIZE && 
            y >= FORWARD_BUTTON_Y && y < FORWARD_BUTTON_Y + BUTTON_SIZE);
}

int getColorFromMenu(int x, int y) {
    if (!showColorMenu) return -1;
    Uint32 colors[] = {COLOR_BLACK, COLOR_WHITE, COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW};
    for (int i = 0; i < 6; i++) {
        if (x >= COLOR_BUTTON_X + BUTTON_SIZE + 10 + (i * 60) && 
            x < COLOR_BUTTON_X + BUTTON_SIZE + 60 + (i * 60) &&
            y >= COLOR_BUTTON_Y && y < COLOR_BUTTON_Y + 50) {
            return colors[i];
        }
    }
    return -1;
}

void drawCircle(SDL_Surface* surface, int x, int y, int radius, Uint32 color) {
    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / sizeof(Uint32);
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx*dx + dy*dy <= radius*radius) {
                int px = x + dx;
                int py = y + dy;
                if (px >= 0 && px < SHEET_WIDTH && py >= 0 && py < SHEET_HEIGHT) {
                    pixels[py * pitch + px] = color;
                }
            }
        }
    }
}

void drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, int radius, Uint32 color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    int currentX = x0;
    int currentY = y0;

    while (1) {
        drawCircle(surface, currentX, currentY, radius, color);
        if (currentX == x1 && currentY == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            currentX += sx;
        }
        if (e2 < dx) {
            err += dx;
            currentY += sy;
        }
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Dibujo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    
    // Cargar imágenes
    SDL_Surface* backImage = SDL_LoadBMP("img/back.bmp");
    SDL_Surface* forwardImage = SDL_LoadBMP("img/forward.bmp");
    SDL_Surface* colorImage = SDL_LoadBMP("img/pencil.bmp");
    
    canvas = SDL_CreateRGBSurface(0, SHEET_WIDTH, SHEET_HEIGHT, 32, 0, 0, 0, 0);
    SDL_FillRect(canvas, NULL, COLOR_SHEET);

    // Guardar estado inicial
    SDL_Surface* initialCopy = SDL_ConvertSurface(canvas, canvas->format, 0);
    pushStack(&undoStack, initialCopy);

    screen(surface, backImage, forwardImage, colorImage);
    SDL_UpdateWindowSurface(window);

    short int running = 1;
    SDL_Event event;
    int prevDrawX = -1, prevDrawY = -1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x, my = event.button.y;
                if (isInsideSlider(mx, my)) {
                    isDraggingSlider = 1;
                }
                else if (isInsideBackButton(mx, my)) {
                    // Deshacer solo si hay más de 1 estado guardado
                    if (undoStack.size > 1) {
                        SDL_Surface* prevState = popStack(&undoStack);
                        SDL_Surface* currentState = SDL_ConvertSurface(canvas, canvas->format, 0);
                        pushStack(&redoStack, currentState);
                        SDL_FreeSurface(canvas);
                        canvas = prevState;
                    }
                }
                else if (isInsideForwardButton(mx, my)) {
                    // Rehacer solo si hay estados en la pila
                    if (redoStack.size > 0) {
                        SDL_Surface* nextState = popStack(&redoStack);
                        SDL_Surface* currentState = SDL_ConvertSurface(canvas, canvas->format, 0);
                        pushStack(&undoStack, currentState);
                        SDL_FreeSurface(canvas);
                        canvas = nextState;
                    }
                }
                else if (isInsideColorButton(mx, my)) {
                    showColorMenu = !showColorMenu;
                }
                else {
                    int newColor = getColorFromMenu(mx, my);
                    if (newColor != -1) currentColor = newColor;
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                isDraggingSlider = 0;
                prevDrawX = -1;
                prevDrawY = -1;
                
                // Guardar estado solo si hubo cambios
                if (event.button.button == SDL_BUTTON_LEFT && isInsideSheet(event.button.x, event.button.y)) {
                    SDL_Surface* copy = SDL_ConvertSurface(canvas, canvas->format, 0);
                    pushStack(&undoStack, copy);
                    // Limpiar rehacer al hacer nueva acción
                    while (redoStack.size > 0) {
                        SDL_FreeSurface(popStack(&redoStack));
                    }
                }
            }
            else if (event.type == SDL_MOUSEMOTION) {
                int mx = event.motion.x, my = event.motion.y;
                if (isDraggingSlider) {
                    brushSize = SLIDER_MIN + ((my - SLIDER_Y) * (SLIDER_MAX - SLIDER_MIN)) / SLIDER_HEIGHT;
                    brushSize = brushSize < SLIDER_MIN ? SLIDER_MIN : brushSize;
                    brushSize = brushSize > SLIDER_MAX ? SLIDER_MAX : brushSize;
                }
                if ((event.motion.state & SDL_BUTTON_LMASK) && isInsideSheet(mx, my)) {
                    int currentX = mx - SHEET_X;
                    int currentY = my - SHEET_Y;
                    
                    if (prevDrawX != -1 && prevDrawY != -1) {
                        drawLine(canvas, prevDrawX, prevDrawY, currentX, currentY, brushSize/2, currentColor);
                    }
                    prevDrawX = currentX;
                    prevDrawY = currentY;
                } else {
                    prevDrawX = -1;
                    prevDrawY = -1;
                }
            }
        }
        screen(surface, backImage, forwardImage, colorImage);
        SDL_UpdateWindowSurface(window);
    }

    // Liberar recursos
    while (undoStack.size > 0) SDL_FreeSurface(popStack(&undoStack));
    while (redoStack.size > 0) SDL_FreeSurface(popStack(&redoStack));
    SDL_FreeSurface(canvas);
    SDL_FreeSurface(backImage);
    SDL_FreeSurface(forwardImage);
    SDL_FreeSurface(colorImage);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}