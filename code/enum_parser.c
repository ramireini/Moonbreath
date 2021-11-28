#define MAX_ENUM_TOKEN_COUNT 1024
#define MAX_ENUM_TOKEN_MEMBER_COUNT 256

typedef enum
{
    EnumTokenError_None,
    
    EnumTokenError_InvalidOpenBrace,
    EnumTokenError_InvalidMemberCount,
    EnumTokenError_InvalidCloseBrace,
    EnumTokenError_InvalidName,
    EnumTokenError_InvalidSemicolon,
    EnumTokenError_EndOfFile,
} EnumTokenError;

typedef struct
{
    u32 length;
    String64 name;
} EnumTokenMember;

typedef struct
{
    b32 valid;
    EnumTokenError error;
    String64 name;
    u32 member_count;
    EnumTokenMember members[MAX_ENUM_TOKEN_MEMBER_COUNT];
} EnumToken;

typedef struct
{
    u32 token_count;
    EnumToken tokens[MAX_ENUM_TOKEN_COUNT];
} EnumParser;

// This is a global so we don't overflow the stack with our arrays.
global EnumParser parser;

internal String64
get_enum_token_file_name(String64 *name)
{
    u32 result_index = 0;
    String64 result = {0};
    
    b32 has_seen_uppercase = false;
    b32 has_seen_lowecase = false;
    
    for(u32 name_index = 0; name_index < get_string_length(name->s); ++name_index)
    {
        if(is_uppercase(name->s[name_index]))
        {
            // Insert underscores before uppercase letters,
            // we don't do it if the first letter of the name is an uppercase letter or
            // if we have multiple uppercase letters in a row.
            if(has_seen_uppercase &&
               has_seen_lowecase)
            {
                has_seen_lowecase = false;
                result.s[result_index++] = '_';
            }
            else
            {
                has_seen_uppercase = true;
            }
            
            result.s[result_index] = get_lowercase(name->s[name_index]);
        }
        else
        {
            has_seen_lowecase = true;
            result.s[result_index] = name->s[name_index];
        }
        
        ++result_index;
    }
    
    // Append array name postfix
    strcat(result.s, "_strings");
    
    return(result);
}

internal char *
read_enum_file(char *file_path)
{
    char *file_contents = 0;
    
    FILE *file = fopen(file_path, "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        memory_size file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        file_contents = malloc(file_size + 1);
        u32 ignored = fread(file_contents, file_size, 1, file);
        file_contents[file_size] = 0;
        
        fclose(file);
    }
    
    return(file_contents);
}

internal char *
get_enum_token_error_string(EnumTokenError error)
{
    char *result = 0;
    
    switch(error)
    {
        case EnumTokenError_None: result = "EnumTokenError_None"; break;
        
        case EnumTokenError_InvalidOpenBrace: result = "EnumTokenError_InvalidOpenBrace"; break;
        case EnumTokenError_InvalidMemberCount: result = "EnumTokenError_InvalidMemberCount"; break;
        case EnumTokenError_InvalidCloseBrace: result = "EnumTokenError_InvalidCloseBrace"; break;
        case EnumTokenError_InvalidName: result = "EnumTokenError_InvalidName"; break;
        case EnumTokenError_InvalidSemicolon: result = "EnumTokenError_InvalidSemicolon"; break;
        case EnumTokenError_EndOfFile: result = "EnumTokenError_EndOfFile"; break;
        
        invalid_default_case;
    }
    
    return(result);
}

