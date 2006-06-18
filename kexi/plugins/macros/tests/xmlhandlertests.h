/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KOMACROTEST_XMLHandlerTests_H
#define KOMACROTEST_XMLHandlerTests_H

#include <kunittest/tester.h>
#include "../lib/macro.h"

namespace KoMacroTest {

	/**
	* The common testsuite used to test common @a KoMacro
	* functionality.
	*/
	class XMLHandlerTests : public KUnitTest::SlotTester
	{
			Q_OBJECT
		public:

			/**
			* Constructor.
			*/
			XMLHandlerTests();

			/**
			* Destructor.
			*/
			virtual ~XMLHandlerTests();

		public slots:

			/**
			* This slot got called by KUnitTest before testing
			* starts.
			*/
			void setUp();

			/**
			* This slot got called by KUnitTest after all tests
			* are done.
			*/
			void tearDown();

			/**
			* Test the @a KoMacro::XMLHandler parseXML()-function.
			*/
			void testParseXML();

			/**
			* Test the @a KoMacro::XMLHandler toXML()-function.
			*/
			void testToXML();


		private:
			/// @internal d-pointer class.
			class Private;
			/// @internal d-pointer instance.
			Private* const d;
			// Compares a XML-Element with a Macro. Call sub-asserts.
			void assertMacroContentEqToXML(const KSharedPtr<KoMacro::Macro> macro, 
				const QDomElement& domelement, 
				bool isitemsempty, bool isactionset, QMap<QString, bool isvariableok);
			// Prints a QMap of Variables to kdDebug().
			void printMvariables(QMap<QString, KSharedPtr<KoMacro::Variable > > mvariables, QString s);

			/** 
			* Sub-methods of testParseXML().
			* Test the correct parsing of a @a QDomElement into a @a Macro
			* respectively expected failure of parsing.
			*/
			// 1.Test - Correct DomElement.
			void testCorrectDomElement();
			// 2.Test - XML-document with bad root element.
			void testBadRoot();
			// 3.Test - XML-document with a missing Variable.
			void testMissingVariable();
			// 4.Test - One more Variable in XML-Document.
			void testMoreVariables();
			// 5.Test - XML-document with wrong macro-xmlversion.
			void testWrongVersion();
			// 6.Test - XML-document if it has a wrong structure like
			// wrong parathesis	or missing end tag.
			void testWrongXMLStruct();
			// 7.Test - XML-document with wrong item- and variable-tags.
			void testWrongItemVarTags();
			// 8.Test-XML-document with maximum field-size.
			void testMaxNum();
			// 9.Test-XML-document with maximum +1 field-size.
			void testMaxNum2();
			// 10.Test-XML-document with minimum field-size.
			void testMinNum();
			// 11.Test-XML-document with minimum field-size.
			void testMinNum2();
			// 12.Test - With a to big number.
			void testBigNumber();
			// 13.Test - With two MacroItems.
			void testTwoMacroItems();
	};

}

#endif
