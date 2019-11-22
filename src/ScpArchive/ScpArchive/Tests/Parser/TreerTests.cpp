#include "TreerTests.hpp"

#include "../../Parser/Treer.hpp"

namespace Tests{
	using namespace Parser;
	
	namespace{
		void assertPageTree(std::string source, Node node){
			PageTree page = makeTreeFromPage(source);
			assertEquals(node, page.pageRoot);
		}
	}
	
	void addTreerTests(Tester& tester){
		tester.add("Parser::makeTreeFromPage basicParagraphs", [](){
			assertPageTree(
			"Some text here\n"
			"\n"
			"more text\n",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"Some text here"}}
						}
					},
					Node{Paragraph{},
						{
							Node{PlainText{"more text"}}
						}
					}
				}
			});
			assertPageTree(
			"Some text here\n"
			"more text",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"Some text here"}},
							Node{LineBreak{}},
							Node{PlainText{"more text"}}
						}
					}
				}
			});
			assertPageTree(
			"Some text here _\n"
			" _\n"
			" _\n"
			"more text",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"Some text here"}},
							Node{LineBreak{}},
							Node{LineBreak{}},
							Node{LineBreak{}},
							Node{PlainText{"more text"}}
						}
					}
				}
			});
			assertPageTree(
			"Some text here\n"
			" \n"
			"  \n"
			" \n"
			"more text\n",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"Some text here"}}
						}
					},
					Node{Paragraph{},
						{
							Node{PlainText{"more text"}}
						}
					}
				}
			});
		});
		
		tester.add("Parser::makeTreeFromPage HyperLink", [](){
			assertPageTree(
			"[[[Page Link]]]",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{HyperLink{"Page Link", "page-link", false}}
						}
					}
				}
			});
			assertPageTree(
			"here is the https://google.com/ link",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"here is the "}},
							Node{HyperLink{"https://google.com/", "https://google.com/", false}},
							Node{PlainText{" link"}}
						}
					}
				}
			});
		});
		
		tester.add("Parser::makeTreeFromPage StyleFormat", [](){
			assertPageTree(
			"**bold text**",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{
								StyleFormat{StyleFormat::Type::Bold},
								{
									Node{PlainText{"bold text"}}
								}
							}
						}
					}
				}
			});
			
			assertPageTree(
			"**double **begin",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"**double **begin"}}
						}
					}
				}
			});
			
			assertPageTree(
			"**bold**text**",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{StyleFormat{StyleFormat::Type::Bold},
								{
									Node{PlainText{"bold"}}
								}
							},
							Node{PlainText{"text**"}}
						}
					}
				}
			});
			
			assertPageTree(
			"** bold**text**",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"** bold"}},
							Node{StyleFormat{StyleFormat::Type::Bold},
								{
									Node{PlainText{"text"}}
								}
							}
						}
					}
				}
			});
			
			assertPageTree(
			"A --B --C D-- E-- F",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"A "}},
							Node{StyleFormat{StyleFormat::Type::Strike},
								{
									Node{PlainText{"B —C D"}}
								}
							},
							Node{PlainText{" E— F"}}
						}
					}
				}
			});
			
			assertPageTree(
			"A ,,B ,,C D,, E,, F",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"A "}},
							Node{StyleFormat{StyleFormat::Type::Sub},
								{
									Node{PlainText{"B ,,C D"}}
								}
							},
							Node{PlainText{" E,, F"}}
						}
					}
				}
			});
			
			assertPageTree(
			"**bold\ntext**",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{StyleFormat{StyleFormat::Type::Bold},
								{
									Node{PlainText{"bold"}},
									Node{LineBreak{}},
									Node{PlainText{"text"}}
								}
							}
						}
					}
				}
			});
		});
		
		tester.add("Parser::makeTreeFromPage StyleFormat Advanced", [](){
			assertPageTree(
			"A **B --C** D-- E",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"A "}},
							Node{StyleFormat{StyleFormat::Type::Bold},
								{
									Node{PlainText{"B "}},
									Node{StyleFormat{StyleFormat::Type::Strike},
										{
											Node{PlainText{"C"}}
										}
									}
								}
							},
							Node{StyleFormat{StyleFormat::Type::Strike},
								{
									Node{PlainText{" D"}}
								}
							},
							Node{PlainText{" E"}}
						}
					}
				}
			});
		});
		
		tester.add("Parser::makeTreeFromPage Heading",[](){
			assertPageTree(
			"+++ heading",
			Node{
				RootPage{},
				{
					Node{Heading{3, false},
						{
							Node{PlainText{"heading"}}
						}
					}
				}
			});
			
			assertPageTree(
			"+++ heading\nnormal text",
			Node{
				RootPage{},
				{
					Node{Heading{3, false},
						{
							Node{PlainText{"heading"}}
						}
					},
					Node{Paragraph{},
						{
							Node{PlainText{"normal text"}}
						}
					}
				}
			});
			
			assertPageTree(
			"+* hidden heading",
			Node{
				RootPage{},
				{
					Node{Heading{1, true},
						{
							Node{PlainText{"hidden heading"}}
						}
					}
				}
			});
			
			assertPageTree(
			"+ s--trik--e",
			Node{
				RootPage{},
				{
					Node{Heading{1, false},
						{
							Node{PlainText{"s"}},
							Node{StyleFormat{StyleFormat::Type::Strike},
								{
									Node{PlainText{"trik"}}
								}
							},
							Node{PlainText{"e"}}
						}
					}
				}
			});
		});
		
		tester.add("Parser::makeTreeFromPage LiteralText",[](){
			assertPageTree(
			"this @@is@@ some @<te&gt;&lt;t>@",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"this "}},
							Node{LiteralText{"is"}},
							Node{PlainText{" some "}},
							Node{LiteralText{"te><t"}}
						}
					}
				}
			});
		});
		
		tester.add("Parser::makeTreeFromPage QuoteBox",[](){
			assertPageTree(
			"> simple quote",
			Node{
				RootPage{},
				{
					Node{QuoteBox{},
						{
							Node{Paragraph{},
								{
									Node{PlainText{"simple quote"}}
								}
							}
						}
					}
				}
			});
		});
		tester.add("Parser::makeTreeFromPage QuoteBox",[](){
			assertPageTree(
			"> quote",
			Node{
				RootPage{},
				{
					Node{QuoteBox{},
						{
							Node{Paragraph{},
								{
									Node{PlainText{"quote"}}
								}
							}
						}
					}
				}
			});
			assertPageTree(
			"> quote\n> part 2",
			Node{
				RootPage{},
				{
					Node{QuoteBox{},
						{
							Node{Paragraph{},
								{
									Node{PlainText{"quote"}},
									Node{LineBreak{}},
									Node{PlainText{"part 2"}}
								}
							}
						}
					}
				}
			});
			assertPageTree(
			">> double quote",
			Node{
				RootPage{},
				{
					Node{QuoteBox{},
						{
							Node{QuoteBox{},
								{
									Node{Paragraph{},
										{
											Node{PlainText{"double quote"}}
										}
									}
								}
							}
						}
					}
				}
			});
			assertPageTree(
			">> double quote\n> drop to single\n> \n> new paragraph",
			Node{
				RootPage{},
				{
					Node{QuoteBox{},
						{
							Node{QuoteBox{},
								{
									Node{Paragraph{},
										{
											Node{PlainText{"double quote"}}
										}
									}
								}
							},
							Node{Paragraph{},
								{
									Node{PlainText{"drop to single"}}
								}
							},
							Node{Paragraph{},
								{
									Node{PlainText{"new paragraph"}}
								}
							}
						}
					}
				}
			});
			assertPageTree(
			"none\n> single\n>> double\n> single again\nnone again\n",
			Node{
				RootPage{},
				{
					Node{Paragraph{},
						{
							Node{PlainText{"none"}}
						}
					},
					Node{QuoteBox{},
						{
							Node{Paragraph{},
								{
									Node{PlainText{"single"}}
								}
							},
							Node{QuoteBox{},
								{
									Node{Paragraph{},
										{
											Node{PlainText{"double"}}
										}
									}
								}
							},
							Node{Paragraph{},
								{
									Node{PlainText{"single again"}}
								}
							}
						}
					},
					Node{Paragraph{},
						{
							Node{PlainText{"none again"}}
						}
					}
				}
			});
			assertPageTree(
			"> quote\n>\n> box",
			Node{
				RootPage{},
				{
					Node{QuoteBox{},
						{
							Node{Paragraph{},
								{
									Node{PlainText{"quote"}}
								}
							},
							Node{Paragraph{},
								{
									Node{PlainText{"box"}}
								}
							}
						}
					}
				}
			});
		});
	}
}





















