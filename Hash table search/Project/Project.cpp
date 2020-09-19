#include <stdio.h>
#include <Windows.h>

using namespace std;

int get_hash_key(char* str) {
	return 1;
}

void output_result(char* str, FILE* file) {
	int status = fputs(str, file);	//TODO:if error
	fputc(10, file);
	if (status == -1) {
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