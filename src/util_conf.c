#ifndef UTIL_CONF_H
#define UTIL_CONF_H

#define ITEM_HEADER "[ITEM]"
#define KEY_VALUE_PAIRS_PER_ITEM 9

/* -- LOOKUP TABLES -- */

char *id_lookup_table[] =
{
  "ID_LESSER_HEALTH_POTION",
  "ID_IRON_SWORD"
};

char *type_lookup_table[] =
{
  "TYPE_CONSUME",
  "TYPE_EQUIP"
};

typedef enum
{
  token_type_key = 0,
  token_type_value,
} token_type;

typedef enum
{
  data_type_int = 0,
  data_type_string
} data_type_e;

typedef struct
{
  char key[32];
  data_type_e type;

  union
  {
    i32 i;
    char str[256];
  } conf_var_u;
} var_t;

typedef struct
{
  var_t *vars;
  i32 key_value_pair_count;
  b32 result;
} conf_t;

// [checks if token can be found from the lookup table]
// 
// [token] [string token to check]
// 
// [returns the number of the token if found]
// [returns -1 if token was not found]
i32 id_lookup(char *token)
{
  for(i32 i = 0; i < id_total; i++)
  {
    if(str_cmp(token, id_lookup_table[i]))
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
i32 type_lookup(char *token)
{
  for(i32 i = 0; i < type_total; i++)
  {
    if(str_cmp(token, type_lookup_table[i]))
    {
      return i;
    }
  }

  return 0;
}

// [checks if character is one of the standard white-space characters]
// 
// [ch] [character to check]
// 
// [returns 1 for true]
// [returns 0 for false]
b32 is_space(i32 ch)
{
  if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\f' || ch == '\r')
  {
    return true;
  }

  return false;
}

// [checks if the given string is a number or not]
// 
// [str] [string to check]
// 
// [returns 1 for true]
// [returns 0 for false]
b32 is_number(char *str)
{
  // handle cases where the pointer is NULL,
  // the character pointed to is a null-terminator
  // or one of the standard white-space characters
  if(str == NULL || *str == '\0' || is_space(*str))
  {
    return false;
  }

  char *p;
  strtod(str, &p);

  // if the value was a number, *p will equal to '\0' and the return value is 1
  // if the value was a character, *p will equal to the first letter and the return value is 0
  return *p == '\0';
}

// [load conf file into a conf_t struct]
// 
// [path] [path to the file]
// 
// [returns 0 for success]
// [returns 1 for failure]
conf_t* conf_load(char *path)
{
  debug("Loading config file %s", path);

  conf_t *conf = malloc(sizeof(conf_t));

  // set initial state
  conf->result = false;

  // read config file
  char *buff = io_read_file(path, "r");
  if(!buff)
  {
    debug("Could not load config");
    return NULL;
  }

  // copy contents
  char str[strlen(buff)];
  strcpy(str, buff);

  // count tokens
  i32 t_count = 0;
  char *token = strtok(str, "=\n");

  // keep tokenizing
  while(token)
  {
    printf("%s\n", token);

    // if a token is an item header then
    // tokenize again and go to next loop
    if(str_cmp(token, ITEM_HEADER))
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
    debug("Syntax error in config file %s\n", path);
    debug("Config is missing a key or value\n");

    free(buff);
    return NULL;
  }
  // not enough key value pairs per item
  else if(t_count % KEY_VALUE_PAIRS_PER_ITEM)
  {
    debug("Syntax error in config file %s\n", path);
    debug("One or more items have missing or excess information\n");

    free(buff);
    return NULL;
  }

  // malloc space for key=value pairs
  conf->key_value_pair_count = t_count / 2;
  conf->vars = malloc(sizeof(var_t) * conf->key_value_pair_count);

  i32 i = 0;
  i32 t = token_type_key;

  // copy buff again since it got mangled by strtok
  strcpy(str, buff);

  // points to the first key
  token = strtok(str, "=\n");

  while(token)
  {
    if(str_cmp(token, ITEM_HEADER))
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
        conf->vars[i].type = data_type_int;
      }
      // it's a specific string
      else if(token[0] == 'I' && token[1] == 'D')
      {
        conf->vars[i].conf_var_u.i = id_lookup(token);
        conf->vars[i].type = data_type_int;
      }
      else if(token[0] == 'T' && token[1] == 'Y' && token[2] == 'P' && token[3] == 'E')
      {
        conf->vars[i].conf_var_u.i = type_lookup(token);
        conf->vars[i].type = data_type_int;
      }
      // it's a general string
      else
      {
        strcpy(conf->vars[i].conf_var_u.str, token);
        conf->vars[i].type = data_type_string;
      }

      i++;
    }

    t = !t;
    token = strtok(NULL, "=\n");
  }

  conf->result = true;

  #if DEBUG
  debug("\nConfig vars:\nkey_value_pair_count: %d\n\n", conf->key_value_pair_count);

  for(i32 i = 0 ; i < conf->key_value_pair_count; i++)
  {
    if (i == conf->key_value_pair_count / 2)
    {
      debug("\n");
    }

    debug("%s = ", conf->vars[i].key);

    if(conf->vars[i].type == conf_type_int)
    {
        debug("%d\n", conf->vars[i].conf_var_u.i);
    }
    else if(conf->vars[i].type == conf_type_string)
    {
        debug("%s\n", conf->vars[i].conf_var_u.s);
    }
  }

  #endif

  free(buff);
  buff = NULL;

  debug("Config %s loaded", path);
  return conf;
}

// [free the malloc'd conf_t pointer]
// 
// [conf] [conf_t pointer]
b32 conf_free(conf_t *conf)
{
  if(!conf)
  {
    return false;
  }

  if(conf->vars)
  {
    free(conf->vars);
  }

  free(conf);
  conf = NULL;

  return true;
}

#endif // UTIL_CONF_H