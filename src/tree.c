#include <stdio.h>
#include <sys/types.h>
#define __USE_MISC
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

typedef uint8_t SETTINGS_T;
enum SETTINGS
{
	SHOW_HIDDEN = 0x01,
};

char *join_str(char *prefix, char *suffix, char* SEP, int SEP_LEN)
{
	size_t prefix_len = strlen(prefix);
	size_t suffix_len = strlen(suffix);

	char *begin = malloc(prefix_len + suffix_len + SEP_LEN + 1);
	assert(begin != NULL);

	char *end = begin;
    memcpy(end, prefix, prefix_len);
    end += prefix_len;
    memcpy(end, SEP, SEP_LEN);
    end += SEP_LEN;
    memcpy(end, suffix, suffix_len);
    end += suffix_len;
    *end = '\0';

	return begin;
}

void recurse_dir(char *dir_path, char *indent, SETTINGS_T settings)
{
	DIR *dir = opendir(dir_path);
	
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		if (ent->d_name[0] == '.' && (settings & SHOW_HIDDEN) == 0) continue; 
		switch (ent->d_type)
		{
		case DT_DIR:
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
			printf("%s%s/\n", indent, ent->d_name);
			
			char *new_path = join_str(dir_path, ent->d_name, "/", 1);
			char *new_indent = join_str(indent, "____", "", 0);
			
			recurse_dir(new_path, new_indent, settings);
			
			free(new_path);
			free(new_indent);
			break;
		default:
			printf("%s%s\n", indent, ent->d_name);
			break;
		}
	}

	closedir(dir);
}

SETTINGS_T arg_parse(int argc, char **argv)
{
	SETTINGS_T settings = 0;
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-h") == 0)
			settings |= SHOW_HIDDEN;
	}
	return settings;
}

int main(int argc, char **argv)
{
	int settings = arg_parse(argc, argv);
	recurse_dir(".", "", settings);
}
