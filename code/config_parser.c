typedef enum
{
    TokenType_None,
    
    TokenType_String,
    TokenType_Uint32,
    TokenType_Bool32
} TokenType;

typedef struct
{
    char name[128];
    
    TokenType type;
    union
    {
        char string[128];
        u32 uint32;
        b32 bool32;
    };
} Token;

typedef struct
{
    char *at;
} Tokenizer;

typedef struct
{
    u32 token_count;
    Token tokens[64];
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
        fread(result, file_size, 1, file);
        result[file_size] = 0;
        
        fclose(file);
    }
    
    return(result);
}

internal b32
is_whitespace(char c)
{
    b32 result = ((c == ' ') ||
                  (c == '\n'));
    return(result);
}

internal b32
is_equals_sign(char c)
{
    b32 result = (c == '=');
    return(result);
}

internal b32
is_end_of_file(char c)
{
    b32 result = (c == '\0');
    return(result);
}

internal b32
is_number(char c)
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

internal void
eat_redundant_characters(Tokenizer *tokenizer)
{
    while(is_whitespace(tokenizer->at[0]) ||
          is_equals_sign(tokenizer->at[0]))
    {
        ++tokenizer->at;
    }
}

internal Token
get_token(Tokenizer *tokenizer)
{
    Token result = {0};
    u32 name_length = 0;
    u32 string_length = 0;
    
    while(!is_whitespace(tokenizer->at[0]))
    {
        result.name[name_length++] = tokenizer->at[0];
        ++tokenizer->at;
    }
    
    eat_redundant_characters(tokenizer);
    
    if(tokenizer->at[0] == 't' &&
       tokenizer->at[1] == 'r' &&
       tokenizer->at[2] == 'u' &&
       tokenizer->at[3] == 'e')
    {
        result.type = TokenType_Bool32;
        result.bool32 = true;
        
        tokenizer->at += 4;
    }
    else if(tokenizer->at[0] == 'f' &&
            tokenizer->at[1] == 'a' &&
            tokenizer->at[2] == 'l' &&
            tokenizer->at[3] == 's' &&
            tokenizer->at[4] == 'e')
    {
        result.type = TokenType_Bool32;
        result.bool32 = false;
        
        tokenizer->at += 5;
    }
    else if(is_alpha(tokenizer->at[0]))
    {
        result.type = TokenType_String;
        
        while(!is_whitespace(tokenizer->at[0]))
        {
            result.string[string_length++] = tokenizer->at[0];
            ++tokenizer->at;
        }
    }
    else if(is_number(tokenizer->at[0]))
    {
        result.type = TokenType_Uint32;
        
        while(!is_whitespace(tokenizer->at[0]))
        {
            for(tokenizer->at[0]; is_number(tokenizer->at[0]); ++tokenizer->at)
            {
                result.uint32 = (tokenizer->at[0] - '0') + (result.uint32 * 10);
            }
        }
    }
    
    return(result);
}

internal char *
get_config_value_string(Config *config, char *token_name)
{
    char *result = false;
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(str_equal(token_name, token->name) &&
           token->type == TokenType_String)
        {
            result = token->string;
            break;
        }
    }
    
    return(result);
}

internal u32
get_config_value_uint32(Config *config, char *token_name)
{
    u32 result = false;
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(str_equal(token_name, token->name) &&
           token->type == TokenType_Uint32)
        {
            result = token->uint32;
            break;
        }
    }
    
    return(result);
}

internal b32
get_config_value_bool32(Config *config, char *token_name)
{
    b32 result = false;
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(str_equal(token_name, token->name) &&
           token->type == TokenType_Bool32)
        {
            result = token->bool32;
            break;
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
        eat_redundant_characters(&tokenizer);
        
        if(is_end_of_file(tokenizer.at[0]))
        {
            break;
        }
        else
        {
            config.tokens[config.token_count++] = get_token(&tokenizer);
        }
    }
    
#if 1
    // Print Config Tokens
    for(u32 index = 0; index < config.token_count; ++index)
    {
        Token *token = &config.tokens[index];
        printf("\nToken Name: %s\n", token->name);
        
        if(token->type == TokenType_String)
        {
            printf("Token type: Value (String)\n");
            printf("Token value: %s\n", token->string);
        }
        else if(token->type == TokenType_Uint32)
        {
            printf("Token type: Value (Uint32)\n");
            printf("Token value: %u\n", token->uint32);
        }
        else if(token->type == TokenType_Bool32)
        {
            printf("Token type: Value (Bool32)\n");
            printf("Token value: %s\n", (token->bool32 == true) ? "true" : "false");
        }
    }
#endif
    
    free(file_contents);
    return(config);
}