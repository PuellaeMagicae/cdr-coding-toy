#include "cdr-coding.c"
#include "ubench.h"


volatile zsi_ref temp;


UBENCH_EX(cdr_coding_test, cdr_coding_nthcdr) {
  zsi_ref big_cons = zsi_iota_cdr_coding (10000, 0);

  UBENCH_DO_BENCHMARK() {
    for (size_t i = 5000; i < 5001; i++) {
      temp = zsi_nthcdr (big_cons, i);
    }
  }
}


UBENCH_EX(cdr_coding_test, normal_compacted_nthcdr) {
  zsi_ref big_cons = zsi_iota_normal_compacted (10000, 0);

  UBENCH_DO_BENCHMARK() {
    for (size_t i = 5000; i < 5001; i++) {
      temp = zsi_nthcdr (big_cons, i);
      /* zsi_debug_println (zsi_nthcdr (big_cons, i)); */
    }
  }
}

UBENCH_EX(cdr_coding_test, normal_nthcdr) {
  zsi_ref big_cons = zsi_iota_normal (10000, 0);

  UBENCH_DO_BENCHMARK() {
    for (size_t i = 5000; i < 5001; i++) {
      temp = zsi_nthcdr (big_cons, i);
      /* zsi_debug_println (zsi_nthcdr (big_cons, i)); */
    }
  }
}

UBENCH_MAIN()
