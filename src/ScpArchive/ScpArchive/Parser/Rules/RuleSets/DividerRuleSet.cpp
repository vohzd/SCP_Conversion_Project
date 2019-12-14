#include "DividerRuleSet.hpp"

namespace Parser{
    bool tryDividerRule(const TokenRuleContext& context){
		if(context.wasNewLine){
			const auto checkFunction = [&context](char c)->bool{
				std::size_t pos = context.pagePos;
				while(pos < context.page.size()){
					if(context.page[pos] == '\n'){
						break;
					}
					else if(context.page[pos] != c){
						return false;
					}
					pos++;
				}
				return true;
			};
			
			if(check(context.page, context.pagePos, "----")){
				return checkFunction('-');
			}
			else if(check(context.page, context.pagePos, "~~~~")){
				return checkFunction('~');
			}
		}
		return false;
	}
	
	TokenRuleResult doDividerRule(const TokenRuleContext& context){
		Divider divider;
		
		if(context.page[context.pagePos] == '-'){
			divider.type = Divider::Type::Line;
		}
		else if(context.page[context.pagePos] == '~'){
			divider.type = Divider::Type::Clear;
		}
		
		std::size_t pos = context.pagePos;
		while(pos < context.page.size()){
			if(context.page[pos] == '\n'){
				break;
			}
			pos++;
		}
		
		TokenRuleResult result;
        result.newPos = pos;
        result.newTokens.push_back(Token{divider, context.pagePos, pos, context.page.substr(context.pagePos, pos - context.pagePos)});
        return result;
	}
	
    void handleDivider(TreeContext& context, const Token& token){
        addAsDiv(context, Node{std::get<Divider>(token.token)});
    }
}