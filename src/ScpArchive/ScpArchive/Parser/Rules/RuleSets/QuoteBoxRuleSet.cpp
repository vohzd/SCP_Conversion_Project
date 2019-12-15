#include "QuoteBoxRuleSet.hpp"

namespace Parser{
    std::string toStringTokenQuoteBoxPrefix(const TokenVariant& tok){
        const QuoteBoxPrefix& quoteBoxPrefix = std::get<QuoteBoxPrefix>(tok);
        return "QuoteBoxPrefix:" + std::to_string(quoteBoxPrefix.degree);
    }
    
    std::string toStringNodeQuoteBox(const NodeVariant& nod){
        return "QuoteBox";
    }
    
    bool tryQuoteBoxPrefixRule(const TokenRuleContext& context){
		if(context.tokens.size() > 0 && context.tokens.back().getType() == Token::Type::QuoteBoxPrefix){
			return false;//a quote box prefix cannot directly follow another quote box
		}
		if(context.wasNewLine && check(context.page, context.pagePos, ">")){
			std::size_t pos = context.pagePos;
			while(pos < context.page.size()){
				if(check(context.page, pos, ">")){
					//keep going
				}
				else if(check(context.page, pos, " ") || check(context.page, pos, "\n")){
					return true;
				}
				else{
					return false;//fail if the quote box prefix ends with anything other than a space
				}
				pos++;
			}
		}
		return false;
	}
	
	TokenRuleResult doQuoteBoxPrefixRule(const TokenRuleContext& context){
		std::size_t pos = context.pagePos;
		QuoteBoxPrefix prefix;
		while(pos < context.page.size()){
			if(check(context.page, pos, " ")){
				prefix.degree = pos - context.pagePos;
				pos++;//skip the space too
				break;
			}
			else if(check(context.page, pos, "\n")){
				prefix.degree = pos - context.pagePos;
				break;
			}
			pos++;
		}
		
		
		TokenRuleResult result;
        result.newPos = pos;
        result.nowNewline = true;//preserve newline status
        result.newTokens.push_back(Token{prefix, context.pagePos, pos, context.page.substr(context.pagePos, pos - context.pagePos)});
        return result;
	}
}
