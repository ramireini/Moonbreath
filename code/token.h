typedef struct
{
    char *at;
} Tokenizer;

internal void
move_tokenizer(Tokenizer *tokenizer, u32 count)
{
    tokenizer->at += count;
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

internal void
skip_tokenizer_whitespace(Tokenizer *tokenizer)
{
    for(;;)
    {
        if(is_whitespace(tokenizer->at[0]))
        {
            move_tokenizer(tokenizer, 1);
        }
        else if(tokenizer->at[0] == '/' &&
                tokenizer->at[1] == '/')
        {
            move_tokenizer(tokenizer, 2);
            
            while(tokenizer->at[0] && !is_end_of_line(tokenizer->at[0]))
            {
                move_tokenizer(tokenizer, 1);
            }
        }
        else if(tokenizer->at[0] == '/' &&
                tokenizer->at[1] == '*')
        {
            move_tokenizer(tokenizer, 2);
            
            while(tokenizer->at[0] && !(tokenizer->at[0] == '*' &&
                                        tokenizer->at[1] == '/'))
            {
                move_tokenizer(tokenizer, 1);
            }
            
            if(tokenizer->at[0] == '*')
            {
                move_tokenizer(tokenizer, 2);
            }
        }
        else
        {
            break;
        }
    }
}