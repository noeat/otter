#include "common_util.h"


Tokenizer::Tokenizer(const std::string& src, const char sep, uint32 vectorReserve /*= 0*/, bool keepEmptyStrings /*= true*/)
{
	str_ = new char[src.length() + 1];
	memcpy(str_, src.c_str(), src.length() + 1);

	if (vectorReserve)
		storage_.reserve(vectorReserve);

	char* posold = str_;
	char* posnew = str_;

	for (;;)
	{
		if (*posnew == sep)
		{
			if (keepEmptyStrings || posold != posnew)
				storage_.push_back(posold);

			posold = posnew + 1;
			*posnew = '\0';
		}
		else if (*posnew == '\0')
		{
			if (posold != posnew)
				storage_.push_back(posold);

			break;
		}

		++posnew;
	}
}