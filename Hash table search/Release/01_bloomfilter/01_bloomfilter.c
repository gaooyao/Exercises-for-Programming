/**********************************************************************
*
* 文件名称：main.cpp
* 内容摘要：计算机应用编程实验-实验1主程序
* 作    者：Jahn1998，gaooyao
* 仓库地址：https://github.com/Jahn1998/Exercises-for-Programming
* 项目日期：2020.9
*
**********************************************************************/


#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifndef	node_struct
#define node_struct

typedef struct HashNode {
    char str[100];
    struct HashNode* next;
}HashNode;

/* �ļ�����Handler,�����ļ��������ӿ��д�ٶ� */
typedef struct FileHandler {
    FILE* file;		//���������ļ�ָ��
    char file_name[99];		//�ļ���
    int open_status;		//��״̬��Ϊ0ʱΪ�رգ�1Ϊreadģʽ��2writeģʽd
    int point;//ָ���ڵ�ǰ�������еĶ�дλ��
    unsigned int buffer_size;//�ļ��������Ĵ�С
    char* buffer;//�ļ�������
}FileHandler;

#endif

//
//  gy.h
//  hashtable_search
//
//  Created by 杨恩 on 2020/9/22.
//



HashNode* create_new_node(char* str);//创建新的hash节点
FileHandler* open_file(char* file_name, char* open_type);//通过文件接口打开新文件
int read_line(FileHandler* file_handler, char** str);//从文件中读取一行
int reset_file(FileHandler* file_handler);
int close_file(FileHandler* file_handler);//关闭文件
int manipulate_m(char* m, unsigned int k, int typ);

/**********************************************************************
*
* 文件名称：gy.cpp
* 内容摘要：计算机应用编程实验-实验1-gaooyao的任务函数
* 作    者：gaooyao
* 仓库地址：https://github.com/Jahn1998/Exercises-for-Programming
* 项目日期：2020.9
*
**********************************************************************/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>




int FILE_BUFFER_SIZE = 262144;    //文件缓冲区大小，单位字节

/*
* 函数名称：create_new_node
* 函数功能：负责根据给定内容创建新的hash节点
* 参数：	str：新创建节点的文本内容
* 返回值：HashNode指针
*/
HashNode* create_new_node(char* str) {
	HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));	//创建空节点
	if (new_node) {
		//创建成功则给其赋值
		new_node->str[0] = '\0';//default value
		new_node->next = NULL;
		strcpy(new_node->str, str);
		return new_node;
	}
	else {
		//创建失败则显示错误信息并退出程序
		printf("create node fail!");
		system("stop");
		return new_node;
	}
	return new_node;
}

/*
* 函数名称：open_file
* 函数功能：根据给定属性创建新的FileHandler对象
* 参数：	file_name：要打开的文件名
*		open_type:文件打开类型，如"r","w"
* 返回值：FileHandler指针
*/
FileHandler* open_file(char* file_name, char* open_type) {
	/* 创建FileHandler */
	FileHandler* file_handler = (FileHandler*)malloc(sizeof(FileHandler));
	if (!file_handler) {
		printf("open %s fail!", file_name);
		return NULL;
	}
	/* 打开文件 */
	FILE* _file = (FILE*)malloc(sizeof(FILE));
	_file = fopen(file_name, open_type);
	if (!_file) {
		printf("open %s fail!", file_name);
		return NULL;
	}
	file_handler->file = _file;

	/* 创建文件缓冲buffer */
	file_handler->buffer_size = FILE_BUFFER_SIZE;
	file_handler->buffer = (char*)malloc(file_handler->buffer_size);
	if (!file_handler->buffer) {
		fclose(file_handler->file);
		return 0;	//分配内存失败
	}
	/* FileHandler属性初始化 */
	strcpy(file_handler->file_name, file_name);
	file_handler->open_status = (open_type == "r" ? 1 : 2);//三元表达式
	file_handler->point = -1;
	return file_handler;
}

