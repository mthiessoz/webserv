#ifndef PARSERJSON_HPP
#define PARSERJSON_HPP

#include <string>
#include <vector>
#include <sstream>


/*
	ParserJSON take a string and convert it into a vector of lexems
	to use it we work with lexem::iterator
	

	key.s functions are used to parse values, parameters:
		- key : string, key of the value
		| keys : array of strings, for nested search ["firstLevel", "secondLevel"] == ({ firstLevel: { secondLevel: ... }}) 
		- dst : reference to the variable where func should store result
		- func : a function who parse should return false if parse success
		- start? : a lexem::iterator pointing at a OPEN_OBJ

	key_map : works same expect it's call func for each items inside an array

	arr : expect a lexem::iterator point at a OPEN_ARR

	predefined functions for parsing primitives types it's attended to be used as a func parameter:
		- to_number
		- to_word
		- to_bool
	
	get_key.s return lexem::iterator at KEY

	all functions, func parameters return true if failed
*/
class ParserJSON
{
	class ReadFileException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};
	class ParsingException : public std::exception
	{
	public:
		virtual const char *what() const throw();
	};

public:
	// LEXER
	enum e_lexem
	{
		OPEN_ARR = 0x1,
		CLOSE_ARR = 0x10,
		OPEN_OBJ = 0b100,
		CLOSE_OBJ = 0b1000,
		KEY = 0b100000,		 // nospace
		WORD = 0b1000000,	 // canspace
		BOOL = 0b10000000, // true | false
		NUMBER = 0b100000000,
		VALUE = 0b111000000 // WORD & BOOL & NUMBER
	};

	struct s_lexem
	{
		ParserJSON::e_lexem lexem;
		std::string value;
	} typedef t_lexem;

	ParserJSON(std::string const &src);
	std::string const &getRaw() const;
	bool get_key(std::string const &key,
							 std::vector<ParserJSON::t_lexem>::const_iterator &dst,
							 std::vector<ParserJSON::t_lexem>::const_iterator start) const;
	bool get_key(std::string const &key,
							 std::vector<ParserJSON::t_lexem>::const_iterator &dst) const;

	template <typename T>
	int key(std::string const &key,
					 T &dst,
					 bool (&func)(ParserJSON const &, std::vector<ParserJSON::t_lexem>::const_iterator const &, T &),
					 std::vector<ParserJSON::t_lexem>::const_iterator start) const
	{
		if (this->get_key(key, start, start))
			return 2;
		return func(*this, start + 1, dst);
	}

	template <typename T>
	int key(std::string const &key,
					 T &dst,
					 bool (&func)(ParserJSON const &, std::vector<ParserJSON::t_lexem>::const_iterator const &, T &)) const
	{
		return this->key(key, dst, func, this->_lexems.begin());
	}

	template <typename T, typename U>
	int keys(T const &keys,
						U &dst,
						bool (&func)(ParserJSON const &, std::vector<ParserJSON::t_lexem>::const_iterator const &, T &),
						std::vector<ParserJSON::t_lexem>::const_iterator start) const
	{
		if (keys.empty())
			return (true);
		if (this->get_keys(keys, start, start))
			return (2);
		return func(*this, start + 1, dst);
	}

	template <typename T, typename U>
	int keys(T const &keys,
						U &dst,
						bool (&func)(ParserJSON const &, std::vector<ParserJSON::t_lexem>::const_iterator const &, T &)) const
	{
		return this->keys(keys, dst, func, this->_lexems.begin());
	}

	template <typename T>
	bool get_keys(T const &keys,
								std::vector<ParserJSON::t_lexem>::const_iterator &dst,
								std::vector<ParserJSON::t_lexem>::const_iterator start) const
	{
		if (keys.empty())
			return (true);
		for (typename T::const_iterator key = keys.begin(); key != keys.end(); ++key)
		{
			if (this->get_key(*key, dst, start))
				return (true);
			start = dst + 1;
		}

		return (false);
	}

	template <typename T>
	bool get_keys(T const &keys,
								std::vector<ParserJSON::t_lexem>::const_iterator &dst) const
	{
		return this->get_keys(keys, dst, this->_lexems.begin());
	}

	template <typename T>
	int key_map(std::string const &key,
							 T &dst,
							 bool (&func)(ParserJSON const &, std::vector<ParserJSON::t_lexem>::const_iterator const &, T &),
							 std::vector<ParserJSON::t_lexem>::const_iterator start) const
	{
		std::vector<ParserJSON::t_lexem>::const_iterator it;

		if (this->get_key(key, it, start))
			return (2);
		return this->arr(it + 1, dst, func);
	}

	template <typename T>
	bool key_map(std::string const &key,
							 T &dst,
							 bool (&func)(ParserJSON const &, std::vector<ParserJSON::t_lexem>::const_iterator const &, T &)) const
	{
		return this->key_map(key, dst, func, this->_lexems.begin());
	}


	template <typename T>
	bool arr(std::vector<ParserJSON::t_lexem>::const_iterator it,
					 T &dst,
					 bool (&func)(ParserJSON const &, std::vector<ParserJSON::t_lexem>::const_iterator const &, T &)) const
	{
		if (it->lexem != OPEN_ARR)
			return true;
		++it;

		for (; it != this->_lexems.end() && it->lexem != CLOSE_ARR; ++it)
		{
			if (func(*this, it, dst))
				return true;
			int depth = 0;
			for (; it != this->_lexems.end(); ++it)
			{
				if (it->lexem == ParserJSON::OPEN_OBJ || it->lexem == ParserJSON::OPEN_ARR)
					++depth;
				else if (it->lexem == ParserJSON::CLOSE_OBJ || it->lexem == ParserJSON::CLOSE_ARR)
					--depth;
				if (depth == 0)
					break;
			}
		}
		return false;
	}

	template <typename T>
	static bool to_number(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const &lexem_value, T &dst)
	{
		(void)json;
		if (lexem_value->lexem != ParserJSON::NUMBER)
			return (true);
		std::stringstream str(lexem_value->value);
		str >> dst;
		std::stringstream ss;
		ss << dst;
		std::string dst_string = ss.str();

		return dst_string != lexem_value->value;
	}

	static bool to_bool(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const &lexem_value, bool &dst);
	static bool to_string(ParserJSON const &json, std::vector<ParserJSON::t_lexem>::const_iterator const &lexem_value, std::string &dst);

	std::string toString() const;
	virtual ~ParserJSON();

