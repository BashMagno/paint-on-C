# paint-on-C
This is a project that I tried to do in less than an hour.
It´s about coding a simple paint program in C.
Functionalities: drawing, undo and redo, 6 different colors and a slider to change the pencil´s width.

**This is not a final version, shortly i´ll try to continue but this time in less than 10 hours. This project is inspired by those YouTube videos that are like: "1 minute, 10 minuets, 1 hours, 10 hours to code bla bla bla...."**


Some cool Software Engineer stuff here :)
**Algorithmic Complexity, Time, and Space of the Code:**

### **Time Complexity:**
1. **Per Frame:**
   - **Best Case (No Drawing):** 
     - `O(SHEET_WIDTH * SHEET_HEIGHT)` due to the `screen()` function, which redraws the entire canvas surface every frame.
   - **Worst Case (Drawing):**
     - `O(SHEET_WIDTH * SHEET_HEIGHT + L)`, where `L` is the length in pixels of the line being drawn in that frame.
     - **`drawLine()` + `drawCircle()`:** 
       - `O(L * r²)`, but since `r` (brush radius) is constant (max 16), it simplifies to `O(L)`.
     - **Rendering (`screen()`):** 
       - `O(SHEET_WIDTH * SHEET_HEIGHT)` due to the canvas blit.

2. **Stack Operations (Undo/Redo):**
   - **`pushStack()`:** 
     - Amortized `O(1)` (realloc grows exponentially).
   - **`popStack()`:** 
     - `O(1)`.

### **Space Complexity:**
1. **Graphic Surfaces:**
   - **Canvas:** 
     - `O(SHEET_WIDTH * SHEET_HEIGHT * 4 bytes)` (32 bits per pixel).
   - **Undo/Redo Stacks:** 
     - `O(k * SHEET_WIDTH * SHEET_HEIGHT)`, where `k` is the number of saved states.

2. **Dynamic Memory (Stacks):**
   - **Surface Storage:** 
     - Each state in undo/redo is a copy of the canvas, occupying the same space as the canvas.

### **Summary:**
- **Total Time per Frame:** 
  - Dominated by canvas rendering (`O(SHEET_WIDTH * SHEET_HEIGHT)`) and drawing operations (`O(L)`).
- **Total Space:** 
  - Primarily determined by the canvas and the copies in undo/redo (`O(k * SHEET_AREA)`).

### **Concrete Values (Based on #define):**
- **SHEET_WIDTH = 1800 - 100 = 1700**
- **SHEET_HEIGHT = 980 - 250 = 730**
- **Space per Canvas:** 
  - `1700 * 730 * 4 ≈ 4.96 MB`.
- **Undo/Redo with 10 States:** 
  - `10 * 4.96 MB ≈ 49.6 MB`.
