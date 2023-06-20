#include <fstream>
#include <map>
#include <set>
#include <stack>

#include "ParserJSON.hpp"

ParserJSON::ParserJSON(std::string const &src) : _raw(src)
{
	parse();
}

// public
std::string const &ParserJSON::getRaw() const
{
	return this->_raw;
}

void ParserJSON::parse()
{
	std::vector<ParserJSON::t_token> tokens;
	tokenizer(tokens);
	lexer(tokens);
	check_unique_keys();
}

std::string ParserJSON::toString() const
{
	std::string s;
	for (std::vector<ParserJSON::t_lexem>::const_iterator it = _lexems.begin(); it != this->_lexems.end(); ++it)
	{
		if (it->lexem & ParserJSON::VALUE)
			s += it->value + ' ';
		else
			s += '\n' + it->value;
	}
	return (s);
}
// RETURN LEXEM_KEY
bool ParserJSON::get_key(std::string const &key,
												 std::vector<ParserJSON::t_lexem>::const_iterator &dst,
												 std::vector<ParserJSON::t_lexem>::const_iterator start) const
{
	int depth = -1;

	if (start->lexem != ParserJSON::OPEN_OBJ)
		return (true);
	for (; start != this->_lexems.end(); ++start)
	{
		if (start->lexem == ParserJSON::OPEN_OBJ)
			++depth;
		else if (start->lexem == ParserJSON::CLOSE_OBJ)
		{
			--depth;
			if (depth < 0)
				break;
		}
		else if (start->lexem == ParserJSON::KEY && start->value == key && depth == 0)
		{
			dst = start;
			return (false);
		}
	}

	return (true);
}

bool ParserJSON::get_key(std::string const &key,
												 std::vector<ParserJSON::t_lexem>::const_iterator &dst) const
{
	return this->get_key(key, dst, this->_lexems.begin());
}

// CHECK KEYS
void ParserJSON::check_unique_keys() const
{
	std::stack<std::set<std::string> > stack;

	for (std::vector<ParserJSON::t_lexem>::const_iterator it = this->_lexems.begin(); it != this->_lexems.end(); ++it)
	{
		if (it->lexem == ParserJSON::OPEN_OBJ)
			stack.push(std::set<std::string>());
		else if (it->lexem == ParserJSON::CLOSE_OBJ)
			stack.pop();
		else if (it->lexem == ParserJSON::KEY)
		{
			std::pair<std::set<std::string>::iterator, bool> const ret = stack.top().insert(it->value);
			if (ret.second == false)
				throw("duplicate key");
		}
	}
}

// LEXER
void ParserJSON::skip_white_and_nl(std::vector<ParserJSON::t_token>::const_iterator &it,
																	 std::vector<ParserJSON::t_token>::const_iterator const &end) const
{
	for (; it != end && (it->token == ParserJSON::TOKEN_WHITE || it->token == ParserJSON::TOKEN_NL); ++it)
	{
	}
}

ParserJSON::t_lexem ParserJSON::token_to_lexem(t_token const &token) const
{
	t_lexem lex;

	switch (token.token)
	{
	case TOKEN_OPEN_SQUARE:
		lex.lexem = ParserJSON::OPEN_ARR;
		break;
	case TOKEN_CLOSE_SQUARE:
		lex.lexem = ParserJSON::CLOSE_ARR;
		break;
	case TOKEN_OPEN_BRACKET:
		lex.lexem = ParserJSON::OPEN_OBJ;
		break;
	case TOKEN_CLOSE_BRACKET:
		lex.lexem = ParserJSON::CLOSE_OBJ;
		break;
	case TOKEN_NUMBER:
		lex.lexem = ParserJSON::NUMBER;
		break;
	default:
		throw("unexpected token to lexem");
	}
	lex.value = token.value;
	return (lex);
}

ParserJSON::t_lexem ParserJSON::parse_string(std::vector<ParserJSON::t_token>::const_iterator &it,
																						 std::vector<ParserJSON::t_token>::const_iterator const &end) const
{
	t_lexem lexem((ParserJSON::t_lexem){.lexem = ParserJSON::WORD, .value = ""});

	if (it->token != ParserJSON::TOKEN_QUOTE)
		throw("expected OPEN QUOTE");
	++it;
	for (; it != end && it->token != ParserJSON::TOKEN_QUOTE && it->token != ParserJSON::TOKEN_NL; ++it)
		lexem.value.push_back(it->value[0]);
	if (it->token != ParserJSON::TOKEN_QUOTE)
		throw("expected CLOSE QUOTE");
	++it;

	return (lexem);
}

