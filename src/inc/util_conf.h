#ifndef UTIL_CONF_H
#define UTIL_CONF_H

#define ITEM_HEADER "[ITEM]"

typedef enum
{
  conf_type_int,
  conf_type_string
} conf_type_e;

typedef struct
{
  char key[32];
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
  int key_value_pair_count;
  int success;
} conf_t;

// [checks if token can be found from the lookup table]
// 
// [token] [string token to check]
// 
// [returns the number of the token if found]
// [returns -1 if token was not found]
int id_lookup(char *token);

// [checks if token can be found from the lookup table]
// 
// [token] [string token to check]
// 
// [returns the number of the token if found]
// [returns -1 if token was not found]
int type_lookup(char *token);

// [checks if the given character is a standard white-space characters]
// 
// [ch] [character to check]
// 
// [returns 1 for true]
// [returns 0 for false]
int is_space(int ch);

// [checks if the given string is a number or not]
// 
// [str] [string to check]
// 
// [returns 1 for true]
// [returns 0 for false]
int is_number(char *str);

// [get the conf_var_t of a key]
// 
// [conf] [conf_t pointer]
// [key] [conf key value]
// 
// [returns a pointer to a conf_var_t structure]
conf_var_t* conf_get_var(conf_t *conf, char *key);

// [load conf file into a conf_t struct]
// 
// [conf] [conf_t pointer]
// [path] [path to the file]
// 
// [returns 1 for success]
// [returns 0 for failure]
int conf_load(conf_t *conf, char *path);

// [free the malloc'd conf_t pointer]
// 
// [conf] [conf_t pointer]
void conf_free(conf_t *conf);

#endif // UTIL_CONF_H