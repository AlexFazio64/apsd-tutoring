#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

const int ROWS = 6;
const int COLS = 12;
const int cells = ROWS * COLS;

void print_grid(int *grid, int rows, int cols, int rank) {
  for (int i = 0; i < rows; i++) {
    printf("Rank %d: %d -> [ ", rank, i);
    for (int j = 0; j < cols; j++)
      printf("%d ", grid[i * cols + j]);
    printf("]\n");
  }
  printf("\n");
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int *grid = NULL;

  if (rank == 0) {
    grid = (int *)malloc(sizeof(int) * cells);
    for (int i = 0; i < cells; i++)
      grid[i] = i;
    print_grid(grid, ROWS, COLS, rank);
  }

  int cols_per_proc = COLS / size;

  MPI_Datatype col;
  MPI_Datatype column;
  MPI_Type_vector(ROWS, cols_per_proc, COLS, MPI_INT, &col);
  MPI_Type_commit(&col);
  MPI_Type_create_resized(col, 0, sizeof(int), &column);
  MPI_Type_commit(&column);

  int *local_grid = (int *)malloc(sizeof(int) * ROWS * cols_per_proc);

  int *sendcounts = (int *)malloc(sizeof(int) * size);
  int *displs = (int *)malloc(sizeof(int) * size);

  for (int i = 0; i < size; i++) {
    sendcounts[i] = 1;
    displs[i] = i * cols_per_proc;
  }

  MPI_Scatterv(grid, sendcounts, displs, column, local_grid,
               ROWS * cols_per_proc, MPI_INT, 0, MPI_COMM_WORLD);

  // update local_grid
  for (int i = 0; i < ROWS * cols_per_proc; i++)
    local_grid[i] += rank;

  MPI_Gatherv(local_grid, ROWS * cols_per_proc, MPI_INT, grid, sendcounts,
              displs, column, 0, MPI_COMM_WORLD);

  print_grid(local_grid, ROWS, cols_per_proc, rank);

  if (rank == 0) {
    printf("Reconstructed grid:\n");
    print_grid(grid, ROWS, COLS, rank);
  }

  MPI_Type_free(&col);
  MPI_Type_free(&column);
  if (rank == 0)
    free(grid);
  free(local_grid);

  MPI_Finalize();
  return 0;
}
