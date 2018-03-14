#include "../rope.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* void* calloc_(size_t num, size_t size) { */
/*     void* ptr = calloc(num, size); */
/*     printf("%p = calloc(%d, %d)\n", ptr, (int)num, (int)size); */
/*     return ptr; */
/* } */

/* void* malloc_(size_t size) { */
/*     void* ptr = malloc(size); */
/*     printf("%p = malloc(%d)\n", ptr, (int)size); */
/*     return ptr; */
/* } */

/* void free_(void* ptr) { */
/*     printf("free(%p)\n", ptr); */
/*     free(ptr); */
/* } */

/* #define calloc calloc_ */
/* #define malloc malloc_ */
/* #define free free_ */

#ifdef TEST_MODE
#define LEAF_SIZE ((size_t)3)
#else
#define LEAF_SIZE ((size_t)32)
#endif

#define TRY(code, label)                        \
    do {                                        \
        if ((code) < 0) {                       \
            goto label;                         \
        }                                       \
    } while (0)
#define TRY_MALLOC(code, label)                 \
    do {                                        \
        if (!(code)) {                          \
            goto label;                         \
        }                                       \
    } while(0)
#define UNIMPLEMENTED()                                 \
    do {                                                \
        fprintf(stderr, "%s:%d: Unimplemented\n",       \
                __FILE__, __LINE__);                    \
        abort();                                        \
    } while (0)
#define UNTESTED()                              \
    do {                                        \
        fprintf(stderr, "%s:%d: Untested\n",    \
                __FILE__, __LINE__);            \
    } while (0)
#define PANIC()                                 \
    do {                                        \
        fprintf(stderr, "%s:%d: Panic\n",       \
                __FILE__, __LINE__);            \
        abort();                                \
    } while (0)

typedef struct Leaf Leaf;
typedef struct Branch Branch;

enum RopeType { LeafType, BranchType };
typedef enum RopeType RopeType;

struct Leaf {
    char data[LEAF_SIZE];
};

struct Branch {
    int balance;
    size_t rank;
    Rope* left;
    Rope* right;
};

struct Rope {
    RopeType type;
    union {
        Leaf leaf;
        Branch branch;
    } data;
};

void Rope_Destroy(Rope* rope) {
    if (rope) {
        if (rope->type == BranchType) {
            Rope_Free(rope->data.branch.left);
            Rope_Free(rope->data.branch.right);
        }
    }
}
void Rope_Free(Rope* rope) {
    Rope_Destroy(rope);
    free(rope);
}

Rope* Rope_New(void) {
    return calloc(1, sizeof(Rope));
}

static Leaf Leaf_New(const char* data, size_t len) {
    Leaf leaf;
    memcpy(leaf.data, data, len);
    if (len != LEAF_SIZE) {
        leaf.data[len] = '\0';
    }
    return leaf;
}

static ptrdiff_t Rope_FromUnwrapped(Rope* rope, const char* data, size_t len) {
    if (len > LEAF_SIZE) {
        ptrdiff_t left_height;
        ptrdiff_t right_height;
        Rope* left;
        Rope* right;
        TRY_MALLOC(left = malloc(sizeof(Rope)), ret);
        TRY_MALLOC(right = malloc(sizeof(Rope)), left);
        TRY(left_height = Rope_FromUnwrapped(left, data, len / 2), right);
        TRY(right_height = Rope_FromUnwrapped(right, data + len / 2, (len + 1) / 2), left_tree);
        rope->type = BranchType;
        rope->data.branch.balance = right_height - left_height;
        rope->data.branch.rank = len / 2;
        rope->data.branch.left = left;
        rope->data.branch.right = right;
        if (right_height - left_height > 1) {
            UNIMPLEMENTED();
        } else if (left_height - right_height > 1) {
            UNIMPLEMENTED();
        }
        if (right_height > left_height) {
            return right_height + 1;
        } else {
            return left_height + 1;
        }
left_tree:
        Rope_Destroy(left);
right:
        free(right);
left:
        free(left);
ret:
        return -1;
    } else {
        rope->type = LeafType;
        rope->data.leaf = Leaf_New(data, len);
        return 0;
    }
}

