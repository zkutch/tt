#include <libintl.h>
#include <locale.h>

#define _(STRING) gettext(STRING)
#define PACKAGE "tt"

#ifndef LOCALEDIR   // translation
#define LOCALEDIR "/usr/share/locale"
#endif


void i18n(void);
