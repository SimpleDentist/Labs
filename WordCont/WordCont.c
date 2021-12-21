#include <stdio.h>
#include <string.h>
#include <io.h>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		printf("Incorrect arguments");
		return 1;
	}

	char letter = 0;
	char last_letter = 0;

	int str_count = 0;
	int word_count = 0;
	int byte_count = 0;
	int size;
	float average_size;

	FILE* file_reader;
	FILE* file_reader_bytes;
	file_reader = fopen(argv[2], "r");

	if (!file_reader)
	{
		printf("file error");
		return 1;
	}

	while ((letter = fgetc(file_reader)) != EOF)
	{
		if ((letter == ' ') && ((last_letter != ' ') && (last_letter != '\n')))
		{
			word_count++;
		}

		if (letter == '\n')
		{
			str_count++;
			word_count++;
		}

		last_letter = letter;
	}

	str_count++;

	fseek(file_reader, 0, SEEK_END);
	size = ftell(file_reader);

	fclose(file_reader);

	average_size = size * (1.0)/ str_count;

	if ((!strcmp(argv[1], "-l")) || (!strcmp(argv[1], "--lines")))
	{
		printf("str: %d\n", str_count);
	}
	else if ((!strcmp(argv[1], "-c")) || (!strcmp(argv[1], "--bytes")))
	{
		printf("bytes: %d\n", size);
		printf("average bytes: %f\n", average_size);
	}
	else if ((!strcmp(argv[1], "-w")) || (!strcmp(argv[1], "--words")))
	{
		printf("words: %d\n", word_count);
	}
	else
	{
		printf("Incorrect arguments");
	}
}