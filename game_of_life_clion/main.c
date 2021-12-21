#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    if ((argc < 5) || (argc == 6) || (argc == 8) || (argc > 9))
    {
        printf("incorrect number of args");
        return 0;
    }

    if (strcmp(argv[1], "--input") != 0)
    {
        printf("%s\n", argv[1]);
        printf("incorrect first arg");
        return 0;
    }

    if (strcmp(argv[3], "--output") != 0)
    {
        printf("incorrect second arg");
        return 0;
    }

    if (argc >= 6)
    {
        if (strcmp(argv[5], "--max_iter") != 0)
        {
            printf("incorrect third arg");
            return 0;
        }

        if (argc == 9)
        {
            if (strcmp(argv[3], "--dump_freq") != 0)
            {
                printf("incorrect forth arg");
                return 0;
            }
        }
    }

    char name[2];
    int offset;
    int width;
    int height;
    int size;
    char size_bytes[4];
    char offset_bytes[4];
    char width_bytes[4];
    char height_bytes[4];

    FILE* file_reader;

    if ((file_reader = fopen(argv[2], "rb")) == NULL)
    {
        printf("cannot open file");
        return 0;
    }

    fread(&name, 1, 2, file_reader);
    fread(&size, 1, 4, file_reader);
    fseek(file_reader, 2, SEEK_SET);
    fread(&size_bytes, 1, 4, file_reader);
    fseek(file_reader, 10, SEEK_SET);
    fread(&offset, 1, 4, file_reader);
    fseek(file_reader, 10, SEEK_SET);
    fread(&offset_bytes, 1, 4, file_reader);

    if ((name[0] != 'B') || (name[1] != 'M'))
    {
        printf("its not a bmp file");
        return 0;
    }

    fseek(file_reader, 18, SEEK_SET);
    fread(&width, 1, 4, file_reader);
    fread(&height, 1, 4, file_reader);
    fseek(file_reader, 18, SEEK_SET);
    fread(&width_bytes, 1, 4, file_reader);
    fread(&height_bytes, 1, 4, file_reader);

    char* directory;
    directory = argv[4];

    int max_steps = 300, save_steps = 1;

    if (argc > 5 && strcmp(argv[5], "--max_iter") == 0)
    {
        max_steps = atoi(argv[6]);
    }

    if (argc > 7 && strcmp(argv[7], "--dump_freq") == 0)
    {
        save_steps = atoi(argv[8]);
    }

    int ost = 4 - ((3 * width) % 4);
    int pix_array[height][width];
    int wr_pix_array[height][width];
    char data_bytes[size];

    fseek(file_reader, offset, SEEK_SET);

    unsigned char bgr[size - offset - 1];
    unsigned char cur[1];

    int num = 0, count = 0, garbage = ost;
    if (ost != 4)
    {
        while (fread(cur, 1, 1, file_reader))
        {
            if (count == 3 * width)
            {
                if (garbage > 0)
                {
                    garbage--;
                }
                if (garbage == 0)
                {
                    garbage = ost;
                    count = 0;
                }
            }
            else
            {
                count++;
                bgr[num] = cur[0];
                num++;
            }
        }
    }
    else
    {
        while (fread(cur, 1, 1, file_reader))
        {
            bgr[num] = cur[0];
            num++;
        }
    }

    int near_cells[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            near_cells[i][j] = 0;
        }
    }

    FILE* file_writer;

    char output_path[99999];
    int prev_pix_array[height][width];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            prev_pix_array[i][j] = pix_array[i][j];
        }
    }

    unsigned char b;
    int cur_i = height - 1;
    int cur_j = 0;


    for (int i = 0; i < size - offset - 1; i += 3)
    {
        b = bgr[i];
        bgr[i] = bgr[i + 2];
        bgr[i + 2] = b;
        if ((bgr[i] == 0) && (bgr[i + 1] == 0) && (bgr[i + 2] == 0))
        {
            pix_array[cur_i][cur_j] = 1;
            cur_j++;

            if (cur_j == width)
            {
                cur_i--;
                cur_j = 0;
            }
        }
        else
        {
            pix_array[cur_i][cur_j] = 0;
            cur_j++;

            if (cur_j == width)
            {
                cur_i--;
                cur_j = 0;
            }
        }
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (pix_array[i][j] == 1)
            {
                if (i > 0)
                {
                    near_cells[i - 1][j]++;
                }
                if ((i > 0) && (j > 0))
                {
                    near_cells[i - 1][j - 1]++;
                }
                if (j > 0)
                {
                    near_cells[i][j - 1]++;
                }
                if ((j > 0) && (i < height - 1))
                {
                    near_cells[i + 1][j - 1]++;
                }
                if (i < height - 1)
                {
                    near_cells[i + 1][j]++;
                }
                if ((i < height - 1) && (j < width - 1))
                {
                    near_cells[i + 1][j + 1]++;
                }
                if (j < width - 1)
                {
                    near_cells[i][j + 1]++;
                }
                if ((i > 0) && (j < width - 1))
                {
                    near_cells[i - 1][j + 1]++;
                }
            }
        }
    }

    int flag1;
    int flag2;

    for (int step = 0; step < max_steps; step++)
    {
        flag1 = 0;
        flag2 = 0;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (pix_array[i][j] == 0 && near_cells[i][j] == 3)
                {
                    pix_array[i][j] = 1;
                    if (i > 0)
                    {
                        near_cells[i - 1][j]++;
                    }
                    if ((i > 0) && (j > 0))
                    {
                        near_cells[i - 1][j - 1]++;
                    }
                    if (j > 0)
                    {
                        near_cells[i][j - 1]++;
                    }
                    if ((j > 0) && (i < height - 1))
                    {
                        near_cells[i + 1][j - 1]++;
                    }
                    if (i < height - 1)
                    {
                        near_cells[i + 1][j]++;
                    }
                    if ((i < height - 1) && (j < width - 1))
                    {
                        near_cells[i + 1][j + 1]++;
                    }
                    if (j < width - 1)
                    {
                        near_cells[i][j + 1]++;
                    }
                    if ((i > 0) && (j < width - 1))
                    {
                        near_cells[i - 1][j + 1]++;
                    }
                }
                if ((pix_array[i][j] == 1) && ((near_cells[i][j] < 2) || (near_cells[i][j] > 3)))
                {
                    pix_array[i][j] = 0;
                    if (i > 0)
                    {
                        near_cells[i - 1][j]--;
                    }
                    if ((i > 0) && (j > 0))
                    {
                        near_cells[i - 1][j - 1]--;
                    }
                    if (j > 0)
                    {
                        near_cells[i][j - 1]--;
                    }
                    if ((j > 0) && (i < height - 1))
                    {
                        near_cells[i + 1][j - 1]--;
                    }
                    if (i < height - 1)
                    {
                        near_cells[i + 1][j]--;
                    }
                    if ((i < height - 1) && (j < width - 1))
                    {
                        near_cells[i + 1][j + 1]--;
                    }
                    if (j < width - 1)
                    {
                        near_cells[i][j + 1]--;
                    }
                    if ((i > 0) && (j < width - 1))
                    {
                        near_cells[i - 1][j + 1]--;
                    }
                }
                if (pix_array[i][j] != prev_pix_array[i][j])
                {
                    flag1 = 1;
                }
                prev_pix_array[i][j] = pix_array[i][j];
                if (pix_array[i][j] == 1)
                {
                    flag2 = 1;
                }
            }
        }
        if ((flag1 == 0) || (flag2 == 0))
        {
            printf("dead end");
            return 0;
        }
        if ((step + 1) % save_steps == 0)
        {
            sprintf(output_path, "D:\\Programs\\CProjects\\game_of_life_clion\\cmake-build-debug\\%s\\%s%d%s", directory, "stage ", step + 1, ".bmp");
            file_writer = fopen(output_path, "w+");
            for (int q = 0; q < height; q++)
            {
                for (int k = 0; k < width; k++)
                {
                    wr_pix_array[height - q][k] = pix_array[q][k];
                }
            }
            data_bytes[0] = 'B';
            data_bytes[1] = 'M';
            for (int i = 2; i < 6; i++)
            {
                data_bytes[i] = size_bytes[i - 2];
            }
            for (int i = 6; i < 10; i++)
            {
                data_bytes[i] = 0;
            }
            for (int i = 10; i < 14; i++)
            {
                data_bytes[i] = offset_bytes[i - 10];
            }
            data_bytes[14] = 40;
            for (int i = 15; i < 18; i++)
            {
                data_bytes[i] = 0;
            }
            for (int i = 18; i < 22; i++)
            {
                data_bytes[i] = width_bytes[i - 18];
            }
            for (int i = 22; i < 26; i++)
            {
                data_bytes[i] = height_bytes[i - 22];
            }
            data_bytes[26] = 1;
            data_bytes[27] = 0;
            data_bytes[28] = 24;
            for (int i = 29; i < 54; i++)
            {
                data_bytes[i] = 0;
            }
            for (int i = num; i < offset; i++)
            {
                data_bytes[i] = 0;
            }
            int num2 = offset;
            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    if (wr_pix_array[i][j] == 1)
                    {
                        for (int k = 0; k < 3; k++)
                        {
                            data_bytes[num2] = 255;
                            num2++;
                        }
                    }
                    else
                    {
                        for (int k = 0; k < 3; k++)
                        {
                            data_bytes[num2] = 0;
                            num2++;
                        }
                    }
                    if (ost != 4 && j == width - 1)
                    {
                        for (int k = 0; k < ost; k++)
                        {
                            data_bytes[num2] = 0;
                            num2++;
                        }
                    }
                }
            }
            for (int i = 0; i < size; i++)
            {
                fputc(data_bytes[i], file_writer);
            }
        }
    }
}