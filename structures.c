#include <stdlib.h>
#include "structures.h"

cell *stack_add(cell *old_cell, int cell_x, int cell_y) {
  cell *new_cell;
  new_cell = (cell *)malloc(sizeof(cell));
  new_cell->x = cell_x;
  new_cell->y = cell_y;
  new_cell->link = old_cell;
  return new_cell;
}

cell *queue_add(cell *old_cell, int cell_x, int cell_y) {
  cell *new_cell;
  new_cell = (cell *)malloc(sizeof(cell));
  new_cell->x = cell_x;
  new_cell->y = cell_y;
  new_cell->link = NULL;
  if(old_cell != NULL) {
    old_cell->link = new_cell;
  }
  return new_cell;
}

cell *rem(cell *old_cell) {
  cell *new_cell;
  new_cell = old_cell->link;
  free(old_cell);
  return new_cell;
}

int is_empty(cell *top_cell) {
  return (top_cell == NULL);
}