#include "dfa_construction.h"

/* ================================ GLOBAL VARIABLES ================================ */
char regex[MAX_REGEX_LEN];
char alphabet[MAX_SYMBOLS];
int alphabet_size = 0;

/* ================================ UTILITY FUNCTIONS ================================ */

/* Initialize an empty NFA */
void init_nfa(NFA *nfa) {
    nfa->num_states = 0;
    nfa->start_state = -1;
    nfa->accept_state = -1;
    nfa->num_transitions = 0;
}

/* Add a transition to NFA */
void add_nfa_transition(NFA *nfa, int from, int to, char symbol) {
    if (nfa->num_transitions >= MAX_TRANSITIONS) return;
    nfa->transitions[nfa->num_transitions].from = from;
    nfa->transitions[nfa->num_transitions].to = to;
    nfa->transitions[nfa->num_transitions].symbol = symbol;
    nfa->num_transitions++;
}

/* Add a state to NFA and return its index */
int add_nfa_state(NFA *nfa) {
    return nfa->num_states++;
}

/* Initialize DFA */
void init_dfa(DFA *dfa) {
    dfa->num_states = 0;
    dfa->start_state = -1;
    dfa->num_symbols = 0;
    for (int i = 0; i < MAX_DFA_STATES; i++) {
        dfa->final_states[i] = false;
        for (int j = 0; j < MAX_SYMBOLS; j++) {
            dfa->transition_table[i][j] = -1;
        }
    }
}

/* ================================ THOMPSON'S CONSTRUCTION ================================ */

/* Create NFA for a single symbol */
/* Create NFA for a single symbol - CORRECTED */
NFA create_symbol_nfa(char symbol) {
    NFA nfa;
    init_nfa(&nfa);
    
    int start = add_nfa_state(&nfa);
    int accept = add_nfa_state(&nfa);
    
    /* IMPORTANT: Store the actual character, not its ASCII value as a number */
    add_nfa_transition(&nfa, start, accept, symbol);
    
    nfa.start_state = start;
    nfa.accept_state = accept;
    
    return nfa;
}

/* Create NFA for epsilon transition */
NFA create_epsilon_nfa(void) {
    NFA nfa;
    init_nfa(&nfa);
    
    int start = add_nfa_state(&nfa);
    int accept = add_nfa_state(&nfa);
    
    add_nfa_transition(&nfa, start, accept, EPSILON);
    
    nfa.start_state = start;
    nfa.accept_state = accept;
    
    return nfa;
}

/* Concatenation of two NFAs: NFA1 followed by NFA2 */
NFA concat_nfa(NFA nfa1, NFA nfa2) {
    NFA result;
    init_nfa(&result);
    
    /* Renumber states: nfa1 states first, then nfa2 states */
    int offset1 = 0;
    int offset2 = nfa1.num_states;
    
    /* Add all states from nfa1 and nfa2 */
    result.num_states = nfa1.num_states + nfa2.num_states;
    result.start_state = nfa1.start_state;
    result.accept_state = nfa2.accept_state + offset2;
    
    /* Copy transitions from nfa1 */
    for (int i = 0; i < nfa1.num_transitions; i++) {
        Transition t = nfa1.transitions[i];
        add_nfa_transition(&result, t.from + offset1, t.to + offset1, t.symbol);
    }
    
    /* Copy transitions from nfa2 with offset */
    for (int i = 0; i < nfa2.num_transitions; i++) {
        Transition t = nfa2.transitions[i];
        add_nfa_transition(&result, t.from + offset2, t.to + offset2, t.symbol);
    }
    
    /* Add epsilon transition from nfa1 accept to nfa2 start */
    add_nfa_transition(&result, nfa1.accept_state + offset1, nfa2.start_state + offset2, EPSILON);
    
    return result;
}

