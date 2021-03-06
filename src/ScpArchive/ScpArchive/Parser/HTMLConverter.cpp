#include "HTMLConverter.hpp"

#include "Rules/RuleSet.hpp"

#include <ctime>
#include <iomanip>

namespace Parser{
    
    void convertNodeToHtml(const HtmlContext& con, const Node& nod){
        std::vector<HtmlRule> rules = getHtmlRules();
        for(const HtmlRule& rule : rules){
            if(rule.type == nod.getType()){
                rule.handleRule(con, nod);
                return;
            }
        }
        throw std::runtime_error("Attempted to convert a node to HTML, but no HtmlRule was defined for this node type");
    }
    
    void delegateNodeBranches(const HtmlContext& con, const Node& nod){
        for(const Node& i : nod.branches){
            convertNodeToHtml(con, i);
        }
    }
	
	std::string getUniqueHtmlId(const HtmlContext& con){
        std::size_t uniqueId = con.uniqueId;
        con.uniqueId++;
        return "unique-id-" + std::to_string(uniqueId);
	}
	
	void convertPageTreeToHtml(MarkupOutStream& out, const PageTree& tree, bool linkImagesLocally){
		HtmlContext context{out, 0, linkImagesLocally};
		convertNodeToHtml(context, tree.pageRoot);
	}
	
	namespace{
		std::array<std::string, 48> colors = { 
			"FF0000", "00FF00", "0000FF", "FFFF00", "FF00FF", "00FFFF",
			"800000", "008000", "000080", "808000", "800080", "008080",
			"C00000", "00C000", "0000C0", "C0C000", "C000C0", "00C0C0",
			"400000", "004000", "000040", "404000", "400040", "004040",
			"200000", "002000", "000020", "202000", "200020", "002020",
			"600000", "006000", "000060", "606000", "600060", "006060",
			"A00000", "00A000", "0000A0", "A0A000", "A000A0", "00A0A0",
			"E00000", "00E000", "0000E0", "E0E000", "E000E0", "00E0E0"
		};//just some basic colors to make the annotated html look nice
		//thank you, https://stackoverflow.com/questions/309149/generate-distinctly-different-rgb-colors-in-graphs#309193
		std::string getColor(int i, bool hover, bool opaque = false){
			constexpr auto toNum = [](char c)->int{
				if(std::isalpha(c)){
					return c - 'A' + 10;
				}
				else{
					return c - '0';
				}
			};
			std::string raw = colors[i % colors.size()];
			
			int r = toNum(raw[0]) * 16 + toNum(raw[1]);
			int g = toNum(raw[2]) * 16 + toNum(raw[3]);
			int b = toNum(raw[4]) * 16 + toNum(raw[5]);
			if(opaque){
				return "rgba("  + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ")";
			}
			if(hover){
				return "rgba("  + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", 0.5)";
			}
			else{
				return "rgba("  + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) + ", 0.25)";
			}
		}
	}
	
	void convertTokenedPageToHtmlAnnotations(MarkupOutStream& out, const TokenedPage& page){
		std::size_t pos = 0; 
		auto tok = page.tokens.begin();
		
		while(pos < page.originalPage.size()){
			if(tok != page.tokens.end() && pos == tok->sourceStart){
				out << "<span style='background-color:"_AM << getColor(tok->token.index(), false) << ";' "_AM
				<< " onMouseOver='this.style.backgroundColor=\""_AM << getColor(tok->token.index(), true) << "\"'"_AM
				<< " onMouseOut='this.style.backgroundColor=\""_AM << getColor(tok->token.index(), false) << "\"'"_AM
				<< " title='"_AM << printToken(*tok).dump(4) << "'>"_AM;
				for(char c : tok->source){
					if(c == '\n'){
						out << "\u00B6<br />"_AM;
					}
					else if(c == ' '){
						out << "&ensp;"_AM;
					}
					else{
						out << c;
					}
				}
				out << "</span>"_AM;
				pos = tok->sourceEnd;
				tok++;
			}
			else{
				if(page.originalPage[pos] == '\n'){
					out << "\u00B6<br />"_AM;
				}
				else if(page.originalPage[pos] == ' '){
						out << "&ensp;"_AM;
					}
				else{
					out << page.originalPage[pos];
				}
				pos++;
			}
		}
		
	}
	
	void convertNodeToHtmlAnnotations(MarkupOutStream& out, const Node& nod){
		out << "<div style='background-color:white;'><div style='border-style:solid;border-color:white;border-width:1px;padding:0.5rem;"_AM
		<< "background-color:"_AM << getColor(nod.node.index(), false) << ";'"_AM
		<< " onMouseOver='this.style.backgroundColor=\""_AM << getColor(nod.node.index(), true) << "\"'"_AM
		<< " onMouseOut='this.style.backgroundColor=\""_AM << getColor(nod.node.index(), false) << "\"'"_AM
		">"_AM << NodeTypeNames.at(nod.node.index());
		{
			out << "<div style='word-wrap:break-word;margin:0.5rem;border-style:solid;border-width:1px;border-color:black;'>"_AM;
			std::string content = printNodeVariant(nod).dump(4);
			for(char c : content){
				if(c == '\n'){
					out << "<br>"_AM;
				}
				else if(c == ' '){
					out << "&nbsp;"_AM;
				}
				else{
					out << c;
				}
			}
			out << "</div>"_AM;
		}
		for(const Node& branch : nod.branches){
			convertNodeToHtmlAnnotations(out, branch);
		}
		out << "</div></div>"_AM;
	}
	
	void convertPageTreeToHtmlAnnotations(MarkupOutStream& out, const PageTree& page){
		convertNodeToHtmlAnnotations(out, page.pageRoot);
	}
	
	void toHtmlShownAuthor(MarkupOutStream& out, const ShownAuthor& author){
		switch(author.type){
			default:
				throw std::runtime_error("Attempted to print ShownAuthor with invalid Type");
				break;
			case ShownAuthor::Type::System:
				out << "[SYSTEM]";
				break;
			case ShownAuthor::Type::User:
				out << "<a href='http://www.wikidot.com/user:info/"_AM << author.linkName << "'>"_AM << author.shownName << "</a>"_AM;
				break;
			case ShownAuthor::Type::Deleted:
				out << "[DELETED]";
				break;
		}
	}
	
	std::string formatTimeStamp(TimeStamp timeStamp){
		std::stringstream str;
		str << std::put_time(std::gmtime(&timeStamp), "%A %d %B %Y, %T") << " UTC";
		return str.str();
	}
	
	std::string redirectLink(std::string rawLink){
		std::string output;
		const auto doRedirect = [&rawLink, &output](std::string oldPrefix, std::string newPrefix){
			if(output == "" && check(rawLink, 0, oldPrefix)){
				output = newPrefix + rawLink.substr(oldPrefix.size(), rawLink.size() - oldPrefix.size());
			}
		};
		doRedirect("http://www.scp-wiki.net/local--files/", "/__system/pageFile/");
		doRedirect("http://www.scp-wiki.net/", "/");
		doRedirect("http://scp-wiki.wikidot.com/", "/");
		doRedirect("http://scp-wiki.wdfiles.com/local--files/", "/__system/pageFile/");
		if(output == ""){
			return rawLink;
		}
		else{
			return output;
		}
	}
	
	
}