private:
	enum e_state
	{
		STATE_IN_VALUE,
		STATE_IN_OBJECT,
		STATE_IN_ARRAY,
		STATE_IN_KEY
	};

	// TOKENIZER
	enum e_token
	{
		TOKEN_OPEN_SQUARE,	 //[
		TOKEN_CLOSE_SQUARE,	 //]
		TOKEN_OPEN_BRACKET,	 //{
		TOKEN_CLOSE_BRACKET, //}
		TOKEN_COLON,				 //:
		TOKEN_COMMA,				 //,
		TOKEN_QUOTE,				 //"

		TOKEN_WHITE,
		TOKEN_NL,
		TOKEN_CHAR,
		TOKEN_NUMBER
	};

	struct s_token
	{
		ParserJSON::e_token token;
		std::string value;
	} typedef t_token;

	std::string const _raw;
	std::vector<ParserJSON::t_lexem> _lexems;

	void tokenizer(std::vector<t_token> &tokens) const;
	e_token get_token(char c) const;

	void lexer(std::vector<t_token> &tokens);
	void skip_white_and_nl(std::vector<ParserJSON::t_token>::const_iterator &it,
												 std::vector<ParserJSON::t_token>::const_iterator const &end) const;
	ParserJSON::t_lexem parse_string(std::vector<ParserJSON::t_token>::const_iterator &it,
																	 std::vector<ParserJSON::t_token>::const_iterator const &end) const;
	ParserJSON::t_lexem parse_bool(std::vector<ParserJSON::t_token>::const_iterator &it,
																 std::vector<ParserJSON::t_token>::const_iterator const &end) const;
	ParserJSON::t_lexem parse_number(std::vector<ParserJSON::t_token>::const_iterator &it,
																	 std::vector<ParserJSON::t_token>::const_iterator const &end) const;
	ParserJSON::t_lexem state_parse_key(std::vector<ParserJSON::t_token>::const_iterator &it,
																			std::vector<ParserJSON::t_token>::const_iterator const &end) const;
	void state_in_object(std::vector<ParserJSON::e_state> &states,
											 std::vector<ParserJSON::t_token>::const_iterator &it);
	void state_in_array(std::vector<ParserJSON::e_state> &states,
											std::vector<ParserJSON::t_token>::const_iterator &it);
	void state_in_value(std::vector<ParserJSON::e_state> &states,
											std::vector<ParserJSON::t_token>::const_iterator &it,
											std::vector<ParserJSON::t_token>::const_iterator const &end);
	bool token_is_open(t_lexem const &l) const;
	ParserJSON::t_lexem token_to_lexem(t_token const &token) const;
	void check_unique_keys() const;

	void parse();
	// Colpien
	ParserJSON();
	ParserJSON(ParserJSON const &rhs);
	ParserJSON &operator=(ParserJSON const &rhs);
};

std::ostream &operator<<(std::ostream &out, ParserJSON const &parser);

#endif