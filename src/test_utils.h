#ifndef MPP_AUTOMATON_TEST_UTILS_H
#define MPP_AUTOMATON_TEST_UTILS_H

typedef char cell;

/**
 *
 *
 * @param buf
 * @param len
 * @param value
 */
void all_equal(cell *buf, unsigned int len, cell value) {
    for (int i = 0; i < len; i++) {
        assert(buf[i] == value);
    }
}


#endif //MPP_AUTOMATON_TEST_UTILS_H
