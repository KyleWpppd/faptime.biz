#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "faptime_urls.h"

struct faptime_url_group {
	char *name;
	char *whitelist;
};

struct faptime_url_group faptime_url_groups[] = {
	{.name=FT_URL_GROUP_ALL		   , .whitelist=FAPTIME_ALL_URL_CHARS},
	{.name=FT_URL_GROUP_ALPHA	   , .whitelist=FAPTIME_ALPHA_URL_CHARS},
	{.name=FT_URL_GROUP_ALPHANUM   , .whitelist=FAPTIME_ALPHANUM_URL_CHARS},
	{.name=FT_URL_GROUP_DEFAULT	   , .whitelist=FAPTIME_ALPHANUM_URL_CHARS},
	{.name=FT_URL_GROUP_UNRESERVED , .whitelist=FAPTIME_UNRESERVED_URL_CHARS},
	{.name=FT_URL_GROUP_WILDTIMES  , .whitelist=FAPTIME_WILDTIMES_URL_CHARS},
};

int faptime_url_is_named_group(char *group) {
	size_t len = sizeof(faptime_url_groups) / sizeof(faptime_url_groups[0]);
	assert(len == 6);
	for (size_t i = 0; i < len; i++) {
		if (strcmp(group, faptime_url_groups[i].name) == 0) {
			return 1;
		}
	}
	return 0;
}

char *faptime_url_getgroup(const char *group, char *defaultgrp) {
	size_t len = sizeof(faptime_url_groups) / sizeof(faptime_url_groups[0]);
	for (size_t i = 0; i < len; i++) {
		if (strcmp(group, faptime_url_groups[i].name) == 0) {
			return strdup(faptime_url_groups[i].whitelist);
		}
	}
	return defaultgrp;
}