Rope* Rope_From(const char* data, size_t len) {
    Rope* rope = malloc(sizeof(Rope));
    if (rope) {
        if (Rope_FromUnwrapped(rope, data, len) < 0) {
            free(rope);
            return 0;
        }
    }
    return rope;
}

Rope* Rope_FromStr(const char* data) {
    return Rope_From(data, strlen(data));
}

size_t Rope_Height(const Rope* rope) {
    if (rope->type == LeafType) {
        return 0;
    } else if (rope->data.branch.balance == 1) {
        return 1 + Rope_Height(rope->data.branch.right);
    } else {
        return 1 + Rope_Height(rope->data.branch.left);
    }
}

static size_t Leaf_Len(const Leaf* leaf) {
    const char* end = memchr(leaf->data, '\0', LEAF_SIZE);
    if (end) {
        return end - leaf->data;
    } else {
        return LEAF_SIZE;
    }
}

size_t Rope_Size(const Rope* rope) {
    if (rope->type == LeafType) {
        return Leaf_Len(&rope->data.leaf);
    } else {
        return rope->data.branch.rank + Rope_Size(rope->data.branch.right);
    }
}

static ptrdiff_t Rope_DoubleRight(Rope* t) {
    /*     ll H
     *   l/
     *       lrl H
     *     lr|
     *       lrr H
     * tX
     *   r H
     *
     *     ll H
     *   L=l|
     *     lrl H
     * T=t|
     *     lrr H
     *   R=lr|
     *     r H
     */
    Rope* l;
    Rope* lr;
    Branch new_top;
    Branch new_left;
    Branch new_right;
    UNTESTED();
    l = t->data.branch.left;
    lr = l->data.branch.right;

    if (lr->data.branch.balance != 0) {
        UNIMPLEMENTED();
    }

    new_top.rank = l->data.branch.rank + lr->data.branch.rank;
    new_top.left = l;
    new_top.right = lr;
    new_top.balance = 0;

    new_left.rank = l->data.branch.rank;
    new_left.left = l->data.branch.left;
    new_left.right = lr->data.branch.left;
    new_left.balance = 0;

    new_right.rank = t->data.branch.rank - l->data.branch.rank - lr->data.branch.rank;
    new_right.left = lr->data.branch.right;
    new_right.right = t->data.branch.right;
    new_right.balance = 0;

    t->data.branch = new_top;
    l->data.branch = new_left;
    lr->data.branch = new_right;

    return 0;
}

static ptrdiff_t Rope_SingleRight(Rope* t) {
    ptrdiff_t ret;
    Branch new_top;
    Branch new_right;
    Rope* l;
    l = t->data.branch.left;

    new_top.rank = l->data.branch.rank;
    new_top.left = l->data.branch.left;
    new_top.right = l;

    new_right.rank = t->data.branch.rank - l->data.branch.rank;
    new_right.left = l->data.branch.right;
    new_right.right = t->data.branch.right;

    if (l->data.branch.balance == -1) {
        /*     ll H
         *   l/
         *     lr H-1
         * tX
         *   r H-1
         *
         *   ll H
         * T=t|
         *     lr H-1
         *   R=l|
         *     r H-1
         */
        new_top.balance = 0;
        new_right.balance = 0;
        ret = 0;
    } else {
        /*     ll H
         *   l|
         *     lr H
         * tX
         *   r H-1
         *
         *   ll H
         * T=t\\
         *     lr H
         *   R=l/
         *     r H-1
         */
        new_top.balance = 1;
        new_right.balance = -1;
        ret = 1;
    }

    t->data.branch = new_top;
    l->data.branch = new_right;
    return ret;
}

