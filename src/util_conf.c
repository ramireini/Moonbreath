#include <ctype.h>
#include <string.h>
#include <util_io.h>
#include <util_conf.h>

conf_var_t* conf_get_var(conf_t *conf, char *key)
{
  if(!conf->success)
  {
    return NULL;
  }

  for(int i = 0; i < conf->length ;i++)
  {
    if(strcmp(conf->vars[i].key, key) == 0)
    {
      return &conf->vars[i];
    }
  }

  return NULL;
}

// [checks if the given string is a number or not]
// [str] [string to check]
int is_number(char *str)
{
  // handle cases where the pointer is NULL, the character pointed to is a null-terminator
  // or the character pointed to is one of the standard white-space characters
  if(str == NULL || *str == '\0' || isspace(*str))
  {
    return 0;
  }

  char *p;
  strtod(str, &p);

  // if the value was a number, *p will equal to '\0' and the return value is 1
  // if the value was a character, *p will equal to the first letter and the return value is 0
  return *p == '\0';
}

int conf_load(conf_t *conf, char *path)
{
  conf->success = 0;

  printf("Loading config file %s\n", path);

  // read conf file
  char *buff = io_read_file(path, "r");
  if(!buff)
  {
    printf("Buff is NULL\n");
    return 0;
  }

  // copy contents
  char str[strlen(buff)];
  strcpy(str, buff);

  // get token count
  int t_count = 0;
  char *token = strtok(str, "=\n");
  while(token)
  {
    t_count++;
    token = strtok(NULL, "=\n");
  }

  // should be divisible by 2
  if(t_count % 2)
  {
    printf("Syntax error in config file %s\n", path);
    printf("Config key is missing a value\n");
    free(buff);
    return 0;
  }

  // malloc space for the key=value pairs
  conf->length = t_count / 2;
  conf->vars = malloc(sizeof(conf_var_t) * conf->length);

  int i = 0;
  int t = 0;

  // copy buff again since it got mangled by the strtok calls
  strcpy(str, buff);

  // now points to the first key
  token = strtok(str, "=\n");

  // while tokens are valid
  while(token)
  {
    if(!t)
    {
      // it's a key, store it
      strcpy(conf->vars[i].key, token);
    }
    else
    {
      // it's a value, store it
      if(is_number(token))
      {
        // store string converted into int
        conf->vars[i].conf_var_u.i = atoi(token);
        conf->vars[i].type = conf_type_int;
      }
      else
      {
        // store string
        strcpy(conf->vars[i].conf_var_u.s, token);
        conf->vars[i].type = conf_type_string;
      }

      // next array element
      i++;
    }

    // switch between a key and a value
    t = !t;

    // keep tokenizing the str
    token = strtok(NULL, "=\n");
  }

  // mark success
  conf->success = 1;

  #if DEBUG
  printf("\nConfig vars:\n\n");

  for(int i = 0 ; i < conf->length; i++)
  {
    if (i == conf->length / 2)
    {
      printf("\n");
    }

    printf("%s = ", conf->vars[i].key);

    switch (conf->vars[i].type)
    {
      case conf_type_int:
      {
        printf("%d\n", conf->vars[i].conf_var_u.i);
      } break;

      case conf_type_string:
      {
        printf("%s\n", conf->vars[i].conf_var_u.s);
      } break;
    }
  }

  printf("\n");
  #endif

  free(buff);
  buff = NULL;

  return 1;
}

void conf_free(conf_t *conf)
{
  // return if not valid
  if(!conf->success)
  {
    return;
  }

  // otherwise, free
  free(conf->vars);
  conf = NULL;
}