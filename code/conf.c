// NOTE(rami): The function interfaces need to be redone

#ifndef UTIL_CONF_H
#define UTIL_CONF_H

#define ITEM_HEADER "[ITEM]"
#define KEY_VALUE_PAIRS_PER_ITEM 9

internal char *
type_lookup_table[] =
{
  "TYPE_CONSUME",
  "TYPE_EQUIP"
};

enum
{
  token_type_key,
  token_type_value,
} token_type;

typedef enum
{
  data_type_int,
  data_type_string
} data_type;

typedef struct
{
  char key[32];
  data_type type;
  union
  {
    i32 i;
    char str[256];
  };
} var_t;

typedef struct
{
  var_t *vars;
  i32 length;
  b32 success;
} conf_t;

// NOTE(rami): Do we need the below 3 functions?
// internal var_t *
// get_conf_var(conf_t *conf, char *key)
// {
<<<<<<< HEAD
//   var_t *result = 0;
=======
//   var_t *result = NULL;
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19

//   if(conf->success)
//   {
//     for(i32 i = 0; i < conf->length; ++i)
//     {
//       if(str_cmp(key, conf->vars[i].key))
//       {
//         result = &conf->vars[i];
//       }
//     }
//   }

//   return(result);
// }

// internal i32
// get_conf_int(conf_t *conf, char *key)
// {
//   i32 result = 0;

//   if(conf->success)
//   {
//     var_t *var = get_conf_var(conf, key);
//     if(var && var->type == data_type_int)
//     {
//       result = var->i;
//     }
//   }

//   return(result);
// }

// internal char *
// get_conf_string(conf_t *conf, char *key)
// {
//   char *result = "asd";

//   if(conf->success)
//   {
//     var_t *var = get_conf_var(conf, key);
//     if(var && var->type == data_type_string)
//     {
//       result = var->str;
//     }
//   }

//   return(result);
// }

// [checks if token can be found from the lookup table]
// 
// [token] [string token to check]
// 
// [returns the number of the token if found]
// [returns -1 if token was not found]
internal i32
get_type(char *token)
{
  i32 result = 0;

  for(i32 i = 0; i < type_count; ++i)
  {
    if(str_cmp(token, type_lookup_table[i]))
    {
      result = i + 1;
      break;
    }
  }

  return(result);
}

internal i32
is_number(char *str)
{
  i32 result = 1;

  while(*str)
  {
    if(*str < 48 || *str > 57)
    {
      result = 0;
      break;
    }

    ++str;
  }

  return(result);
}

// [load conf file into a conf_t struct]
// 
// [path] [path to the file]
internal conf_t*
load_conf(char *path)
{
  printf("Loading config file %s\n", path);

  conf_t *conf = malloc(sizeof(conf_t));
  conf->success = 0;

  // read config file
  char *buff = read_file(path, "r");
  if(!buff)
  {
    printf("Could not load config\n");
<<<<<<< HEAD
    return(0);
=======
    return(NULL);
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19
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
    // if a token is an item header then
    // tokenize again and go to next loop
    if(str_cmp(token, ITEM_HEADER))
    {
<<<<<<< HEAD
      token = strtok(0, "=\n");
=======
      token = strtok(NULL, "=\n");
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19
      continue;
    }

    ++t_count;
<<<<<<< HEAD
    token = strtok(0, "=\n");
=======
    token = strtok(NULL, "=\n");
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19
  }

  printf("token count: %d\n\n", t_count);

  // not equal amount of keys and variables
  if(t_count % 2)
  {
    printf("Syntax error in config file %s\n\n", path);
    printf("Config is missing a key or value\n\n");

    free(buff);
<<<<<<< HEAD
    return(0);
=======
    return(NULL);
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19
  }
  // not enough key value pairs per item
  else if(t_count % KEY_VALUE_PAIRS_PER_ITEM)
  {
    printf("Syntax error in config file %s\n\n", path);
    printf("One or more items have missing or excess information\n\n");

    free(buff);
<<<<<<< HEAD
    return(0);
=======
    return(NULL);
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19
  }

  // malloc space for key=value pairs
  conf->length = t_count / 2;
  conf->vars = malloc(sizeof(var_t) * conf->length);

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
<<<<<<< HEAD
      token = strtok(0, "=\n");
=======
      token = strtok(NULL, "=\n");
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19
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
        conf->vars[i].i = atoi(token);
        conf->vars[i].type = data_type_int;
      }
      // it's a specific string
      else if(token[0] == 'I' && token[1] == 'D')
      {
        conf->vars[i].i = atoi(token);
        conf->vars[i].type = data_type_int;
      }
      else if(token[0] == 'T' && token[1] == 'Y' && token[2] == 'P' && token[3] == 'E')
      {
        conf->vars[i].i = get_type(token);
        conf->vars[i].type = data_type_int;
      }
      // it's a general string
      else
      {
        strcpy(conf->vars[i].str, token);
        conf->vars[i].type = data_type_string;
      }

      ++i;
    }

    t = !t;
<<<<<<< HEAD
    token = strtok(0, "=\n");
=======
    token = strtok(NULL, "=\n");
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19
  }

  conf->success = 1;

  printf("length: %d\n", conf->length);

  for(i32 i = 0 ; i < conf->length; ++i)
  {
    printf("%s = ", conf->vars[i].key);

    if(conf->vars[i].type == data_type_int)
    {
        printf("%d\n", conf->vars[i].i);
    }
    else if(conf->vars[i].type == data_type_string)
    {
        printf("%s\n", conf->vars[i].str);
    }
  }

  free(buff);
<<<<<<< HEAD
  buff = 0;
=======
  buff = NULL;
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19

  printf("\nConfig %s loaded\n\n", path);
  return(conf);
}

// [free the malloc'd conf_t pointer]
// 
// [conf] [conf_t pointer]
//
// [returns 1 for success]
// [returns 0 for failure]
internal i32
free_conf(conf_t *conf)
{
  i32 result = 0;

  if(conf)
  {
    if(conf->vars)
    {
      free(conf->vars);
<<<<<<< HEAD
      conf->vars = 0;
    }

    free(conf);
    conf = 0;
=======
      conf->vars = NULL;
    }

    free(conf);
    conf = NULL;
>>>>>>> fa69d4b6a42a23e98d2b272438fe1cd48685db19

    result = 1;
  }

  return(result);
}

#endif // UTIL_CONF_H