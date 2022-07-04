#include <iostream>
#include <string>
#include <regex>

class StringValidator {
public:
	virtual ~StringValidator() {};
	virtual StringValidator* setNext(StringValidator* nextValidator) = 0;
	virtual std::string validate(std::string) = 0;
};

class BaseValidator : public StringValidator {
protected:
	StringValidator* next = nullptr;
public:
	virtual ~BaseValidator() { delete next; };
	StringValidator* setNext(StringValidator* nextValidator) override {
		next = nextValidator;
		return nextValidator;
	}
	virtual std::string validate(std::string testString) override {
		if (next) {
			return next->validate(testString);
		}
		return "Success";
	}
};

class NotEmptyValidator : public BaseValidator {
public:
	NotEmptyValidator() {};
	std::string validate(std::string testString) override {
		puts("Checking if empty...");

		if (testString.empty()) {
			return "Please enter a value";
		}

		return BaseValidator::validate(testString);
	}
};

class LengthValidator : public BaseValidator {
	int minLength;
public:
	LengthValidator(int minLength) : minLength(minLength) {};
	std::string validate(std::string testString) override {
		puts("Checking string length...");

		if (testString.length() < minLength) {
			return "Please enter a value longer than " + std::to_string(minLength); 
		}

		return BaseValidator::validate(testString);
	}
};

class RegexValidator : public BaseValidator {
	std::string patternName;
	std::string regexString;
public:
	RegexValidator(std::string patternName, std::string regexString) : patternName(patternName), regexString(regexString){};
	std::string validate(std::string testString) override {
		puts("Checking regex match...");

		if (!std::regex_match(testString, std::regex(regexString))) {
			return "The value entered does not match the proper format for a " + patternName;
		}

		return BaseValidator::validate(testString);
	}
};

class HistoryValidator : public BaseValidator {
	std::vector<std::string> historyItems;
public:
	HistoryValidator(std::vector<std::string> historyItems) : historyItems(historyItems) {};
	std::string validate(std::string testString) override {
		puts("Checking if string has been used before...");

		if (std::find(historyItems.begin(), historyItems.end(), testString) != historyItems.end()) {
			return "Please enter a value that you haven't entered before";
		}

		return BaseValidator::validate(testString);
	}
};

int main(int argc, const char* argv[]) {
	StringValidator* emailValidator = new BaseValidator;
	emailValidator
		->setNext(new NotEmptyValidator)
		->setNext(new RegexValidator("email address", "^\\w+([-+.']\\w+)*@\\w+([-.]\\w+)*\\.\\w+([-.]\\w+)*$"));

	std::cout << "Checking Emails ----------------------\n";
	std::cout << "Input: \n" << emailValidator->validate("") << "\n\n";
	std::cout << "Input: shaun\n" << emailValidator->validate("shaun") << "\n\n";
	std::cout << "Input: shaun@test.com\n" << emailValidator->validate("shaun@test.com") << "\n\n";

	delete emailValidator;
	return 0;
}