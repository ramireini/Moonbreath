#ifndef UTIL_CONF_H
#define UTIL_CONF_H

#include <util_io.h>

#define ITEM_HEADER "[ITEM]"
#define KEY_VALUE_PAIRS_PER_ITEM 9

/* -- LOOKUP TABLES -- */

static char *id_lookup_table[] = {
  "ID_NONE",
  "ID_LESSER_HEALTH_POTION",
  "ID_IRON_SWORD"
};

static char *type_lookup_table[] = {
  "TYPE_NONE",
  "TYPE_CONSUME",
  "TYPE_EQUIP"
};

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
    int32 i;
    char s[256];
  } conf_var_u;
} conf_var_t;

typedef struct
{
  conf_var_t *vars;
  int32 success;
  int32 key_value_pair_count;
} conf_t;

// [checks if token can be found from the lookup table]
// 
// [token] [string token to check]
// 
// [returns the number of the token if found]
// [returns -1 if token was not found]
static int32 id_lookup(char *token)
{
  for(int32 i = 0; i < ID_LAST; i++)
  {
    if(!strcmp(token, id_lookup_table[i]))
    {
      return i;
    }
  }

  return 0;
}

// [checks if token can be found from the lookup table]
// 
// [token] [string token to check]
// 
// [returns the number of the token if found]
// [returns -1 if token was not found]
static int32 type_lookup(char *token)
{
  for(int32 i = 0; i < TYPE_LAST; i++)
  {
    if(!strcmp(token, type_lookup_table[i]))
    {
      return i;
    }
  }

  return 0;
}

// [checks if the given character is a standard white-space characters]
// 
// [ch] [character to check]
// 
// [returns 1 for true]
// [returns 0 for false]
static int32 is_space(int32 ch)
{
  if(ch == ' ' ||
     ch == '\t' ||
     ch == '\n' ||
     ch == '\v' ||
     ch == '\f' ||
     ch == '\r')
  {
    return 1;
  }

  return 0;
}

// [checks if the given string is a number or not]
// 
// [str] [string to check]
// 
// [returns 1 for true]
// [returns 0 for false]
static int32 is_number(char *str)
{
  // handle cases where the pointer is NULL,
  // the character pointed to is a null-terminator
  // or one of the standard white-space characters
  if(str == NULL || *str == '\0' || is_space(*str))
  {
    return 0;
  }

  char *p;
  strtod(str, &p);

  // if the value was a number, *p will equal to '\0' and the return value is 1
  // if the value was a character, *p will equal to the first letter and the return value is 0
  return *p == '\0';
}

// [load conf file into a conf_t struct]
// 
// [conf] [conf_t pointer]
// [path] [path to the file]
// 
// [returns 0 for success]
// [returns 1 for failure]
static int32 conf_load(conf_t *conf, char *path)
{
  printf("Loading config file %s\n", path);

  // set initial state
  conf->success = 0;

  // read config file
  char *buff = io_read_file(path, "r");
  if(!buff)
  {
    printf("Could not load config\n");
    return 0;
  }

  // copy contents
  char str[strlen(buff)];
  strcpy(str, buff);

  // count tokens
  int32 t_count = 0;
  char *token = strtok(str, "=\n");

  // keep tokenizing
  while(token)
  {
    // if a token is an item header then
    // tokenize again and go to next loop
    if(!strcmp(token, ITEM_HEADER))
    {
      token = strtok(NULL, "=\n");
      continue;
    }

    t_count++;
    token = strtok(NULL, "=\n");
  }

  // not equal amount of keys and variables
  if(t_count % 2)
  {
    printf("Syntax error in config file %s\n", path);
    printf("Config is missing a key or value\n");

    free(buff);
    return 0;
  }
  // not enough key value pairs per item
  else if(t_count % KEY_VALUE_PAIRS_PER_ITEM)
  {
    printf("Syntax error in config file %s\n", path);
    printf("One or more items have missing or excess information\n");

    free(buff);
    return 0;
  }

  // malloc space for key=value pairs
  conf->key_value_pair_count = t_count / 2;
  conf->vars = malloc(sizeof(conf_var_t) * conf->key_value_pair_count);

  // i = index
  // t = 0, key
  // t = 1, value
  int32 i = 0;
  int32 t = 0;

  // copy buff again since it got mangled by the strtok calls
  strcpy(str, buff);

  // now points to the first key
  token = strtok(str, "=\n");

  // keep tokenizing if not NULL
  while(token)
  {
    // if a token is an item header then
    // tokenize again and go to next loop
    if(!strcmp(token, ITEM_HEADER))
    {
      token = strtok(NULL, "=\n");
      continue;
    }

    // it's a key
    if(!t)
    {
      strcpy(conf->vars[i].key, token);
    }
    // it's a value
    else
    {
      // it's a number value
      if(is_number(token))
      {
        // store str converted into an int
        conf->vars[i].conf_var_u.i = atoi(token);
        conf->vars[i].type = conf_type_int;
      }
      // it's a specific string
      else if(token[0] == 'I' && token[1] == 'D')
      {
        conf->vars[i].conf_var_u.i = id_lookup(token);
        conf->vars[i].type = conf_type_int;
      }
      else if(token[0] == 'T' && token[1] == 'Y' && token[2] == 'P' && token[3] == 'E')
      {
        conf->vars[i].conf_var_u.i = type_lookup(token);
        conf->vars[i].type = conf_type_int;
      }
      // it's a general string
      else
      {
        strcpy(conf->vars[i].conf_var_u.s, token);
        conf->vars[i].type = conf_type_string;
      }

      // next index
      i++;
    }

    // switch between key and value
    t = !t;

    // keep tokenizing str
    token = strtok(NULL, "=\n");
  }

  // set success
  conf->success = 1;

  printf("Config file %s successfully loaded\n", path);

  #if DEBUG
  printf("\nConfig vars:\nkey_value_pair_count: %d\n\n", conf->key_value_pair_count);

  for(int32 i = 0 ; i < conf->key_value_pair_count; i++)
  {
    if (i == conf->key_value_pair_count / 2)
    {
      printf("\n");
    }

    printf("%s = ", conf->vars[i].key);

    if(conf->vars[i].type == conf_type_int)
    {
        printf("%d\n", conf->vars[i].conf_var_u.i);
    }
    else if(conf->vars[i].type == conf_type_string)
    {
        printf("%s\n", conf->vars[i].conf_var_u.s);
    }
  }

  printf("\n");
  #endif

  free(buff);
  buff = NULL;

  return 1;
}

// [free the malloc'd conf_t pointer]
// 
// [conf] [conf_t pointer]
static void conf_free(conf_t *conf)
{
  if(conf->vars)
  {
    free(conf->vars);
    conf = NULL;
  }
}

#endif // UTIL_CONF_H