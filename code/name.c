typedef enum
{
    NameType_Item,
    NameType_NPC
} NameType;

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
get_random_vowel(Random *random)
{
    char vowels[5] = "aeiou";
    return(vowels[get_random_number(random, 0, 4)]);
}

internal char
get_random_consonant(Random *random)
{
    char consonants[21] = "bcdfghjklmnpqrstvxzwy";
    return(consonants[get_random_number(random, 0, 20)]);
}

internal char
get_random_character(Random *random)
{
    char result = ('a' + get_random_number(random, 0, 25));
    return(result);
}

internal char *
get_random_name(Random *random, char *name, NameType type)
{
    u32 space_pos = 0;
    u32 name_length = 0;
    u32 name_index = 0;
    
    if(type == NameType_Item)
    {
        name_length = get_random_number(random, 3, 8);
    }
    else if(type == NameType_NPC)
    {
        name_length = get_random_number(random, 8, 12);
        space_pos = get_random_number(random, 3, name_length - 3);
    }
    
    while(name_index < name_length)
    {
        char prev = name_index ? name[name_index - 1] : 0;
        char prev_prev = name_index >= 2 ? name[name_index - 2] : 0;
        
        if(!name_index || prev == ' ')
        {
            char random_c = get_random_character(random);
            name[name_index++] = make_uppercase(random_c);
        }
        else if(space_pos && name_index >= space_pos)
        {
            space_pos = 0;
            name[name_index++] = ' ';
        }
        else
        {
            if(is_vowel(prev))
            {
                name[name_index++] = get_random_consonant(random);
                
                if(!is_consonant(prev_prev) &&
                       !get_random_number(random, 0, 2) &&
                       name_index < name_length)
                {
                    name[name_index++] = get_random_consonant(random);
                }
            }
            else
            {
                name[name_index++] = get_random_vowel(random);
                
                if(!is_vowel(prev_prev) &&
                       !get_random_number(random, 0, 2) &&
                   name_index < name_length)
                {
                    name[name_index++] = get_random_vowel(random);
                }
            }
        }
    }
    
    name[name_index] = 0;
    return(name);
}