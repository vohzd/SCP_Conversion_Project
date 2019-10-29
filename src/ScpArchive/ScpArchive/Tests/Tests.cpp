#include "AutomatedTester.hpp"

#include "Database/ImporterTests.hpp"
#include "Database/DatabaseTests.hpp"
#include "Database/JsonTests.hpp"

namespace Tests{
	void runAllTests(){
		Tester tester;
		
		addImporterTests(tester);
		addJsonTests(tester);
		addDatabaseTests(tester);
		
		addTesterTests(tester);
		
		tester.runTests();
	}
}
