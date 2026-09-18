#include "lexer.hpp"
#include <expected>

namespace ASM::Lexer {
    using enum LexerState;
    Lexer::Lexer(std::string_view source)
        : m_state(IDENT),
        m_source(source),
        m_building(""),
        m_line(0),
        m_col(0),
        m_ptr(0)
    {}

    lexer_return_t Lexer::lex(void)
    {
        m_state = IDENT;
        m_building.clear();
        m_tokens.clear();
        while (m_peek() != '\0')
        {
            internal_return_t ret;
            if ((ret = m_dispatch()).has_value())
                return std::unexpected(ret.value());
        }
        return m_tokens;
    }
}
