#include <sstream>

#include "lib/argparser/ArgParser.hpp"
#include "test_functions.hpp"
#include "ArgParserUnitTestSuite.hpp"
#include <gtest/gtest.h>

using namespace ArgumentParser;

TEST_F(ArgParserUnitTestSuite, EmptyTest) {
  ArgParser parser("My Empty Parser");

  ASSERT_TRUE(parser.Parse(SplitString("app")));
}

TEST_F(ArgParserUnitTestSuite, StringTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument("param1");

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST_F(ArgParserUnitTestSuite, ShortNameTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument('p', "param1");

  ASSERT_TRUE(parser.Parse(SplitString("app -p=value1")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST_F(ArgParserUnitTestSuite, DefaultTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument("param1").Default("value1");

  ASSERT_TRUE(parser.Parse(SplitString("app")));
  ASSERT_EQ(parser.GetStringValue("param1"), "value1");
}

TEST_F(ArgParserUnitTestSuite, NoDefaultTest) {
  ArgParser parser("My Parser");
  parser.AddStringArgument("param1");

  ASSERT_FALSE(parser.Parse(SplitString("app")));
}

TEST_F(ArgParserUnitTestSuite, StoreValueTest) {
  ArgParser parser("My Parser");
  std::string value;
  parser.AddStringArgument("param1").StoreValue(value);

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1")));
  ASSERT_EQ(value, "value1");
}

TEST_F(ArgParserUnitTestSuite, MultiStringTest) {
  ArgParser parser("My Parser");
  std::string value;
  parser.AddStringArgument("param1").StoreValue(value);
  parser.AddStringArgument('a', "param2");

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=value1 --param2=value2")));
  ASSERT_EQ(parser.GetStringValue("param2"), "value2");
}

TEST_F(ArgParserUnitTestSuite, IntTest) {
  ArgParser parser("My Parser");
  parser.AddIntArgument("param1");

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=100500")));
  ASSERT_EQ(parser.GetIntValue("param1"), 100500);
}

TEST_F(ArgParserUnitTestSuite, MultiValueTest) {
  ArgParser parser("My Parser");
  std::vector<int> int_values;
  parser.AddIntArgument('p', "param1").MultiValue().StoreValues(int_values);

  ASSERT_TRUE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
  ASSERT_EQ(parser.GetIntValue("param1", 0), 1);
  ASSERT_EQ(int_values[1], 2);
  ASSERT_EQ(int_values[2], 3);
}

TEST_F(ArgParserUnitTestSuite, MinCountMultiValueTest) {
  ArgParser parser("My Parser");
  std::vector<int> int_values;
  size_t MinArgsCount = 10;
  parser.AddIntArgument('p', "param1").MultiValue(MinArgsCount).StoreValues(int_values);

  ASSERT_FALSE(parser.Parse(SplitString("app --param1=1 --param1=2 --param1=3")));
}

TEST_F(ArgParserUnitTestSuite, FlagTest) {
  ArgParser parser("My Parser");
  parser.AddFlag('f', "flag1");

  ASSERT_TRUE(parser.Parse(SplitString("app --flag1")));
  ASSERT_TRUE(parser.GetFlag("flag1"));
}

TEST_F(ArgParserUnitTestSuite, FlagsTest) {
  ArgParser parser("My Parser");
  bool flag3;
  parser.AddFlag('a', "flag1");
  parser.AddFlag('b', "flag2").Default(true);
  parser.AddFlag('c', "flag3").StoreValue(flag3);

  ASSERT_TRUE(parser.Parse(SplitString("app -ac")));
  ASSERT_TRUE(parser.GetFlag("flag1"));
  ASSERT_TRUE(parser.GetFlag("flag2"));
  ASSERT_TRUE(flag3);
}

TEST_F(ArgParserUnitTestSuite, PositionalArgTest) {
  ArgParser parser("My Parser");
  std::vector<int> values;
  parser.AddIntArgument("Param1").MultiValue(1).Positional().StoreValues(values);

  ASSERT_TRUE(parser.Parse(SplitString("app 1 2 3 4 5")));
  ASSERT_EQ(values[0], 1);
  ASSERT_EQ(values[2], 3);
  ASSERT_EQ(values.size(), 5);
}

TEST_F(ArgParserUnitTestSuite, HelpTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");

  ASSERT_TRUE(parser.Parse(SplitString("app --help")));
  ASSERT_TRUE(parser.Help());
}

TEST_F(ArgParserUnitTestSuite, HelpStringTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddStringArgument('i', "input", "File path for input file").MultiValue(1);
  parser.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser.AddFlag('p', "flag2", "Use some logic");
  parser.AddIntArgument("number", "Some Number");

  ASSERT_TRUE(parser.Parse(SplitString("app --help")));

  ASSERT_EQ(
      parser.HelpDescription(),
      "My Parser\n"
      "Some Description about program\n"
      "\n"
      "OPTIONS:\n"
      "-i,  --input=<string>:  File path for input file [repeated, min args = 1]\n"
      "     --number=<int>:  Some Number\n"
      "-s,  --flag1:  Use some logic [default = true]\n"
      "-p,  --flag2:  Use some logic\n"
      "\n"
      "-h,  --help:  Display this help and exit\n"
  );
}

TEST_F(ArgParserUnitTestSuite, CompositeStringTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddCompositeArgument('i',
                              "input",
                              "File path for input file").AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser.AddCompositeArgument('o',
                              "output",
                              "File path for output directory").AddValidate(&IsValidFilename).AddIsGood(&IsDirectory);
  parser.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser.AddFlag('p', "flag2", "Use some logic");
  parser.AddIntArgument("number", "Some Number");

  ASSERT_TRUE(parser.Parse(SplitString(
      "app --number 2 -s -i " + kTemporaryFileName + " -o=" + kTemporaryDirectoryName)));

  ASSERT_EQ(parser.GetCompositeValue("input"), kTemporaryFileName);
  ASSERT_EQ(parser.GetCompositeValue("output"), kTemporaryDirectoryName);
}

TEST_F(ArgParserUnitTestSuite, NegativeCompositeStringTest1) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddCompositeArgument('i',
                              "input",
                              "File path for input file").AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser.AddCompositeArgument('o',
                              "output",
                              "File path for output directory").AddValidate(&IsValidFilename).AddIsGood(&IsDirectory);
  parser.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser.AddFlag('p', "flag2", "Use some logic");
  parser.AddIntArgument("number", "Some Number");

  ASSERT_FALSE(parser.Parse(SplitString("app --number 2 -s -i hfeooohfe -o=" + kTemporaryDirectoryName)));
}

TEST_F(ArgParserUnitTestSuite, NegativeCompositeStringTest2) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddCompositeArgument('i',
                              "input",
                              "File path for input file").AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser.AddCompositeArgument('o',
                              "output",
                              "File path for output directory").AddValidate(&IsValidFilename).AddIsGood(&IsDirectory);
  parser.AddFlag('s', "flag1", "Use some logic").Default(true);
  parser.AddFlag('p', "flag2", "Use some logic");
  parser.AddIntArgument("number", "Some Number");

  ASSERT_FALSE(parser.Parse(SplitString("app --number 2 -s -i " + kTemporaryFileName + " -o=./aiejfpeqjfoiqwd")));
}

TEST_F(ArgParserUnitTestSuite, RepeatedParsingTest) {
  ArgParser parser("My Parser");
  parser.AddHelp('h', "help", "Some Description about program");
  parser.AddCompositeArgument('i',
                              "input",
                              "File path for input file").AddValidate(&IsValidFilename).AddIsGood(&IsRegularFile);
  parser.AddCompositeArgument('o',
                              "output",
                              "File path for output directory").AddValidate(&IsValidFilename).AddIsGood(&IsDirectory);
  parser.AddFlag('s', "flag1", "Test is a file").Default(true);
  parser.AddFlag('p', "flag2", "Test is directory");
  parser.AddIntArgument("number", "Some Number");
  ConcreteArgumentBuilder<CompositeString>* test_argument_ptr =
      &parser.AddCompositeArgument('t', "test", "File path for test").AddValidate(&IsValidFilename);

  ASSERT_TRUE(parser.Parse(SplitString("app --number 2 -s -i " + kTemporaryFileName + " -o=" + kTemporaryDirectoryName
                                           + " --test=" + kTemporaryFileName)));

  if (parser.GetFlag("flag1")) {
    test_argument_ptr->AddIsGood(&IsRegularFile);
  } else if (parser.GetFlag("flag2")) {
    test_argument_ptr->AddIsGood(&IsDirectory);
  }

  ASSERT_TRUE(parser.Parse(SplitString("app --number 2 -s -i " + kTemporaryFileName + " -o="
                                           + kTemporaryDirectoryName + " --test=" + kTemporaryFileName)));
}
