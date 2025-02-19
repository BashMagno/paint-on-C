# paint-on-C
This is a project i tried to do in less than an hour. Its coding a simple paint program in C, adding, drawing, undo and redo, 6 different colors and a slider to change the pencil`s width.
Complejidad Algorítmica, Tiempo y Espacio del Código:

Complejidad Temporal:
Por Fotograma (Frame):

Mejor Caso (Sin Dibujar):

O(SHEET_WIDTH * SHEET_HEIGHT) debido a la función screen(), que redibuja toda la superficie del folio (canvas) en cada frame.

Peor Caso (Dibujando):

O(SHEET_WIDTH * SHEET_HEIGHT + L), donde L es la longitud en píxeles de la línea dibujada en ese frame.

drawLine() + drawCircle():

O(L * r²), pero como r (radio del pincel) es constante (máx. 16), se simplifica a O(L).

Rendering (screen()):

O(SHEET_WIDTH * SHEET_HEIGHT) por el blit del canvas.

Operaciones de Pila (Undo/Redo):

pushStack():

Amortizado O(1) (realloc crece exponencialmente).

popStack():

O(1).

Complejidad Espacial:
Superficies Gráficas:

Canvas:

O(SHEET_WIDTH * SHEET_HEIGHT * 4 bytes) (32 bits por píxel).

Pilas Undo/Redo:

O(k * SHEET_WIDTH * SHEET_HEIGHT), donde k es el número de estados guardados.

Memoria Dinámica (Stacks):

Almacenamiento de superficies:

Cada estado en undo/redo es una copia del canvas, ocupando el mismo espacio que este.

Resumen:
Tiempo Total por Frame:

Dominado por el rendering del canvas (O(SHEET_WIDTH * SHEET_HEIGHT)) y las operaciones de dibujo (O(L)).

Espacio Total:

Principalmente por el canvas y las copias en undo/redo (O(k * SHEET_AREA)).

Valores Concretos (Basados en #define):
SHEET_WIDTH = 1800 - 100 = 1700

SHEET_HEIGHT = 980 - 250 = 730

Espacio por Canvas:

1700 * 730 * 4 ≈ 4.96 MB.

Undo/Redo con 10 estados:

10 * 4.96 MB ≈ 49.6 MB.
