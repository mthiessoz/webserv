#include "ParserJSON.hpp"
#include "server_conf.hpp"

// HELPER
bool parse_words(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const &word, std::vector<std::string> &words)
{
	std::string tmp;

	if (parse_word(json, word, tmp))
		return (true);
	words.push_back(tmp);
	return (false);
}

bool parse_word(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const & word, std::string &dst)
{
	(void) json;
	if (word->lexem != ParserJSON::WORD || has_space(word->value) || word->value.empty())
		return (true);
	dst = word->value;
	return (false);
}

// HELPER
bool parse_strings(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const &word, std::vector<std::string> &words)
{
	std::string tmp;

	if (ParserJSON::to_string(json, word, tmp))
		return (true);
	words.push_back(tmp);
	return (false);
}
