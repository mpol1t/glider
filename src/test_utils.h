#ifndef MPP_AUTOMATON_TEST_UTILS_H
#define MPP_AUTOMATON_TEST_UTILS_H

typedef char cell;

/**
 * Tests whether all values in the array are equal to value.
 *
 * @param buf   Array to test.
 * @param len   Array length.
 * @param value Value to test against.
 */
void all_equal(cell *buf, unsigned int len, cell value) {
    for (int i = 0; i < len; i++) {
        assert(buf[i] == value);
    }
}


#endif //MPP_AUTOMATON_TEST_UTILS_H
