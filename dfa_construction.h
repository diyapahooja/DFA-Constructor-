/**
 * ============================================================================
 * DFA CONVERTER - Header File
 * ============================================================================
 * Title: Automatic Construction of DFA from Regular Expression
 * 
 * Description:
 *   This header file contains all function declarations, constants, and
 *   data structures for converting regular expressions to minimized DFAs.
 * 
 * 
 * Submitted by:
 *   - Tehreem Zubair (68610)
 *   - Diya Pahooja (68699)
 * 
 * Course: Compiler Construction Lab
 * ============================================================================
 */

#ifndef DFA_CONVERTER_H
#define DFA_CONVERTER_H

/* ================================ HEADERS ================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/* ================================ CONSTANTS ================================ */

/** Maximum number of states in NFA/DFA */
#define MAX_STATES         100

/** Maximum number of symbols in alphabet */
#define MAX_SYMBOLS        10

/** Maximum number of transitions */
#define MAX_TRANSITIONS    200

/** Maximum stack size for expression parsing */
#define MAX_STACK          100

/** Maximum NFA states */
#define MAX_NFA_STATES     100

/** Maximum DFA states */
#define MAX_DFA_STATES     100

/** Maximum size for state sets */
#define MAX_SETS           100

/** Maximum length of regular expression */
#define MAX_REGEX_LEN      100

/** Maximum length of input string */
#define MAX_INPUT_STR      100

/** Maximum symbol set size */
#define MAX_SYMBOL_SET     128

/** Epsilon transition representation */
#define EPSILON           'E'

/* ================================ DATA STRUCTURES ================================ */

/**
 * Transition Structure
 * Represents a single transition in an NFA or DFA
 */
typedef struct {
    int from;           /**< Source state ID */
    int to;             /**< Destination state ID */
    char symbol;        /**< Input symbol (or EPSILON for e-transitions) */
} Transition;

/**
 * NFA Structure (Non-deterministic Finite Automaton)
 * Represents an NFA built using Thompson's Construction
 */
typedef struct {
    int num_states;                         /**< Total number of states */
    int start_state;                        /**< Start state index */
    int accept_state;                       /**< Accept/Final state index */
    Transition transitions[MAX_TRANSITIONS]; /**< Array of all transitions */
    int num_transitions;                    /**< Number of transitions */
} NFA;

/**
 * DFA Structure (Deterministic Finite Automaton)
 * Represents a DFA after subset construction and minimization
 */
typedef struct {
    int num_states;                                     /**< Total states */
    int start_state;                                    /**< Start state */
    bool final_states[MAX_DFA_STATES];                  /**< Final state flags */
    int transition_table[MAX_DFA_STATES][MAX_SYMBOLS];  /**< Transition table */
    char symbols[MAX_SYMBOLS];                          /**< Input alphabet */
    int num_symbols;                                    /**< Alphabet size */
} DFA;

/**
 * StateSet Structure
 * Represents a set of NFA states (used in subset construction)
 */
typedef struct {
    int states[MAX_NFA_STATES];     /**< Array of state IDs in this set */
    int count;                      /**< Number of states in this set */
} StateSet;

/**
 * NFA Stack Structure
 * Stack for storing NFA fragments during Thompson's Construction
 */
typedef struct {
    NFA fragments[MAX_STACK];   /**< Stack of NFA fragments */
    int top;                    /**< Top index of stack */
} NFAStack;

/* ================================ FUNCTION DECLARATIONS ================================ */

/* ------------------------- Utility Functions ------------------------- */

/**
 * Initializes an empty NFA structure
 * @param nfa Pointer to NFA to initialize
 */
void init_nfa(NFA *nfa);

/**
 * Adds a transition to an NFA
 * @param nfa Pointer to NFA
 * @param from Source state
 * @param to Destination state
 * @param symbol Input symbol or EPSILON
 */
void add_nfa_transition(NFA *nfa, int from, int to, char symbol);

/**
 * Adds a new state to an NFA
 * @param nfa Pointer to NFA
 * @return ID of the new state
 */
int add_nfa_state(NFA *nfa);

/**
 * Initializes an empty DFA structure
 * @param dfa Pointer to DFA to initialize
 */
void init_dfa(DFA *dfa);

/**
 * Checks if a state is in a StateSet
 * @param set Pointer to StateSet
 * @param state State to check
 * @return true if state exists in set, false otherwise
 */
bool is_in_set(StateSet *set, int state);

/* ------------------------- Thompson's Construction ------------------------- */

/**
 * Creates an NFA for a single symbol
 * @param symbol The input symbol
 * @return NFA that accepts exactly that symbol
 */
NFA create_symbol_nfa(char symbol);

/**
 * Creates an NFA for epsilon transition
 * @return NFA that accepts empty string
 */
