typedef struct
{
    u32 seed;
} Random;

internal Random
set_random_seed(u32 value)
{
    assert(value);
    
    Random result = {value};
    return(result);
}

internal u32
get_random_u32(Random *random)
{
    u32 result = random->seed;
    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;
    random->seed = result;
    
	return(result);
}

internal s32
get_random_number(Random *random, s32 min, s32 max)
{
    assert(min <= max);
    
    s32 result = min + (get_random_u32(random) % ((max + 1) - min));
    return(result);
}

internal u32
get_random_number_from_v2u(Random *random, v2u a)
{
    u32 result = get_random_number(random, a.min, a.max);
    return(result);
}

internal u32
get_random_chance(Random *random)
{
    u32 result = get_random_number(random, 1, 100);
    return(result);
}

internal b32
hit_random_chance(Random *random, u32 value)
{
    b32 result = (get_random_chance(random) <= value);
    return(result);
}