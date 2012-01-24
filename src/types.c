#include <uberlisp/types.h>

void init_mem() {
  CSTART_p = CEND_p = UPTR(&__heap_start) - PTR_CACHE_SIZE;
  SSTART_p = SEND_p = UPTR(&__bss_end);

  memset(CPTR(SSTART_p), 0, TOTALMEM());
}

uptr_t build_cons(uptr_t car, uptr_t cdr) {
  if (FREEMEM() < sizeof(uptr_t)) {
    int offset = __GC__();
    if (IS_CONS(car) && car < CSTART_p) car += offset;
    if (IS_CONS(cdr) && cdr < CSTART_p) cdr += offset;
  }

  if (IS_PTR(cdr) && cdr == CSTART_p) {
    CSTART_p -= sizeof(uptr_t);
    *UPTR_PTR(CSTART_p) = car;
    *UPTR_PTR(CSTART_p + sizeof(uptr_t)) |= CADR_FLAG;
    CRECENT_p = CSTART_p;
    return CSTART_p;
  } 

  CSTART_p -= sizeof(Cons);
  Cons *new_cons = CONS_PTR(CSTART_p);
  new_cons->car = car;
  new_cons->cdr = cdr;
  CRECENT_p = CSTART_p;
  return UPTR(new_cons);
}

void __mk_sym(uint32_t s) {
  SVAL(SEND_p) = s;
  SEND_p += 4;
}

void __set_env(uptr_t env) {
  ENV_p = env;
}

uptr_t build_symbol(char *name) {
  if (FREEMEM() < sizeof(uint32_t)) __GC__();

  SVAL(SEND_p) = hash_sym(name);

  uptr_t finder = SSTART_p;

  while (SVAL(finder) != SVAL(SEND_p)) finder += 4;

  if (finder == SEND_p)
    SEND_p += 4;
  else
    SVAL(SEND_p) = 0;
  
  return finder;
}

uint32_t hash_sym(char *name) {
  int len = strlen(name);
  if (len > 6) len = 6;

  uint32_t hash = 0;

  if (len <= 4) {
    int i;
    for (i = 0; i < len; i++)
      ((char*)&hash)[i] = (char)toupper(name[i]);
    
    hash |= LIT_SYM_FLAG;
  } else {
    int i;
    for (i = 0; i < len; ++i) {
      char cur = name[i];

      if (isalpha(cur))
        hash |= ((uint32_t)((char)toupper(cur) - 'A' + 1)) << (5*i);
      else
        hash |= ((uint32_t)USCORE_HSH) << (5*i);
    }
  }

  return hash;
}

void unhash_sym(char *buf, uptr_t sym_p) {
  uint32_t hash = *SYM_PTR(sym_p);

  if (hash & LIT_SYM_FLAG) {
    hash &= ~LIT_SYM_FLAG;
    strncpy(buf, (char *)&hash, 4);
  } else {
    char *cur = buf;
    while(hash) {
      *cur = (char)(hash & ((uint32_t)0x1F));
      if (*cur == USCORE_HSH)
        *cur = '_';
      else
        *cur = *cur - 1 + 'A';

      hash >>= 5;
      ++cur;
    }
  }
}