static ptrdiff_t Rope_DoubleLeft(Rope* t) {
    /*   l H
     * tX
     *       rll H
     *     rl|
     *       rlr H
     *   r/
     *     rr H
     *
     *     l H
     *   L=rl|
     *     rll H
     * T=t|
     *     rlr H
     *   R=r|
     *     rr H
     */
    Rope* r;
    Rope* rl;
    Branch new_top;
    Branch new_left;
    Branch new_right;
    r = t->data.branch.right;
    rl = r->data.branch.left;

    if (rl->data.branch.balance != 0) {
        UNIMPLEMENTED();
    }

    new_top.rank = t->data.branch.rank + rl->data.branch.rank;
    new_top.left = rl;
    new_top.right = r;
    new_top.balance = 0;

    new_left.rank = t->data.branch.rank;
    new_left.left = t->data.branch.left;
    new_left.right = rl->data.branch.left;
    new_left.balance = 0;

    new_right.rank = r->data.branch.rank - rl->data.branch.rank;
    new_right.left = rl->data.branch.right;
    new_right.right = r->data.branch.right;
    new_right.balance = 0;

    t->data.branch = new_top;
    rl->data.branch = new_left;
    r->data.branch = new_right;

    return 0;
}

static ptrdiff_t Rope_SingleLeft(Rope* t) {
    ptrdiff_t ret;
    Branch new_top;
    Branch new_left;
    Rope* r;
    r = t->data.branch.left;

    new_top.rank = r->data.branch.rank + t->data.branch.rank;
    new_top.left = r;
    new_top.right = r->data.branch.right;

    new_left.rank = t->data.branch.rank;
    new_left.left = t->data.branch.left;
    new_left.right = r->data.branch.left;

    if (r->data.branch.balance == -1) {
        /*   l H-1
         * tX
         *     rl H-1
         *   r/
         *     rr H
         *
         *     l H-1
         *   L=r|
         *     rl H-1
         * T=t|
         *   rr H
         */
        new_top.balance = 0;
        new_left.balance = 0;
        ret = 0;
    } else {
        /*   l H-1
         * tX
         *     rl H
         *   r|
         *     rr H
         *
         *     l H-1
         *   L\\
         *     rl H
         * T/
         *   rr H
         */
        new_top.balance = -1;
        new_left.balance = 1;
        ret = 1;
    }

    t->data.branch = new_top;
    r->data.branch = new_left;
    return ret;
}

static ptrdiff_t Rope_RebalanceRight(Rope* rope, ptrdiff_t difference) {
    ptrdiff_t rem_difference = 0;
    while (--difference >= 0) {
        if (rope->data.branch.balance == -1) {
            if (rope->data.branch.left->data.branch.balance == 1) {
                rem_difference += Rope_DoubleRight(rope);
            } else {
                rem_difference += Rope_SingleRight(rope);
            }
        } else if (rope->data.branch.balance == 0) {
            --rope->data.branch.balance;
            ++rem_difference;
        } else {
            --rope->data.branch.balance;
        }
    }
    return rem_difference;
}

static ptrdiff_t Rope_RebalanceLeft(Rope* rope, ptrdiff_t difference) {
    ptrdiff_t rem_difference = 0;
    while (--difference >= 0) {
        if (rope->data.branch.balance == 1) {
            if (rope->data.branch.right->data.branch.balance == -1) {
                rem_difference += Rope_DoubleLeft(rope);
            } else {
                rem_difference += Rope_SingleLeft(rope);
            }
        } else if (rope->data.branch.balance == 0) {
            ++rope->data.branch.balance;
            ++rem_difference;
        } else {
            ++rope->data.branch.balance;
        }
    }
    return rem_difference;
}

static ptrdiff_t Rope_Rebalance(Rope* rope, ptrdiff_t difference) {
    if (difference > 0) {
        return Rope_RebalanceLeft(rope, difference);
    } else {
        return Rope_RebalanceRight(rope, difference);
    }
}

