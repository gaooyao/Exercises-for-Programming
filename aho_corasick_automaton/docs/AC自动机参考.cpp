/* https://www.cnblogs.com/hyfhaha/p/10802604.html */
#include <bits/stdc++.h>
#define maxn 1000001
using namespace std;
struct kkk
{
    int son[26], flag, fail;
} trie[maxn];
int n, cnt;
char s[1000001];
queue<int> q;
void insert(char *s)
{
    int u = 1, len = strlen(s);
    for (int i = 0; i < len; i++)
    {
        int v = s[i] - 'a';
        if (!trie[u].son[v])
            trie[u].son[v] = ++cnt;
        u = trie[u].son[v];
    }
    trie[u].flag++;
}
void getFail()
{
    for (int i = 0; i < 26; i++)
        trie[0].son[i] = 1; //初始化0的所有儿子都是1
    q.push(1);
    trie[1].fail = 0; //将根压入队列
    while (!q.empty())
    {
        int u = q.front();
        q.pop();
        for (int i = 0; i < 26; i++)
        {                            //遍历所有儿子
            int v = trie[u].son[i];  //处理u的i儿子的fail，这样就可以不用记父亲了
            int Fail = trie[u].fail; //就是fafail，trie[Fail].son[i]就是和v值相同的点
            if (!v)
            {
                trie[u].son[i] = trie[Fail].son[i];
                continue;
            }                                 //不存在该节点，第二种情况
            trie[v].fail = trie[Fail].son[i]; //第三种情况，直接指就可以了
            q.push(v);                        //存在实节点才压入队列
        }
    }
}
int query(char *s)
{
    int u = 1, ans = 0, len = strlen(s);
    for (int i = 0; i < len; i++)
    {
        int v = s[i] - 'a';
        int k = trie[u].son[v]; //跳Fail
        while (k > 1 && trie[k].flag != -1)
        {                                           //经过就不统计了
            ans += trie[k].flag, trie[k].flag = -1; //累加上这个位置的模式串个数，标记已经过
            k = trie[k].fail;                       //继续跳Fail
        }
        u = trie[u].son[v]; //到下一个儿子
    }
    return ans;
}
int main()
{
    cnt = 1; //代码实现细节，编号从1开始
    scanf("%d", &n);
    for (int i = 1; i <= n; i++)
    {
        scanf("%s", s);
        insert(s);
    }
    getFail();
    scanf("%s", s);
    printf("%d\n", query(s));
    return 0;
}