/* Union (alternation) of two NFAs: NFA1 | NFA2 */
NFA union_nfa(NFA nfa1, NFA nfa2) {
    NFA result;
    init_nfa(&result);
    
    int new_start = add_nfa_state(&result);
    int new_accept = add_nfa_state(&result);
    
    int offset1 = 1;                      /* After new_start */
    int offset2 = offset1 + nfa1.num_states;
    
    /* Renumber and add states from nfa1 and nfa2 */
    result.num_states = 1 + nfa1.num_states + nfa2.num_states + 1;
    
    /* Copy transitions from nfa1 */
    for (int i = 0; i < nfa1.num_transitions; i++) {
        Transition t = nfa1.transitions[i];
        add_nfa_transition(&result, t.from + offset1, t.to + offset1, t.symbol);
    }
    
    /* Copy transitions from nfa2 */
    for (int i = 0; i < nfa2.num_transitions; i++) {
        Transition t = nfa2.transitions[i];
        add_nfa_transition(&result, t.from + offset2, t.to + offset2, t.symbol);
    }
    
    /* Epsilon transitions from new start to both NFAs */
    add_nfa_transition(&result, new_start, nfa1.start_state + offset1, EPSILON);
    add_nfa_transition(&result, new_start, nfa2.start_state + offset2, EPSILON);
    
    /* Epsilon transitions from both accept states to new accept */
    add_nfa_transition(&result, nfa1.accept_state + offset1, new_accept, EPSILON);
    add_nfa_transition(&result, nfa2.accept_state + offset2, new_accept, EPSILON);
    
    result.start_state = new_start;
    result.accept_state = new_accept;
    
    return result;
}

/* Kleene star closure: NFA* */
NFA star_nfa(NFA nfa_in) {
    NFA result;
    init_nfa(&result);
    
    int new_start = add_nfa_state(&result);
    int new_accept = add_nfa_state(&result);
    
    int offset = 1;                       /* After new_start */
    
    /* Copy transitions from nfa_in */
    result.num_states = 1 + nfa_in.num_states + 1;
    
    for (int i = 0; i < nfa_in.num_transitions; i++) {
        Transition t = nfa_in.transitions[i];
        add_nfa_transition(&result, t.from + offset, t.to + offset, t.symbol);
    }
    
    /* Epsilon transitions: new_start -> nfa_start, and new_start -> new_accept */
    add_nfa_transition(&result, new_start, nfa_in.start_state + offset, EPSILON);
    add_nfa_transition(&result, new_start, new_accept, EPSILON);
    
    /* Epsilon: nfa_accept -> nfa_start (loop) */
    add_nfa_transition(&result, nfa_in.accept_state + offset, nfa_in.start_state + offset, EPSILON);
    
    /* Epsilon: nfa_accept -> new_accept */
    add_nfa_transition(&result, nfa_in.accept_state + offset, new_accept, EPSILON);
    
    result.start_state = new_start;
    result.accept_state = new_accept;
    
    return result;
}

/* Convert infix regex to postfix (handles operators: | * . ) */
void infix_to_postfix(const char *infix, char *postfix) {
    char stack[MAX_STACK];
    int top = -1;
    int j = 0;
    int len = strlen(infix);
    
    /* Preprocess: insert explicit concatenation operator '.' where needed */
    char processed[MAX_REGEX_LEN * 2];
    int p_len = 0;
    
    for (int i = 0; i < len; i++) {
        char c = infix[i];
        processed[p_len++] = c;
        
        if (i + 1 < len) {
            char next = infix[i + 1];
            /* Insert '.' if current char is operand/ )/* and next is operand/( */
            if ((isalnum(c) || c == ')' || c == '*') && (isalnum(next) || next == '(')) {
                processed[p_len++] = '.';
            }
        }
    }
    processed[p_len] = '\0';
    
    /* Shunting-yard algorithm */
    for (int i = 0; i < p_len; i++) {
        char c = processed[i];
        
        if (isalnum(c)) {          /* Operand */
            postfix[j++] = c;
        } else if (c == '(') {
            stack[++top] = c;
        } else if (c == ')') {
            while (top >= 0 && stack[top] != '(') {
                postfix[j++] = stack[top--];
            }
            top--;  /* Pop '(' */
        } else if (c == '*') {      /* Unary operator - highest precedence */
            while (top >= 0 && stack[top] == '*') {
                postfix[j++] = stack[top--];
            }
            stack[++top] = c;
        } else if (c == '.') {      /* Concatenation - precedence 2 */
            while (top >= 0 && (stack[top] == '.' || stack[top] == '*')) {
                postfix[j++] = stack[top--];
            }
            stack[++top] = c;
        } else if (c == '|') {      /* Union - lowest precedence */
            while (top >= 0 && (stack[top] == '.' || stack[top] == '|' || stack[top] == '*')) {
                postfix[j++] = stack[top--];
            }
            stack[++top] = c;
        }
    }
    
    while (top >= 0) {
        postfix[j++] = stack[top--];
    }
    postfix[j] = '\0';
}

