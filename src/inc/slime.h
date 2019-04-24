#ifndef SLIME_H
#define SLIME_H

#define SLIME_COUNT 10

typedef struct
{
  bool32 active;
  bool32 in_combat;
  entity_t entity;
} slime_t;

#endif // SLIME_H