#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void create(int argc, char* argv[])
{
	FILE* file_writer;

	file_writer = fopen(argv[2], "w");

	int files = argc - 4;
	char str_files[2];

	_itoa(files, str_files, 10);

	fwrite(str_files, sizeof(str_files), 1, file_writer);
	fwrite('\n', 1, 1, file_writer);

	for (int i = 0; i < argc - 4; i++)
	{
		int size;
		char symbol[2];

		FILE* file_reader;
		file_reader = fopen(argv[4 + i], "r");

		fwrite(strlen(argv[4 + i]), sizeof(strlen(argv[4 + i])), 1, file_writer);
		fwrite(argv[4 + i], sizeof(argv[4 + i]), 1, file_writer);

		fseek(file_reader, 0, SEEK_END);
		size = ftell(file_reader);

		fseek(file_reader, 0, SEEK_SET);

		for (int j = 0; j < size; j++)
		{
			fread(symbol, 1, 1, file_reader);
			fwrite(symbol, 1, 1, file_writer);
		}

		fwrite('\n', 1, 1, file_writer);
		fwrite('\n', 1, 1, file_writer);

		free(symbol);
		fclose(file_reader);
		remove(argv[4 + i]);
	}

	fclose(file_writer);
}

void extract(int argc, char* argv[])
{
	FILE* file_reader;

	file_reader = fopen(argv[2], "r");

	fseek(file_reader, 0, SEEK_END);
	int size = ftell(file_reader);
	fseek(file_reader, 0, SEEK_SET);

	char str_files[2];
	fread(str_files, 1, 1, file_reader);
	int files = atoi(str_files);
	fseek(file_reader, 1, SEEK_CUR);
	
	for (int i = 0; i < files; i++)
	{
		char* file_name;
		int str_len;
		fread(&str_len, 1, 1, file_reader);
		file_name = (char*)malloc(str_len + 1);
		fread(file_name, str_len, 1, file_reader);

		FILE* file_writer;
		file_writer = fopen(file_name, "w");

		while (1)
		{
			char symbol[2];
			char symbol2[2];
			fread(symbol, 1, 1, file_reader);
			if (symbol == '\n')
			{
				fread(symbol2, 1, 1, file_reader);
				if (symbol2 == '\n')
				{
					free(symbol);
					free(symbol2);
					break;
				}

				fseek(file_reader, -1, SEEK_CUR);
			}

			fwrite(symbol, 1, 1, file_writer);
			free(symbol);
			free(symbol2);
		}
		
		free(file_name);
		fclose(file_writer);
	}
	fclose(file_reader);
	remove(argv[2]);
}

void list(char* argv[])
{
	FILE* file_reader;
	file_reader = fopen(argv[2], "r");

	int files;
	fread(&files, 1, 1, file_reader);
	fseek(file_reader, 1, SEEK_CUR);
	printf("Files in archive:\n");

	for (int i = 0; i < files; i++)
	{
		int str_len;
		char* name;
		fread(&str_len, 1, 1, file_reader);
		name = (char*)malloc(str_len + 1);
		fread(name, str_len, 1, file_reader);
		
		printf("%s\n", name);

		while (1)
		{
			char sym[2];
			char sym2[2];

			fread(sym, 1, 1, file_reader);
			if (sym == '\n')
			{
				fread(sym2, 1, 1, file_reader);
				if (sym2 == "\n")
				{
					free(sym);
					free(sym2);
					break;
				}

				fseek(file_reader, -1, SEEK_CUR);
			}
		}

		free(name);
	}
}


int main(int argc, char* argv[])
{
	if (strcmp(argv[3], "--create") == 0)
	{
		create(argc, argv);
	}
	else if(strcmp(argv[3], "--extract") == 0)
	{
		extract(argc, argv);
	}
	else if (strcmp(argv[3], "--list") == 0)
	{
		list(argv);
	}
	else
	{
		printf("argument error");
		return 0;
	}

	return 1;
}