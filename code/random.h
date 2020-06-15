typedef struct
{
    u32 seed;
} RandomState;

internal RandomState
set_random_seed(u32 value)
{
    assert(value);
    
    RandomState result = {value};
    return(result);
}

internal u32
next_random_u32(RandomState *random)
{
    u32 result = random->seed;
    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;
    random->seed = result;
    
	return(result);
}

internal s32
random_number(RandomState *random, s32 min, s32 max)
{
    s32 result = min + (next_random_u32(random) % ((max + 1) - min));
    return(result);
}