#include <stdio.h>
#include <Windows.h>
using namespace std;

/*************************************************
Function:       get_hash_key
Description:    负责计算给定字符串的哈希值
Input:          char* str：   char指针
Return:         unsigned int: 字符串的哈希值
*************************************************/
unsigned int get_hash_key(char* str) {
	return 1;
}



/*************************************************
Function:       output_result
Description:    把字符串写进输出文件
Input:          char* str：  指向要写入的字符串的指针
				FILE* file： 文件指针
*************************************************/
void output_result(char* str, FILE* file) {
	int status = fputs(str, file);	//TODO:if error
	fputc(10, file);
	if (status == -1) {

		MessageBox(NULL, TEXT("Hello, bccn!"), TEXT("HelloMsg"), 0);

		//TODO: 弹窗提示保存出错
	}
}

int main() {
	FILE* f_str = NULL;
	FILE* f_dict = NULL;
	FILE* f_result = NULL;

	//TODO: initialize_hash_table;

	//TODO: build_hash_table;

	//TODO: search and output;

	fopen_s(&f_dict, "../Data/dict.txt", "r");
	if (!f_dict) {
		return 0;
	}

	fopen_s(&f_result, "../Data/result.txt", "w");
	if (!f_result) {
		return 0;
	}


	for (size_t i = 0; i < 10; i++)
	{
		char str[99];
		fscanf_s(f_dict, "%s", str, 99);
		printf("%s", str);
		output_result(str, f_result);

	}

	fclose(f_dict);
	fclose(f_result);
}




// SDBMHash算法
unsigned int SDBMHash(char* str) {
	unsigned int hash = 0;
	while (*str)
	{
		// equivalent to: hash = 65599*hash + (*str++);
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}
	return (hash & 0x7FFFFFFF);
}


// RS Hash算法
unsigned int RSHash(char* str) {
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	while (*str)
	{
		hash = hash * a + (*str++);
		a *= b;
	}
	return (hash & 0x7FFFFFFF);
}


// JS Hash算法
unsigned int JSHash(char* str) {
	unsigned int hash = 1315423911;
	while (*str)
	{
		hash ^= ((hash << 5) + (*str++) + (hash >> 2));
	}
	return (hash & 0x7FFFFFFF);
}



// P.J.Weinberger Hash算法
unsigned int PJWHash(char* str) {
	unsigned int BitsInUnignedInt = (unsigned int)(sizeof(unsigned int) * 8);
	unsigned int ThreeQuarters = (unsigned int)((BitsInUnignedInt * 3) / 4);
	unsigned int OneEighth = (unsigned int)(BitsInUnignedInt / 8);
	unsigned int HighBits = (unsigned int)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
	unsigned int hash = 0;
	unsigned int test = 0;
	while (*str)
	{
		hash = (hash << OneEighth) + (*str++);
		if ((test = hash & HighBits) != 0)
		{
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}
	return (hash & 0x7FFFFFFF);
}



// ELF Hash算法
unsigned int ELFHash(char* str) {
	unsigned int hash = 0;
	unsigned int x = 0;
	while (*str)
	{
		hash = (hash << 4) + (*str++);
		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);             hash &= ~x;
		}
	}
	return (hash & 0x7FFFFFFF);
}

// BKDR Hash算法
unsigned int BKDRHash(char* str) {
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;
	while (*str)
	{
		hash = hash * seed + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}



// DJB Hash算法
unsigned int DJBHash(char* str)
{
	unsigned int hash = 5381;
	while (*str)
	{
		hash += (hash << 5) + (*str++);
	}
	return (hash & 0x7FFFFFFF);
}



// AP Hash算法
unsigned int APHash(char* str) {
	unsigned int hash = 0;
	int i;
	for (i = 0; *str; i++)
	{
		if ((i & 1) == 0)
		{
			hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
		}
		else
		{
			hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
		}
	}
	return (hash & 0x7FFFFFFF);
}