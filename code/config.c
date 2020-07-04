typedef enum
{
    TokenType_None,
    
    TokenType_Uint,
    TokenType_Bool,
    TokenType_Char,
    TokenType_String
} TokenType;

typedef struct
{
    b32 success;
    
    union
    {
        u32 uint;
        b32 bool;
        char c;
        char string[64];
    };
} ConfigValue;

typedef struct
{
    char name[64];
    
    TokenType type;
    union
    {
        u32 uint;
        b32 bool;
        char c;
        char string[64];
    };
} Token;

typedef struct
{
    char *at;
} Tokenizer;

typedef struct
{
    u32 token_count;
    Token tokens[32];
} Config;

internal char *
read_file(char *file_path)
{
    char *result = 0;
    
    FILE *file = fopen(file_path, "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        u32 file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        result = (char *)malloc(file_size + 1);
        u32 fread_result = fread(result, file_size, 1, file);
        result[file_size] = 0;
        
        fclose(file);
    }
    
    return(result);
}

internal b32
is_end_of_file(char c)
{
    b32 result = (c == '\0');
    return(result);
}

internal b32
is_numeric(char c)
{
    b32 result = ((c >= '0') && (c <= '9'));
    return(result);
}

internal b32
is_alpha(char c)
{
    b32 result = (((c >= 'a') && (c <= 'z') ||
                   (c >= 'A') && (c <= 'Z')));
    return(result);
}  

internal b32
is_end_of_line(char c)
{
    b32 result = ((c == '\n') ||
                  (c == '\r'));
    
    return(result);
}

internal b32
is_whitespace(char c)
{
    b32 result = ((c == ' ') ||
                  (c == '\t') ||
                  is_end_of_line(c));
    
    return(result);
}

internal void
eat_all_whitespace(Tokenizer *tokenizer)
{
    for(;;)
    {
        if(is_whitespace(tokenizer->at[0]) ||
           tokenizer->at[0] == '=' ||
           tokenizer->at[0] == '"')
        {
            ++tokenizer->at;
        }
        else if(tokenizer->at[0] == '#')
        {
            ++tokenizer->at;
            while(tokenizer->at[0] && !is_end_of_line(tokenizer->at[0]))
            {
                ++tokenizer->at;
            }
        }
        else
        {
            break;
        }
    }
}

internal ConfigValue
get_config_uint(Config *config, char *token_name)
{
    ConfigValue result = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(token->type == TokenType_Uint && str_equal(token_name, token->name))
        {
            result.success = true;
            result.uint = token->uint;
            break;
        }
    }
    
    return(result);
}

internal ConfigValue
get_config_bool(Config *config, char *token_name)
{
    ConfigValue result = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(token->type == TokenType_Bool && str_equal(token_name, token->name))
        {
            result.success = true;
            result.bool = token->bool;
            break;
        }
    }
    
    return(result);
}

internal ConfigValue
get_config_char(Config *config, char *token_name)
{
    ConfigValue result = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(token->type == TokenType_Char && str_equal(token_name, token->name))
        {
            result.success = true;
            result.c = token->c;
            break;
        }
    }
    
    return(result);
}

internal ConfigValue
get_config_string(Config *config, char *token_name)
{
    ConfigValue result = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(token->type == TokenType_String && str_equal(token_name, token->name))
        {
            result.success = true;
            strcpy(result.string, token->string);
            break;
        }
    }
    
    return(result);
}

internal Token
get_token(Tokenizer *tokenizer)
{
    Token result = {0};
    u32 token_name_length = 0;
    u32 string_value_length = 0;
    
    for(;;)
    {
        if(!is_whitespace(tokenizer->at[0]))
        {
            result.name[token_name_length++] = tokenizer->at[0];
            ++tokenizer->at;
        }
        else
        {
            eat_all_whitespace(tokenizer);
            break;
        }
    }
    
    if(tokenizer->at[0] == 't' &&
       tokenizer->at[1] == 'r' &&
       tokenizer->at[2] == 'u' &&
       tokenizer->at[3] == 'e')
    {
        result.type = TokenType_Bool;
        result.bool = true;
        
        tokenizer->at += 4;
    }
    else if(tokenizer->at[0] == 'f' &&
            tokenizer->at[1] == 'a' &&
            tokenizer->at[2] == 'l' &&
            tokenizer->at[3] == 's' &&
            tokenizer->at[4] == 'e')
    {
        result.type = TokenType_Bool;
        result.bool = false;
        
        tokenizer->at += 5;
    }
    else if(is_alpha(tokenizer->at[0]))
    {
        if(tokenizer->at[1] && is_alpha(tokenizer->at[1]))
        {
            result.type = TokenType_String;
            
            while(tokenizer->at[0] && is_alpha(tokenizer->at[0]))
            {
                result.string[string_value_length++] = tokenizer->at[0];
                ++tokenizer->at;
            }
        }
        else
        {
            result.type = TokenType_Char;
            result.c = tokenizer->at[0];
            ++tokenizer->at;
        }
    }
    else if(is_numeric(tokenizer->at[0]))
    {
        result.type = TokenType_Uint;
        
        while(tokenizer->at[0] && is_numeric(tokenizer->at[0]))
        {
            result.uint = (tokenizer->at[0] - '0') + (result.uint * 10);
            ++tokenizer->at;
        }
    }
    
    return(result);
}

internal Config
parse_config_file(char *file_path)
{
    char *file_contents = read_file(file_path);
    
    Config config = {0};
    Tokenizer tokenizer = {0};
    tokenizer.at = file_contents;
    
    for(;;)
    {
        eat_all_whitespace(&tokenizer);
        if(is_end_of_file(tokenizer.at[0]))
        {
            break;
        }
        else
        {
            config.tokens[config.token_count++] = get_token(&tokenizer);
        }
    }
    
#if 0
    // Print Config Tokens
    for(u32 index = 0; index < config.token_count; ++index)
    {
        Token *token = &config.tokens[index];
        printf("\nToken Name: %s\n", token->name);
        
        if(token->type == TokenType_Uint)
        {
            printf("Token type: Uint32, Value: %u\n", token->uint);
        }
        else if(token->type == TokenType_Bool)
        {
            printf("Token type: Bool32, Value: %s\n", (token->bool == true) ? "true" : "false");
        }
        else if(token->type == TokenType_Char)
        {
            printf("Token type: Char, Value: %c\n", token->c);
        }
        else if(token->type == TokenType_String)
        {
            printf("Token type: String, Value: %s\n", token->string);
        }
    }
#endif
    
    free(file_contents);
    return(config);
}