static ptrdiff_t Rope_InsertShort(Rope* rope, size_t pos, const char* data, size_t len) {
    if (rope->type == LeafType) {
        char array[LEAF_SIZE * 2];
        size_t leaf_len;
        leaf_len = Leaf_Len(&rope->data.leaf);
        if (pos > leaf_len) {
            return -1;
        }
        memcpy(array, rope->data.leaf.data, pos);
        memcpy(array + pos, data, len);
        memcpy(array + pos + len, rope->data.leaf.data + pos, leaf_len - pos);
        return Rope_FromUnwrapped(rope, array, leaf_len + len);
    } else if (pos <= rope->data.branch.rank) {
        ptrdiff_t res = Rope_InsertShort(rope->data.branch.left, pos, data, len);
        if (res >= 0) {
            rope->data.branch.rank += len;
            return Rope_RebalanceRight(rope, res);
        } else {
            return -1;
        }
    } else {
        ptrdiff_t res = Rope_InsertShort(rope->data.branch.right, pos - rope->data.branch.rank, data, len);
        if (res >= 0) {
            return Rope_RebalanceLeft(rope, res);
        } else {
            return -1;
        }
    }
}

static char* Rope_ToString(const Rope* rope, char* out) {
    if (rope->type == LeafType) {
        size_t len = Leaf_Len(&rope->data.leaf);
        memcpy(out, rope->data.leaf.data, len);
        return out + len;
    } else {
        return Rope_ToString(rope->data.branch.right,
                             Rope_ToString(rope->data.branch.left,
                                           out));
    }
}

static char* Rope_ToStringBefore(const Rope* rope, size_t pos, const char* data, size_t data_len, char* out) {
    if (rope->type == LeafType) {
        size_t len = Leaf_Len(&rope->data.leaf);
        if (pos > len) {
            return 0;
        }
        memcpy(out, rope->data.leaf.data, pos);
        memcpy(out + pos, data, data_len);
        memcpy(out + pos + data_len, rope->data.leaf.data + pos, len - pos);
        return out + len + data_len;
    } else if (pos <= rope->data.branch.rank) {
        out = Rope_ToStringBefore(rope->data.branch.left, pos, data, data_len, out);
        if (!out) { PANIC(); }
        return Rope_ToString(rope->data.branch.right, out);
    } else {
        return Rope_ToStringBefore(rope->data.branch.right, pos - rope->data.branch.rank, data, data_len,
                                   Rope_ToString(rope->data.branch.left, out));
    }
}

static ptrdiff_t Rope_InsertLong(Rope* rope, size_t rope_height, size_t rope_size, size_t pos,
                                 const char* data, size_t len, size_t desired_height) {
    if (rope_height <= desired_height) {
        ptrdiff_t new_height;
        char* new_data;
        if (!(new_data = malloc(rope_size + len))) { return -1; }
        if (!Rope_ToStringBefore(rope, pos, data, len, new_data)) { goto new_data; }
        {
            Rope backup = *rope;
            if ((new_height = Rope_FromUnwrapped(rope, new_data, rope_size + len)) < 0) {
                goto new_data;
            }
            Rope_Destroy(&backup);
        }
        free(new_data);
        return new_height - (ptrdiff_t) rope_height;
new_data:
        free(new_data);
        return -1;
    } else if (rope->type == LeafType) {
        /* this can be taken out later once debug is done */
        PANIC();
    } else if (pos <= rope->data.branch.rank) {
        ptrdiff_t new_height;
        if (rope->data.branch.balance > 0) {
            rope_height -= 2;
        } else {
            --rope_height;
        }
        rope_size = rope->data.branch.rank;
        if ((new_height = Rope_InsertLong(rope->data.branch.left, rope_height, rope_size, pos,
                                          data, len, desired_height)) < 0) {
            return -1;
        }
        return Rope_RebalanceRight(rope, new_height);
    } else {
        ptrdiff_t new_height;
        if (rope->data.branch.balance < 0) {
            rope_height -= 2;
        } else {
            --rope_height;
        }
        rope_size -= rope->data.branch.rank;
        pos -= rope->data.branch.rank;
        if ((new_height = Rope_InsertLong(rope->data.branch.right, rope_height, rope_size, pos,
                                          data, len, desired_height)) < 0) {
            return -1;
        }
        return Rope_RebalanceLeft(rope, new_height);
    }
}