/* Build NFA from postfix regular expression using Thompson's construction */
NFA build_nfa_from_postfix(const char *postfix) {
    NFAStack stack;
    stack.top = -1;
    
    for (int i = 0; postfix[i] != '\0'; i++) {
        char c = postfix[i];
        
        if (isalnum(c)) {
            /* Add symbol to alphabet if not already present */
            bool found = false;
            for (int k = 0; k < alphabet_size; k++) {
                if (alphabet[k] == c) {
                    found = true;
                    break;
                }
            }
            if (!found && c != EPSILON) {
                alphabet[alphabet_size++] = c;
            }
            
            NFA sym_nfa = create_symbol_nfa(c);
            stack.fragments[++stack.top] = sym_nfa;
        } else if (c == '*') {
            NFA nfa = stack.fragments[stack.top--];
            NFA star = star_nfa(nfa);
            stack.fragments[++stack.top] = star;
        } else if (c == '.') {
            NFA nfa2 = stack.fragments[stack.top--];
            NFA nfa1 = stack.fragments[stack.top--];
            NFA concat = concat_nfa(nfa1, nfa2);
            stack.fragments[++stack.top] = concat;
        } else if (c == '|') {
            NFA nfa2 = stack.fragments[stack.top--];
            NFA nfa1 = stack.fragments[stack.top--];
            NFA uni = union_nfa(nfa1, nfa2);
            stack.fragments[++stack.top] = uni;
        }
    }
    
    if (stack.top != 0) {
        printf("Error: Invalid regular expression\n");
        exit(1);
    }
    
    return stack.fragments[0];
}

/* Print NFA details */
/* Compare function for sorting transitions */
int compare_transitions(const void *a, const void *b) {
    Transition *t1 = (Transition *)a;
    Transition *t2 = (Transition *)b;
    
    /* First sort by 'from' state */
    if (t1->from != t2->from) {
        return t1->from - t2->from;
    }
    /* Then by symbol (epsilon first, then alphabetically) */
    if (t1->symbol != t2->symbol) {
        /* Put epsilon transitions first */
        if (t1->symbol == EPSILON) return -1;
        if (t2->symbol == EPSILON) return 1;
        return t1->symbol - t2->symbol;
    }
    /* Then by 'to' state */
    return t1->to - t2->to;
}

/* Print NFA details in ORDERED format */
void print_nfa(NFA *nfa) {
    printf("\n========== NFA (Thompson's Construction) ==========\n");
    printf("Number of states: %d\n", nfa->num_states);
    printf("Start state: %d\n", nfa->start_state);
    printf("Accept state: %d\n", nfa->accept_state);
    printf("\nTransitions (ordered by source state):\n");
    
    /* Create a copy of transitions to sort */
    Transition sorted_trans[MAX_TRANSITIONS];
    for (int i = 0; i < nfa->num_transitions; i++) {
        sorted_trans[i] = nfa->transitions[i];
    }
    
    /* Sort the transitions */
    qsort(sorted_trans, nfa->num_transitions, sizeof(Transition), compare_transitions);
    
    /* Print sorted transitions */
    int current_state = -1;
    for (int i = 0; i < nfa->num_transitions; i++) {
        Transition t = sorted_trans[i];
        
        /* Print state header when state changes */
        if (t.from != current_state) {
            current_state = t.from;
            printf("\n  From State %d:\n", current_state);
        }
        
        /* Print the transition */
        if (t.symbol == EPSILON) {
            printf("    +-- e --> %d\n", t.to);
        } else {
            printf("    +-- %c --> %d\n", (char)t.symbol, t.to);
        }
    }
    printf("\n===================================================\n");
}
/* ================================ SUBSET CONSTRUCTION (NFA -> DFA) ================================ */

