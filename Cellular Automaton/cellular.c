#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool running = true;
void sigint_handler(int sig) { running = false; }

#define type bool
#define ROWS 360
#define COLS 640
#define MAX_STEPS 200
#define SCALE 2

void printGrid(type **grid, int rows, int cols) {
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++)
      printf("%s", grid[i][j] ? "[*]" : "[ ]");
  printf("\n");

  printf("\n");
}

int count_neighbors(type **grid, int rows, int cols, int i, int j) {
  int neighbors = 0;
  for (int di = -1; di <= 1; di++)
    for (int dj = -1; dj <= 1; dj++) {
      int ni = i + di;
      int nj = j + dj;
      if (ni < 0 || ni >= rows || nj < 0 || nj >= cols)
        continue;
      neighbors += grid[ni][nj];
    }
  return neighbors;
}

void updateGrid(type **grid, int rows, int cols, type **out) {
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      int neighbors = count_neighbors(grid, rows, cols, i, j);
      neighbors -= grid[i][j];
      if (grid[i][j] && (neighbors < 2 || neighbors > 3))
        out[i][j] = false;
      else if (!grid[i][j] && neighbors == 3)
        out[i][j] = true;
      else
        out[i][j] = grid[i][j];
    }
}

type **createGrid(int rows, int cols, bool random) {
  type **grid = (type **)malloc(rows * sizeof(type *));
  for (int i = 0; i < rows; i++) {
    grid[i] = (type *)malloc(cols * sizeof(type));
    for (int j = 0; j < cols; j++)
      grid[i][j] = random ? rand() % 2 : false;
  }
  return grid;
}

void freeGrid(type **grid, int rows) {
  for (int i = 0; i < rows; i++)
    free(grid[i]);
  free(grid);
}

void swap(type ***a, type ***b) {
  type **tmp = *a;
  *a = *b;
  *b = tmp;
}

void draw2file(type **grid, int step, unsigned char *data) {
  for (int i = 0; i < ROWS * SCALE; i++)
    for (int j = 0; j < COLS * SCALE; j++) {
      unsigned char color = grid[i / SCALE][j / SCALE] * 255;
      int index = (i * COLS * SCALE + j) * 3;
      data[index + 0] = color;
      data[index + 1] = color;
      data[index + 2] = color;
    }

  char filename[100];
  sprintf(filename, "out/%d.png", step);
  stbi_write_png(filename, COLS * SCALE, ROWS * SCALE, 3, data,
                 COLS * SCALE * 3);
}

// #define FFMPEG_PATH "out/ffmpeg.exe"
#ifndef FFMPEG_PATH
#define FFMPEG_PATH "ffmpeg" // if ffmpeg is in PATH
#endif

#define FPS 20
#define PATH "out/%d.png"
#define FILENAME "out/cell.mp4"
#define HWACCEL "-hwaccel cuda"
#ifndef HWACCEL
#define HWACCEL "" // NO NVIDIA GPU
#endif

void render() {
  char cmd[1000];
  sprintf(cmd,
          "%s %s -framerate %d -i %s -c:v libx264 -crf 10 -r %d -b:v 20M "
          "-pix_fmt yuv420p %s -y",
          FFMPEG_PATH, HWACCEL, FPS, PATH, FPS, FILENAME);
  printf("RENDERING:\n%s\n", cmd);
  system(cmd);
}

int main(int argc, char **argv) {
  signal(SIGINT, sigint_handler);
  type **grid = createGrid(ROWS, COLS, true);
  type **out = createGrid(ROWS, COLS, false);

  const int size = ROWS * COLS * SCALE * SCALE * 3 * sizeof(unsigned char);
  unsigned char *data = (unsigned char *)malloc(size);

  for (int i = 0; i < MAX_STEPS && running; i++) {
    updateGrid(grid, ROWS, COLS, out);
    swap(&grid, &out);
    draw2file(grid, i, data);
  }

  freeGrid(grid, ROWS);
  freeGrid(out, ROWS);
  free(data);

  render();
  return 0;
}