int Rope_Insert(Rope* rope, size_t pos, const char* data, size_t len) {
    if (len <= LEAF_SIZE) {
        if (Rope_InsertShort(rope, pos, data, len) < 0) {
            return -1;
        } else {
            return 0;
        }
    } else {
        size_t desired_height = 0;
        while (len > /* max size at height */ (LEAF_SIZE << desired_height)) {
            ++desired_height;
        }
        // here desired_height is the height of the tree produced by Rope_From(data, len).
        // thus we look (depth first in direction of pos) for a place to create a new tree with height desired_height + 1.
        if (Rope_InsertLong(rope, Rope_Height(rope), Rope_Size(rope), pos, data, len, desired_height) < 0) {
            return -1;
        } else {
            return 0;
        }
    }
}

int Rope_InsertStr(Rope* rope, size_t pos, const char* string) {
    if (string) {
        return Rope_Insert(rope, pos, string, strlen(string));
    } else {
        return -1;
    }
}

#ifdef TEST_MODE
#include "test.h"

static void Rope_DebugAssert_(const Rope* rope, size_t* height, size_t* size) {
    
}

static void Rope_DebugAssert(const Rope* rope) {
    size_t height;
    size_t size;
    Rope_DebugAssert_(rope, &height, &size);
}

static char* rope_string = 0;
static size_t rope_string_capacity = 0;
static size_t Rope_ToStringDebugConcat(size_t offset, const char* data, size_t len) {
    while (rope_string_capacity < offset + len + 1) {
        char* new_string;
        if (rope_string_capacity < 32) { rope_string_capacity = 32; }
        new_string = malloc(rope_string_capacity * 2);
        if (!new_string) { PANIC(); }
        memcpy(new_string, rope_string, offset);
        free(rope_string);
        rope_string = new_string;
        rope_string_capacity *= 2;
    }
    memcpy(rope_string + offset, data, len);
    offset += len;
    rope_string[offset] = '\0';
    return offset;
}

static size_t Rope_ToStringDebug(size_t offset, Rope* rope, size_t depth) {
    if (rope->type == LeafType) {
        size_t i;
        offset = Rope_ToStringDebugConcat(offset, "\n", 1);
        for (i = 0; i < depth; ++i) {
            offset = Rope_ToStringDebugConcat(offset, "  ", 2);
        }
        return Rope_ToStringDebugConcat(offset, rope->data.leaf.data, Leaf_Len(&rope->data.leaf));
    } else {
        size_t i;
        char balance;
        offset = Rope_ToStringDebug(offset, rope->data.branch.left, depth + 1);
        offset = Rope_ToStringDebugConcat(offset, "\n", 1);
        for (i = 0; i < depth; ++i) {
            offset = Rope_ToStringDebugConcat(offset, "  ", 2);
        }
        switch (rope->data.branch.balance) {
        case -1:
            balance = '/';
            break;
        case 0:
            balance = '|';
            break;
        case 1:
            balance = '\\';
            break;
        default:
            PANIC();
            break;
        }
        offset = Rope_ToStringDebugConcat(offset, &balance, 1);
        return Rope_ToStringDebug(offset, rope->data.branch.right, depth + 1);
    }
}

#define ASSERT_ROPE_EQ(expected_string, rope, label)                    \
    do {                                                                \
        Rope_ToStringDebug(0, rope, 0);                                 \
        if (strcmp(expected_string, rope_string) == 0) {                \
            ++successes_assert;                                         \
        } else {                                                        \
            if (_ret == 0) {                                            \
                PRINT_TEST_FAILED();                                    \
            }                                                           \
            fprintf(stderr, "%s:%d: Assertion failed.  Expected \"%s\" but found \"%s\"\n", \
                    __FILE__, __LINE__, expected_string, rope_string);  \
            _ret = 1;                                                   \
            goto label;                                                 \
        }                                                               \
    } while (0)

