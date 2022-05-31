//
// Created by Nicolas Reyland on 5/27/22.
//

#include "lexer/shell_grammar/rules.h"

//static Combinator rule_2_combinator(void);

CSTNode * shell_grammar_rules(Token * tokens, size_t num_tokens, size_t * num_nodes) {
    // rule parsers
//    Combinator rule_2_parser = rule_2_combinator();
    //
    size_t token_index = 0;
    while (/*token_index < num_tokens*/1) {
        // Rule 1 (inferred)
        /* When the TOKEN is exactly a reserved word, the token identifier for that reserved word shall result.
         * Otherwise, the token WORD shall be returned.
         * Also, if the parser is in any state where only a reserved word
         * could be the next correct token, proceed as above.
         *
         * Rule 1 is not directly referenced in the grammar, but is referred to by other rules, or applies globally.
         * */

        // Rule 2
        /* The expansions specified in Redirection shall occur.
         * As specified there, exactly one field can result (or the result is unspecified),
         * and there are additional requirements on pathname expansion. */

        return NULL;
    }
}