internal EnumToken
parse_enum_token(Tokenizer *tokenizer)
{
    EnumToken token = {0};
    u32 name_length = 0;
    
    skip_tokenizer_whitespace(tokenizer);
    
    if(is_end_of_file(tokenizer->at[0]))
    {
        token.error = EnumTokenError_EndOfFile;
    }
    else
    {
        // Header
        if(tokenizer->at[0] == 't' &&
           tokenizer->at[1] == 'y' &&
           tokenizer->at[2] == 'p' &&
           tokenizer->at[3] == 'e' &&
           tokenizer->at[4] == 'd' &&
           tokenizer->at[5] == 'e' &&
           tokenizer->at[6] == 'f' &&
           tokenizer->at[7] == ' ' &&
           tokenizer->at[8] == 'e' &&
           tokenizer->at[9] == 'n' &&
           tokenizer->at[10] == 'u' &&
           tokenizer->at[11] == 'm')
        {
            move_tokenizer(tokenizer, 12);
            skip_tokenizer_whitespace(tokenizer);
            
            //printf("tokenizer->at: %s\n", tokenizer->at);
            //printf("tokenizer->at: %c\n", tokenizer->at);
            
            // Open brace
            if(tokenizer->at[0] && tokenizer->at[0] == '{')
            {
                move_tokenizer(tokenizer, 1);
                skip_tokenizer_whitespace(tokenizer);
                
                // Members
                while(tokenizer->at[0] && tokenizer->at[0] != '}')
                {
                    if(tokenizer->at[0] && tokenizer->at[0] == ',')
                    {
                        ++token.member_count;
                    }
                    else
                    {
                        EnumTokenMember *member = &token.members[token.member_count];
                        member->name.s[member->length++] = tokenizer->at[0];
                    }
                    
                    move_tokenizer(tokenizer, 1);
                    skip_tokenizer_whitespace(tokenizer);
                }
                
                if(token.member_count)
                {
                    // Closing brace
                    if(tokenizer->at[0] && tokenizer->at[0] == '}')
                    {
                        move_tokenizer(tokenizer, 1);
                        skip_tokenizer_whitespace(tokenizer);
                        
                        // Name
                        while(tokenizer->at[0] && is_alpha(tokenizer->at[0]))
                        {
                            token.name.s[name_length++] = tokenizer->at[0];
                            move_tokenizer(tokenizer, 1);
                        }
                        skip_tokenizer_whitespace(tokenizer);
                        
                        if(token.name.s[0])
                        {
                            // Semicolon
                            if(tokenizer->at[0] == ';')
                            {
                                move_tokenizer(tokenizer, 1);
                                token.valid = true;
                            }
                            else
                            {
                                token.error = EnumTokenError_InvalidSemicolon;
                            }
                        }
                        else
                        {
                            token.error = EnumTokenError_InvalidName;
                        }
                    }
                    else
                    {
                        token.error = EnumTokenError_InvalidCloseBrace;
                    }
                }
                else
                {
                    token.error = EnumTokenError_InvalidMemberCount;
                }
            }
            else
            {
                token.error = EnumTokenError_InvalidOpenBrace;
            }
        }
        else
        {
            // We didn't find anything we care about so advance
            move_tokenizer(tokenizer, 1);
        }
    }
    
    return(token);
}

internal void
parse_enum_tokens(EnumParser *parser, char *file_name)
{
    char *file_contents = read_enum_file(get_os_path(file_name).s);
    Tokenizer tokenizer = {0};
    tokenizer.at = file_contents;
    
    //printf("file_contents: %p\n", file_contents);
    //printf("file_contents: %s\n", file_contents);
    
    b32 parsing = true;
    while(parsing)
    {
        EnumToken token = parse_enum_token(&tokenizer);
        if(token.valid)
        {
            parser->tokens[parser->token_count++] = token;
        }
        else if(token.error || parser->token_count >= MAX_ENUM_TOKEN_COUNT)
        {
            parsing = false;
        }
    }
    
    free(file_contents);
}

internal void
parse_enum_tokens_for_file_wildcard(char *file_wildcard)
{
    WIN32_FIND_DATA find_data = {0};
    HANDLE find_handle = FindFirstFileA(file_wildcard, &find_data);
    while(find_handle != INVALID_HANDLE_VALUE)
    {
        //printf("find_data.cFileName: %s\n", find_data.cFileName);
        parse_enum_tokens(&parser, find_data.cFileName);
        
        if(!FindNextFile(find_handle, &find_data))
        {
            break;
        }
    }
    
    // Close the valid handle
    if(find_handle != INVALID_HANDLE_VALUE)
    {
        FindClose(find_handle);
    }
}

internal void
parse_and_write_enum_tokens()
{
    parse_enum_tokens_for_file_wildcard("*.c");
    parse_enum_tokens_for_file_wildcard("*.h");
    
#if 0 // Print enum tokens
    printf("\n");
    
    for(u32 token_index = 0; token_index < parser.token_count; ++token_index)
    {
        EnumToken *token = &parser.tokens[token_index];
        printf(" - Enum Token[%u] -\n", token_index);
        printf("Enum Error: %s\n", get_enum_token_error_string(token->error));
        printf("Enum Name: \"%s\"\n", token->name.s);
        printf("Enum Member Count: %u\n", token->member_count);
        
        for(u32 member_index = 0; member_index < token->member_count; ++member_index)
        {
            EnumTokenMember *member = &token->members[member_index];
            printf("Enum Member[%u]: \"%s\" (%u)\n", member_index, member->name.s, member->length);
        }
        
        printf("\n");
    }
#endif
    
    //return;
    
    // Remove the result file if it already exists
    char *result_file_name = "enum_parser_result.c";
    remove(result_file_name);
    
    // Write enum tokens as character arrays
    FILE *result_file = fopen(result_file_name, "a");
    
    for(u32 token_index = 0; token_index < parser.token_count; ++token_index)
    {
        EnumToken *token = &parser.tokens[token_index];
        
        String64 array_name = get_enum_token_file_name(&token->name);
        //printf("Array Name: %s\n", array_name.s);
        fprintf(result_file, "char *%s[%u] =\n", array_name.s, token->member_count);
        
        fprintf(result_file, "{\n");
        
        for(u32 member_index = 0; member_index < token->member_count; ++member_index)
        {
            EnumTokenMember *member = &token->members[member_index];
            fprintf(result_file, "    \"%s\",\n", member->name.s);
        }
        
        fprintf(result_file, "};\n\n");
    }
    fclose(result_file);
}