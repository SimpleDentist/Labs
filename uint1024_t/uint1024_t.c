#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	unsigned int chunk[35];
}uint1024_t;

uint1024_t from_uint(unsigned int x)
{
	uint1024_t num;

	memset(&num, 0, sizeof(unsigned int) * 35);
	
	int i = 34;
	while (x != 0)
	{
		num.chunk[i] = x % 1000000000;
		x /= 1000000000;
		i--;
	}
	return num;
}

uint1024_t add_op(uint1024_t x, uint1024_t y)
{
	uint1024_t result;
	memset(&result, 1, sizeof(unsigned int) * 35);
	int buffer = 0;

	for (int i = 34; i >= 0; i--)
	{
		result.chunk[i] = (x.chunk[i] + y.chunk[i] + buffer) % 1000000000;
		buffer = (x.chunk[i] + y.chunk[i]) / 1000000000;
	}

	return result;
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y)
{
	uint1024_t result;
	int buffer = 0;
	memset(&result, 0, sizeof(unsigned int) * 35);

	for (int i = 34; i >= 0; i--)
	{
		if (x.chunk[i] < y.chunk[i])
		{
			result.chunk[i] = 1000000000 + x.chunk[i] - y.chunk[i] - buffer;
			buffer = 1;
		}
		else
		{
			result.chunk[i] = x.chunk[i] - y.chunk[i] - buffer;
			buffer = 0;
		}
	}
	return result;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y)
{
	uint1024_t result;
	memset(&result, 0, sizeof(unsigned int) * 35);
	unsigned long long mult = 0;
	unsigned long long buffer = 0;

	for (int i = 34; i >= 0; i--)
	{
		for (int j = 0; i - j >= 0; j++)
		{
			buffer = result.chunk[i - j] + (long long)x.chunk[i] * (long long)y.chunk[34 - j] + mult;
			result.chunk[i - j] = buffer % 1000000000;
			mult = buffer / 1000000000;
		}
	}

	return result;
}

void printf_value(uint1024_t x)
{
	int flag = 0;
	int zero_len;

	for (int i = 0; i < 35; i++)
	{
		if (x.chunk[i] != 0)
		{
			if (flag == 0)
			{
				printf("%u", x.chunk[i]);
				flag = 1;
			}
			else
			{
				int num_str[9];
				_itoa(x.chunk[i], num_str, 10);
				zero_len = 9 - strlen(num_str);
				while (zero_len != 0)
				{
					printf("0");
					zero_len--;
				}
				printf("%u", x.chunk[i]);
			}
		}
		else
		{
			if (flag == 1)
			{
				printf("000000000");
			}
		}
	}
}

void scanf_value(uint1024_t* y)
{
	char str_num[310];
	scanf("%s", str_num);

	memset(y, 0, sizeof(unsigned int) * 35);

	int len = strlen(str_num);

	char str_chunk[10];
	str_chunk[9] = '\0';
	int flag = 0;

	for (int i = 34; i >= 0; i--)
	{
		if (len < 9)
		{
			strncpy(str_chunk, str_num, len);
			flag = 1;
		}
		else
		{
			int j = len - 9;
			strncpy(str_chunk, str_num + j, 9);
		}

		y->chunk[i] = atol(str_chunk);
		
		for (int k = strlen(str_chunk) - 1; k >= 0; k--)
		{
			str_chunk[k] = '\0';
		}

		len -= 9;

		if ((flag == 1) || (len == 0))
		{
			break;
		}
	}
}

main()
{
	printf("unsigned int 999999999 to uint1024_t:\n");
	printf_value(from_uint(999999999));

	uint1024_t x;
	uint1024_t y;

	printf("Input x:\n");
	scanf_value(&x);
	printf("Input y:\n");
	scanf_value(&y);

	printf("x + y: ");
	printf_value(add_op(y, x));
	printf("x - y: ");
	printf_value(subtr_op(y, x));
	printf("x * y: ");
	printf_value(mult_op(y, x));

	return 1;
}