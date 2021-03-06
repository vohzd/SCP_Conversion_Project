#ifndef LISTRULESET_HPP
#define LISTRULESET_HPP

#include "../RuleSet.hpp"

namespace Parser{
    nlohmann::json printTokenListPrefix(const TokenVariant& tok);
    
    nlohmann::json printNodeList(const NodeVariant& nod);
    nlohmann::json printNodeListElement(const NodeVariant& nod);
	
    nlohmann::json printNodeAdvList(const NodeVariant& nod);
    nlohmann::json printNodeAdvListElement(const NodeVariant& nod);
	
	bool tryListPrefixRule(const TokenRuleContext& context);
	TokenRuleResult doListPrefixRule(const TokenRuleContext& context);
	
	void toHtmlNodeList(const HtmlContext& con, const Node& nod);
	void toHtmlNodeListElement(const HtmlContext& con, const Node& nod);
	
    void handleAdvList(TreeContext& context, const Token& token);
    void handleAdvListElement(TreeContext& context, const Token& token);
	
	void toHtmlNodeAdvList(const HtmlContext& con, const Node& nod);
	void toHtmlNodeAdvListElement(const HtmlContext& con, const Node& nod);
	
	const inline auto listRuleSet = RuleSet{"List", {
	    TokenPrintRule{Token::Type::ListPrefix, printTokenListPrefix},
	    NodePrintRule{Node::Type::List, printNodeList},
	    NodePrintRule{Node::Type::ListElement, printNodeListElement},
	    NodePrintRule{Node::Type::AdvList, printNodeAdvList},
	    NodePrintRule{Node::Type::AdvListElement, printNodeAdvListElement},
	    
		TokenRule{tryListPrefixRule, doListPrefixRule},
		
		HtmlRule{Node::Type::List, toHtmlNodeList},
		HtmlRule{Node::Type::ListElement, toHtmlNodeListElement},
		
        SectionRule{SectionType::AdvList, {"ol", "ol_", "ul", "ul_"}, SubnameType::None, ModuleType::Unknown, {},
                ContentType::Surround, ParameterType::Quoted, true},
		SectionRule{SectionType::AdvListElement, {"li", "li_"}, SubnameType::None, ModuleType::Unknown, {},
                ContentType::Surround, ParameterType::Quoted, true},
		
        TreeRule{{Token::Type::Section, SectionType::AdvList}, handleAdvList},
        TreeRule{{Token::Type::Section, SectionType::AdvListElement}, handleAdvListElement},
		
		HtmlRule{Node::Type::AdvList, toHtmlNodeAdvList},
		HtmlRule{Node::Type::AdvListElement, toHtmlNodeAdvListElement}
	}};
	
}

#endif // LISTRULESET_HPP