/*
* 函数名称：read_line
* 函数功能：从文件中读取一行文本
* 参数：	file_handler：被读取的FileHandler对象
*		str：存放被读取文本的地址
* 返回值：int，1为读取成功，0为读取失败
* TODO:已知bug：若文件结尾为一空行则不会被输出
* TODO:du read dict.txt fail when new buffer。
*/
int read_line(FileHandler* file_handler, char** str) {
	if (file_handler->open_status != 1) return 0;	//打开模式为写入时不允许读取
	int old_point = file_handler->point;//记录当前指针位置
	do {
		//如果已读到文件末尾则返回
		if (file_handler->point != -1 && *(file_handler->buffer + file_handler->point) == -1)return 0;

		//根据当前指针判断是否需要读入新数据
		int is_read_new_data = 0;
		//如果buffer为空，则读入新buffer
		if (file_handler->point == -1) {
			int read_num;
			read_num = fread(file_handler->buffer, 1, file_handler->buffer_size, file_handler->file);
			if (!read_num) {
				return 0;
			}
			else {
				is_read_new_data = 1;
			}
		}
		//如果buffer内不足一行，则读入新数据
		if (file_handler->point + 1 >= file_handler->buffer_size) {
			//copy buffer尾部数据至头部
			int copy_num = 0;
			while (old_point < file_handler->buffer_size) {
				*(file_handler->buffer + copy_num) = *(file_handler->buffer + old_point + 1);
				copy_num++;
				old_point++;
			}
			//读入新数据
			int read_num;
			read_num = fread((char*)(file_handler->buffer + copy_num), 1, file_handler->buffer_size - copy_num, file_handler->file);
			if (!read_num) {
				return 0;
			}
			else {
				is_read_new_data = 1;
			}
		}
		//若读入新数据则重置buffer指针
		if (is_read_new_data) {
			file_handler->point = 0;
			//判断读入的新数据是否可用
			while (*(file_handler->buffer + file_handler->point) != 10 && *(file_handler->buffer + file_handler->point) != 0 && *(file_handler->buffer + file_handler->point) != -1) {
				file_handler->point++;
				if (file_handler->point > file_handler->buffer_size) {
					//读取到的整个buffer内容少于一行，则超过最大处理大小//-1结束//10换行符
					system("stop");
					return 0;
				}
			}
			//数据读取成功，str指针指向字符串开头并返回
			*(file_handler->buffer + file_handler->point) = 0;//断行
			(*str) = &(file_handler->buffer[0]);
			return 1;
		}
		file_handler->point++;
	} while (*(file_handler->buffer + file_handler->point) != '\n');//point指针循环后移，直到遇到换行符则停止
	//没有读入新数据，但已找到被读文本的开头和结尾
	*(file_handler->buffer + file_handler->point) = 0;//断行
	(*str) = (file_handler->buffer + old_point + 1);
	return 1;
}

int reset_file(FileHandler* file_handler) {
	file_handler->point = -1;
	rewind(file_handler->file);
	return 1;
}

/*r
* 函数名称：close_file
* 函数功能：关闭文件，释放FileHandler对象
* 参数：	file_handler：被关闭的FileHandler对象
* 返回值：int,是否关闭成功
*/
int close_file(FileHandler* file_handler) {
	//若文件为写入方式打开，则把缓冲区内容写入文件
	if (file_handler->open_status == 2 && file_handler->point != -1) {
		fwrite(file_handler->buffer, file_handler->point, 1, file_handler->file);
	}
	//关闭文件并释放空间
	fclose(file_handler->file);
	free(file_handler->buffer);
	file_handler->open_status = 0;
	free(file_handler);
	return 1;
}
/*r
* 函数名称：manipulate_m
* 函数功能：读取与写入bloom过滤器
* 参数：    char*m：操作空间；
 unsigned int k： 哈希值
* 返回值：int,
*/
int manipulate_m(char* m, unsigned int h, int typ)
{//mzhizhen zijie
	if (typ == 1)//写
	{
		unsigned char b = (char)128;//1000,0000永远只有1位是1
		b = b >> (h % 8);
		*(m + h / 8) = *(m + h / 8) | b;//
	}
	if (typ == 0)//读，判断是0是1;&&b
	{
		unsigned char b = (char)128;
		b = b >> (h % 8);//0001,0000假如m+k/8为a,若a为0101，0000。b=0001,0000;0001,0000
		if (!(*(m + h / 8) & b))
			return 0;//0则跳过这一行
	}
	return 1;
}


//int n = 0;
//FileHandler* file;
//char* test_str = NULL;
//file = open_fileb("dict.txt", "r");
//while (read_line(file, &test_str)) {
//	n++;
//	if (n % 10000 == 0)
//		printf("%d %s\n", n, test_str);
//}
//system("pause");
//return 0;


//
//  yn.h
//  hashtable_search
//
//  Created by 杨恩 on 2020/9/22.
//

#ifndef yn_h
#define yn_h

int get_hash(char* str);
int write_line(FileHandler* file_handler, char* str);
int num_m(float f, int n);
int num_k(int m, int n);
char* create_m(int m);
#endif /* yn_h */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#ifndef murmurhash
#define murmurhash
/*

 */

#include <stdint.h>
void MurmurHash3_x64_128(const void* key, const int len, const uint32_t seed, void* out);

#endif // !murmurhash
#include <stdlib.h>
#include <stdint.h>

unsigned long rotl64_self(uint64_t x, int8_t r)
{
	return (x << r) | (x >> (64 - r));
}


