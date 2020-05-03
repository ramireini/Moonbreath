internal b32
is_vowel(char character)
{
    b32 result = false;
    
    char vowels[10] = "aeiouAEIOU";
    for(u32 index = 0; index < 10; ++index)
    {
        if(character == vowels[index])
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal b32
is_consonant(char character)
{
    b32 result = false;
    
    char consonants[42] = "bcdfghjklmnpqrstvxzwyBCDFGHJKLMNPQRSTVXZWY";
    for(u32 index = 0; index < 42; ++index)
    {
        if(character == consonants[index])
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

internal char
random_vowel(random_state_t *random)
{
    char vowels[5] = "aeiou";
    return(vowels[random_number(random, 0, 4)]);
}

internal char
random_consonant(random_state_t *random)
{
    char consonants[21] = "bcdfghjklmnpqrstvxzwy";
    return(consonants[random_number(random, 0, 20)]);
}

internal char
random_character(random_state_t *random)
{
    char result = ('a' + random_number(random, 0, 25));
    return(result);
}

internal char *
random_name(random_state_t *random, char *name)
{
    b32 has_space = false;
    u32 name_index = 0;
    u32 name_length = 3;
    name_length += random_number(random, 0, 8);
    
    
    while(name_index < name_length)
    {
        char prev = name_index ? name[name_index - 1] : 0;
        char prev_prev = name_index >= 2 ? name[name_index - 2] : 0;
        
        if(!name_index || prev == ' ')
        {
            name[name_index++] = toupper(random_character(random));
        }
        else if(!has_space &&
                name_index >= 4 &&
                name_index < (name_length - 4))
        {
            has_space = true;
            name[name_index++] = ' ';
        }
        else
        {
            if(is_vowel(prev))
            {
                name[name_index++] = random_consonant(random);
                
                if(!random_number(random, 0, 4) &&
                   name_index < name_length)
                {
                    name[name_index++] = random_consonant(random);
                }
            }
            else
            {
                name[name_index++] = random_vowel(random);
                
                if(!random_number(random, 0, 4) &&
                   name_index < name_length)
                {
                    name[name_index++] = random_vowel(random);
                }
            }
        }
    }
    
    name[name_index] = 0;
    return(name);
}