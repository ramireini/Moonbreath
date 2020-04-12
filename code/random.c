internal void
set_random_seed(u32 value)
{
    state.seed = value;
}

internal u32
get_next_random_u32()
{
    // NOTE(rami): Must be initialized to not zero.
    u32 result = state.seed;
    result ^= result << 13;
    result ^= result >> 17;
    result ^= result << 5;
    state.seed = result;
    
	return(result);
}

internal s32
random_number(s32 min, s32 max)
{
    s32 result = min + (get_next_random_u32() % ((max + 1) - min));
    return(result);
}