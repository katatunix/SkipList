#include "SkipList.h"

#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <conio.h>

class IntComparer : IKeyComparer<int>
{
public:
	int compare(const int& key1, const int& key2) const
	{
		return key1 == key2 ? 0 : (key1 < key2 ? -1 : 1);
	}
};

void main()
{
	srand((unsigned int)time(NULL));

	const IntComparer intComparer;
	SkipList<int, int> list( (const IKeyComparer<int>&)intComparer );
	
	/*list.insert(4, 300);
	list.insert(2, 100);
	list.insert(1, 200);
	list.insert(300, 400);
	list.insert(3, 400);
	*/

	for (int i = 0; i < 2000000; i++)
	{
		int key = rand() % 1000000000;
		if (rand() % 2) key = -key;
		if (rand() % 2) key *= 1234;
		list.insert(key, 777);
	}

	list.printAll("e:\\test.txt");
	printf("finish!");
	getch();
}
