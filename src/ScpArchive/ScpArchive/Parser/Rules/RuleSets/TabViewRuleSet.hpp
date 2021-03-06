#ifndef TABVIEWRULESET_HPP
#define TABVIEWRULESET_HPP

#include "../RuleSet.hpp"

namespace Parser{
    nlohmann::json printNodeTabView(const NodeVariant& nod);
    nlohmann::json printNodeTab(const NodeVariant& nod);
	
    void handleTabView(TreeContext& context, const Token& token);
    void handleTab(TreeContext& context, const Token& token);
	
	void toHtmlNodeTabView(const HtmlContext& con, const Node& nod);
	void toHtmlNodeTab(const HtmlContext& con, const Node& nod);
	
	const inline auto tabViewRuleSet = RuleSet{"TabView", {
        NodePrintRule{Node::Type::TabView, printNodeTabView},
        NodePrintRule{Node::Type::Tab, printNodeTab},
        
        SectionRule{SectionType::TabView, {"tabview"}, SubnameType::None, ModuleType::Unknown, {},
                ContentType::Surround, ParameterType::None, false},
        SectionRule{SectionType::Tab, {"tab"}, SubnameType::Parameter, ModuleType::Unknown, {},
                ContentType::Surround, ParameterType::None, false},
		
        TreeRule{{Token::Type::Section, SectionType::TabView}, handleTabView},
        TreeRule{{Token::Type::Section, SectionType::Tab}, handleTab},
        
        HtmlRule{Node::Type::TabView, toHtmlNodeTabView},
        HtmlRule{Node::Type::Tab, toHtmlNodeTab}
	}};
	
}

#endif // TABVIEWRULESET_HPP