// Microsoft Visual Studio

#if defined(_MSC_VER)

#define FORCE_INLINE	__forceinline


#define ROTL32(x,y)	_rotl(x,y)
#define ROTL64(x,y)	_rotl64_self(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else	// defined(_MSC_VER)

#define	FORCE_INLINE inline __attribute__((always_inline))

inline uint32_t rotl32(uint32_t x, int8_t r)
{
	return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64(uint64_t x, int8_t r)
{
	return (x << r) | (x >> (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	_rotl64_self(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE uint32_t getblock32(const uint32_t* p, int i)
{
	return p[i];
}

FORCE_INLINE uint64_t getblock64(const uint64_t* p, int i)
{
	return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint32_t fmix32(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

//----------

FORCE_INLINE uint64_t fmix64(uint64_t k)
{
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xff51afd7ed558ccd);
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
	k ^= k >> 33;

	return k;
}


void MurmurHash3_x64_128(const void* key, const int len, const uint32_t seed, void* out) {
	const uint8_t* data = (const uint8_t*)key;
	const int nblocks = len / 16;
	uint64_t h1 = seed;
	uint64_t h2 = seed;
	const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);
	const uint64_t* blocks = (const uint64_t*)(data);
	for (int i = 0; i < nblocks; i++) {
		uint64_t k1 = getblock64(blocks, i * 2 + 0);
		uint64_t k2 = getblock64(blocks, i * 2 + 1);
		k1 *= c1; k1 = rotl64_self(k1, 31); k1 *= c2; h1 ^= k1;
		h1 = rotl64_self(h1, 27); h1 += h2; h1 = h1 * 5 + 0x52dce729;
		k2 *= c2; k2 = rotl64_self(k2, 33); k2 *= c1; h2 ^= k2;
		h2 = rotl64_self(h2, 31); h2 += h1; h2 = h2 * 5 + 0x38495ab5;
	}
	const uint8_t* tail = (const uint8_t*)(data + nblocks * 16);
	uint64_t k1 = 0;
	uint64_t k2 = 0;
	switch (len & 15) {
	case 15: k2 ^= ((uint64_t)tail[14]) << 48;
	case 14: k2 ^= ((uint64_t)tail[13]) << 40;
	case 13: k2 ^= ((uint64_t)tail[12]) << 32;
	case 12: k2 ^= ((uint64_t)tail[11]) << 24;
	case 11: k2 ^= ((uint64_t)tail[10]) << 16;
	case 10: k2 ^= ((uint64_t)tail[9]) << 8;
	case  9: k2 ^= ((uint64_t)tail[8]) << 0;
		k2 *= c2; k2 = rotl64_self(k2, 33); k2 *= c1; h2 ^= k2;
	case  8: k1 ^= ((uint64_t)tail[7]) << 56;
	case  7: k1 ^= ((uint64_t)tail[6]) << 48;
	case  6: k1 ^= ((uint64_t)tail[5]) << 40;
	case  5: k1 ^= ((uint64_t)tail[4]) << 32;
	case  4: k1 ^= ((uint64_t)tail[3]) << 24;
	case  3: k1 ^= ((uint64_t)tail[2]) << 16;
	case  2: k1 ^= ((uint64_t)tail[1]) << 8;
	case  1: k1 ^= ((uint64_t)tail[0]) << 0;
		k1 *= c1; k1 = rotl64_self(k1, 31); k1 *= c2; h1 ^= k1;
	};
	h1 ^= len; h2 ^= len;
	h1 += h2;
	h2 += h1;
	h1 = fmix64(h1);
	h2 = fmix64(h2);
	h1 += h2;
	h2 += h1;
	((uint64_t*)out)[0] = h1;
	((uint64_t*)out)[1] = h2;
}

#include <stdint.h>

int write_line_num = 0;	//�Ƿ���result�ļ���д���кţ�0��д��1д��


/*
* the Name of this  : write_line
* the Function of this : write the targeted lines into result.txt
* Parameter 1 : FileHandler* file_handler : where to be exported
* Parameter 2 : char* str : what to be exported
* value of return : int, 0
*/

int write_line(FileHandler* file_handler, char* str) {


	//if the file is not under the writing status
	if (file_handler->open_status != 2) {
		return 0;
	}

	//�����Ҫд���к�
	if (write_line_num) {
		file_handler->point++;
		//di
		*(file_handler->buffer + file_handler->point) = 0xB5;
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = 0xDA;

		//convert the numbers to be auto-changed
		char target[20];//we have 6306 numbers
		static int num = 1;//numbers will change
		char* t = target;
		sprintf(target, "%d", num);

		while (*t != 0) {
			file_handler->point++;
			*(file_handler->buffer + file_handler->point) = *t;
			t++;
		}
		num++;

		//hang:
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = 0xD0;
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = 0xD0;
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = 0x3A;
	}

	//���ַ���д�뻺������
	while (*str != 0) {
		file_handler->point++;
		*(file_handler->buffer + file_handler->point) = *str;
		str++;
	}
	//���뻻�з�
	file_handler->point++;
	*(file_handler->buffer + file_handler->point) = 10;

	return 0;


}

/*
* the Name of this  : num_m
* the Function of this : calculate the number of memory that we need --- mλ
* Parameter 1 : float f : the error that we allow=0.0000001
* Parameter 2 : int n : the number of the char=1270574
* value of return : int
*/
int num_m(float f, int n)
{
	float m;
	m = n * 1.44 * log2(1 / f);
	m = m / 1 + 1;
	return (int)m;//mλ
}
/*
* the Name of this  : num_k
* the Function of this : calculate the number of Hash that we need
* Parameter 1 : int m : the number of memory that we need
* Parameter 2 : int n : the number of the char=1270574
* value of return : int
*/
int num_k(int m, int n)
{
	float k;
	k = 0.693 * m / n;
	k = k / 1 + 1;
	return (int)k;
}
/*
* the Name of this  : create_m
* the Function of this : ����m��С�Ŀռ�
* Parameter 1 : int m : int(m/8+1)���ֽڴ�С
* value of return : new_m����new_m��ͷ�ĵ�ַ��
*/

char* create_m(int m) {
	m = m / 8 + 1;
	char* new_m = (char*)malloc(m);//malloc�ֽ� 
	for (int i = 0; i < m; i++)
	{
		*(new_m + i) = 0;
	}
	return new_m;
}




#include <unistd.h>


/* 变量的初始化 */
clock_t start, end;		//时间变量，用于统计运行时间
FileHandler* string_file;	//string.txt file handler
FileHandler* dict_file;		//dict.txt file handler	
FileHandler* result_file;	//result.txt file handler
char* str;
int n = 1270574;//待存储的字符串个数
float fp_list[3] = { 0.0001, 0.00001, 0.000001 };
char result_file_name_list[3][99] = { "/home/wanglei/course/result/result_37_0.0001.txt", "/home/wanglei/course/result/result_37_0.00001.txt", "/home/wanglei/course/result/result_37_0.000001.txt" };

int main() {
    /* 打开dict.txt，根据dict.txt建立内存 */
    dict_file = open_file("/home/wanglei/course/dict.txt", "r");
    string_file = open_file("/home/wanglei/course/string.txt", "r");
    start = clock();
    int cycle_num;
    for (cycle_num = 0; cycle_num < 3; cycle_num++) {
        char* new_m;
        int m, k;
        float fp = fp_list[cycle_num];//期望的错误率
        int num = 0;
        m = num_m(fp, n);//m位
        //printf("需要%d位的连续存储空间\n",m);
        k = num_k(m, n);//k个哈希函数
        //printf("需要Hash函数%d个\n",k);
        new_m = create_m(m);//连续的内存位置
        uint64_t* out[2];

        result_file = open_file(result_file_name_list[cycle_num], "w");

        int tt = 0;
        while (read_line(dict_file, &str) == 1) {
            //tt++;
            //printf("dict%d\n", tt);

            for (int seed = 0; seed < 24; seed++) {//一行计算24次
                MurmurHash3_x64_128(str, strlen(str), seed, out);//计算哈希值
                manipulate_m(new_m, (unsigned int)out[1] % m, 1);//置0置1
            }//
        }
        /* 根据dict.txt建立内存结束 */
        /* 打开string.txt，根据string.txt查询 */
        tt = 0;
        while (read_line(string_file, &str) == 1) {
            tt++;
            int t = 0;
            for (int seed = 0; seed < 24; seed++) {
                if (t == 0) {
                    MurmurHash3_x64_128(str, strlen(str), seed, out);
					if (!(manipulate_m(new_m, (unsigned int)out[1] % m, 0))) {
						t = 1;
					}
                        
                }
            }
            if (t == 0)
            {
                num++;
                write_line(result_file, str);
            }
        }
        printf("Error rate:%f string match:%d ", fp, num);
        printf("\n");
        //printf("use %f seconds.\n", (float)(end - start) / CLOCKS_PER_SEC);
        /* 根据string.txt查询结束 */
        /* 关闭文件，结束程序 */
        close_file(result_file);
        reset_file(dict_file);
        reset_file(string_file);


    }
    end = clock();
    close_file(dict_file);
    close_file(string_file);
    printf("runtime: %f s\n", (float)(end - start) / CLOCKS_PER_SEC);
    sleep(3);
    return 0;
}
