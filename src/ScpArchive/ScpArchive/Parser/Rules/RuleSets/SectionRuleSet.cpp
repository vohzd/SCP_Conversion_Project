#include "SectionRuleSet.hpp"

#include <sstream>

namespace Parser{
    nlohmann::json printTokenSection(const TokenVariant& tok){
        const Section& section = std::get<Section>(tok);
        nlohmann::json out;
        out["typeString"] = section.typeString;
        out["mainParameter"] = section.mainParameter;
        out["parameters"] = nlohmann::json::object();
        for(auto i = section.parameters.begin(); i != section.parameters.end(); i++){
            out["parameters"][i->first] = i->second;
        }
        return out;
    }
    
    nlohmann::json printTokenSectionStart(const TokenVariant& tok){
        const SectionStart& section = std::get<SectionStart>(tok);
        nlohmann::json out;
        out["typeString"] = section.typeString;
        out["mainParameter"] = section.mainParameter;
        out["parameters"] = nlohmann::json::object();
        for(auto i = section.parameters.begin(); i != section.parameters.end(); i++){
            out["parameters"][i->first] = i->second;
        }
        return out;
    }
    
    nlohmann::json printTokenSectionEnd(const TokenVariant& tok){
        const SectionEnd& section = std::get<SectionEnd>(tok);
        return section.typeString;
    }
    
    nlohmann::json printTokenSectionComplete(const TokenVariant& tok){
        const SectionComplete& section = std::get<SectionComplete>(tok);
        nlohmann::json out;
        out["typeString"] = section.typeString;
        out["mainParameter"] = section.mainParameter;
        out["parameters"] = nlohmann::json::object();
        for(auto i = section.parameters.begin(); i != section.parameters.end(); i++){
            out["parameters"][i->first] = i->second;
        }
        out["contents"] = section.contents;
        return out;
    }
    
    namespace{
        inline void toLower(std::string& str){
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        }
        
        inline std::string getSectionContent(const TokenRuleContext& context, std::size_t& pos){
            std::string content;
            {
                int layer = 0;
                while(pos < context.page.size()){
                    if(context.page[pos] == '['){
                        layer++;
                    }
                    else if(context.page[pos] == ']'){
                        layer--;
                        
                    }
                    content += context.page[pos];
                    if(layer == 0){
                        break;
                    }
                    pos++;
                }
                if(layer != 0){
                    return "";
                }
                pos++;//go one past the "]]"
            }
            //remove the outside [[ ... ]]
            content = content.substr(1, content.size() - 2);
            trimString(content);
            content = content.substr(1, content.size() - 2);
            trimString(content);
            return content;
        }
        
        inline void getName(std::string& content, std::string& name){
            std::size_t findPos = std::string::npos;
            for(std::size_t pos = 0; pos < content.size(); pos++){
                if(isspace(content[pos])){
                    findPos = pos;
                    break;
                }
            }
            
            if(findPos != std::string::npos){
                name = content.substr(0, findPos);
                content.erase(0, findPos);
                trimString(content);
            }
            else{
                name = content;
                content.clear();
            }
        }
        
        inline const std::optional<SectionRule> findMatchingSection(std::string content){
            if(content.size() > 0 && content[0] == '/'){
                content = content.substr(1, content.size() - 1);
                trimString(content);
                toLower(content);
                
                const std::vector<SectionRule> sectionRules = getSectionRules();
                for(auto i = sectionRules.begin(); i != sectionRules.end(); i++){
                    const SectionRule& rule = *i;
                    if(rule.contentType == ContentType::Surround){
                        if(std::find(rule.matchingNames.begin(), rule.matchingNames.end(), content) != rule.matchingNames.end()){
                            return *i;
                        }
                    }
                }
                return {};
            }
            else{
                std::string name;
                std::string subName;
                getName(content, name);
                toLower(name);
                getName(content, subName);
                toLower(subName);
                
                const std::vector<SectionRule> sectionRules = getSectionRules();
                for(auto i = sectionRules.begin(); i != sectionRules.end(); i++){
                    const SectionRule& rule = *i;
                    if(std::find(rule.matchingNames.begin(), rule.matchingNames.end(), name) != rule.matchingNames.end()){
                        if(rule.subnameType == SubnameType::None){
                            return *i;
                        }
                        if(rule.subnameType == SubnameType::Parameter && subName != ""){
                            return *i;
                        }
                        if(rule.subnameType == SubnameType::Module){
                            if(std::find(rule.matchingModules.begin(), rule.matchingModules.end(), subName) != rule.matchingModules.end()){
                                return *i;
                            }
                        }
                    }
                }
                return {};
            }
        }
    }
    
    bool trySectionRule(const TokenRuleContext& context){
        if(context.page.size() > context.pagePos + 4){
            if(context.page[context.pagePos] == '[' && context.page[context.pagePos + 1] == '['){
                std::size_t pos = context.pagePos;
                std::string content = getSectionContent(context, pos);
                if(findMatchingSection(content) != std::nullopt){
                    return true;
                }
            }
        }
        return false;
    }
    