ParserJSON::t_lexem ParserJSON::parse_bool(std::vector<ParserJSON::t_token>::const_iterator &it,
																					 std::vector<ParserJSON::t_token>::const_iterator const &end) const
{
	static const std::string true_litteral("true");
	static const std::string false_litteral("false");

	ParserJSON::t_lexem lexem((ParserJSON::t_lexem){.lexem = ParserJSON::BOOL, .value = ""});
	size_t i = 0;

	switch (it->value[0])
	{
	case 't':
		i = true_litteral.length();
		break;
	case 'f':
		i = false_litteral.length();
		break;
	}

	for (size_t j = 0; it != end && j < i; ++j)
	{
		lexem.value.push_back(it->value[0]);
		++it;
	}
	if (lexem.value.compare(true_litteral) != 0 && lexem.value.compare(false_litteral) != 0)
		throw("unexpected BOOL");

	return (lexem);
}

ParserJSON::t_lexem ParserJSON::parse_number(std::vector<ParserJSON::t_token>::const_iterator &it,
																						 std::vector<ParserJSON::t_token>::const_iterator const &end) const
{
	ParserJSON::t_lexem lexem((ParserJSON::t_lexem){.lexem = ParserJSON::NUMBER, .value = ""});

	for (; it != end && isdigit(it->value[0]); ++it)
		lexem.value.push_back(it->value[0]);
	return (lexem);
}

ParserJSON::t_lexem ParserJSON::state_parse_key(std::vector<ParserJSON::t_token>::const_iterator &it,
																								std::vector<ParserJSON::t_token>::const_iterator const &end) const
{
	ParserJSON::t_lexem lexem((ParserJSON::t_lexem){.lexem = ParserJSON::KEY, .value = ""});

	if (it->token != ParserJSON::TOKEN_QUOTE)
		throw "expected key open_quote KEY";
	++it;
	for (; it != end && it->token == ParserJSON::TOKEN_CHAR; ++it)
		lexem.value.push_back(it->value[0]);
	if (it->token != ParserJSON::TOKEN_QUOTE)
		throw "expected close_quote KEY";
	++it;
	skip_white_and_nl(it, end);
	if (it->token != ParserJSON::TOKEN_COLON)
		throw "expected colon KEY";
	++it;

	return (lexem);
}

void ParserJSON::state_in_object(std::vector<ParserJSON::e_state> &states,
																 std::vector<ParserJSON::t_token>::const_iterator &it)
{
	switch (it->token)
	{
	case ParserJSON::TOKEN_CLOSE_BRACKET:
		this->_lexems.push_back(token_to_lexem(*it));
		states.pop_back();
		break;
	case ParserJSON::TOKEN_COMMA:
		if (this->_lexems.back().lexem == ParserJSON::OPEN_OBJ)
			throw("unexpected comma OBJECT");
		states.push_back(ParserJSON::STATE_IN_KEY);
		break;
	default:
		throw("unexpected OBJECT");
	}
	++it;
}

void ParserJSON::state_in_array(std::vector<ParserJSON::e_state> &states,
																std::vector<ParserJSON::t_token>::const_iterator &it)
{
	switch (it->token)
	{
	case ParserJSON::TOKEN_CLOSE_SQUARE:
		this->_lexems.push_back(token_to_lexem(*it));
		states.pop_back();
		break;
	case ParserJSON::TOKEN_COMMA:
		if (this->_lexems.back().lexem != ParserJSON::OPEN_OBJ)
		{
			states.push_back(ParserJSON::STATE_IN_VALUE);
			break;
		}
		break;
	default:
		throw("unexpected ARRAY");
	}
	++it;
}

void ParserJSON::state_in_value(std::vector<ParserJSON::e_state> &states,
																std::vector<ParserJSON::t_token>::const_iterator &it,
																std::vector<ParserJSON::t_token>::const_iterator const &end)
{
	switch (it->token)
	{
	case ParserJSON::TOKEN_OPEN_BRACKET:
		this->_lexems.push_back(token_to_lexem(*it));
		states.pop_back();
		states.push_back(ParserJSON::STATE_IN_OBJECT);
		skip_white_and_nl(it, end);
		++it;
		if (it->token == ParserJSON::TOKEN_CLOSE_BRACKET)
		{
		}
		else
			states.push_back(ParserJSON::STATE_IN_KEY);
		break;
	case ParserJSON::TOKEN_OPEN_SQUARE:
		this->_lexems.push_back(token_to_lexem(*it));
		states.pop_back();
		states.push_back(ParserJSON::STATE_IN_ARRAY);
		++it;
		skip_white_and_nl(it, end);
		if (it->token == ParserJSON::TOKEN_CLOSE_SQUARE) // empty array
		{
		}
		else
			states.push_back(ParserJSON::STATE_IN_VALUE);
		break;
	case ParserJSON::TOKEN_QUOTE:
		this->_lexems.push_back(parse_string(it, end));
		states.pop_back();
		break;
	case ParserJSON::TOKEN_NUMBER:
		this->_lexems.push_back(parse_number(it, end));
		states.pop_back();
		break;
	case ParserJSON::TOKEN_CHAR:
		this->_lexems.push_back(parse_bool(it, end));
		states.pop_back();
		break;
	default:
		throw("unexpected VALUE");
	}
}