NFA create_epsilon_nfa(void);

/**
 * Concatenates two NFAs (A followed by B)
 * @param nfa1 First NFA
 * @param nfa2 Second NFA
 * @return NFA that accepts L(nfa1) concatenated with L(nfa2)
 */
NFA concat_nfa(NFA nfa1, NFA nfa2);

/**
 * Creates union of two NFAs (A or B)
 * @param nfa1 First NFA
 * @param nfa2 Second NFA
 * @return NFA that accepts L(nfa1) ? L(nfa2)
 */
NFA union_nfa(NFA nfa1, NFA nfa2);

/**
 * Applies Kleene star to an NFA (A*)
 * @param nfa_in Input NFA
 * @return NFA that accepts zero or more repetitions of L(nfa_in)
 */
NFA star_nfa(NFA nfa_in);

/**
 * Converts infix regular expression to postfix notation
 * @param infix Input infix regex
 * @param postfix Output postfix string
 */
void infix_to_postfix(const char *infix, char *postfix);

/**
 * Builds NFA from postfix regular expression using Thompson's Construction
 * @param postfix Postfix regular expression
 * @return Complete NFA for the regular expression
 */
NFA build_nfa_from_postfix(const char *postfix);

/**
 * Prints NFA details with sorted transitions
 * @param nfa Pointer to NFA to print
 */
void print_nfa(NFA *nfa);

/* ------------------------- Subset Construction (NFA ? DFA) ------------------------- */

/**
 * Computes epsilon closure of a set of NFA states
 * @param nfa Pointer to NFA
 * @param states Input set of states
 * @return Set of all states reachable via epsilon transitions
 */
StateSet epsilon_closure(NFA *nfa, StateSet *states);

/**
 * Computes move operation on a set of states with a symbol
 * @param nfa Pointer to NFA
 * @param states Current set of states
 * @param symbol Input symbol
 * @return Set of states reachable via 'symbol'
 */
StateSet move_set(NFA *nfa, StateSet *states, char symbol);

/**
 * Checks if two state sets are equal
 * @param a First set
 * @param b Second set
 * @return true if sets contain same states, false otherwise
 */
bool are_sets_equal(StateSet *a, StateSet *b);

/**
 * Finds index of a state set in DFA state list
 * @param dfa_sets Array of DFA state sets
 * @param dfa_state_count Number of DFA states
 * @param set Set to find
 * @return Index if found, -1 otherwise
 */
int find_set_index(StateSet dfa_sets[], int dfa_state_count, StateSet *set);

/**
 * Converts NFA to DFA using subset construction
 * @param nfa Pointer to NFA
 * @param symbols Array of input symbols
 * @param num_symbols Number of symbols
 * @return Equivalent DFA
 */
DFA nfa_to_dfa(NFA *nfa, char symbols[], int num_symbols);

/**
 * Prints DFA transition table
 * @param dfa Pointer to DFA
 * @param title Title for the output
 */
void print_dfa_transition_table(DFA *dfa, const char *title);

/* ------------------------- DFA Minimization ------------------------- */

/**
 * Minimizes a DFA using table-filling method
 * @param dfa Pointer to DFA to minimize
 * @return Minimized DFA
 */
DFA minimize_dfa(DFA *dfa);

/* ------------------------- String Validation ------------------------- */

/**
 * Validates a string against the minimized DFA
 * @param dfa Pointer to minimized DFA
 * @param input Input string to validate
 * @return true if accepted, false if rejected
 */
bool validate_string(DFA *dfa, const char *input);

/* ------------------------- Additional Features ------------------------- */

/**
 * Validates regular expression syntax
 * @param regex Regular expression to validate
 * @return true if valid, false otherwise
 */
bool validate_regex_syntax(const char *regex);

/**
 * Prints performance evaluation metrics
 * @param nfa Pointer to NFA
 * @param dfa Pointer to DFA (before minimization)
 * @param min_dfa Pointer to minimized DFA
 */
void print_performance_evaluation(NFA *nfa, DFA *dfa, DFA *min_dfa);

/**
 * Debug function to view raw transition data
 * @param nfa Pointer to NFA
 */
void debug_transition_storage(NFA *nfa);

/**
 * Comparison function for sorting transitions (used by qsort)
 * @param a First transition
 * @param b Second transition
 * @return Negative if a<b, zero if equal, positive if a>b
 */
int compare_transitions(const void *a, const void *b);

/* ================================ GLOBAL VARIABLES ================================ */

/** Global regex string storage */
extern char regex[MAX_REGEX_LEN];

/** Global alphabet array */
extern char alphabet[MAX_SYMBOLS];

/** Current size of alphabet */
extern int alphabet_size;

#endif /* DFA_CONVERTER_H */
