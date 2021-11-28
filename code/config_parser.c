#define MAX_CONFIG_TOKEN_COUNT 32

typedef enum
{
    ConfigTokenError_None,
    
    ConfigTokenError_EndOfFile,
    ConfigTokenError_InvalidName,
    ConfigTokenError_InvalidSeparator,
    ConfigTokenError_InvalidValue
} ConfigTokenError;

typedef enum
{
    ConfigTokenValue_None,
    
    ConfigTokenValue_Uint,
    ConfigTokenValue_Bool,
    ConfigTokenValue_String
} ConfigTokenValue;

typedef struct
{
    b32 valid;
    
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
    
    ConfigTokenError error;
    ConfigTokenValue value;
    union
    {
        u32 uint;
        b32 boolean;
        String64 string;
    };
} ConfigToken;

typedef struct
{
    u32 token_count;
    ConfigToken tokens[MAX_CONFIG_TOKEN_COUNT];
} Config;

internal b32
is_end_of_file(char c)
{
    b32 result = (c == '\0');
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

internal b32
is_config_token_valid(ConfigTokenValue value, char *token_name, ConfigToken *token)
{
    b32 result = (token->value == value &&
                  strings_match(token_name, token->name.s));
    
    return(result);
}

internal ConfigValue
get_config_uint(Config *config, char *token_name)
{
    ConfigValue value = {0};
    
    for(u32 index = 0; index < config->token_count; ++index)
    {
        ConfigToken *token = &config->tokens[index];
        if(is_config_token_valid(ConfigTokenValue_Uint, token_name, token))
        {
            value.valid = true;
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
        ConfigToken *token = &config->tokens[index];
        if(is_config_token_valid(ConfigTokenValue_Bool, token_name, token))
        {
            value.valid = true;
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
        ConfigToken *token = &config->tokens[index];
        if(is_config_token_valid(ConfigTokenValue_String, token_name, token))
        {
            result.valid = true;
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

internal ConfigToken
get_config_token(Tokenizer *tokenizer)
{
    ConfigToken token = {0};
    u32 name_length = 0;
    u32 string_length = 0;
    
    skip_tokenizer_whitespace(tokenizer);
    
    if(is_end_of_file(tokenizer->at[0]))
    {
        token.error = ConfigTokenError_EndOfFile;
    }
    else
    {
        // Name
        if(is_alpha(tokenizer->at[0]))
        {
            while(tokenizer->at[0] && !is_whitespace(tokenizer->at[0]))
            {
                token.name.s[name_length++] = tokenizer->at[0];
                move_tokenizer(tokenizer, 1);
            }
        }
        else
        {
            token.error = ConfigTokenError_InvalidName;
        }
        
        // Separator
        if(tokenizer->at[0] && tokenizer->at[0] == ' ' &&
           tokenizer->at[1] && tokenizer->at[1] == '=' &&
           tokenizer->at[2] && tokenizer->at[2] == ' ')
        {
            move_tokenizer(tokenizer, 3);
        }
        else
        {
            token.error = ConfigTokenError_InvalidSeparator;
        }
        
        // Value
        if(tokenizer->at[0] && is_special(tokenizer->at[0]))
        {
            token.value = ConfigTokenValue_String;
            
            token.string.s[0] = tokenizer->at[0];
            move_tokenizer(tokenizer, 1);
        }
        else if(tokenizer->at[0] && is_alpha(tokenizer->at[0]))
        {
            if(tokenizer->at[0] && tokenizer->at[0] == 't' &&
               tokenizer->at[1] && tokenizer->at[1] == 'r' &&
               tokenizer->at[2] && tokenizer->at[2] == 'u' &&
               tokenizer->at[3] && tokenizer->at[3] == 'e')
            {
                token.value = ConfigTokenValue_Bool;
                
                token.boolean = true;
                move_tokenizer(tokenizer, 4);
            }
            else if(tokenizer->at[0] && tokenizer->at[0] == 'f' &&
                    tokenizer->at[1] && tokenizer->at[1] == 'a' &&
                    tokenizer->at[2] && tokenizer->at[2] == 'l' &&
                    tokenizer->at[3] && tokenizer->at[3] == 's' &&
                    tokenizer->at[4] && tokenizer->at[4] == 'e')
            {
                token.value = ConfigTokenValue_Bool;
                
                token.boolean = false;
                move_tokenizer(tokenizer, 5);
            }
            else if(tokenizer->at[1] && is_alpha(tokenizer->at[1]))
            {
                token.value = ConfigTokenValue_String;
                
                while(tokenizer->at[0] && is_alpha(tokenizer->at[0]))
                {
                    token.string.s[string_length++] = tokenizer->at[0];
                    move_tokenizer(tokenizer, 1);
                }
            }
            else if(tokenizer->at[0])
            {
                token.value = ConfigTokenValue_String;
                
                token.string.s[0] = tokenizer->at[0];
                move_tokenizer(tokenizer, 1);
            }
        }
        else if(is_numeric(tokenizer->at[0]))
        {
            token.value = ConfigTokenValue_Uint;
            
            // String to number
            while(tokenizer->at[0] && is_numeric(tokenizer->at[0]))
            {
                token.uint = (tokenizer->at[0] - '0') + (token.uint * 10);
                move_tokenizer(tokenizer, 1);
            }
        }
        else
        {
            token.error = ConfigTokenError_InvalidValue;
        }
    }
    
    return(token);
}

internal Config
get_config(MemoryArena *memory_arena, char *file_path)
{
    Config config = {0};
    TemporaryMemory temporary_memory = start_temporary_memory(memory_arena);
    
    Tokenizer tokenizer = {0};
    tokenizer.at = read_file(temporary_memory.arena, file_path);
    
    b32 parsing = true;
    while(parsing)
    {
        ConfigToken token = get_config_token(&tokenizer);
        
        if(token.error == ConfigTokenError_EndOfFile)
        {
            parsing = false;
        }
        else if(token.error == ConfigTokenError_InvalidName)
        {
            parsing = false;
            printf("Config Token Error: Invalid token name\n");
        }
        else if(token.error == ConfigTokenError_InvalidSeparator)
        {
            parsing = false;
            printf("Config Token Error: Invalid separator for token name: %s\n", token.name.s);
        }
        else if(token.error == ConfigTokenError_InvalidValue)
        {
            parsing = false;
            printf("Config Token Error: Invalid value for token name: %s\n", token.name.s);
        }
        else if(config.token_count >= MAX_CONFIG_TOKEN_COUNT)
        {
            parsing = false;
            printf("Config Token Error: Reached maximum token count.\n");
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
        ConfigToken *token = &config.tokens[index];
        printf("Config Token Name: %s\n", token->name);
        
        if(token->value == ConfigTokenValue_Uint)
        {
            printf("Config Token Value: %u\n", token->uint);
        }
        else if(token->value == ConfigTokenValue_Bool)
        {
            printf("Config Token Value: %s\n", get_bool_string(token->boolean));
        }
        else if(token->value == ConfigTokenValue_String)
        {
            printf("Config Token Value: %s\n", token->string);
        }
        
        printf("\n");
    }
#endif
    
    end_temporary_memory(temporary_memory);
    return(config);
}