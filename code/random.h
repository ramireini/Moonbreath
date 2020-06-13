typedef struct
{
    u32 seed;
} random_state_t;

internal random_state_t
set_random_seed(u32 value)
{
    assert(value);
    
    random_state_t result = {value};
    return(result);
}

internal u32
next_random_u32(random_state_t *random)
{
    u32 result = random->seed;
    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;
    random->seed = result;
    
	return(result);
}

internal s32
random_number(random_state_t *random, s32 min, s32 max)
{
    s32 result = min + (next_random_u32(random) % ((max + 1) - min));
    return(result);
}