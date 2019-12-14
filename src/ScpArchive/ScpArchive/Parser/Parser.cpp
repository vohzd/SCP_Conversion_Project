#include "Parser.hpp"

#include "Rules/RuleSet.hpp"

#include <sstream>

namespace Parser{
	std::string& trimLeft(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}
	// trim from end
	std::string& trimRight(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}
	// trim from both ends
	std::string& trimString(std::string& s) {
		return trimLeft(trimRight(s));
	}
	
	std::string normalizePageName(std::string link){
		std::string output;
		for(char c : link){
			if(isalnum(c) || c == '#'){
				output += tolower(c);
			}
			else if(c == ':'){
				while(output.size() > 0 && output.back() == '-'){//remove trailing white space
					output.erase(output.size() - 1);
				}
				output += ':';
			}
			else if(isspace(c) || c == '-' || c == '.'){
				if(output.size() == 0 || output.back() == ':'){
					//do nothing
				}
				else{
				output += '-';
				}
			}
		}
		while(output.size() > 0 && output.back() == '-'){//remove trailing white space
			output.erase(output.size() - 1);
		}
		return output;
	}
	
	bool Section::operator==(const Section& tok)const{
		return type == tok.type && typeString == tok.typeString && moduleType == tok.moduleType && mainParameter == tok.mainParameter && parameters == tok.parameters;
	}
		
	bool SectionEnd::operator==(const SectionEnd& tok)const{
		return type == tok.type && typeString == tok.typeString;
	}
	
	bool SectionComplete::operator==(const SectionComplete& tok)const{
		return type == tok.type && typeString == tok.typeString && moduleType == tok.moduleType && mainParameter == tok.mainParameter && parameters == tok.parameters && contents == tok.contents;
	}
	
	bool Divider::operator==(const Divider& tok)const{
		return type == tok.type;
	}
    
	bool Heading::operator==(const Heading& tok)const{
		return degree == tok.degree && hidden == tok.hidden;
	}
		
	bool QuoteBoxPrefix::operator==(const QuoteBoxPrefix& tok)const{
		return degree == tok.degree;
	}
	
    bool ListPrefix::operator==(const ListPrefix& tok)const{
        return type == tok.type && degree == tok.degree;
    }
	
	bool InlineFormat::operator==(const InlineFormat& tok)const{
		return type == tok.type && begin == tok.begin && end == tok.end && color == tok.color;
	}
	
	bool HyperLink::operator==(const HyperLink& tok)const{
		return shownText == tok.shownText && url == tok.url && newWindow == tok.newWindow;
	}
	
	bool LiteralText::operator==(const LiteralText& tok)const{
		return text == tok.text;
	}
		
	bool PlainText::operator==(const PlainText& tok)const{
		return text == tok.text;
	}
	
	bool LineBreak::operator==(const LineBreak& tok)const{
		return true;
	}
	
	bool NewLine::operator==(const NewLine& tok)const{
		return true;
	}
	
	Token::Type Token::getType()const{
		return static_cast<Type>(token.index());
	}
	
	bool Token::operator==(const Token& tok)const{
		return token == tok.token && sourceStart == tok.sourceStart && sourceEnd == tok.sourceEnd && source == tok.source;
	}
	
	std::string tokenVariantToString(const Token& tok){
		std::stringstream ss;
		
		switch(tok.getType()){
			default:
				ss << "Unknown";
				break;
            case Token::Type::Section:
				{
					const Section& section = std::get<Section>(tok.token);
                    ss << "Section:" << section.typeString << ", " << section.mainParameter << ", {";
                    for(auto i = section.parameters.begin(); i != section.parameters.end(); i++){
                        ss << i->first << ": " << i->second << ", ";
                    }
                    ss << "}";
				}
				break;
			case Token::Type::SectionStart:
				{
					const SectionStart& section = std::get<SectionStart>(tok.token);
                    ss << "SectionStart:" << section.typeString << ", " << section.mainParameter << ", {";
                    for(auto i = section.parameters.begin(); i != section.parameters.end(); i++){
                        ss << i->first << ": " << i->second << ", ";
                    }
                    ss << "}";
				}
				break;
			case Token::Type::SectionEnd:
				{
					const SectionEnd& section = std::get<SectionEnd>(tok.token);
                    ss << "SectionEnd:" << section.typeString;
				}
				break;
			case Token::Type::SectionComplete:
				{
					const SectionComplete& section = std::get<SectionComplete>(tok.token);
                    ss << "SectionComplete:" << section.typeString << ", " << section.mainParameter << ", {";
                    for(auto i = section.parameters.begin(); i != section.parameters.end(); i++){
                        ss << i->first << ": " << i->second << ", ";
                    }
                    ss << "}";
				}
				break;
			case Token::Type::Divider:
                {
					const Divider& divider = std::get<Divider>(tok.token);
                    ss << "Divider:";
                    switch(divider.type){
					default:
						ss << "Unknown";
						break;
					case Divider::Type::Line:
						ss << "Line";
						break;
					case Divider::Type::Clear:
						ss << "Clear";
						break;
                    }
				}
				break;
			case Token::Type::Heading:
                {
					const Heading& heading = std::get<Heading>(tok.token);
                    ss << "Heading:" << heading.degree << ", " << (heading.hidden?"true":"false");
				}
				break;
			case Token::Type::QuoteBoxPrefix:
				{
					const QuoteBoxPrefix& quoteBoxPrefix = std::get<QuoteBoxPrefix>(tok.token);
                    ss << "QuoteBoxPrefix:" << quoteBoxPrefix.degree;
				}
				break;
            case Token::Type::ListPrefix:
				{
					const ListPrefix& listPrefix = std::get<ListPrefix>(tok.token);
                    ss << "ListPrefix:";
                    switch(listPrefix.type){
                    default:
                        ss << "Unknown";
                        break;
                    case ListPrefix::Type::Bullet:
                        ss << "Bullet";
                        break;
                    case ListPrefix::Type::Number:
                        ss << "Number";
                        break;
                    }
                    ss << ", " << listPrefix.degree;
				}
				break;
			case Token::Type::InlineFormat:
				ss << "InlineFormat:";
				{
					const InlineFormat& format = std::get<InlineFormat>(tok.token);
					switch(format.type){
						default:
							ss << "Unknown";
							break;
						case InlineFormat::Type::Bold:
							ss << "Bold";
							break;
						case InlineFormat::Type::Italics:
							ss << "Italics";
							break;
						case InlineFormat::Type::Strike:
							ss << "Strike";
							break;
						case InlineFormat::Type::Underline:
							ss << "Underline";
							break;
						case InlineFormat::Type::Super:
							ss << "Super";
							break;
						case InlineFormat::Type::Sub:
							ss << "Sub";
							break;
						case InlineFormat::Type::Monospace:
							ss << "Monospace";
							break;
						case InlineFormat::Type::Color:
							ss << "Color";
							break;
					}
					ss << "[" << (format.begin?"true":"false") << "," << (format.end?"true":"false") << "," << format.color << "]";
				}
				break;
			case Token::Type::HyperLink:
				{
					const HyperLink& link = std::get<HyperLink>(tok.token);
					ss << "HyperLink:\"" << link.shownText << "\", \"" << link.url << "\", " << (link.newWindow?"true":"false");
				}
				break;
			case Token::Type::LiteralText:
				ss << "LiteralText:\"" << std::get<LiteralText>(tok.token).text << "\"";
				break;
			case Token::Type::PlainText:
				ss << "PlainText:\"" << std::get<PlainText>(tok.token).text << "\"";
				break;
			case Token::Type::LineBreak:
				ss << "LineBreak";
				break;
			case Token::Type::NewLine:
				ss << "NewLine";
				break;
		}
		return ss.str();
	}
	
