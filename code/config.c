#define MAX_TOKEN_COUNT 32

typedef enum
{
    TokenError_None,
    
    TokenError_EndOfFile,
    TokenError_InvalidName,
    TokenError_InvalidSeparator,
    TokenError_InvalidValue
} TokenError;

typedef enum
{
    TokenValue_None,
    
    TokenValue_Uint,
    TokenValue_Bool,
    TokenValue_String
} TokenValue;

typedef struct
{
    b32 is_valid;
    
    union
    {
        u32 uint;
        b32 boolean;
        String64 string;
    };
} ConfigValue;

typedef struct
{
    String64 name;
    
    TokenError error;
    TokenValue value;
    union
    {
        u32 uint;
        b32 boolean;
        String64 string;
    };
} Token;

typedef struct
{
    u32 token_count;
    Token tokens[MAX_TOKEN_COUNT];
} Config;

typedef struct
{
    char *at;
} Tokenizer;

internal ConfigValue
get_config_uint(Config *config, char *token_name)
{
    ConfigValue value = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(token->value == TokenValue_Uint && strings_match(token_name, token->name.s))
        {
            value.is_valid = true;
            value.uint = token->uint;
            break;
        }
    }
    
    return(value);
}

internal ConfigValue
get_config_bool(Config *config, char *token_name)
{
    ConfigValue value = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(token->value == TokenValue_Bool && strings_match(token_name, token->name.s))
        {
            value.is_valid = true;
            value.boolean = token->boolean;
            break;
        }
    }
    
    return(value);
}

internal ConfigValue
get_config_string(Config *config, char *token_name)
{
    ConfigValue result = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        Token *token = &config->tokens[index];
        if(token->value == TokenValue_String && strings_match(token_name, token->name.s))
        {
            result.is_valid = true;
            strcpy(result.string.s, token->string.s);
            break;
        }
    }
    
    return(result);
}

internal char *
read_file(MemoryArena *memory_arena, char *file_path)
{
    char *file_contents = 0;
    
    FILE *file = fopen(file_path, "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
         memory_size file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        file_contents = push_memory(memory_arena, file_size + 1);
        u32 ignored = fread(file_contents, file_size, 1, file);
        file_contents[file_size] = 0;
        
        fclose(file);
    }
    
    return(file_contents);
}

internal b32
is_end_of_file(char c)
{
    b32 result = (c == '\0');
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
                  (c == '\v') ||
                  (c == '\f') ||
                  is_end_of_line(c));
    
    return(result);
}

internal b32
is_special(char c)
{
    b32 result = ((c >= '!') && (c <= '/') ||
                  (c >= ':') && (c <= '@') ||
                  (c >= '[') && (c <= '_') ||
                  (c >= '{') && (c <= '~'));
    
    return(result);
}

internal void
eat_all_whitespace(Tokenizer *tokenizer)
{
    for(;;)
    {
        assert_loop_count();
        
        if(is_whitespace(tokenizer->at[0]))
        {
            ++tokenizer->at;
        }
        else if(tokenizer->at[0] == '/' &&
                tokenizer->at[1] == '/')
        {
            tokenizer->at += 2;
            
            while(tokenizer->at[0] && !is_end_of_line(tokenizer->at[0]))
            {
                ++tokenizer->at;
            }
        }
        else if(tokenizer->at[0] == '/' &&
                tokenizer->at[1] == '*')
        {
            tokenizer->at += 2;
            
            while(tokenizer->at[0] && !(tokenizer->at[0] == '*' &&
                                        tokenizer->at[1] == '/'))
            {
                ++tokenizer->at;
            }
            
            if(tokenizer->at[0] == '*')
            {
                tokenizer->at += 2;
            }
        }
        else
        {
            break;
        }
    }
}