	TokenRuleResult doSectionRule(const TokenRuleContext& context){
        std::size_t pos = context.pagePos;
        std::string content = getSectionContent(context, pos);
        const SectionRule rule = *findMatchingSection(content);
        
        TokenRuleResult result;
        
        if(rule.contentType == ContentType::Surround && content[0] == '/'){
            content = content.substr(1, content.size() - 1);
            trimString(content);
            SectionEnd section;
            section.type = rule.type;
            section.typeString = content;
            result.newTokens.push_back(Token{section, context.pagePos, pos, context.page.substr(context.pagePos, pos - context.pagePos)});
        }
        else{
            Section section;
            section.type = rule.type;
            getName(content, section.typeString);
            
            if(rule.subnameType != SubnameType::None){
                if(rule.parameterType == ParameterType::None && rule.subnameType == SubnameType::Parameter){
                    //if there are no parameters and it wants a parameter, give it the whole of content
                    trimString(content);
                    section.mainParameter = content;
                }
                else{
                    getName(content, section.mainParameter);
                }
                
                if(rule.subnameType == SubnameType::Module){
                    section.moduleType = rule.moduleType;
                }
            }
            
            const auto setSectionParameter = [&section](std::string key, std::string value){
                if(section.parameters.find(key) == section.parameters.end()){
                    section.parameters[key] = value;
                }
                else{
                    //strange behavior where included things get overwritten, I don't understand the exact behavior but this should work so I will do it for now
                    std::string previousValue = section.parameters[key];
                    if(check(previousValue, 0, "{$") && check(previousValue, previousValue.size() - 1, "}")){//if this looks like an include parameter
                        //then we can override it
                        section.parameters[key] = value;
                    }
                }
            };
            
            if(rule.parameterType == ParameterType::Lined){
                std::stringstream parameterStream(content);
                std::string line;
                while(std::getline(parameterStream, line, '|')){
                    trimString(line);
                    if(line != ""){
                        std::size_t findPos = line.find('=');
                        if(findPos == std::string::npos){
                            setSectionParameter(line, "");
                        }
                        else{
                            std::string key = line.substr(0, findPos);
                            std::string value = line.substr(findPos + 1, line.size() - findPos - 1);
                            trimString(key);
                            trimString(value);
                            setSectionParameter(key, value);
                        }
                    }
                }
            }
            else if(rule.parameterType == ParameterType::Quoted){
                struct HelperToken{
                    enum class Type{Key, Value, Equals};
                    Type type;
                    std::string content;
                };
                
                std::vector<HelperToken> tokens;
                for(std::size_t tpos = 0; tpos < content.size();){
                    if(content[tpos] == '='){
                        tokens.push_back(HelperToken{HelperToken::Type::Equals});
                        tpos++;
                    }
                    else if(content[tpos] == '"'){
                        tpos++;
                        std::string value;
                        while(tpos < content.size()){
                            char c = content[tpos];
                            if(c == '"'){
                                tpos++;
                                break;
                            }
                            value += c;
                            tpos++;
                        }
                        tokens.push_back(HelperToken{HelperToken::Type::Value, value});
                    }
                    else if(isspace(content[tpos])){
                        tpos++;
                    }
                    else{
                        std::string key;
                        while(tpos < content.size()){
                            char c = content[tpos];
                            if(c == '=' || isspace(c) || c == '"'){
                                break;
                            }
                            key += c;
                            tpos++;
                        }
                        tokens.push_back(HelperToken{HelperToken::Type::Key, key});
                    }
                }
                
                for(std::size_t tpos = 0; tpos < tokens.size();){
                    if(tokens[tpos].type == HelperToken::Type::Key){
                        std::string key = tokens[tpos].content;
                        tpos++;
                        if(tpos < tokens.size() && tokens[tpos].type == HelperToken::Type::Equals){
                            tpos++;
                            if(tpos < tokens.size() && tokens[tpos].type == HelperToken::Type::Value){
                                std::string value = tokens[tpos].content;
                                setSectionParameter(key, value);
                                tpos++;
                            }
                            else{
                                setSectionParameter(key, "");
                            }
                        }
                        else{
                            setSectionParameter(key, "");
                        }
                    }
                    else{
                        //ignore everything else
                        tpos++;
                    }
                }
            }
            
            if(rule.contentType == ContentType::None){
                result.newTokens.push_back(Token{section, context.pagePos, pos, context.page.substr(context.pagePos, pos - context.pagePos)});
            }
            else if(rule.contentType == ContentType::Surround){
                SectionStart sectionStart;
                sectionStart.type = section.type;
                sectionStart.typeString = section.typeString;
                sectionStart.moduleType = section.moduleType;
                sectionStart.mainParameter = section.mainParameter;
                sectionStart.parameters = section.parameters;
                result.newTokens.push_back(Token{sectionStart, context.pagePos, pos, context.page.substr(context.pagePos, pos - context.pagePos)});
            }
            else if(rule.contentType == ContentType::Contain){
                SectionComplete sectionComplete;
                sectionComplete.type = section.type;
                sectionComplete.typeString = section.typeString;
                sectionComplete.moduleType = section.moduleType;
                sectionComplete.mainParameter = section.mainParameter;
                sectionComplete.parameters = section.parameters;
                std::size_t contentStart = pos;
                while(pos < context.page.size()){
                    if(check(context.page, pos, "[[/")){
                        std::size_t tempPos = pos;
                        std::string endContent = getSectionContent(context, tempPos);
                        if(check(endContent, 0, "/")){
							endContent = endContent.substr(1, endContent.size() - 1);//remove the first /
							trimString(endContent);
							toLower(endContent);
							if(endContent == section.typeString){
								break;
							}
                        }
                    }
					pos++;
                }
                sectionComplete.contents = context.page.substr(contentStart, pos - contentStart);
                if(rule.allowInline == false && check(sectionComplete.contents, 0, "\n")){//non-inline don't need that first \n
					sectionComplete.contents.erase(sectionComplete.contents.begin());
                }
                if(pos + 4 < context.page.size()){
                    getSectionContent(context, pos);//go to the end of the [[/...]]
                }
                
                result.newTokens.push_back(Token{sectionComplete, context.pagePos, pos, context.page.substr(context.pagePos, pos - context.pagePos)});
            }
        }
        
        result.newPos = pos;
        return result;
	}
}