TEST(Test_Rope_New) {
    Rope* rope;
    rope = Rope_New();
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
", rope, cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_From_1) {
    Rope* rope;
    rope = Rope_FromStr("as");
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
as", rope, cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_From_2) {
    Rope* rope;
    rope = Rope_FromStr("asdf");
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
  as\n\
|\n\
  df", rope, cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_Insert_1) {
    Rope* rope;
    rope = Rope_FromStr("asdf");
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
  as\n\
|\n\
  df", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 3, "g"), cleanup);
    ASSERT_ROPE_EQ("\n\
  as\n\
|\n\
  dgf", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 4, "h"), cleanup);
    ASSERT_ROPE_EQ("\n\
  as\n\
\\\n\
    dg\n\
  |\n\
    hf", rope, cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_Insert_2) {
    Rope* rope;
    rope = Rope_FromStr("asdf");
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
  as\n\
|\n\
  df", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 0, "g"), cleanup);
    ASSERT_ROPE_EQ("\n\
  gas\n\
|\n\
  df", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 0, "h"), cleanup);
    ASSERT_ROPE_EQ("\n\
    hg\n\
  |\n\
    as\n\
/\n\
  df", rope, cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_Insert_3) {
    Rope* rope;
    rope = Rope_FromStr("asdf");
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
  as\n\
|\n\
  df", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 2, "g"), cleanup);
    ASSERT_ROPE_EQ("\n\
  asg\n\
|\n\
  df", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 3, "h"), cleanup);
    ASSERT_ROPE_EQ("\n\
    as\n\
  |\n\
    gh\n\
/\n\
  df", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 5, "jk"), cleanup);
    ASSERT_ROPE_EQ("\n\
    as\n\
  |\n\
    gh\n\
|\n\
    dj\n\
  |\n\
    kf", rope, cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_Insert_4) {
    Rope* rope;
    rope = Rope_FromStr("abcdefghijklmnop");
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
      ab\n\
    |\n\
      cd\n\
  |\n\
      ef\n\
    |\n\
      gh\n\
|\n\
      ij\n\
    |\n\
      kl\n\
  |\n\
      mn\n\
    |\n\
      op", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 5, "qrs"), cleanup);
    ASSERT_ROPE_EQ("\n\
      ab\n\
    |\n\
      cd\n\
  \\\n\
        eq\n\
      |\n\
        rsf\n\
    /\n\
      gh\n\
/\n\
      ij\n\
    |\n\
      kl\n\
  |\n\
      mn\n\
    |\n\
      op", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 11, "tuv"), cleanup);
    ASSERT_ROPE_EQ("\n\
      ab\n\
    |\n\
      cd\n\
  \\\n\
        eq\n\
      |\n\
        rsf\n\
    |\n\
        gh\n\
      |\n\
        tuv\n\
/\n\
      ij\n\
    |\n\
      kl\n\
  |\n\
      mn\n\
    |\n\
      op", rope, cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_Insert_5) {
    Rope* rope;
    rope = Rope_FromStr("abcd");
    ASSERT(rope, cleanup);
    ASSERT_ROPE_EQ("\n\
  ab\n\
|\n\
  cd", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 2, "e"), cleanup);
    ASSERT_ROPE_EQ("\n\
  abe\n\
|\n\
  cd", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 2, "fg"), cleanup);
    ASSERT_ROPE_EQ("\n\
    ab\n\
  |\n\
    fge\n\
/\n\
  cd", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 1, "hi"), cleanup);
    ASSERT_ROPE_EQ("\n\
    ah\n\
  |\n\
    ib\n\
|\n\
    fge\n\
  |\n\
    cd", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 9, "jk"), cleanup);
    ASSERT_ROPE_EQ("\n\
    ah\n\
  |\n\
    ib\n\
\\\n\
    fge\n\
  \\\n\
      cd\n\
    |\n\
      jk", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 8, "lm"), cleanup);
    ASSERT_ROPE_EQ("\n\
    ah\n\
  |\n\
    ib\n\
\\\n\
      fge\n\
    |\n\
      cl\n\
  |\n\
      md\n\
    |\n\
      jk", rope, cleanup);

    /* If there was no rebalance here
    ah
  |
    ib
\\
    ghe
  X
        cl
      |
        md
    /
      jk
    */

cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_Insert_6) {
    Rope* rope;
    rope = Rope_New();
    ASSERT(rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 0, "abcdefghijklmnop"), cleanup);
    ASSERT_ROPE_EQ("\n\
      ab\n\
    |\n\
      cd\n\
  |\n\
      ef\n\
    |\n\
      gh\n\
|\n\
      ij\n\
    |\n\
      kl\n\
  |\n\
      mn\n\
    |\n\
      op", rope, cleanup);
    ASSERT(3 == Rope_Height(rope), cleanup);
    ASSERT(16 == Rope_Size(rope), cleanup);

    ASSERT(-1 == Rope_InsertStr(rope, 35, "abcdefghijklmnop"), cleanup);
    ASSERT_ROPE_EQ("\n\
      ab\n\
    |\n\
      cd\n\
  |\n\
      ef\n\
    |\n\
      gh\n\
|\n\
      ij\n\
    |\n\
      kl\n\
  |\n\
      mn\n\
    |\n\
      op", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 3, "qrstuvwxyz123456"), cleanup);
    ASSERT_ROPE_EQ("\n\
        ab\n\
      |\n\
        cq\n\
    |\n\
        rs\n\
      |\n\
        tu\n\
  |\n\
        vw\n\
      |\n\
        xy\n\
    |\n\
        z1\n\
      |\n\
        23\n\
|\n\
        45\n\
      |\n\
        6d\n\
    |\n\
        ef\n\
      |\n\
        gh\n\
  |\n\
        ij\n\
      |\n\
        kl\n\
    |\n\
        mn\n\
      |\n\
        op", rope, cleanup);

    ASSERT(0 == Rope_InsertStr(rope, 9, "7890"), cleanup);
    ASSERT_ROPE_EQ("\n\
        ab\n\
      |\n\
        cq\n\
    |\n\
        rs\n\
      |\n\
        tu\n\
  \\\n\
          v7\n\
        |\n\
          89\n\
      |\n\
          0w\n\
        |\n\
          xy\n\
    /\n\
        z1\n\
      |\n\
        23\n\
/\n\
        45\n\
      |\n\
        6d\n\
    |\n\
        ef\n\
      |\n\
        gh\n\
  |\n\
        ij\n\
      |\n\
        kl\n\
    |\n\
        mn\n\
      |\n\
        op", rope, cleanup);

cleanup:
    Rope_Free(rope);
}
END_TEST

TEST(Test_Rope_Insert_Failures) {
    Rope* rope;
    rope = Rope_New();
    ASSERT(rope, cleanup);
    ASSERT(-1 == Rope_InsertStr(rope, 0, 0), cleanup);
    ASSERT(-1 == Rope_InsertStr(rope, 2, "hi"), cleanup);
cleanup:
    Rope_Free(rope);
}
END_TEST

void test_rope(void) {
    RUN(Test_Rope_New);
    RUN(Test_Rope_From_1);
    RUN(Test_Rope_From_2);
    RUN(Test_Rope_Insert_1);
    RUN(Test_Rope_Insert_2);
    RUN(Test_Rope_Insert_3);
    RUN(Test_Rope_Insert_4);
    RUN(Test_Rope_Insert_5);
    RUN(Test_Rope_Insert_6);
    RUN(Test_Rope_Insert_Failures);
    free(rope_string);
}
#endif
