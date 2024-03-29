#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <mpi.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

bool running = true;
void sigint_handler(int sig) { running = false; }

// #define PRINT

#define type int
#define ROWS 720
#define COLS 1280
#define CELLS ROWS *COLS
#define MAX_STEPS 200
#define SCALE 1

typedef struct {
  type *grid;
  int rows;
  int cols;
  type *out;
  int start_row;
  int end_row;
  int thread_num;
} ThreadArgs;

int count_neighbors(type *grid, int rows, int cols, int i, int j) {
  int neighbors = 0;
  for (int di = -1; di <= 1; di++)
    for (int dj = -1; dj <= 1; dj++) {
      int ni = i + di;
      int nj = j + dj;
      if (ni < 0 || ni >= rows || nj < 0 || nj >= cols)
        continue;
      neighbors += grid[ni * cols + nj];
    }
  return neighbors;
}

void updateGrid(type *grid, int rows, int cols, type *out) {
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) {
      int neighbors = count_neighbors(grid, rows, cols, i, j);
      neighbors -= grid[i * cols + j];
      if (grid[i * cols + j] && (neighbors < 2 || neighbors > 3))
        out[i * cols + j] = false;
      else if (!grid[i * cols + j] && neighbors == 3)
        out[i * cols + j] = true;
      else
        out[i * cols + j] = grid[i * cols + j];
    }
}

// Thread function
void *updateGridThread(void *arguments) {
  ThreadArgs *args = (ThreadArgs *)arguments;
  printf("I'm the Posix Thread N.%d\n", args->thread_num);

  for (int i = args->start_row; i < args->end_row; i++) {
    for (int j = 0; j < args->cols; j++) {
      int neighbors = count_neighbors(args->grid, args->rows, args->cols, i, j);
      neighbors -= args->grid[i * args->cols + j];

      if (args->grid[i * args->cols + j] && (neighbors < 2 || neighbors > 3))
        args->out[i * args->cols + j] = false;
      else if (!args->grid[i * args->cols + j] && neighbors == 3)
        args->out[i * args->cols + j] = true;
      else
        args->out[i * args->cols + j] = args->grid[i * args->cols + j];
    }
  }

  pthread_exit(NULL);
}

// Parallelized updateGrid function
void parallelUpdateGrid(type *grid, int rows, int cols, type *out, int num_threads) {
  pthread_t threads[num_threads];
  ThreadArgs threadArgs[num_threads];

  int rows_per_thread = rows / num_threads;
  int remaining_rows = rows % num_threads;
  int current_row = 0;

  for (int i = 0; i < num_threads; i++) {
    threadArgs[i].grid = grid;
    threadArgs[i].rows = rows;
    threadArgs[i].cols = cols;
    threadArgs[i].out = out;
    threadArgs[i].start_row = current_row;
    threadArgs[i].end_row = current_row + rows_per_thread + (i < remaining_rows ? 1 : 0);
    threadArgs[i].thread_num = i;

    pthread_create(&threads[i], NULL, updateGridThread, (void *)&threadArgs[i]);

    current_row = threadArgs[i].end_row;
  }

    for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
  }
}

void swap(type **a, type **b) {
  type *tmp = *a;
  *a = *b;
  *b = tmp;
}

void draw2file_linear(type *grid, int step, unsigned char *data) {
  for (int i = 0; i < ROWS * SCALE; ++i) {
    for (int j = 0; j < COLS * SCALE; ++j) {
      unsigned char value = grid[(i / SCALE) * COLS + (j / SCALE)] * 255;
      int index = (i * COLS * SCALE + j) * 3;
      data[index + 0] = value;
      data[index + 1] = value;
      data[index + 2] = value;
    }
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

#define FPS 30
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
  system(cmd);
}

int main(int argc, char **argv) {
  signal(SIGINT, sigint_handler);
  MPI_Init(&argc, &argv);
  double start = MPI_Wtime();

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  type *grid = NULL;
  type *out = NULL;
  unsigned char *data = NULL;

  if (rank == 0) {
    srand(time(NULL));
    grid = (type *)malloc(sizeof(type) * CELLS);
    out = (type *)malloc(sizeof(type) * CELLS);
    for (int i = 0; i < CELLS; i++)
      grid[i] = rand() % 2;

    const int size = ROWS * COLS * 3 * sizeof(unsigned char) * SCALE * SCALE;
    data = (unsigned char *)malloc(size);
  }

  int cols_per_proc = COLS / size;

  MPI_Datatype col;
  MPI_Datatype column;
  MPI_Type_vector(ROWS, cols_per_proc, COLS, MPI_INT, &col);
  MPI_Type_commit(&col);
  MPI_Type_create_resized(col, 0, sizeof(type), &column);
  MPI_Type_commit(&column);

  type *local_grid = (type *)malloc(sizeof(type) * ROWS * cols_per_proc);
  type *local_updated = (type *)malloc(sizeof(type) * ROWS * cols_per_proc);

  type *sendcounts = (type *)malloc(sizeof(type) * size);
  type *displs = (type *)malloc(sizeof(type) * size);

  for (int i = 0; i < size; i++) {
    sendcounts[i] = 1;
    displs[i] = i * cols_per_proc;
  }

#ifdef PRINT
  if (rank == 0)
    draw2file_linear(grid, 0, data);
#endif

  for (int i = 0; i < MAX_STEPS && running; i++) {
    printf("\n_________________ROUND %d_________________\n", i);
    MPI_Scatterv(grid, sendcounts, displs, column, local_grid,
                 ROWS * cols_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

    // updateGrid(local_grid, ROWS, cols_per_proc, local_updated);
    parallelUpdateGrid(grid, ROWS, cols_per_proc, local_updated, 30);

    MPI_Gatherv(local_updated, ROWS * cols_per_proc, MPI_INT, out, sendcounts,
                displs, column, 0, MPI_COMM_WORLD);

    if (rank == 0) {
      swap(&grid, &out);
#ifdef PRINT
      draw2file_linear(grid, i + 1, data);
#endif
    }

    MPI_Barrier(MPI_COMM_WORLD);
  }

  MPI_Type_free(&col);
  MPI_Type_free(&column);

  free(local_grid);
  free(local_updated);
  free(sendcounts);
  free(displs);

  if (rank == 0) {
    free(grid);
    free(out);
    free(data);
  }

#ifdef PRINT
  if (rank == 0)
    render();
#endif

  double end = MPI_Wtime();
  if (rank == 0)
    printf("Time: %f\n", end - start);
  MPI_Finalize();
  return 0;
}