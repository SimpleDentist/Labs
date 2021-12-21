#include <stdio.h>
#include <string.h>

struct list
{
	int value;
	struct list* ptr;
};

struct list* init(int a)
{
struct list* lst;
lst = (struct list*)malloc(sizeof(struct list));
lst->value = a;
lst->ptr = lst;
return(lst);
}

struct list* addelem(struct list* lst, int data)
{
	struct list* temp, * p;
	temp = (struct list*)malloc(sizeof(struct list));
	p = lst->ptr;
	lst->ptr = temp;
	temp->value = data;
	temp->ptr = p;
	return(temp);
}

struct list* deletelem(struct list* root)
{
	struct list* temp;
	temp = root->ptr;
	free(root);
	return(temp);
}

int getvalue(struct list* lst)
{
	struct list* p;
	p = lst;
	return(p->value);
}


int main()
{
	int i = 0;
	int j = 0;
	int count = 0;
	int len;
	int flag = 0;
	int max_requests = 0;
	int leght = 0;
	int sec;
	int min;
	int hour;
	int day;
	int addedflag = 0;
	int tail;
	int head;
	int inputed_time;

	char line[400];
	char timeline[401];
	char requestline[401];
	const char* sep = " ";
	const char* sep_1 = "\"";
	const char* sep_2 = "[";
	const char* sep_3 = "]";

	FILE* file_reader;
	FILE* file_writer_errors;

	if ((file_reader = fopen("access_log_Jul95", "r")) == NULL)
	{
		printf("Cannot open file access_log_Jul95 for read");
		return 0;
	}

	if ((file_writer_errors = fopen("log_Jul95_errors", "w")) == NULL)
	{
		printf("Cannot open file log_Jul95_errors for write");
		return 0;
	}

	printf("Input time(sec):\n");
	scanf("%d", &inputed_time);

	while (fgets(line, 400, file_reader))
	{
		strcpy(timeline, line);
		strcpy(requestline, line);


		for (char* p = strtok(line, sep); p != NULL; p = strtok(NULL, sep))
		{

			if (flag == 1)
			{
				if (p[0] == '5')
				{
					if (strtok(requestline, sep_1) != NULL)
					{
						char* errorstr = strtok(NULL, sep_1);
						if (errorstr != NULL)
						{
							fprintf(file_writer_errors, "%s\n", errorstr);
						}

					}

					count++;
				}
				flag = 0;
			}

			len = strlen(p);
			if (p[len - 1] == '"')
			{
				flag = 1;
			}
		}

		if (strtok(timeline, sep_2) != NULL)
		{
			char * time = strtok(NULL, sep_2);
			if (time != NULL)
			{
				sec = (time[18] - '0') * 10 + (time[19] - '0');
				min = (time[15] - '0') * 10 + (time[16] - '0');
				hour = (time[12] - '0') * 10 + (time[13] - '0');
				day = (time[0] - '0') * 10 + (time[1] - '0');
				sec += min * 60 + hour * 60 * 60 + day * 24 * 60 * 60;

				if (addedflag == 0)
				{
					tail = init(sec);
					addedflag = 1;
					leght++;
					head = tail;
				}
				else
				{
					head = addelem(head, sec);
					leght++;
				}

				while (getvalue(head) - getvalue(tail) >= inputed_time)
				{
					if (leght > max_requests)
					{
						max_requests = leght;
					}
					leght = 0;
					tail = deletelem(tail);
				}
			}
		}

		if (i == 10)
		{
			break;
		}

	}

	printf("max request per inputed time: %d", max_requests);
	fprintf(file_writer_errors, "Number of erroneous requests: %d\n", count);
	
	fclose(file_writer_errors);
	fclose(file_reader);
	return 0;
}