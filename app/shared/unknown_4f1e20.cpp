// Earth & Beyond client (reconstruction): a number spelled out in English words. Notes at the end of the file.
#include <string>
#include <string.h>
#include <ctype.h>

std::string Unknown_4F2440(const char *text);

// 0xB756BC: the numbers below twenty
static const char *Ones[20] = {	// NAME 2: what it holds
	"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine",
	"ten", "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"
};

// 0xB7570C: the tens from twenty (read as Tens[tens - 2])
static const char *Tens[8] = {	// NAME 2: what it holds
	"twenty", "thirty", "forty", "fifty", "sixty", "seventy", "eighty", "ninety"
};

// @0x4F1E20
std::string Number_To_Words(int number)	// NAME 2: what it does
{
	std::string text("");
	if (number < 0) {
		text += "negative ";
		number = -number;
	}
	if (number < 20) {
		text += Ones[number];
		return(text);
	}
	if (number < 1000) {
		int hundreds = number / 100;
		unsigned int tens = (number % 100) / 10;
		unsigned int units = number % 10;
		if (hundreds != 0) {
			text += Ones[hundreds];
			text += " hundred ";
		}
		if (tens > 1) {
			text += Tens[tens - 2];
			if (units > 0) {
				text += "-";
			}
		} else {
			units += tens * 10;
		}
		if (units != 0) {
			text += Ones[units];
		}
		return(Unknown_4F2440(text.c_str()));
	}
	unsigned int billions = (unsigned int)number / 1000000000;
	unsigned int millions = (unsigned int)number % 1000000000 / 1000000;
	unsigned int thousands = (unsigned int)number % 1000000 / 1000;
	unsigned int rest = (unsigned int)number % 1000;
	if (billions != 0) {
		text += Number_To_Words(billions);
		text += " billion ";
	}
	if (millions != 0) {
		text += Number_To_Words(millions);
		text += " million ";
	}
	if (thousands != 0) {
		text += Number_To_Words(thousands);
		text += " thousand ";
	}
	if (rest != 0) {
		text += Number_To_Words(rest);
	}
	return(Unknown_4F2440(text.c_str()));
}

// @0x4F2440
std::string Unknown_4F2440(const char *text)	// NAME 1: neutral (D10): the text without its leading and trailing white space
{
	const char *end = text + strlen(text) - 1;
	while (isspace(*text)) {
		text++;
	}
	while (end > text && isspace(*end)) {
		end--;
	}
	return(std::string(text, end + 1));
}

/*
**	Reconstruction notes (QC, 2026-11-16)
**
**	client.exe 0x4F1E20-0x4F25BF (no assert, no __FILE__: the file name is PROVISIONAL, D137, from the queue's row 406),
**	after an iostream object (0x4F1D50) and before money.cpp's neighbours. Three functions: the number in words 0x4F1E20
**	(no direct caller found: recursive for the billions, millions and thousands; the tables 0xB756BC 'zero'..'nineteen'
**	and 0xB7570C 'twenty'..'ninety', which the code reads as Tens[tens - 2]), the trim 0x4F2440 (isspace at both ends,
**	the string built from the range) and <string>'s guarded static 0x4F25A0 (0xC416FC, atexit 0x42B320), which closes
**	the object. The divisions of the thousands' path are unsigned in the client (div), those below 1000 signed (idiv),
**	the tens and units compared unsigned (jbe).
*/
