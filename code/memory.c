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
start_temporary_memory(MemoryArena *arena)
{
 ++arena->temporary_memory_count;
 
    TemporaryMemory result = {0};
    result.arena = arena;
 result.used = arena->used;
 
    return(result);
}

internal void
end_temporary_memory(TemporaryMemory memory)
{
 MemoryArena *arena = memory.arena;
    
 assert(arena->used >= memory.used);
 arena->used = memory.used;
    
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
 
 //printf("Used Game Memory: %lu/%lu (%lu added)\n", arena->used, arena->size, size);
 
 return(result);
}

#define pop_memory_struct(arena, type) pop_memory(arena, sizeof(type))
internal void
pop_memory(MemoryArena *arena, memory_size size)
{
    assert(arena->used >= size);
    arena->used -= size;
}

internal void
init_memory_arena(MemoryArena *arena, u8 *base, memory_size size)
{
    arena->base = base;
    arena->size = size;
    arena->used = 0;
}