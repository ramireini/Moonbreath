typedef enum
{
    name_type_item,
    name_type_npc
} name_type;

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
random_name(random_state_t *random, char *name, name_type type)
{
    u32 space_position = 0;
    u32 name_length = 0;
    u32 name_index = 0;
    
    if(type == name_type_item)
    {
        name_length = random_number(random, 3, 8);
    }
    else if(type == name_type_npc)
    {
        name_length = random_number(random, 8, 12);
        space_position = random_number(random, 3, name_length - 3);
    }
    
    while(name_index < name_length)
    {
        char prev = name_index ? name[name_index - 1] : 0;
        char prev_prev = name_index >= 2 ? name[name_index - 2] : 0;
        
        if(!name_index || prev == ' ')
        {
            name[name_index++] = toupper(random_character(random));
        }
        else if(space_position && name_index >= space_position)
        {
            space_position = 0;
            name[name_index++] = ' ';
        }
        else
        {
            if(is_vowel(prev))
            {
                name[name_index++] = random_consonant(random);
                
                if(!is_consonant(prev_prev) &&
                   !random_number(random, 0, 2) &&
                   name_index < name_length)
                {
                    name[name_index++] = random_consonant(random);
                }
            }
            else
            {
                name[name_index++] = random_vowel(random);
                
                if(!is_vowel(prev_prev) &&
                   !random_number(random, 0, 2) &&
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