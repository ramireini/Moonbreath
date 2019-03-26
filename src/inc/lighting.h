#ifndef LIGHTING_H
#define LIGHTING_H

#define LIGHTING_COLOR_ON 255, 255, 255
#define LIGHTING_COLOR_OFF 55, 55, 55

enum
{
  lighting_off,
  lighting_on,
};

typedef struct
{
  bool seen;
  u8 val;
} lighting_t;

#endif // LIGHTING_H