	std::string toString(const Token& tok){
		
		std::stringstream ss;
		
		ss << "{";
		ss << tokenVariantToString(tok);
		ss << "} -> [" << tok.sourceStart << ", " << tok.sourceEnd << ") = \"";
		for(char c : tok.source){
			switch(c){
				default:
					ss << c;
					break;
				case '\n':
					ss << "\\n";
					break;
				case '\r':
					ss << "\\r";
					break;
				case '\t':
					ss << "\\t";
					break;
			}
		}
		ss << "\"";
		return ss.str();
	}
	
	std::ostream& operator<<(std::ostream& out, const Token& tok){
		out << toString(tok);
		return out;
	}
	
	namespace{
		TokenRuleContext applyTokenizingRules(std::string&& page, std::vector<TokenRule> rules){
			TokenRuleContext context;
			context.page = page;
			context.pagePos = 0;
			context.wasNewLine = true;
			for(; context.pagePos < context.page.size();){
				bool allRulesFailed = true;
				for(const TokenRule& rule : rules){
					if(rule.tryRule(context)){
						allRulesFailed = false;
						
						TokenRuleResult result = rule.doRule(context);
						context.pagePos = result.newPos;
						context.wasNewLine = result.nowNewline;
						context.tokens.insert(context.tokens.end(), result.newTokens.begin(), result.newTokens.end());
						
						break;
					}
				}
				if(allRulesFailed){
					throw std::runtime_error("All parsing rules failed, this should not be possible");
				}
			}
			return context;
		}
	}
	
	TokenedPage tokenizePage(std::string page){
		//just to save myself some headaches, lets replace all nonbreaking spaces with normal spaces
		//this might be refactored out in the future if the token rules are updated to deal with nonbreaking spaces more correctly
		const auto replaceAll = [](std::string str, const std::string& from, const std::string& to)->std::string{
            size_t start_pos = 0;
            while((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
            }
            return str;
        };
		
		page = replaceAll(page, {static_cast<char>(0b11000010), static_cast<char>(0b10100000)}, " ");
		
		TokenRuleContext context = applyTokenizingRules(std::move(page), getTokenRules());
		
		TokenedPage output;
		output.originalPage = std::move(context.page);
		
		//merge all PlainText Tokens
		for(const Token& tok : context.tokens){
			if(output.tokens.size() > 0){
				Token& back = output.tokens.back();
				if(back.getType() == Token::Type::PlainText && tok.getType() == Token::Type::PlainText
						&& back.sourceEnd == tok.sourceStart){
					PlainText& backText = std::get<PlainText>(back.token);
					const PlainText& tokText = std::get<PlainText>(tok.token);
					backText.text += tokText.text;
					back.source += tok.source;
					back.sourceEnd = tok.sourceEnd;
					
					continue;
				}
			}
			output.tokens.push_back(tok);
		}
		
		return output;
	}
	
	std::vector<std::string> getPageLinks(std::string page){
		const std::vector<TokenRule> rules = getTokenRules();
		/*{
			TokenRule{tryCommentRule, doCommentRule},
			TokenRule{tryTripleLinkRule, doTripleLinkRule},
			TokenRule{trySingleLinkRule, doSingleLinkRule},
			TokenRule{tryNullRule, doNullRule}
		};*/
		
		TokenRuleContext context = applyTokenizingRules(std::move(page), rules);
		std::vector<std::string> links;
		
		for(const Token& tok : context.tokens){
			if(tok.getType() == Token::Type::HyperLink){
				HyperLink link = std::get<HyperLink>(tok.token);
				if(!check(link.url, 0, "http://") && !check(link.url, 0, "https://")){
					links.push_back(link.url);
				}
			}
		}
		
		return links;
	}
}










