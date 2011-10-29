#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>

void * const MPOOL;
void * const MSTART_p;
void * const MEND_p;
void * HEAP_p;

#define SET_CONSTVP(ptr, val) (*(void **)&ptr) = (val)

#define INT2_T (uint8_t)0x20
#define INT4_T (uint8_t)0x40
#define INT8_T (uint8_t)0x80

#define SET_INT_T(ptr, type) (*(uint8_t*)ptr) = type
#define MV_HEAP(bytes) (*(uint8_t**)&HEAP_p) += bytes

typedef struct {
  short val_p;
} Object;

void *store_int(long value) {
  void *new_int_p = HEAP_p;
  if (value <= SHRT_MAX && value >= SHRT_MIN) {
    SET_INT_T(HEAP_p, INT2_T);
    MV_HEAP(1);
    *(short*)HEAP_p = (short)value;
    MV_HEAP(sizeof(short));
  } else if (value <= INT_MAX && value >= INT_MIN) {
    SET_INT_T(HEAP_p, INT4_T);
    MV_HEAP(1);
    *(int*)HEAP_p = (int)value;
    MV_HEAP(sizeof(int));
  } else {
    SET_INT_T(HEAP_p, INT8_T);
    MV_HEAP(1);
    *(long*)HEAP_p = value;
    MV_HEAP(sizeof(long));
  }

  return new_int_p;
}

#define IS_INT2(int_p) (*(uint8_t*)(int_p) & INT2_T)
#define IS_INT4(int_p) (*(uint8_t*)(int_p) & INT4_T)
#define IS_INT8(int_p) (*(uint8_t*)(int_p) & INT8_T)

long read_int(void *int_p, uint8_t **cursor) {
  void *val_p = (uint8_t*)int_p + 1;
  if (IS_INT2(int_p)) {
    *cursor += sizeof(uint8_t) + sizeof(short);
    return (long)(*(short*)val_p);
  }
  if (IS_INT4(int_p)) {
    *cursor += sizeof(uint8_t) + sizeof(int);
    return (long)(*(int*)val_p);
  }
  *cursor += sizeof(uint8_t) + sizeof(long);
  return *(long*)val_p;
}

void print_ints(void *list) {
  uint8_t *cursor = list;
  while(*cursor) {
    printf("%li\n", read_int(cursor, &cursor));
  }
}

int main() {
  SET_CONSTVP(MPOOL, malloc(0x900));
  SET_CONSTVP(MSTART_p, MPOOL + 0x100);
  SET_CONSTVP(MEND_p, MPOOL + 0x900);
  HEAP_p = MSTART_p;

  memset(MSTART_p, 0, 0x800);

  void *sp1 = store_int(0);
  void *sp2 = store_int(-1);
  void *sp3 = store_int(SHRT_MAX);
  void *sp4 = store_int(SHRT_MIN);

  void *ip1 = store_int(SHRT_MAX + 1);
  void *ip2 = store_int(SHRT_MIN - 1);
  void *ip3 = store_int(INT_MAX);
  void *ip4 = store_int(INT_MIN);

  void *lp1 = store_int((long)INT_MAX + 1);
  void *lp2 = store_int((long)INT_MIN - 1);
  void *lp3 = store_int(LONG_MAX);
  void *lp4 = store_int(LONG_MIN);

  /*
  uint8_t *cursor = sp1;
  printf("%li should be 0\n", read_int(sp1, &cursor));
  printf("%li should be -1\n", read_int(sp2, &cursor));
  printf("%li should be %d\n", read_int(sp3, &cursor), SHRT_MAX);
  printf("%li should be %d\n", read_int(sp4, &cursor), SHRT_MIN);
  printf("%li should be %d\n", read_int(ip1, &cursor), SHRT_MAX+1);
  printf("%li should be %d\n", read_int(ip2, &cursor), SHRT_MIN-1);
  printf("%li should be %d\n", read_int(ip3, &cursor), INT_MAX);
  printf("%li should be %d\n", read_int(ip4, &cursor), INT_MIN);
  printf("%li should be %li\n", read_int(lp1, &cursor), (long)INT_MAX+1);
  printf("%li should be %li\n", read_int(lp2, &cursor), (long)INT_MIN-1);
  printf("%li should be %li\n", read_int(lp3, &cursor), LONG_MAX);
  printf("%li should be %li\n", read_int(lp4, &cursor), LONG_MIN);
  */
  print_ints(sp1);

  return 0;
}
