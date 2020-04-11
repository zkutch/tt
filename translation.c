#include "translation.h"
// #include <stdio.h>
void i18n(void)
{
    setlocale(LC_MESSAGES,"");
    setlocale(LC_ALL, "");        
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);
//     printf("\nTranslations in %s\n", LOCALEDIR);
}
