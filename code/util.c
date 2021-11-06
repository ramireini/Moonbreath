#define get_bool_string(value) (value ? "True": "False")
#define get_array_count(array) (sizeof(array) / sizeof((array)[0]))

#define toggle(flags, new_flag) \
{ \
if(is_set(flags, new_flag)) \
{ \
unset(flags, new_flag); \
} \
else \
{ \
set(flags, new_flag); \
} \
}

#define set(flags, new_flags) (flags |= (new_flags))
#define unset(flags, new_flags) (flags &= ~(new_flags))
#define is_set(flags, new_flags) ((flags & (new_flags)) == (new_flags))

#define print_v2u(name) \
printf("%s.x: %u\n", #name, name.x); \
printf("%s.y: %u\n\n", #name, name.y); \

#define print_v2s(name) \
printf("%s.x: %d\n", #name, name.x); \
printf("%s.y: %d\n\n", #name, name.y); \

#define print_v4u(name) \
printf("%s.x: %u\n", #name, name.x); \
printf("%s.y: %u\n", #name, name.y); \
printf("%s.w: %u\n", #name, name.w); \
printf("%s.h: %u\n\n", #name, name.h); \

#define print_v4s(name) \
printf("%s.x: %d\n", #name, name.x); \
printf("%s.y: %d\n", #name, name.y); \
printf("%s.w: %d\n", #name, name.w); \
printf("%s.h: %d\n\n", #name, name.h); \

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
is_v4s_zero(v4s a)
{
    b32 result = (a.x == 0 &&
                  a.y == 0 &&
                  a.w == 0 &&
                  a.h == 0);
    
    return(result);
}

internal b32
is_v2u_any_set(v2u a)
{
    b32 result = (a.x ||
                  a.y);
    
    return(result);
}

internal b32
is_v2u_set(v2u a)
{
    b32 result = (a.x &&
                  a.y);
    
    return(result);
}

internal b32
is_v4u_set(v4u a)
{
    b32 result = (a.x &&
                  a.y &&
                  a.w &&
                  a.h);
    
    return(result);
}

internal b32
is_v2u_zero(v2u a)
{
    b32 result = (a.x == 0 &&
                  a.y == 0);
    
    return(result);
}

internal b32
is_v4u_zero(v4u a)
{
    b32 result = (a.x == 0 &&
                  a.y == 0 &&
                  a.w == 0 &&
                  a.h == 0);
    
    return(result);
}

internal u32
get_string_length(char *string)
{
    u32 result = 0;
    
    while(*string++)
    {
        ++result;
    }
    
    return(result);
}

internal char
get_sign(s32 value)
{
    char result = (value < 0) ? '-' : '+';
    return(result);
}

internal u32
get_absolute(s32 value)
{
    u32 result = (value < 0) ? -value : value;
    return(result);
}

internal String8
get_signed_absolute(s32 value)
{
    String8 result = {0};
    sprintf(result.s, "%c%u", get_sign(value), get_absolute(value));
    
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
cardinal_and_ordinal_distance(v2u pos, v2u target)
{
    u32 result = 0;
    
    while(!is_v2u_equal(pos, target))
    {
        if(pos.x < target.x)
        {
            ++pos.x;
        }
        else if(pos.x > target.x)
        {
            --pos.x;
        }
        
        if(pos.y < target.y)
        {
            ++pos.y;
        }
        else if(pos.y > target.y)
        {
            --pos.y;
        }
        
        ++result;
    }
    
    return(result);
}

internal u32
get_ratio(f32 min, f32 max, f32 width)
{
    assert(min);
    assert(max);
    assert(width);
    
    u32 result = (u32)((min / max) * width);
    return(result);
}

internal u32
get_v2u_area(v2u size)
{
    assert(is_v2u_set(size));
    
    u32 result = size.w * size.h;
    return(result);
}

internal u32
get_rect_area(v4u rect)
{
    assert(is_v4u_set(rect));
    
    v2u size = {rect.w, rect.h};
    u32 result = get_v2u_area(size);
    
    return(result);
}

internal v2u
get_rect_center(v4u rect)
{
    assert(is_v4u_set(rect));
    
    v2u result =
    {
        rect.x + (rect.w / 2),
        rect.y + (rect.h / 2),
    };
    
    return(result);
}

internal b32
is_pos_inside_rect(v4u rect, v2u pos)
{
    b32 result = (pos.x >= rect.x &&
                  pos.y >= rect.y &&
                  pos.x <= (rect.x + rect.w) &&
                  pos.y <= (rect.y + rect.h));
    
    return(result);
}

internal b32
is_numeric(char c)
{
    b32 result = ((c >= '0') && (c <= '9'));
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
    b32 result = (is_lowercase(c) || is_uppercase(c));
    return(result);
}

internal b32
is_hex(char c)
{
    b32 result = (is_numeric(c) || is_uppercase(c));
    return(result);
}

internal b32
is_alpha_or_space(char c)
{
    b32 result = (is_alpha(c) || c == ' ');
    return(result);
}

internal char
get_lowercase(char c)
{
    assert(is_uppercase(c));
    
    char result = (c + 32);
    return(result);
}

internal char
get_uppercase(char c)
{
    assert(is_lowercase(c));
    
    char result = (c - 32);
    return(result);
}

internal b32
are_chars_equal_no_case(char a, char b)
{
    b32 result = false;
    
    if(is_alpha_or_space(a) &&
       is_alpha_or_space(b))
    {
        if(is_uppercase(a)) a = get_lowercase(a);
        if(is_uppercase(b)) b = get_lowercase(b);
        
        result = (a == b);
    }
    
    return(result);
}

internal b32
string_has_string(char *a, char *b, b32 case_sensitive)
{
    assert(a);
    assert(b);
    
    while(a[0])
    {
        // Start substring search
        if(are_chars_equal_no_case(a[0], b[0]))
        {
            char *sub_a = a;
            char *sub_b = b;
            
            // While matching
            while(sub_a[0] && sub_b[0] &&
                  are_chars_equal_no_case(sub_a[0], sub_b[0]))
            {
                ++sub_a;
                ++sub_b;
                
                // Has been matching and search string ended
                if(sub_b[0] == '\0')
                {
                    return(true);
                }
            }
            
        }
        
        ++a;
    }
    
    return(false);
}

internal b32
strings_match(char *a, char *b)
{
    assert(a);
    assert(b);
    
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

internal u32
get_rect_width(v4u rect)
{
    u32 result = rect.x + rect.w;
    return(result);
}

internal u32
get_rect_height(v4u rect)
{
    u32 result = rect.y + rect.h;
    return(result);
}

internal f32
lerp(f32 start, f32 end, f32 step)
{
    f32 result = (start * (1.0f - step)) + (end * step);
    return(result);
}

internal u32
get_u32_from_up_rounded_f32(f32 value)
{
    u32 result = value + 0.5f;
    return(result);
}