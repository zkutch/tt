#include <libintl.h>
#include <locale.h>

#define _(STRING) gettext(STRING)
#define PACKAGE "tt"

#define make_str(a) # a
#define STR(a) make_str(a)


#ifndef LOCALEDIR  // translation
#define LOCALEDIR "/usr/share/locale"
#endif


void i18n(void);