/* Find epsilon closure of a set of states */
/* Find epsilon closure of a set of states - CORRECTED */
StateSet epsilon_closure(NFA *nfa, StateSet *states) {
    StateSet closure;
    closure.count = 0;
    
    /* Initialize closure with given states */
    for (int i = 0; i < states->count; i++) {
        closure.states[closure.count++] = states->states[i];
    }
    
    /* Iteratively add states reachable via epsilon transitions */
    bool changed = true;
    while (changed) {
        changed = false;
        
        for (int i = 0; i < closure.count; i++) {
            int s = closure.states[i];
            /* Find all epsilon transitions from s */
            for (int t = 0; t < nfa->num_transitions; t++) {
                if (nfa->transitions[t].from == s && nfa->transitions[t].symbol == EPSILON) {
                    int target = nfa->transitions[t].to;
                    /* Check if already in closure */
                    bool already = false;
                    for (int k = 0; k < closure.count; k++) {
                        if (closure.states[k] == target) {
                            already = true;
                            break;
                        }
                    }
                    if (!already) {
                        closure.states[closure.count++] = target;
                        changed = true;
                    }
                }
            }
        }
    }
    
    return closure;
}

/* Move: from a set of states, follow symbol transitions */
StateSet move_set(NFA *nfa, StateSet *states, char symbol) {
    StateSet result;
    result.count = 0;
    
    for (int i = 0; i < states->count; i++) {
        int s = states->states[i];
        for (int t = 0; t < nfa->num_transitions; t++) {
            if (nfa->transitions[t].from == s && nfa->transitions[t].symbol == symbol) {
                int target = nfa->transitions[t].to;
                bool already = false;
                for (int k = 0; k < result.count; k++) {
                    if (result.states[k] == target) {
                        already = true;
                        break;
                    }
                }
                if (!already) {
                    result.states[result.count++] = target;
                }
            }
        }
    }
    return result;
}

