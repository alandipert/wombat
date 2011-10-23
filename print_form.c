#include <stdio.h>
#include "types.h"
#include "print_form.h"

/* Printing *******************************/

void print_form(void* form) {
  switch(type(form)) {
  case INT:
    printf("%d", IVAL(form));
    break;
  case SYMBOL:
    printf("%s", SVAL(form));
    break;
  case SPECIAL:
    printf("#<special:%s>", ((Special*)form)->name);
    break;
  case FUNCTION:
    printf("#<function>");
    break;
  case CONS:
    printf("(");
    do {
      if(CAR(form)) {
        print_form(CAR(form));
        if(CDR(form)) {
          if(type(CDR(form)) == CONS) {
            if(CAR(CDR(form))) {
              printf(" ");
            }
          } else {
            printf(" . ");
            print_form(CDR(form));
            break;
          }
        }
      } else {
        /* printf("CAR was empty.\n"); */
      }
    } while((form = CDR(form)));
    printf(")");
    break;
  }
}
