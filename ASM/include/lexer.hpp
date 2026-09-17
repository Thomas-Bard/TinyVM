#ifndef LEXER_HPP
#define LEXER_HPP

#include <string_view>
#include <string>
#include <cstdint>
#include <expected>
#include <vector>
#include <optional>

#include "tokens.hpp"

namespace ASM::Lexer
{
    enum class LexerState
    {
        IDENT,
        STR,
        NUM,
    };

    struct LexerError
    {
        uint64_t line;
        uint64_t col;
        std::string message;
    };

    typedef std::expected<std::vector<Tokens::Token>, LexerError> lexer_return_t;
    typedef std::optional<LexerError> internal_return_t;

    class Lexer
    {
        public:
            Lexer(std::string_view source);
            lexer_return_t lex(void);
        private:
            LexerState m_state;
            std::vector<Tokens::Token> m_tokens;
            std::string m_source;

            std::string m_building;
            uint64_t m_line;
            uint64_t m_col;
            uint64_t m_ptr;

            char m_peek();
            char m_peekahead();
            char m_peekrelative(uint64_t off);

            char m_consume(void);
            void m_emit(Tokens::Token const& token);

            internal_return_t m_dispatch(void);
            internal_return_t m_ident(void);
            internal_return_t m_num(void);
            internal_return_t m_str(void);
    };
}

#endif
