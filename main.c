#include <stdlib.h>
#include <string.h>
#include "dataio.h"

// spacetimescaling <init_path> [-i <init_template> <init_length>] [-o <final_path>] <final_template> <final_width> <final_height> <final_length> <scaling_method>
int main(int argc, char *argv[])
{
    char *spath, *init_template = NULL, *final_path, *final_template, *scaling_method;
    // Wczytywanie danych wejsciowych
    int i = 2;
    if(!argv[1])
        return 1;
    if(!strcmp(argv[2], "-i"))
    {
        if(!(init_template = argv[3]) || !(init_length = convert_to_int(argv[4])) || !check_tmplt(init_template, argv[4]))
            return 1;
        i = 5;
    }
    final_path = spath = malloc(strlen(argv[1]) + 2);
    strcpy(spath, argv[1]);
    if(*(argv[1] + strlen(argv[1]) - 1) != '/')
    {
        #ifdef _WIN32
            if(*(argv[1] + strlen(argv[1]) - 1) != '\\')
                strcat(spath, "/");
        #else
            strcat(spath, "/");
        #endif // _WIN32
    }
    if(!strcmp(argv[i], "-o"))
    {
        if(!argv[i + 1])
        {
            free(spath);
            return 1;
        }
        final_path = malloc(strlen(argv[i + 1]) + 2);
        strcpy(final_path, argv[i + 1]);
        if(*(argv[i + 1] + strlen(argv[i + 1]) - 1) != '/')
        {
            #ifdef _WIN32
                if(*(argv[i + 1] + strlen(argv[i + 1]) - 1) != '\\')
                    strcat(final_path, "/");
            #else
                strcat(final_path, "/");
            #endif // _WIN32
        }
        i+=2;
    }
    if(!(final_template = argv[i]) || !(final_width = convert_to_int(argv[i + 1])) || !(final_height = convert_to_int(argv[i + 2])) || !(final_length = convert_to_int(argv[i + 3])) || !check_tmplt(final_template, argv[i + 3]) || !(scaling_method = argv[i + 4]) || (strcmp(scaling_method, "NEIGHBOR") && strcmp(scaling_method, "LINEAR") && strcmp(scaling_method, "BICUBIC")) || argv[i + 5])
    {
        if(final_path != spath)
            free(final_path);
        free(spath);
        dispose();
        return 1;
    }
    if(!imgread(spath, init_template) || (!strcmp(scaling_method, "LINEAR") && init_width < 2) || (!strcmp(scaling_method, "BICUBIC") && (init_width < 3 || init_height < 3)))
    {
        if(final_path != spath)
            free(final_path);
        free(spath);
        dispose();
        return 2;
    }
    interpolate(scaling_method);
    if(!imgwrite(final_path, final_template))
    {
        if(final_path != spath)
            free(final_path);
        free(spath);
        dispose();
        return 3;
    }
    if(final_path != spath)
            free(final_path);
    free(spath);
    dispose();
    return 0;
}