void ParserJSON::lexer(std::vector<t_token> &tokens)
{
	std::vector<ParserJSON::e_state> states;
	std::vector<ParserJSON::t_token>::const_iterator it = tokens.begin();

	skip_white_and_nl(it, tokens.end());
	if (it->token != ParserJSON::TOKEN_OPEN_BRACKET)
		throw("expected global OPEN_BRACKET");
	this->_lexems.push_back(token_to_lexem(*it));
	states.push_back(ParserJSON::STATE_IN_OBJECT);
	states.push_back(ParserJSON::STATE_IN_KEY);
	++it;
	while (it != tokens.end())
	{
		skip_white_and_nl(it, tokens.end());
		if (states.empty())
			break;
		switch (states.back())
		{
		case ParserJSON::STATE_IN_OBJECT:
			state_in_object(states, it);
			break;
		case ParserJSON::STATE_IN_ARRAY:
			state_in_array(states, it);
			break;
		case ParserJSON::STATE_IN_VALUE:
			state_in_value(states, it, tokens.end());
			break;
		case ParserJSON::STATE_IN_KEY:
			this->_lexems.push_back(state_parse_key(it, tokens.end()));
			states.pop_back();
			states.push_back(ParserJSON::STATE_IN_VALUE);
			break;
		default:
			throw("unexpected GLOBAL");
		}
	}
	if (this->_lexems.back().lexem != ParserJSON::CLOSE_OBJ)
		throw("expected CLOSE_BRACKET");
}

// TOKENIZER
ParserJSON::e_token ParserJSON::get_token(char c) const
{
	switch (c)
	{
	case '[':
		return ParserJSON::TOKEN_OPEN_SQUARE;
		break;
	case ']':
		return ParserJSON::TOKEN_CLOSE_SQUARE;
		break;
	case '{':
		return ParserJSON::TOKEN_OPEN_BRACKET;
	case '}':
		return ParserJSON::TOKEN_CLOSE_BRACKET;
	case ':':
		return ParserJSON::TOKEN_COLON;
	case ',':
		return ParserJSON::TOKEN_COMMA;
	case '\"':
		return ParserJSON::TOKEN_QUOTE;
	case '\n':
		return ParserJSON::TOKEN_NL;
	}
	if (isblank(c))
		return (ParserJSON::TOKEN_WHITE);
	if (isdigit(c))
		return (ParserJSON::TOKEN_NUMBER);
	return (ParserJSON::TOKEN_CHAR);
}

void ParserJSON::tokenizer(std::vector<ParserJSON::t_token> &tokens) const
{
	t_token t;

	for (std::string::const_iterator it = this->_raw.begin(); it != this->_raw.end(); ++it)
	{
		ParserJSON::e_token e = get_token(*it);
		t.value = *it;
		t.token = e;
		tokens.push_back(t);
	}
}

bool ParserJSON::to_bool(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const &lexem_value, bool &dst)
{
	(void) json;
	static const std::string true_litteral("true");
	static const std::string false_litteral("false");

	if (lexem_value->lexem != ParserJSON::BOOL)
		return (true);
	if (lexem_value->value == true_litteral)
		dst = true;
	else
		dst = false;
	return (false);
}

bool ParserJSON::to_string(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const &lexem_value, std::string &dst)
{
	(void) json;

	if (lexem_value->lexem != ParserJSON::WORD)
		return (true);

	dst = lexem_value->value;
	return (false);
}

// Colpien
ParserJSON::ParserJSON()
{
}

ParserJSON::ParserJSON(ParserJSON const &rhs) : _raw(rhs._raw)
{
}

ParserJSON &ParserJSON::operator=(ParserJSON const &rhs)
{
	if (this != &rhs)
	{
	}
	return (*this);
}

ParserJSON::~ParserJSON()
{
}

// EXCEPTION
// ReadFileException
const char *ParserJSON::ReadFileException::what() const throw()
{
	return "ParserJSON::ReadFileException";
}

// ParsingException
const char *ParserJSON::ParsingException::what() const throw()
{
	return "ParserJSON::ParsingException";
}