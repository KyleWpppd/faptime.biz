#ifndef HAVE_FAPTIME_URLS_H
#define HAVE_FAPTIME_URLS_H

#define _FT_RFC3986_ALPHA							\
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ"                    \
    "abcdefghijklmnopqrstuvwxyz"

#define _FT_RFC3986_DIGIT "0123456789"
#define _FT_RFC3986_OTHER_UNRES "-._~"
#define _FT_RFC3986_SUBDELIMS "!$&\'()*+,;="
#define _FT_RFC3986_EXTRAS ":@"

#define _FT_RFC3986_UNRESERVED                                          \
    _FT_RFC3986_ALPHA                                                   \
    _FT_RFC3986_DIGIT                                                   \
    _FT_RFC3986_OTHER_UNRES

#define FAPTIME_ALPHA_URL_CHARS                 \
    _FT_RFC3986_ALPHA

#define FAPTIME_ALPHANUM_URL_CHARS                      \
    _FT_RFC3986_ALPHA                                   \
    _FT_RFC3986_DIGIT

#define FAPTIME_ALL_URL_CHARS                                       \
    _FT_RFC3986_UNRESERVED                                          \
    _FT_RFC3986_SUBDELIMS                                           \
    _FT_RFC3986_EXTRAS

#define FAPTIME_UNRESERVED_URL_CHARS            \
    _FT_RFC3986_UNRESERVED

#define FAPTIME_WILDTIMES_URL_CHARS                                 \
    _FT_RFC3986_OTHER_UNRES                                         \
    _FT_RFC3986_SUBDELIMS                                           \
    _FT_RFC3986_EXTRAS

#define FT_URL_GROUP_ALL "all"
#define FT_URL_GROUP_ALPHA "alphas"
#define FT_URL_GROUP_ALPHANUM "alphanums"
#define FT_URL_GROUP_UNRESERVED "unreserved"
#define FT_URL_GROUP_WILDTIMES "wildtimes"

/* Just a mapping to another category */
#define FT_URL_GROUP_DEFAULT "alphanums"
#define FAPTIME_DEFAULT_URL_CHARS FAPTIME_ALPHANUM_URL_CHARS


int faptime_url_is_named_group(char *group);
char *faptime_url_getgroup(char *groupkey);


#endif
