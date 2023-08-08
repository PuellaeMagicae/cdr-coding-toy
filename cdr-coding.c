#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>

typedef struct unused_struct *zsi_ref;
typedef uintptr_t zsi_bits;

typedef enum {
  ZSI_FIXNUM_TAG          = 0b010,
  ZSI_CONS_TAG     = 0b000,
  ZSI_MISC_OBJEC_TAG      = 0b110,
  ZSI_TYPE_TAG_MASK       = 0b111,
} zsi_bit_tag;

#define CDR_CODING_BIT 0b001

#define ZSI_PACK(x) ((zsi_ref) (x))
#define ZSI_UNPACK(x) ((zsi_bits) (x))

#define ZSI_EOL        (ZSI_PACK (0b110))

#define ZSI_MAKE_FIXNUM(x) (ZSI_PACK ((((x) << 3) | ZSI_FIXNUM_TAG)))

typedef struct zsi_pair {
  zsi_ref car;
  zsi_ref cdrs[];
} zsi_pair;

zsi_ref
zsi_cons (zsi_ref car, zsi_ref cdr)
{
  zsi_pair *cons;

  cons = (zsi_pair *) malloc (2 * sizeof (zsi_ref));
  cons->car = car;
  cons->cdrs[0] = cdr;
  return ZSI_PACK (cons);
}

zsi_ref
zsi_car (zsi_ref cons)
{
  zsi_bits bits = ZSI_UNPACK (cons);
  zsi_pair *p = (zsi_pair *) (bits & (~ZSI_TYPE_TAG_MASK));

  bits = ZSI_UNPACK (p->car) & ~0x1;
  return ZSI_PACK (bits);

}

zsi_ref
zsi_cdr (zsi_ref cons)
{
  zsi_bits bits = ZSI_UNPACK (cons);
  zsi_pair *p = (zsi_pair *) (bits & (~ZSI_TYPE_TAG_MASK));

  if ((ZSI_UNPACK (p->car) & 0x1) & (ZSI_UNPACK (p->cdrs[0]) & 0x1)) {
    return ZSI_PACK ((zsi_bits) p->cdrs);
  } else {
    return p->cdrs[0];
  }
}

zsi_ref
zsi_listn (zsi_ref *argv, size_t argc)
{
  size_t list_len = argc + 1;
  zsi_ref *val = malloc (list_len * sizeof (zsi_ref));
  zsi_bits bits = (zsi_bits) val;

  for (size_t i = 0; i < argc; i++) {
    zsi_bits bits = ZSI_UNPACK (argv[i]) | CDR_CODING_BIT;

    val[i] = ZSI_PACK (bits);
  }

  val[argc] = ZSI_EOL;

  return ZSI_PACK (bits);
}

zsi_ref
zsi_nthcdr (zsi_ref list, size_t num)
{
  for (size_t i = 0; i < num; list = zsi_cdr (list), i++);
  return list;
}

zsi_ref
zsi_list_ref (zsi_ref list, size_t num)
{
  return zsi_car (zsi_nthcdr (list, num));
}

zsi_ref
zsi_iota_cdr_coding (size_t count, int start)
{
  zsi_ref *nums = (zsi_ref *) malloc (sizeof (zsi_ref) * (1 + count));

  for (size_t i = 0; i < count; i++) {
    zsi_bits bits = ZSI_UNPACK (ZSI_MAKE_FIXNUM (start + i));
    bits += 0x1;
    nums[i] = ZSI_PACK (bits);
  }

  nums[count] = ZSI_EOL;
  return ZSI_PACK ((zsi_bits) nums);
}

zsi_ref
zsi_iota_normal (size_t count, int start)
{
  zsi_ref result = ZSI_EOL;
  size_t end = start + count - 1;

  for (intptr_t i = end; i >= start; i--) {
    result = zsi_cons (ZSI_MAKE_FIXNUM ((uintptr_t) i), result);
  }

  return result;
}

zsi_ref
zsi_iota_normal_compacted (size_t count, int start)
{
  zsi_ref *nums = (zsi_ref *) malloc (sizeof (zsi_ref) * 2 * count);
  zsi_ref tail = ZSI_EOL;

  for (int i = count - 1; i >= 0; i--) {
    size_t j = i * 2;
    nums[j] = ZSI_MAKE_FIXNUM ((uintptr_t) start + i);
    nums[j + 1] = tail;
    tail = ZSI_PACK ((zsi_bits) &nums[j]);
  }

  return ZSI_PACK ((zsi_bits) nums);
}


void
zsi_debug_print (zsi_ref value)
{
  zsi_bits bits = ZSI_UNPACK (value);
  zsi_bit_tag tag = bits & ZSI_TYPE_TAG_MASK;
  zsi_bits untagged_bits = bits & (~ZSI_TYPE_TAG_MASK);

  if (value == ZSI_EOL) {
    printf ("()");
  } else {
    switch (tag) {
    case ZSI_CONS_TAG:
      printf ("(");
      zsi_debug_print (zsi_car (value));
      printf (" . ");
      zsi_debug_print (zsi_cdr (value));
      printf (")");
      break;
    case ZSI_FIXNUM_TAG:
      printf ("%d", bits >> 3);
      break;
    default:
      abort ();
    }
  }
}


void
zsi_debug_println (zsi_ref value)
{
  zsi_debug_print (value);
  printf ("\n");
}
