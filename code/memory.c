#define zero_struct(structure) zero_size(&(structure), sizeof(structure));
#define zero_array(pointer, count) zero_size(pointer, sizeof((pointer)[0]) * (count));
internal void
zero_size(void *pointer, memory_size size)
{
    u8 *byte = pointer;
    while(size--)
    {
        *byte++ = 0;
    }
}

internal TemporaryMemory
begin_temporary_memory(MemoryArena *arena)
{
    TemporaryMemory result = {0};
    
    result.arena = arena;
    result.used = arena->used;
    
    ++arena->temporary_memory_count;
    
    return(result);
}

internal void
end_temporary_memory(TemporaryMemory temp_mem)
{
    MemoryArena *arena = temp_mem.arena;
    
    assert(arena->used >= temp_mem.used);
    arena->used = temp_mem.used;
    
    assert(arena->temporary_memory_count);
    --arena->temporary_memory_count;
}

#define push_memory_struct(arena, type) (type *)push_memory(arena, sizeof(type))
internal void *
push_memory(MemoryArena *arena, memory_size size)
{
    assert((arena->used + size) <= arena->size);
    
    void *result = arena->base + arena->used;
    arena->used += size;
    
    return(result);
}

internal void
pop_memory(MemoryArena *arena, memory_size size)
{
    assert(arena->used >= size);
    arena->used -= size;
}

internal void
init_arena(MemoryArena *arena, u8 *base, memory_size size)
{
    arena->base = base;
    arena->size = size;
    arena->used = 0;
}