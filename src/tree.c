#include <stdio.h>
#define __USE_MISC
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

enum SETTINGS_E {
  SHOW_HIDDEN = 0x01 << 0,
};

typedef struct IGNORE_LIST {
	char *name;
	struct IGNORE_LIST *next;
} IGNORE_LIST;

typedef uint8_t SETTINGS_T;
typedef struct {
	SETTINGS_T settings;
	IGNORE_LIST *ignore;
} SETTINGS;

char *join_str(char *prefix, char *suffix, char* SEP, int SEP_LEN) {
	size_t prefix_len = strlen(prefix);
	size_t suffix_len = strlen(suffix);

	char *begin = malloc(prefix_len + suffix_len + SEP_LEN + 1);
	assert("Failed to allocate begin" && begin != NULL);

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

int check_ignore(char *input, SETTINGS settings) {

	IGNORE_LIST *ignored = settings.ignore;

	while (ignored != NULL) {
		if (strcmp(ignored->name, input) == 0) {
			return 1;
		}
		ignored = ignored->next;
	}
	
	return 0;
}

void recurse_dir(char *dir_path, char *indent, SETTINGS settings) {
	DIR *dir = opendir(dir_path);
	
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL) {
		// Ignore files that start with . but show them if SHOW_HIDDEN is enabled
		if ((settings.settings & SHOW_HIDDEN) == 0
			&& ent->d_name[0] == '.') continue;

		// Ignore files specified to be ignored
		if (check_ignore(ent->d_name, settings) == 1) continue;
		
		switch (ent->d_type) {
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

IGNORE_LIST *make_ignore_node (char *name, IGNORE_LIST *next) {
	IGNORE_LIST *out = malloc(sizeof(IGNORE_LIST));
	out->name = name;
	out->next = next;
	return out;
}

SETTINGS make_settings(SETTINGS_T settings, IGNORE_LIST *ignore) {
	SETTINGS out;
	out.settings = settings;
	out.ignore = ignore;
	return out;
}

SETTINGS arg_parse(int argc, char **argv) {
	SETTINGS_T settings = 0;
	IGNORE_LIST *ignore = NULL;
	
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0) {
			settings |= SHOW_HIDDEN;
		}
		else if (strcmp(argv[i], "-i") == 0) {
			assert("No path after -i" && ++i < argc);
			ignore = make_ignore_node(argv[i], ignore);
		}
		else {
			printf("Unknown argument: %s\n", argv[i]);
			exit(1);
		}
	}
	return make_settings(settings, ignore);
}

int main(int argc, char **argv) {
	SETTINGS settings = arg_parse(argc, argv);
	recurse_dir(".", "", settings);

	// Free memory
	
	IGNORE_LIST *ignored = settings.ignore;
	while (ignored != NULL) {
		IGNORE_LIST *x = ignored;
		ignored = ignored->next;
		free(x);
	}
	
}
