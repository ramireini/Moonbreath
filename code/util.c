#define array_count(array) (sizeof(array) / sizeof((array)[0]))

internal v2u
make_v2u(u32 a, u32 b)
{
    v2u result = {a, b};
    return(result);
}

internal v4s
make_v4s(s32 a, s32 b, s32 c, s32 d)
{
    v4s result = {a, b, c, d};
    return(result);
}

internal v4u
make_v4u(u32 a, u32 b, u32 c, u32 d)
{
    v4u result = {a, b, c, d};
    return(result);
}

internal b32
is_v2u_equal(v2u a, v2u b)
{
    b32 result = (a.x == b.x &&
                  a.y == b.y);
    
    return(result);
}

internal b32
is_v2u_zero(v2u a)
{
    b32 result = (a.x == 0 &&
                  a.y == 0);
    
    return(result);
}

internal void
print_v2u(char *name, v2u a)
{
    printf("%s.x: %u\n", name, a.x);
    printf("%s.y: %u\n\n", name, a.y);
}

internal void
print_v2s(char *name, v2s a)
{
    printf("%s.x: %d\n", name, a.x);
    printf("%s.y: %d\n\n", name, a.y);
}

internal void
print_v4u(char *name, v4u a)
{
    printf("%s.x: %u\n", name, a.x);
    printf("%s.y: %u\n", name, a.y);
    printf("%s.w: %u\n", name, a.w);
    printf("%s.h: %u\n\n", name, a.h);
}

internal void
print_v4s(char *name, v4s a)
{
    printf("%s.x: %d\n", name, a.x);
    printf("%s.y: %d\n", name, a.y);
    printf("%s.w: %d\n", name, a.w);
    printf("%s.h: %d\n\n", name, a.h);
}

internal char
get_sign(s32 value)
{
    char result = value < 0 ? '-' : '+';
    return(result);
}

internal u32
get_string_length(char *string)
{
    u32 length = 0;
    
    while(*string++)
    {
        ++length;
    }
    
    return(length);
}

internal u32
get_absolute(s32 value)
{
    u32 result = value < 0 ? -value : value;
    return(result);
}

internal f32
get_slope(f32 x1, f32 y1, f32 x2, f32 y2)
{
    f32 result = (x1 - x2) / (y1 - y2);
    return(result);
}

internal f32
get_distance(u32 x1, u32 y1, u32 x2, u32 y2)
{
    f32 result = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return(result);
}

internal u32
get_cardinal_distance(v2u a, v2u b)
{
    u32 result = get_absolute(a.x - b.x) + get_absolute(a.y - b.y);
    return(result);
}

internal u32
get_ordinal_and_ordinal_distance(v2u a, v2u b)
{
    u32 result = 0;
    
    while(!is_v2u_equal(a, b))
    {
        if(a.x < b.x)
        {
            ++a.x;
        }
        else if(a.x > b.x)
        {
            --a.x;
        }
        
        if(a.y < b.y)
        {
            ++a.y;
        }
        else if(a.y > b.y)
        {
            --a.y;
        }
        
        ++result;
    }
    
    return(result);
}

internal u32
get_ratio(f32 min, f32 max, f32 width)
{
    u32 result = (u32)((min / max) * width);
    return(result);
}

internal v2u
get_v2u_from_index(u32 index, u32 width)
{
    v2u result = {index, 0};
    
    if(index >= width)
    {
        result.x = index % width;
        result.y = index / width;
    }
    
    return(result);
}

internal u32
get_rect_area(v4u rect)
{
    u32 area = (rect.w * rect.h);
    return(area);
}

internal v2u
get_rect_center(v4u rect)
{
    v2u result =
    {
        rect.x + (rect.w / 2),
        rect.y + (rect.h / 2),
    };
    
    return(result);
}

internal b32
is_inside_rect(v4u rect, v2u pos)
{
    b32 result = (pos.x >= rect.x &&
                  pos.y >= rect.y &&
                  pos.x <= (rect.x + rect.w) &&
                  pos.y <= (rect.y + rect.h));
    
    return(result);
}

internal b32
is_lowercase(char c)
{
    b32 result = (c >= 'a') && (c <= 'z');
    return(result);
}

internal b32
is_uppercase(char c)
{
    b32 result = (c >= 'A') && (c <= 'Z');
    return(result);
}

internal b32
is_alpha(char c)
{
    b32 result = is_lowercase(c) || is_uppercase(c);
    return(result);
}

internal char
make_uppercase(char c)
{
    char result = 0;
    
    if(is_lowercase(c))
    {
        result = c + 32;
    }
    
    return(result);
}

internal b32
strings_match(char *a, char *b)
{
    b32 result = false;
    
    while(*a && *b &&
          *a++ == *b++)
    {
        if(*a == '\0' &&
           *b == '\0')
        {
            result = true;
            break;
        }
    }
    
    return(result);
}

#define zero_struct(structure) zero_size(&(structure), sizeof(structure));
#define zero_array(pointer, count) zero_size(pointer, sizeof((pointer)[0]) * count);
internal void
zero_size(void *pointer, u32 size)
{
    u8 *byte = pointer;
    while(size--)
    {
        *byte++ = 0;
    }
}