internal Token
get_token(Tokenizer *tokenizer)
{
    Token token = {0};
    u32 token_name_length = 0;
    u32 string_value_length = 0;
    
    eat_all_whitespace(tokenizer);
    
    if(is_end_of_file(tokenizer->at[0]))
    {
        token.error = TokenError_EndOfFile;
    }
    else
    {
        // Token name
        if(is_alpha(tokenizer->at[0]))
        {
            while(tokenizer->at[0] && !is_whitespace(tokenizer->at[0]))
            {
                token.name.s[token_name_length++] = tokenizer->at[0];
                ++tokenizer->at;
            }
        }
        else
        {
            token.error = TokenError_InvalidName;
        }
        
        // Token separator
        if(tokenizer->at[0] && tokenizer->at[0] == ' ' &&
           tokenizer->at[1] && tokenizer->at[1] == '=' &&
           tokenizer->at[2] && tokenizer->at[2] == ' ')
        {
            tokenizer->at += 3;
        }
        else
        {
            token.error = TokenError_InvalidSeparator;
        }
        
        // Token value
        if(tokenizer->at[0] && is_special(tokenizer->at[0]))
        {
            token.value = TokenValue_String;
            
            token.string.s[0] = tokenizer->at[0];
            ++tokenizer->at;
        }
        else if(tokenizer->at[0] && is_alpha(tokenizer->at[0]))
        {
            if(tokenizer->at[0] && tokenizer->at[0] == 't' &&
               tokenizer->at[1] && tokenizer->at[1] == 'r' &&
               tokenizer->at[2] && tokenizer->at[2] == 'u' &&
               tokenizer->at[3] && tokenizer->at[3] == 'e')
            {
                token.value = TokenValue_Bool;
                
                token.boolean = true;
                tokenizer->at += 4;
            }
            else if(tokenizer->at[0] && tokenizer->at[0] == 'f' &&
                    tokenizer->at[1] && tokenizer->at[1] == 'a' &&
                    tokenizer->at[2] && tokenizer->at[2] == 'l' &&
                    tokenizer->at[3] && tokenizer->at[3] == 's' &&
                    tokenizer->at[4] && tokenizer->at[4] == 'e')
            {
                token.value = TokenValue_Bool;
                
                token.boolean = false;
                tokenizer->at += 5;
            }
            else if(tokenizer->at[1] && is_alpha(tokenizer->at[1]))
            {
                token.value = TokenValue_String;
                
                while(tokenizer->at[0] && is_alpha(tokenizer->at[0]))
                {
                    token.string.s[string_value_length++] = tokenizer->at[0];
                    ++tokenizer->at;
                }
            }
            else if(tokenizer->at[0])
            {
                token.value = TokenValue_String;
                
                token.string.s[0] = tokenizer->at[0];
                ++tokenizer->at;
            }
        }
        else if(is_numeric(tokenizer->at[0]))
        {
            token.value = TokenValue_Uint;
            
            // String to number
            while(tokenizer->at[0] && is_numeric(tokenizer->at[0]))
            {
                token.uint = (tokenizer->at[0] - '0') + (token.uint * 10);
                ++tokenizer->at;
            }
        }
        else
        {
            token.error = TokenError_InvalidValue;
        }
    }
    
    return(token);
}

internal Config
get_config(MemoryArena *memory_arena, char *file_path)
{
    Config config = {0};
    
    TemporaryMemory temporary_memory = start_temporary_memory(memory_arena);
    char *file_contents = read_file(temporary_memory.arena, file_path);
    
    Tokenizer tokenizer = {0};
    tokenizer.at = file_contents;
    
    b32 is_parsing = true;
    while(is_parsing)
    {
        Token token = get_token(&tokenizer);
        if(token.error == TokenError_EndOfFile || config.token_count >= MAX_TOKEN_COUNT)
        {
            is_parsing = false;
        }
        else if(token.error == TokenError_InvalidName)
        {
            is_parsing = false;
            
            printf("Token Error: Invalid token name\n");
        }
        else if(token.error == TokenError_InvalidSeparator)
        {
            is_parsing = false;
            
            printf("Token Error: Invalid separator for token name: %s\n", token.name.s);
        }
        else if(token.error == TokenError_InvalidValue)
        {
            is_parsing = false;
            
            printf("Token Error: Invalid value for token name: %s\n", token.name.s);
        }
        else
        {
            config.tokens[config.token_count++] = token;
        }
    }
    
#if 0
    printf("Token Count: %u\n\n", config.token_count);
    
    for(u32 index = 0; index < config.token_count; ++index)
    {
        Token *token = &config.tokens[index];
        printf("Token Name: %s\n", token->name);
        
        if(token->value == TokenValue_Uint)
        {
            printf("Token Value: %u\n", token->uint);
        }
        else if(token->value == TokenValue_Bool)
        {
            printf("Token Value: %s\n", token->boolean ? "true" : "false");
        }
        else if(token->value == TokenValue_String)
        {
            printf("Token Value: %s\n", token->string);
        }
        
        printf("\n");
    }
#endif
    
    end_temporary_memory(temporary_memory);
    return(config);
}
