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
