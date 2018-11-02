#ifndef UTIL_CONF_H
#define UTIL_CONF_H

typedef enum
{
  conf_type_int,
  conf_type_string
} conf_type_e;

typedef struct
{
  char key[52];
  conf_type_e type;
  union
  {
    int i;
    char s[256];
  } conf_var_u;
} conf_var_t;

typedef struct
{
  conf_var_t *vars;
  int length;
  int success;
} conf_t;

// [get the conf_var_t of a key]
// 
// [conf] [conf_t pointer]
// [key] [conf key value]
conf_var_t* conf_get_var(conf_t *conf, char *key);

// [load conf file into a conf_t struct]
// 
// [conf] [conf_t pointer]
// [path] [path to the file]
int conf_load(conf_t *conf, char *path);

// [free the malloc'd conf_t pointer]
// 
// [conf] [conf_t pointer]
void conf_free(conf_t *conf);

#endif // UTIL_CONF_H