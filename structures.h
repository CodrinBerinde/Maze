#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

typedef struct cell {
  int x, y;
  struct cell *link;
} cell;

extern cell *stack_add(cell *old_cell, int cell_x, int cell_y);
extern cell *queue_add(cell *old_cell, int cell_x, int cell_y);
extern cell *rem(cell *old_cell);
extern int is_empty(cell *top_cell);

#endif