/* Compare two state sets for equality */
bool are_sets_equal(StateSet *a, StateSet *b) {
    if (a->count != b->count) return false;
    
    /* Simple sort both for comparison */
    for (int i = 0; i < a->count; i++) {
        bool found = false;
        for (int j = 0; j < b->count; j++) {
            if (a->states[i] == b->states[j]) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

/* Find index of a state set in the DFA state list */
int find_set_index(StateSet dfa_states[], int dfa_state_count, StateSet *set) {
    for (int i = 0; i < dfa_state_count; i++) {
        if (are_sets_equal(&dfa_states[i], set)) {
            return i;
        }
    }
    return -1;
}

/* Convert NFA to DFA using subset construction */
/* Convert NFA to DFA using subset construction - CORRECTED */
DFA nfa_to_dfa(NFA *nfa, char symbols[], int num_symbols) {
    DFA dfa;
    init_dfa(&dfa);
    
    /* Store DFA states as sets of NFA states */
    StateSet dfa_sets[MAX_DFA_STATES];
    int dfa_state_count = 0;
    
    /* Compute epsilon closure of NFA start state */
    StateSet start_set;
    start_set.count = 1;
    start_set.states[0] = nfa->start_state;
    StateSet start_closure = epsilon_closure(nfa, &start_set);
    
    dfa_sets[dfa_state_count++] = start_closure;
    dfa.start_state = 0;
    
    /* Subset construction main loop */
    int current = 0;
    while (current < dfa_state_count) {
        for (int sym_idx = 0; sym_idx < num_symbols; sym_idx++) {
            char sym = symbols[sym_idx];
            
            /* Move on symbol, then epsilon closure */
            StateSet move = move_set(nfa, &dfa_sets[current], sym);
            StateSet closure = epsilon_closure(nfa, &move);
            
            if (closure.count > 0) {
                int existing = find_set_index(dfa_sets, dfa_state_count, &closure);
                if (existing == -1) {
                    existing = dfa_state_count;
                    dfa_sets[dfa_state_count++] = closure;
                }
                dfa.transition_table[current][sym_idx] = existing;
            } else {
                dfa.transition_table[current][sym_idx] = -1;
            }
        }
        current++;
    }
    
    /* Copy symbols to DFA */
    dfa.num_symbols = num_symbols;
    for (int i = 0; i < num_symbols; i++) {
        dfa.symbols[i] = symbols[i];
    }
    dfa.num_states = dfa_state_count;
    
    /* CRITICAL FIX: Mark final states correctly */
    /* A DFA state is final ONLY IF it contains the NFA's accept state */
    for (int i = 0; i < dfa_state_count; i++) {
        dfa.final_states[i] = false;  // Initialize all to false
        for (int j = 0; j < dfa_sets[i].count; j++) {
            if (dfa_sets[i].states[j] == nfa->accept_state) {
                dfa.final_states[i] = true;
                break;
            }
        }
    }
    
    return dfa;
}

/* ================================ DFA MINIMIZATION ================================ */

/* Hopcroft-style minimization using partition refinement */
/* Corrected DFA Minimization using Table Filling Method (Hopcroft's algorithm) */
DFA minimize_dfa(DFA *dfa) {
    if (dfa->num_states <= 1) {
        return *dfa;
    }
    
    int num_states = dfa->num_states;
    int num_symbols = dfa->num_symbols;
    
    /* Step 1: Create distinguishable table */
    bool distinguishable[MAX_DFA_STATES][MAX_DFA_STATES];
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < num_states; j++) {
            distinguishable[i][j] = false;
        }
    }
    
    /* Step 2: Mark all final vs non-final pairs as distinguishable */
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < num_states; j++) {
            if (dfa->final_states[i] != dfa->final_states[j]) {
                distinguishable[i][j] = distinguishable[j][i] = true;
            }
        }
    }
    
    /* Step 3: Iteratively mark distinguishable pairs */
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < num_states; i++) {
            for (int j = i + 1; j < num_states; j++) {
                if (!distinguishable[i][j]) {
                    for (int sym = 0; sym < num_symbols; sym++) {
                        int next_i = dfa->transition_table[i][sym];
                        int next_j = dfa->transition_table[j][sym];
                        
                        if (next_i == -1 && next_j == -1) continue;
                        if (next_i == -1 || next_j == -1) {
                            distinguishable[i][j] = distinguishable[j][i] = true;
                            changed = true;
                            break;
                        }
                        
                        if (distinguishable[next_i][next_j]) {
                            distinguishable[i][j] = distinguishable[j][i] = true;
                            changed = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    /* Step 4: Build equivalence classes */
    int state_group[MAX_DFA_STATES];
    for (int i = 0; i < num_states; i++) {
        state_group[i] = -1;
    }
    
    int group_count = 0;
    for (int i = 0; i < num_states; i++) {
        if (state_group[i] == -1) {
            state_group[i] = group_count;
            for (int j = i + 1; j < num_states; j++) {
                if (!distinguishable[i][j]) {
                    state_group[j] = group_count;
                }
            }
            group_count++;
        }
    }
    
    /* Step 5: Build minimized DFA */
    DFA min_dfa;
    init_dfa(&min_dfa);
    
    min_dfa.num_states = group_count;
    min_dfa.start_state = state_group[dfa->start_state];
    min_dfa.num_symbols = num_symbols;
    
    for (int i = 0; i < num_symbols; i++) {
        min_dfa.symbols[i] = dfa->symbols[i];
    }
    
    for (int i = 0; i < group_count; i++) {
        int rep = -1;
        for (int j = 0; j < num_states; j++) {
            if (state_group[j] == i) {
                rep = j;
                break;
            }
        }
        
        for (int j = 0; j < num_states; j++) {
            if (state_group[j] == i && dfa->final_states[j]) {
                min_dfa.final_states[i] = true;
                break;
            }
        }
        
        for (int sym = 0; sym < num_symbols; sym++) {
            int next = dfa->transition_table[rep][sym];
            if (next != -1) {
                min_dfa.transition_table[i][sym] = state_group[next];
            } else {
                min_dfa.transition_table[i][sym] = -1;
            }
        }
    }
    
    return min_dfa;
}


/* ================================ DISPLAY FUNCTIONS ================================ */

/* Print DFA transition table */
void print_dfa_transition_table(DFA *dfa, const char *title) {
    printf("\n========== %s ==========\n", title);
    printf("States: %d\n", dfa->num_states);
    printf("Start state: %d\n", dfa->start_state);
    printf("Final states: ");
    for (int i = 0; i < dfa->num_states; i++) {
        if (dfa->final_states[i]) {
            printf("%d ", i);
        }
    }
    printf("\n\nTransition Table:\n");
    printf("State\t");
    for (int i = 0; i < dfa->num_symbols; i++) {
        printf("|\t%c\t", dfa->symbols[i]);
    }
    printf("\n");
    
    for (int i = 0; i < dfa->num_states; i++) {
        printf("%d\t", i);
        for (int j = 0; j < dfa->num_symbols; j++) {
            if (dfa->transition_table[i][j] != -1) {
                printf("|\t%d\t", dfa->transition_table[i][j]);
            } else {
                printf("|\t-\t");
            }
        }
        printf("\n");
    }
    printf("======================================\n");
}

/* Validate a string against DFA */
bool validate_string(DFA *dfa, const char *input) {
    int current_state = dfa->start_state;
    
    for (int i = 0; input[i] != '\0'; i++) {
        char c = input[i];
        
        /* Find symbol index */
        int sym_idx = -1;
        for (int j = 0; j < dfa->num_symbols; j++) {
            if (dfa->symbols[j] == c) {
                sym_idx = j;
                break;
            }
        }
        
        if (sym_idx == -1) {
            printf("Invalid character '%c' in input string\n", c);
            return false;
        }
        
        int next = dfa->transition_table[current_state][sym_idx];
        if (next == -1) {
            return false;   /* No transition */
        }
        current_state = next;
    }
    
    return dfa->final_states[current_state];
}



bool validate_regex_syntax(const char *regex) {
    int paren_balance = 0;
    int last_char_was_operator = 1;  // Start of expression
    
    for (int i = 0; regex[i] != '\0'; i++) {
        char c = regex[i];
        
        // Check for valid characters
        if (!isalnum(c) && c != '|' && c != '*' && c != '(' && c != ')' && c != '.') {
            printf("Error: Invalid character '%c' in regex\n", c);
            return false;
        }
        
        // Check parentheses balance
        if (c == '(') {
            paren_balance++;
            last_char_was_operator = 1;
        } else if (c == ')') {
            paren_balance--;
            if (paren_balance < 0) {
                printf("Error: Unmatched closing parenthesis\n");
                return false;
            }
            last_char_was_operator = 0;
        } else if (c == '|') {
            if (last_char_was_operator) {
                printf("Error: '|' operator without left operand\n");
                return false;
            }
            last_char_was_operator = 1;
        } else if (c == '*') {
            if (last_char_was_operator) {
                printf("Error: '*' operator without operand\n");
                return false;
            }
            // '*' is unary, doesn't change last_char_was_operator
        } else {  // operand
            last_char_was_operator = 0;
        }
    }
    
    if (paren_balance != 0) {
        printf("Error: Unmatched opening parenthesis\n");
        return false;
    }
    
    if (last_char_was_operator) {
        printf("Error: Expression ends with operator\n");
        return false;
    }
    
    return true;
}


//void print_performance_evaluation(NFA *nfa, DFA *dfa, DFA *min_dfa) {
//    printf("\n========== PERFORMANCE EVALUATION ==========\n");
//    printf("| %-25s | %5s |\n", "Metric", "Value");
//    printf("|-----------------------------------------|\n");
//    printf("| %-25s | %5d |\n", "NFA States", nfa->num_states);
//    printf("| %-25s | %5d |\n", "DFA States (before minimization)", dfa->num_states);
//    printf("| %-25s | %5d |\n", "Minimized DFA States", min_dfa->num_states);
//    
//    if (dfa->num_states > 0) {
//        float reduction = (1 - (float)min_dfa->num_states / dfa->num_states) * 100;
//        printf("| %-25s | %5.1f%% |\n", "State Reduction", reduction);
//    }
//    
//    printf("| %-25s | %5d |\n", "Alphabet Size", dfa->num_symbols);
//    printf("=============================================\n");
//}

/* ================================ MAIN PROGRAM ================================ */

int main() {
    printf("============================================================\n");
    printf("  AUTOMATIC CONSTRUCTION OF DFA FROM REGULAR EXPRESSION\n");
    printf("  Compiler Construction - Open Ended Lab Project\n");
    printf("  Submitted by: Tehreem Zubair (68610) & Diya Pahooja (68699)\n");
    printf("============================================================\n\n");
    
    /* Step 1: Input Regular Expression with validation */
    printf("Enter a regular expression (supports: a-z, 0-9, |, *, (, ) )\n");
    printf("Example: (a|b)*abb\n");
    printf("Regex: ");
    fgets(regex, MAX_REGEX_LEN, stdin);
    regex[strcspn(regex, "\n")] = '\0';
    
    if (strlen(regex) == 0) {
        printf("No input provided. Using default: (a|b)*abb\n");
        strcpy(regex, "(a|b)*abb");
    }
    
    /* FIX: Validate regex syntax */
    if (!validate_regex_syntax(regex)) {
        printf("Invalid regular expression. Exiting...\n");
        return 1;
    }
    
    printf("\nInput Regular Expression: %s\n", regex);
    
    /* Step 2: Convert to Postfix */
    char postfix[MAX_REGEX_LEN * 2];
    infix_to_postfix(regex, postfix);
    printf("Postfix form: %s\n", postfix);
    
    /* Step 3: Build NFA using Thompson's Construction */
    alphabet_size = 0;
    NFA nfa = build_nfa_from_postfix(postfix);
    print_nfa(&nfa);
    
    /* Sort alphabet for deterministic output */
    for (int i = 0; i < alphabet_size - 1; i++) {
        for (int j = i + 1; j < alphabet_size; j++) {
            if (alphabet[i] > alphabet[j]) {
                char temp = alphabet[i];
                alphabet[i] = alphabet[j];
                alphabet[j] = temp;
            }
        }
    }
    
    /* Step 4: Convert NFA to DFA */
    DFA dfa = nfa_to_dfa(&nfa, alphabet, alphabet_size);
    print_dfa_transition_table(&dfa, "DFA (Before Minimization)");
    
    /* Step 5: Minimize DFA */
    DFA min_dfa = minimize_dfa(&dfa);
    print_dfa_transition_table(&min_dfa, "MINIMIZED DFA");
    
    /* FIX: Add performance evaluation */
   // print_performance_evaluation(&nfa, &dfa, &min_dfa);
    
    /* Step 6: String Validation */
    printf("\n========== STRING VALIDATION ==========\n");
    char input[MAX_INPUT_STR];
    
    // Clear input buffer
    while (getchar() != '\n');
    
    while (1) {
        printf("\nEnter a string to validate (or 'quit' to exit): ");
        fgets(input, MAX_INPUT_STR, stdin);
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "quit") == 0 || strcmp(input, "QUIT") == 0 || strcmp(input, "q") == 0) {
            printf("\nExiting string validation...\n");
            break;
        }
        
        if (strlen(input) == 0) {
            printf("Please enter a string or 'quit'\n");
            continue;
        }
        
        bool accepted = validate_string(&min_dfa, input);
        if (accepted) {
            printf("Result: ACCEPTED\n");
        } else {
            printf("Result: REJECTED\n");
        }
    }
    
    /* Step 7: Project Summary */
    printf("\n========== PROJECT SUMMARY ==========\n");
    printf("[SUCCESS] Regular Expression: %s\n", regex);
    printf("[SUCCESS] NFA states generated: %d\n", nfa.num_states);
    printf("[SUCCESS] DFA states (pre-minimization): %d\n", dfa.num_states);
    printf("[SUCCESS] Minimized DFA states: %d\n", min_dfa.num_states);
    printf("[SUCCESS] Alphabet size: %d\n", alphabet_size);
    printf("[SUCCESS] Reduction achieved: %d -> %d states\n", dfa.num_states, min_dfa.num_states);
    printf("\nProject completed successfully.\n");
    
    printf("\nPress Enter to exit...");
    getchar();
    
    return 0;
}

