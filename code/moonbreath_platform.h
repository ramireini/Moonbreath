#ifndef MOONBREATH_PLATFORM_H
#define MOONBREATH_PLATFORM_H

// NOTE(rami):
// Includes the operations that the platform layer is supposed to be able to perform
// on behalf of our cross platform code, an API of sorts
// All of the platform non specific code will call to it if needed

internal char *platform_get_file_contents(char *path);

#endif // MOONBREATH_PLATFORM_H