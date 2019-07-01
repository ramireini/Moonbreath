#ifndef MOONBREATH_PLATFORM_H
#define MOONBREATH_PLATFORM_H

// NOTE(rami):
// Some sort of an API that platform non-specific code will call so that the platform layer
// can perform platform specific tasks

internal char *platform_get_file_contents(char *path);

#endif // MOONBREATH_PLATFORM_H