#include "FootNoteRuleSet.hpp"

namespace Parser{
	std::string toStringNodeFootNote(const NodeVariant& nod){
        return "FootNote:" + std::to_string(std::get<FootNote>(nod).number);
	}
	
	std::string toStringNodeFootNoteBlock(const NodeVariant& nod){
        return "FootNoteBlock:\"" + std::get<FootNoteBlock>(nod).title + "\"";
	}
	
    void handleFootNote(TreeContext& context, const Token& token){
        handleSectionStartEnd(token, 
        [&](const SectionStart& section){
            makeTextAddable(context);
            pushStack(context, Node{FootNote{}});
        }, [&](const SectionEnd& section){
            if(isInside(context, Node::Type::FootNote)){
                popSingle(context, Node::Type::FootNote);
            }
        });
    }
    
    void handleFootNoteBlock(TreeContext& context, const Token& token){
        const Section& section = std::get<Section>(token.token);
        FootNoteBlock block;
        if(section.parameters.find("title") != section.parameters.end()){
            block.title = section.parameters.find("title")->second;
        }
        else{
            block.title = "Footnotes";
        }
        addAsDiv(context, Node{block});
    }
	
	void postTreeRuleFootNotes(TreeContext& context){
        std::vector<Node> footnotes;
        bool foundBlock = false;
        travelPageTreeNodes(context.stack.back(), [&foundBlock, &footnotes](Node& node)->bool{
            Node::Type type = node.getType();
            if(type == Node::Type::FootNoteBlock || type == Node::Type::TableOfContents){
                foundBlock = true;
                return false;
            }
            if(type == Node::Type::FootNote){
                FootNote& footnote = std::get<FootNote>(node.node);
                footnote.number = footnotes.size() + 1;
                footnotes.push_back(node);
            }
            return true;
        });
        if(!foundBlock && footnotes.size() > 0){
            addAsDiv(context, Node{FootNoteBlock{"Footnotes"}});
        }
        travelPageTreeNodes(context.stack.back(), [&footnotes](Node& node)->bool{
            Node::Type type = node.getType();
            if(type == Node::Type::FootNoteBlock){
                node.branches = footnotes;
                return false;
            }
            return true;
        });
	}
	
	void toHtmlNodeFootNote(const HtmlContext& con, const Node& nod){
        const FootNote& footnote = std::get<FootNote>(nod.node);
        con.out << "<sup>["_AM << std::to_string(footnote.number) << "]</sup>"_AM;
	}
	
	void toHtmlNodeFootNoteBlock(const HtmlContext& con, const Node& nod){
        if(nod.branches.size() > 0){
            const FootNoteBlock& block = std::get<FootNoteBlock>(nod.node);
            con.out << "<h2>"_AM << block.title << "</h2>"_AM
            << "<ol>"_AM;
            for(const Node& branch : nod.branches){
                con.out << "<li>"_AM;
                delegateNodeBranches(con, branch);
                con.out << "</li>"_AM;
            }
            con.out << "</ol>"_AM;
        }
	}
}