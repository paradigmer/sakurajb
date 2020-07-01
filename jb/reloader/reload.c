/*
 * reload.c - reload daemons
 *
 * Copyright (c) 2020 dora2ios
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>

int main(int argc, const char **argv)
{
    
    const char *jl;
    pid_t pd = 0;
    
    jl = "/usr/libexec/dirhelper";
    posix_spawn(&pd, jl, NULL, NULL, (char **)&(const char*[]){ jl, NULL }, NULL);
    waitpid(pd, NULL, 0);
    
    jl = "/bin/launchctl";
    posix_spawn(&pd, jl, NULL, NULL, (char **)&(const char*[]){ jl, "load", "/Library/LaunchDaemons", NULL }, NULL);
    waitpid(pd, NULL, 0);
    
    jl = "/usr/libexec/CrashHousekeeping_";
    posix_spawn(&pd, jl, NULL, NULL, (char **)&(const char*[]){ jl, NULL }, NULL);
    waitpid(pd, NULL, 0);

    return 0;
}

