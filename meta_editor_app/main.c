#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void show_get(FILE* file_reader, char* command)
{
	while (fgetc(file_reader) != NULL)
	{
		fseek(file_reader, -1, SEEK_CUR);

		char tag[4];
		char char_size[4];
		char char_flags[2];
		char* tag_value;
		int size;
		int flags;

		fread(tag, 1, 4, file_reader);
		fread(char_size, 1, 4, file_reader);
		fread(char_flags, 1, 2, file_reader);

		size = char_size[0] + char_size[1] + char_size[2] + char_size[3];
		flags = char_flags[0] + char_flags[1];

		tag_value = (char*)malloc(size);
		fread(tag_value, 1, size, file_reader);
		
		if (command != NULL)
		{
			if ((command[0] == tag[0]) && (command[1] == tag[1]) && (command[2] == tag[2]) && (command[3] == tag[3]))
			{
				printf("%c%c%c%c\t", tag[0], tag[1], tag[2], tag[3]);
				for (int i = 0; i < size; i++)
				{
					printf("%c", tag_value[i]);
				}
				printf("\n");
			}
		}
		else
		{
			printf("%c%c%c%c\t", tag[0], tag[1], tag[2], tag[3]);
			for (int i = 0; i < size; i++)
			{
				printf("%c", tag_value[i]);
			}
			printf("\n");
		}

		for (int j = 0; j < 4; j++)
		{
			tag[j] = '\0';
			char_size[j] = '\0';
			if (j < 2)
			{
				char_flags[j] = '\0';
			}
		}

		for (int k = 0; k < size; k++)
		{
			tag_value[k] = '\0';
		}
	}
}

void set(FILE* file_reader, char* command, char* value)
{
	while (fgetc(file_reader) != NULL)
	{
		fseek(file_reader, -1, SEEK_CUR);
		char tag[10];
		int size;
		int diff;

		fread(tag, 1, 10, file_reader);

		if ((tag[0] == command[0]) && (tag[1] == command[1]) && (tag[2] == command[2]) && (tag[3] == command[3]))
		{
			size = tag[4] + tag[5] + tag[6] + tag[7];
			
			diff = size - strlen(value) + 1;

			fseek(file_reader, 0, SEEK_CUR);
			if (diff > 0)
			{
				fwrite(" ", 1, 1, file_reader);
				fwrite(value, strlen(value), 1, file_reader);
				for (int i = 0; i < diff - 2; i++)
				{
					fwrite(" ", 1, 1, file_reader);
				}
			}
			else
			{
				fwrite(value, strlen(value), 1, file_reader);
			}

			break;
		}

		for (int j = 0; j < 10; j++)
		{
			tag[j] = '\0';
		}
	}
}

int main(int argc, char* argv[])
{
	if ((argc < 3) || (argc > 4))
	{
		printf("incorrect num of args\n");
		return 0;
	}

	char* filepath;
	char* command;

	filepath = strtok(argv[1], "=");
	if (filepath != NULL)
	{
		filepath = strtok(NULL, "=");
	}

	FILE* file_reader;

	if ((file_reader = fopen(filepath, "r+b")) != 0)
	{
		char id3[6];
		char char_filesize[4];
		int filesize;

		fread(id3, 1, 6, file_reader);

		if ((id3[0] != 'I') || (id3[1] != 'D') || (id3[2] != '3'))
		{
			printf("It's not a mp3 file");
			return 0;
		}

		fread(char_filesize, 1, 4, file_reader);
		filesize = 10 + ((int)char_filesize[0] << 21) + ((int)char_filesize[1] << 14) + ((int)char_filesize[2] << 7) + (int)char_filesize[3];
		filesize /= 8;

		command = strtok(argv[2], "=");

		if ((strcmp(command, "--show") == 0) || (strcmp(command, "--get") == 0))
		{
			if (command != NULL)
			{
				command = strtok(NULL, "=");
			}
			show_get(file_reader, command);
		}
		else if (strcmp(command, "--set") == 0)
		{
			if (command != NULL)
			{
				command = strtok(NULL, "=");
			}

			char* value;
			value = strtok(argv[3], "=");
			if (value != NULL)
			{
				value = strtok(NULL, "=");
			}

			set(file_reader, command, value);
		}
		else
		{
			printf("Incorrect command");
			return 0;
		}

		fclose(file_reader);
	}

	return 1;
}