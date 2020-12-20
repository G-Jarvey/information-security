#include <C:\gmp\gmp.h>
#include <iostream>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct keys
{
	char* n;
	char* d;
	char* e;
};

mpz_t* set_primes()//生成大素数p和q 
{
	gmp_randstate_t grt;//随机数种子
	gmp_randinit_default(grt); //设置随机数生成算法为默认
	gmp_randseed_ui(grt, time(NULL)); //设置随机化种子为当前时间

	mpz_t p, q;//mpz_t为整数型
	mpz_init(p); //初始化p和q大素数
	mpz_init(q);

	do {
		mpz_urandomb(p, grt, 2048);//随机生成0-2^1024的一个数
	} while (mpz_probab_prime_p(p, 25) == 0);//素数判断函数，若是素数返还1，重复操作25次基本可保证生成的是素数 
	do {
		mpz_urandomb(q, grt, 2048);
	} while (mpz_probab_prime_p(q, 25) == 0);

	/*
	mpz_urandomb(p, grt, 1024);
	mpz_urandomb(q, grt, 1024);

	mpz_nextprime(p, p);  //gmp的素数生成函数
	mpz_nextprime(q, q);
	*/

	mpz_t* primes = new mpz_t[2];//定义primes数组，存放大素数对（p，q）
	mpz_init(primes[0]);
	mpz_init(primes[1]);

	mpz_set(primes[0], p);
	mpz_set(primes[1], q);

	mpz_clear(p);
	mpz_clear(q);

	return primes;
}

keys* set_key()
{
	mpz_t* primes = set_primes();//生成的p，q传进来
	//计算公共模数n
	mpz_t n;
	mpz_init(n);
	mpz_mul(n, primes[0], primes[1]);//n = p*q

	//计算欧拉函数φ(n) = (p-1)*(q-1)
	mpz_t φn;
	mpz_init(φn);
	mpz_sub_ui(primes[0], primes[0], 1);//p=p-1
	mpz_sub_ui(primes[1], primes[1], 1);//q=q-1
	mpz_mul(φn, primes[0], primes[1]);//φn=p*q

	//如何得知公钥e与φ(n)互质，用辗转相除法，如果最大公约数为1即为互质，还有一个最快最简单的方法，那就是直接设为一个常用的数字65537即可
	mpz_t e;
	mpz_init_set_ui(e, 65537);

	//求e的逆元，即e*d mod (φn)=1;
	mpz_t d;
	mpz_init(d);
	mpz_invert(d, e, φn);//用gmp自带的求数论逆元函数对其进行求解

	//将n，d转换成char类型方便函数传递
	char* str_n = new char[2058];
	char* str_e = new char[2058];
	char* str_d = new char[2058];
	mpz_get_str(str_n, 10, n);
	mpz_get_str(str_e, 10, e);
	mpz_get_str(str_d, 10, d);

	keys* key = new keys;
	key->n = str_n;
	key->d = str_d;
	key->e = str_e;

	mpz_clear(n);
	mpz_clear(e);
	mpz_clear(d);
	mpz_clear(φn);

	return key;//返还key结构体，包括（n，e，d）
}

char* encrypt(const char* plaintext, const char* str_n, const char* str_e)
{
	//将明文x进行加密得到密文y=x^e mod n
	mpz_t x;//要加密的消息
	mpz_t n, y, e;
	mpz_init(y);
	//将输入的消息转化为整形
	mpz_init_set_str(x, plaintext, 10);
	//cout << x << endl;
	mpz_init_set_str(n, str_n, 10);
	mpz_init_set_str(e, str_e, 10);
	mpz_powm(y, x, e, n);  //使用gmp中的模幂计算函数 y=x^e mod n
	char* ci_y = new char[2058];
	mpz_get_str(ci_y, 10, y);

	return ci_y;
}

char* decrypt(const char* ciphertext, const char* str_n, const char* str_d)
{
	//解密函数算法 x=y^d mod n
	mpz_t x, y, n, d;
	mpz_init(x);
	mpz_init_set_str(y, ciphertext, 10);//密文转10进制传进y
	mpz_init_set_str(n, str_n, 10);
	mpz_init_set_str(d, str_d, 10);
	mpz_powm(x, y, d, n);   //使用gmp中的模幂计算函数 x=y^d mod n

	char* pl_x = new char[2058];
	mpz_get_str(pl_x, 10, x);

	return pl_x;
}

int main()
{
	keys* key = set_key();
	cout << "公钥（n，e）" << endl << "n：" << key->n << endl << "e：" << key -> e << endl << endl;
	//cout << "d为：" << key->d << endl << endl;

	char x[2058], y[2058];
	char* plaintext;
	char* ciphertext;


	int flag = 1;
	while (flag)
	{
		cout << "  ******************" << endl;
		cout << "    需要执行什么操作？" << endl;
		cout << "        1.加密数字" << endl;
		cout << "        2.解密数字" << endl;
		cout << "        3.退出" << endl;
		cout << "  ******************" << endl;
		cout << "请输入指令：";
		char order;
		cin >> order;
		switch (order)
		{
		case '1':
			cout << "请输入要加密的数字：";
			cin >> x;
			ciphertext = encrypt(x, key->n, key->e);
			cout << "密文为：" << ciphertext << endl << endl;
			break;
		case '2':
			cout << "请输入要解密的数字：";
			cin >> y;
			plaintext = decrypt(y, key->n, key->d);
			cout << "明文为：" << plaintext << endl << endl;
			break;
		case '3':
			flag = 0;
			break;
		default:
			cout << "请输入1-3！" << endl;
		}
	}
}